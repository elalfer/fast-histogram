#include <array>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include <benchmark/benchmark.h>

#include <fast_histogram.h>

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

#ifdef __AVX2__

static void BenchByteHistogramLong16(benchmark::State& state) {
  uint32_t h[256];
  for (auto _ : state) {
    memset(h, 0, 256 * sizeof(uint32_t));
    ByteHistogramLong<16>(h, dataBuf, SIZE);
  }
}
BENCHMARK(BenchByteHistogramLong16); //->Range(8, SIZE);

#endif // __AVX2__

int main(int argc, char **argv) {
  if (argc > 1) {
    const auto fname = std::string{argv[1]};
    if (!std::filesystem::exists(fname)) {
      std::cerr << "ERROR: Input file does not exist\n";
      return 1;
    }

    std::cout << "Reading file: " << fname << std::endl;

    // Read file and replace data buffer with it
    std::ifstream t(fname);
    std::stringstream buffer;
    buffer << t.rdbuf();

    static std::string dataStr = buffer.str();
    SIZE = dataStr.size();

    std::cout << "Input size: " << SIZE << std::endl;

    dataBuf = (const unsigned char *)dataStr.c_str();
  } else {
    // Generate random buffer
    dataBuf = genRandBuffer(SIZE);
  }

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  return 0;
}

