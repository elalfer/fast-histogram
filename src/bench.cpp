#include <fast_histogram.h>

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>

#include <benchmark/benchmark.h>

using namespace fast_histogram;

static size_t SIZE = (1024 * 1024);

unsigned char* genRandBuffer(size_t size) {
  unsigned char* buf = (unsigned char*) malloc(SIZE);
  for (size_t i=0; i<size; i++) {
    buf[i] = (unsigned char) ( (i * 1023 + 123123) % 256);
  }
  return buf;
}

static const unsigned char* dataBuf;

// Benchmark definitions

static void BenchByteHistogram(benchmark::State& state) {
  uint32_t h[256];
  for (auto _ : state) {
    memset(h, 0, 256 * sizeof(uint32_t));
    ByteHistogram(h, dataBuf, SIZE /*state.range(0)*/);
  }
}
BENCHMARK(BenchByteHistogram); //->Range(8, SIZE);

static void BenchByteHistogramX4(benchmark::State& state) {
  uint32_t h[256 * 4];
  for (auto _ : state) {
    memset(h, 0, 256 * 4 * sizeof(uint32_t));
    ByteHistogramX4(h, dataBuf, SIZE);
  }
}
BENCHMARK(BenchByteHistogramX4); //->Range(8, SIZE);

static void BenchByteHistogramX256(benchmark::State& state) {
  uint32_t h[256];
  for (auto _ : state) {
    ByteHistogramX256(h, dataBuf, SIZE);
  }
}
BENCHMARK(BenchByteHistogramX256); //->Range(8, SIZE);

static void BenchByteHistogramLong16(benchmark::State& state) {
  uint32_t h[256];
  for (auto _ : state) {
    memset(h, 0, 256 * sizeof(uint32_t));
    ByteHistogramLong<16>(h, dataBuf, SIZE);
  }
}
BENCHMARK(BenchByteHistogramLong16); //->Range(8, SIZE);

// Unittests
#if 0

TEST(Histogram, X4) {
  uint32_t hOrig[256] = {0};
  uint32_t hX4[256 * 4] = {0};

  ByteHistogram(hOrig, dataBuf, SIZE);
  ByteHistogramX4(hX4, dataBuf, SIZE);

  EXPECT_EQ(memcmp(hOrig, hX4, 256*sizeof(uint32_t)), 0);
}

TEST(Histogram, X256) {
  uint32_t hOrig[256] = {0};
  uint32_t hX256[256] = {0};

  ByteHistogram(hOrig, dataBuf, SIZE);
  ByteHistogramX256(hX256, dataBuf, SIZE);

  EXPECT_EQ(memcmp(hOrig, hX256, 256*sizeof(uint32_t)), 0);
}

TEST(Histogram, Long16) {
  uint32_t hOrig[256] = {0};
  uint32_t hLong16[256] = {0};

  ByteHistogram(hOrig, dataBuf, SIZE);
  ByteHistogramLong<16>(hLong16, dataBuf, SIZE);

  for (int i = 0; i < 256; i++) {
    EXPECT_EQ(hOrig[i], hLong16[i]);
  }
}

#endif

int main(int argc, char **argv) {
  if (argc > 1) {
    // Read file and replace data buffer with it
    std::ifstream t(std::string{argv[1]});
    std::stringstream buffer;
    buffer << t.rdbuf();

    static std::string dataStr = buffer.str();
    SIZE = dataStr.size();

    dataBuf = (const unsigned char *)dataStr.c_str();
  } else {
    // Generate random buffer
    dataBuf = genRandBuffer(SIZE);
  }

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  return 0;
}

