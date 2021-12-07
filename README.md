[![unit tests](https://github.com/KaceCottam/concurrenthashmap/actions/workflows/test.yml/badge.svg)](https://github.com/KaceCottam/concurrenthashmap/actions/workflows/test.yml)
# Concurrent Hash Map in C++20

A concurrent hash map in C++20 using std::async, constexpr and concepts.

Based on Java's `ConcurrentHashMap`.

## Dependencies

Requires `g++10` for building. If you would like to test, you also require `catch2`.

Build tests using `make test`, or an example app use `make main`

Run the simple tests using `./test` after building.

This is a single header include. Include `chashmap.h` to gain access to the library, make sure you compile with `-lpthread` or your compiler's equivalent.