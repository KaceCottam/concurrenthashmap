test: test.cpp chashmap.h
	g++ $< -o $@ --std=c++20 -g -Wall -Wextra -Werror -Wpedantic -lpthread --coverage
