all:
	clang++ -O3 -g -std=c++20 ./src/main.cpp -o bench.clang -mavx2 -lbenchmark -lgtest
	# g++ -O3 -g -std=c++20 ./src/main.cpp -o bench.gcc -mavx2 -lbenchmark -lgtest

