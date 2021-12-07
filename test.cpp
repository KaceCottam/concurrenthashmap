#include <future>
#include <optional>
#include <string>
#include <concepts>
#include <iterator>
#include <thread>
#include <unistd.h>
#include <chrono>

#include "chashmap.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("concurrent hash map") {
  chashmap<std::string, int> hashTable;
  {
    auto p = hashTable.insert("hello, world!", 5);
    p.wait();
  }
  {
    auto p = hashTable.get("hello, world!");
    REQUIRE(hashTable["hello, world!"] == 5);
    p.wait();
    REQUIRE(*p.get() == 5);
  }
  {
    auto p = hashTable.get("not in hashtable!");
    p.wait();
    REQUIRE(p.get() == nullptr);
  }

  REQUIRE(hashTable["hello, world!"] == 5);

  { 
    auto p = hashTable.insert("hello, world!", 10);
    p.wait();
  }
  {
    auto p = hashTable.get("hello, world!");
    p.wait();
    REQUIRE(*p.get() == 5);
  }
  {
    auto p = hashTable.insert_or_assign("hello, world!", 10);
    p.wait();
  }
  {
    auto p = hashTable.get("hello, world!");
    p.wait();
    REQUIRE(*p.get() == 10);
  }
  REQUIRE(hashTable["hello, world!"] == 10);
  hashTable["now in hashtable!"] = 399;
  REQUIRE(hashTable["now in hashtable!"] == 399);
  hashTable["neat"] = 403;
  REQUIRE(hashTable["neat"] == 403);
  hashTable["n"] = 0;
  hashTable["na"] = 0;
  hashTable["nb"] = 0;
  hashTable["nc"] = 0;
  hashTable["nca"] = 0;
  hashTable["ncad"] = 0;
  hashTable["ncadx"] = 0;
  // resize
  hashTable["ncadxx"] = 0;
  hashTable["ncadxxx"] = 0;
  hashTable["ncadxxxx"] = 0;
  hashTable["ncadxxxxx"] = 0;
  hashTable["ncadxxxxxx"] = 0;
  hashTable["ncadxxxxxxx"] = 0;
  hashTable["ncadxxxxxxxx"] = 0;
  hashTable["ncadxxxxxxxxx"] = 0;

  // we know "neat" is inside the hashTable
  {
    auto p = hashTable.get("neat");
    p.wait();
    REQUIRE(p.get() != nullptr);
  }
  REQUIRE(hashTable["neat"] == 403);

  // remove "neat" from hashTable
  // marked it for lazy deletion
  {
    auto p = hashTable.erase("neat");
    p.wait();
    REQUIRE(p.get() == 1);
  }
  {
    auto p = hashTable.get("neat");
    p.wait();
    REQUIRE(p.get() == nullptr);
  }

  // reinsert "neat"
  {
    auto p = hashTable.insert("neat", 500);
    p.wait();
  }
  {
    auto p = hashTable.get("neat");
    p.wait();
    REQUIRE(p.get() != nullptr);
    REQUIRE(hashTable["neat"] == 500);
  }

  {
    auto p1 = hashTable.insert({ { "foo", 100 },
                                 { "bar", 10000 },
                                 { "foobar", 10000 } });
    p1.wait();
    // every now and then, some of these fails
    REQUIRE(hashTable["foobar"] == 10000); 
    REQUIRE(hashTable["bar"] == 10000);
    REQUIRE(hashTable["foo"] == 100);
  }
  {
    auto p = hashTable.find("foobar");
    p.wait();
    REQUIRE(p.get() != hashTable.end());
  }
  {
   const auto& chashTable = hashTable;
   auto p = chashTable.find("foobar");
   p.wait();
   REQUIRE(p.get() != hashTable.cend());
  }
  {
    auto p = hashTable.compute("foobar", [](auto value){ return value + 1; });
    p.wait();
    auto value = p.get();
    REQUIRE(value.has_value());
    REQUIRE(value.value() == 10001);
  }
  {
    hashTable.clear();
    REQUIRE(hashTable.size() == 0);
    auto p2 = hashTable.empty();
    p2.wait();
    REQUIRE(p2.get() == true);
  }
  {
    REQUIRE(hashTable.max_size() > 0);
  }
  {
    auto p = hashTable.insert({ {"hello", 1},
                                {"world", 2} });
    p.wait();
    REQUIRE(hashTable.size() == 2);
  }
  {
    auto p = hashTable.count("hello");
    p.wait();
    REQUIRE(p.get() == 1);
  }
  {
    auto newTable = hashTable;
    auto p = newTable.merge("hello", 5, [](const int& left, const int& right){ return left + right; });
    p.wait();
    REQUIRE(p.get() == 6);
    REQUIRE(newTable["hello"] == 6);
    auto otherTable = std::move(newTable);
    auto p1 = otherTable.merge("not in the table", 1, [](const int& left, const int& right){ return left + right; });
    p1.wait();
    REQUIRE(p1.get() == 1);
  }
  {
    auto p = hashTable.find("bobcat");
    p.wait();
    REQUIRE(p.get() == hashTable.end());
  }
}

// TEST_CASE("mass insertions") {
//   auto ht2 = chashmap<int, int, (int)1e6>(10000);
//   for (int i = 0; i < 10000; ++i) {
//     ht2.insert(i, i);
//   }

//   using namespace std::chrono_literals;

//   std::this_thread::sleep_for(1s);
//   REQUIRE(ht2.size() == 10000);
//   for (int i = 0; i < 10000; ++i) {
//     REQUIRE(ht2[i] == i);
//   }
// }