test: test.cpp chashmap.h
	clang++ $< -o $@ --std=c++20 -g -Wall -Wextra -Werror -Wpedantic