#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <fast_histogram.h>

using namespace fast_histogram;

#define SIZE (1024 * 1024)

std::vector<unsigned char> genRandBuffer(size_t size) {
  std::vector<unsigned char> buf(size);
  std::mt19937 gen(0);
  for (size_t i=0; i<size; i++) {
    buf[i] = static_cast<unsigned char>( gen() % 256 );
  }
  return buf;
}

static const std::vector<unsigned char> dataBuf = genRandBuffer(SIZE);

TEST(Histogram, X4) {
    uint32_t hOrig[256] = {0};
    uint32_t hX4[256 * 4] = {0};
  
    ByteHistogram(hOrig, dataBuf.data(), SIZE);
    ByteHistogramX4(hX4, dataBuf.data(), SIZE);
  
    EXPECT_EQ(memcmp(hOrig, hX4, 256*sizeof(uint32_t)), 0);
  }
  
  TEST(Histogram, X256) {
    uint32_t hOrig[256] = {0};
    uint32_t hX256[256] = {0};
  
    ByteHistogram(hOrig, dataBuf.data(), SIZE);
    ByteHistogramX256(hX256, dataBuf.data(), SIZE);
  
    EXPECT_EQ(memcmp(hOrig, hX256, 256*sizeof(uint32_t)), 0);
  }
  
  TEST(Histogram, Long16) {
    uint32_t hOrig[256] = {0};
    uint32_t hLong16[256] = {0};
  
    ByteHistogram(hOrig, dataBuf.data(), SIZE);
    ByteHistogramLong<16>(hLong16, dataBuf.data(), SIZE);
  
    for (int i = 0; i < 256; i++) {
      EXPECT_EQ(hOrig[i], hLong16[i]);
    }
  }

  int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
  }
