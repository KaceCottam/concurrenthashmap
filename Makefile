CXX ?= g++

test: test.cpp chashmap.h
	$(CXX) $< -o $@ --std=c++20 -Wall -Wextra -Werror -Wpedantic -lpthread -O3
	
main: main.cpp chashmap.h
	$(CXX) $< -o $@ --std=c++20 -Wall -Wextra -Werror -Wpedantic -lpthread -O3

coverage: test.cpp chashmap.h
	test -d $@ || mkdir -v $@
	$(CXX) $< -o $@/test-cov --std=c++20 -g -Wall -Wextra -Werror -Wpedantic -lpthread --coverage
	mv test.gcno $@/test.gcno
	cd $@ && ./test-cov && mv ../test.gcda ./test.gcda
	cd $@ && lcov --directory . --capture --output-file coverage.lcov
	cd $@ && genhtml coverage.lcov && firefox index.html

.PHONY: clean
clean:
	test -f main && rm main || true
	test -f test && rm test || true