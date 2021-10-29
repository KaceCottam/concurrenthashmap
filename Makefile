test: test.cpp chashmap.h
	clang++ $< -o $@ --std=c++20 -ffast-math -O3 -Wall -Wextra -Werror -Wpedantic -lpthread --coverage
