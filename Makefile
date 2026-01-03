CXX_OPT=-O3 -g -std=c++20 -mavx2
CXX=clang++

GOOGLE_BENCH_PATH=$(HOME)/libs/google_benchmark

all:
	mkdir -p bin
	$(CXX) $(CXX_OPT) -L$(GOOGLE_BENCH_PATH)/lib -I$(GOOGLE_BENCH_PATH)/include -Iinclude ./src/bench.cpp -o ./bin/bench -lbenchmark
