#include <string>
#include <cassert> // we shall do simple tests with assert
#include "chashmap.h"

int main() {
  chashmap<std::string, int> hashTable;
  
  hashTable.insert("hello, world!", 5);
  assert(*hashTable.get("hello, world!") == 5);
  assert(hashTable.get("not in hashtable!") == nullptr);

  assert(hashTable["hello, world!"] == 5);

  hashTable.insert("hello, world!", 10);
  assert(*hashTable.get("hello, world!") == 5);
  hashTable.insert_or_assign("hello, world!", 10);
  assert(*hashTable.get("hello, world!") == 10);
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
  assert(hashTable.get("neat") != nullptr);
  assert(hashTable["neat"] == 403);

  // remove "neat" from hashTable
  // marked it for lazy deletion
  hashTable.erase("neat");
  assert(hashTable.get("neat") == nullptr);

  // reinsert "neat"
  hashTable.insert("neat", 500);
  assert(hashTable.get("neat") != nullptr);
  assert(hashTable["neat"] == 500);
}