#include "chashmap.h"

int main() {
    chashmap<int, int> hashmap;
    hashmap.insert({1, 0}).wait();
    return hashmap[1];
}