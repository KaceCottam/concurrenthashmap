#include <string>
#include <cassert> // we shall do simple tests with assert
#include <concepts>
#include <iterator>
#include "chashmap.h"

int main() {
  chashmap<std::string, int> hashTable;
  {
    auto p = hashTable.insert("hello, world!", 5);
    p.wait();
  }
  {
    auto p = hashTable.get("hello, world!");
    assert(hashTable["hello, world!"] == 5);
    p.wait();
    assert(*p.get() == 5);
  }
  {
    auto p = hashTable.get("not in hashtable!");
    p.wait();
    assert(p.get() == nullptr);
  }

  assert(hashTable["hello, world!"] == 5);

  { 
    auto p = hashTable.insert("hello, world!", 10);
    p.wait();
  }
  {
    auto p = hashTable.get("hello, world!");
    p.wait();
    assert(*p.get() == 5);
  }
  {
    auto p = hashTable.insert_or_assign("hello, world!", 10);
    p.wait();
  }
  {
    auto p = hashTable.get("hello, world!");
    p.wait();
    assert(*p.get() == 10);
  }
  assert(hashTable["hello, world!"] == 10);
  hashTable["now in hashtable!"] = 399;
  assert(hashTable["now in hashtable!"] == 399);
  hashTable["neat"] = 403;
  assert(hashTable["neat"] == 403);
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
    assert(p.get() != nullptr);
  }
  assert(hashTable["neat"] == 403);

  // remove "neat" from hashTable
  // marked it for lazy deletion
  {
    auto p = hashTable.erase("neat");
    p.wait();
    assert(p.get() == 1);
  }
  {
    auto p = hashTable.get("neat");
    p.wait();
    assert(p.get() == nullptr);
  }

  // reinsert "neat"
  {
    auto p = hashTable.insert("neat", 500);
    p.wait();
  }
  {
    auto p = hashTable.get("neat");
    p.wait();
    assert(p.get() != nullptr);
    assert(hashTable["neat"] == 500);
  }

  {
    auto p1 = hashTable.insert("foo", 100);
    auto p2 = hashTable.insert("bar", 10000);
    auto p3 = hashTable.insert("foobar", 10000);
    p3.wait();
    assert(hashTable["foobar"] == 10000);
    p1.wait();
    p2.wait();
  }
  {
    auto p = hashTable.find("foobar");
    p.wait();
    assert(p.get() != hashTable.end());
  }
  {
   const auto& chashTable = hashTable;
   auto p = chashTable.find("foobar");
   p.wait();
   assert(p.get() != hashTable.cend());
  }
  {
    auto p = hashTable.compute("foobar", [](auto value){ return value + 1; });
    p.wait();
    auto value = p.get();
    assert(value.has_value());
    assert(value.value() == 10001);
  }
}