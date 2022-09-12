#include <array>
#include <iostream>
#include <immintrin.h>
#include <benchmark/benchmark.h>

#include <gtest/gtest.h>

#define SIZE (1024 * 1024)

unsigned char* genRandBuffer(size_t size) {
  unsigned char* buf = (unsigned char*) malloc(SIZE);
  for (size_t i=0; i<size; i++) {
    buf[i] = (unsigned char) ( (i * 1023 + 123123) % 256);
  }
  return buf;
}

static const unsigned char* dataBuf = genRandBuffer(SIZE);

/// All histogram implementations

void ByteHistogram(uint32_t *out, const unsigned char *buf, size_t len) {
  for (size_t i=0; i<len; i++) {
    out[(int)buf[i]]++;
  }
}

// Collect histogram into 4 different buffers
void ByteHistogramX4(uint32_t *out, const unsigned char *buf, size_t len) {
  uint32_t* h0 = out;
  uint32_t* h1 = out + 256;
  uint32_t* h2 = out + 256 * 2;
  uint32_t* h3 = out + 256 * 3;
  size_t i = 0;
  size_t l4 = len & (~3);
  for (; i < l4; i += 4) {
    uint32_t v = *((uint32_t*)&buf[i]);
    h0[v & 0xFF]++;
    h1[(v >> 8) & 0xFF]++;
    h2[(v >> 16) & 0xFF]++;
    h3[(v >> 24) & 0xFF]++;
  }

  for (; i<len; i++) {
    out[(uint32_t)buf[i]]++;
  }

  for (int k=0; k<256; k++) {
    out[k] += h1[k] + h2[k] + h3[k];
  }
}

template <int BITS=16, typename BUCKET_TYPE=uint16_t>
void ByteHistogramLong(uint32_t* out, const unsigned char* buf, size_t len) {
  using histo_int_t = BUCKET_TYPE;
  constexpr int bucketsCnt = 1 << BITS;
  histo_int_t hT[bucketsCnt] = {0};

  // #define LOAD_TMP(idx) _mm256_cvtepu8_epi32(_mm_cvtsi64_si128(*((uint64_t*)&hT[(idx)])))
  #define LOAD_TMP(idx) _mm256_cvtepu16_epi32(_mm_loadu_si128((__m128i_u*)&hT[(idx)]))
  // #define LOAD_TMP(idx) _mm256_loadu_si256((__m256i_u*)&hT[idx])

  // Create 32bit histogram
  size_t i = 0;
  for (; i < (len & ~1); i += 2) {

    // TODO bench RMW on BDW/SKL
    hT[*((uint16_t*)&buf[i])]++;


    // flush temp storage on overflow flag only if accumulator is < 32bit
    // should be very uncommon situation for more or less
    // evenly distributed values 
    // Shouldn't happen more often than 1/64K only when all values are the same
  }


  // Merge histograms using AVX2
  // 1. Sum by 256
  for (int i = 0; i < 256; i += 8) {

    auto v0 = _mm256_setzero_si256();
    auto v1 = _mm256_setzero_si256();
    for (int k = 0; k < bucketsCnt; k += 512) {
      auto d = LOAD_TMP(i + k); 
      v0 = _mm256_add_epi32(v0, d);
      d = LOAD_TMP(i + k + 256); 
      v1 = _mm256_add_epi32(v1, d);
    }
    v0 = _mm256_add_epi32(v0, v1);
    _mm256_storeu_si256((__m256i_u*)&out[i], v0);
  }

  // 2. Horizontal sum by 256 element slices
  for (int i = 0; i < 256; i++) {
    // Unroll by 2 to accumulate dep chain and use 2 loads per cycle
    auto v0 = LOAD_TMP(i * 256);
    auto v1 = LOAD_TMP(i * 256 + 8);
    for (int k = 16; k < 256; k += 16) {
      v0 = _mm256_add_epi32(v0, LOAD_TMP(i*256+k));
      v1 = _mm256_add_epi32(v1, LOAD_TMP(i*256+k+8));
    }
    v0 = _mm256_add_epi32(v0, v1);

    __m128i r = _mm_add_epi32(_mm256_castsi256_si128(v0), _mm256_extracti128_si256(v0, 1));
    r = _mm_add_epi32(r, _mm_srli_si128(r, 2 * sizeof(uint32_t)));
    r = _mm_add_epi32(r, _mm_srli_si128(r, 1 * sizeof(uint32_t)));
    out[i] += _mm_cvtsi128_si32(r);
  }

  // Add last byte if len is odd
  if (len % 1) {
    out[buf[len - 1]]++;
  }
}

// Benchmark definitions

static void BenchByteHistogram(benchmark::State& state) {
  uint32_t h[256];
  for (auto _ : state) {
    memset(h, 0, 256 * sizeof(uint32_t));
    ByteHistogram(h, dataBuf, state.range(0));
  }
}
BENCHMARK(BenchByteHistogram)->Range(8, SIZE);

static void BenchByteHistogramX4(benchmark::State& state) {
  uint32_t h[256 * 4];
  for (auto _ : state) {
    memset(h, 0, 256 * 4 * sizeof(uint32_t));
    ByteHistogramX4(h, dataBuf, state.range(0));
  }
}
BENCHMARK(BenchByteHistogramX4)->Range(8, SIZE);

static void BenchByteHistogramLong16(benchmark::State& state) {
  uint32_t h[256];
  for (auto _ : state) {
    memset(h, 0, 256 * sizeof(uint32_t));
    ByteHistogramLong<16>(h, dataBuf, state.range(0));
  }
}
BENCHMARK(BenchByteHistogramLong16)->Range(8, SIZE);

// Unittests

TEST(Histogram, X4) {
  uint32_t hOrig[256] = {0};
  uint32_t hX4[256 * 4] = {0};

  ByteHistogram(hOrig, dataBuf, SIZE);
  ByteHistogramX4(hX4, dataBuf, SIZE);

  EXPECT_EQ(memcmp(hOrig, hX4, 256*sizeof(uint32_t)), 0);
}

TEST(Histogram, Long16) {
  uint32_t hOrig[256] = {0};
  uint32_t hLong16[256] = {0};

  ByteHistogram(hOrig, dataBuf, SIZE);
  ByteHistogramLong<16>(hLong16, dataBuf, SIZE);

  EXPECT_EQ(memcmp(hOrig, hLong16, 256*sizeof(uint32_t)), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto test_res = RUN_ALL_TESTS();
  if (test_res != 0) {
    return test_res;
  }

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  return 0;
}

