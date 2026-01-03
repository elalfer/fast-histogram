#include <stdint.h>
#include <string.h>
#include <immintrin.h>

namespace fast_histogram {

  // Baseline byte granularity histogram
  // Least memory requirements
  void ByteHistogram(uint32_t *out, const unsigned char *buf, size_t len) {
    for (size_t i=0; i<len; i++) {
      out[(int)buf[i]]++;
    }
  }
  
  // Collect histogram into 4 different buffers
  // Reduces some collisions issues for some microarch
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
  
  // Accumulate histogram into 256 buckets
  // Assuming len is less than 16K input elements
  void ByteHistogramX256(uint32_t *out, const unsigned char *buf, size_t len) {
    using BucketType = uint16_t;
    BucketType t[256][256];
    memset(t, 0, sizeof t);
    
    size_t i = 0;
    for (; i < (len & (~255)); i += 256) {
  #pragma unroll(32)
      for (int k = 0; k < 256; k++) {
        t[k][buf[i+k]]++;
      }
    }
    
    // Accumulate current values
    for (int k = 0; k < 256; k += 8) {
  
  #ifdef __AVX2__
      __m256i s = _mm256_setzero_si256();
      for (int l = 0; l < 256; l++) {
        auto tmp = _mm256_cvtepu16_epi32(_mm_loadu_si128((__m128i_u*)&t[l][k]));
        s = _mm256_add_epi32(s, tmp);
      }
      _mm256_storeu_si256((__m256i_u*)&out[k], s);
  #else  
      uint32_t s = 0;
      for (int l = 0; l < 256; l++) {
        s += t[l][k];
      }
      out[k] = s;
  #endif
  
    }
    
    // Finish the tail
    for (; i < len; i++) {
      out[buf[i]]++;
    }
  }
  
  // 
  template <int BITS=16, typename BUCKET_TYPE=uint8_t>
  void ByteHistogramLong(uint32_t* out, const unsigned char* buf, size_t len) {
    using histo_int_t = BUCKET_TYPE;
    constexpr int bucketsCnt = 1 << BITS;
    histo_int_t hT[bucketsCnt] = {0};
  
    // Create 32bit histogram
    size_t k = 0;
    uint16_t tv;
  
    for (; k < (len & ~7); k += 2 * 4) {
      #define STORE_16BIT \
          "  movq %%rcx, %%rbx\n\t" \
          "  andq $0xFF, %%rbx\n\t" \
          "  addq $256, (%[out], %%rbx, 4)\n\t" \
          "  shrq $8, %%rcx\n\t" \
          "  addq $256, (%[out], %%rcx, 4)\n\t"
  
      asm volatile(
          "  movzwq (%[inp], %[i], 1), %%rcx\n\t"
          "  addb $1, (%[histo], %%rcx, 1)\n\t"
          "  jc flush_0\n\t"
          "inc_0:"
          "  movzwq 2(%[inp], %[i], 1), %%rcx\n\t"
          "  addb $1, (%[histo], %%rcx, 1)\n\t"
          "  jc flush_1\n\t"
          "inc_1:"
          "  movzwq 4(%[inp], %[i], 1), %%rcx\n\t"
          "  addb $1, (%[histo], %%rcx, 1)\n\t"
          "  jc flush_2\n\t"
          "inc_2:"
          "  movzwq 6(%[inp], %[i], 1), %%rcx\n\t"
          "  addb $1, (%[histo], %%rcx, 1)\n\t"
          "  jc flush_3\n\t"
          
          "jmp end_loop\n\t"
  
          "flush_0:\n\t"
          STORE_16BIT
          "  jmp inc_0\n\t"
          "flush_1:\n\t"
          STORE_16BIT
          "  jmp inc_1\n\t"
          "flush_2:\n\t"
          STORE_16BIT
          "  jmp inc_2\n\t"
          "flush_3:\n\t"
          STORE_16BIT
  
          "end_loop:\n\t"
          :
          : [histo]"r"(hT), [out]"r"(out),
            [inp]"r"(buf), [i]"r"(k)
          : "rbx", "rcx"
          );
    }
  
    #define LOAD_TMP(idx) _mm256_cvtepu8_epi32(_mm_cvtsi64_si128(*((uint64_t*)&hT[(idx)])))
    // #define LOAD_TMP(idx) _mm256_cvtepu16_epi32(_mm_loadu_si128((__m128i_u*)&hT[(idx)]))
    // #define LOAD_TMP(idx) _mm256_loadu_si256((__m256i_u*)&hT[idx])
  
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
      _mm256_storeu_si256((__m256i_u*)&out[i], 
        _mm256_add_epi32(_mm256_loadu_si256((__m256i_u*)&out[i]), v0));
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
  
    // Add last few bytes
    for(; k < len; k++) {
      out[buf[k]]++;
    }
  }
  
} // namespace