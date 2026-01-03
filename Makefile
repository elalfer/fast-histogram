all:
	# clang++ -O3 -pg -g -std=c++20 -L/home/user/libs/google_benchmark/lib -I/home/user/libs/google_benchmark/include -Iinclude ./src/bench.cpp -o bench.clang -mavx2 -lbenchmark
	g++ -O3 -pg -g -std=c++20 -L/home/user/libs/google_benchmark/lib -I/home/user/libs/google_benchmark/include -Iinclude ./src/bench.cpp -o bench.clang -mavx2 -lbenchmark

