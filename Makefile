CXX_OPT=-O3 -g -std=c++20 -mavx2
CXX=clang++

GOOGLE_BENCH_PATH=$(HOME)/libs/google_benchmark
GOOGLE_TEST_PATH=$(HOME)/libs/google_test

mkbin:
	mkdir -p bin

test: mkbin
	$(CXX) $(CXX_OPT) -L$(GOOGLE_TEST_PATH)/lib -I$(GOOGLE_TEST_PATH)/include -Iinclude ./src/test.cpp -o ./bin/test -lgtest
	./bin/test

all: test
	$(CXX) $(CXX_OPT) -L$(GOOGLE_BENCH_PATH)/lib -I$(GOOGLE_BENCH_PATH)/include -Iinclude ./src/bench.cpp -o ./bin/bench -lbenchmark
