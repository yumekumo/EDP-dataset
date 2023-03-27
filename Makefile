run: solve.cpp
	clang++ -g -D_GLIBCXX_DEBUG -ftrapv -Wall -O2 -std=c++17 solve.cpp && ./a.out
test: solve.cpp
	clang++ -g -std=c++17 solve.cpp -o ./debug/solve.out