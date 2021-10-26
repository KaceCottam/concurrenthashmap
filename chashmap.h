#ifndef CHASHMAP_H
#define CHASHMAP_H
#include <concepts>
#include <initializer_list>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <iostream>
#include <iterator>
#include <vector>
#include <memory>

template<class T>
concept Hashable = requires {
    std::hash<T>();
};

template<class T>
concept DefaultConstructible = requires {
    T();
};

template<Hashable Key, class T>
class chashmap;
template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator+(const typename chashmap<Key, T>::iterator& lhs, typename chashmap<Key, T>::difference_type n);
template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator+(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::iterator& lhs);
template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator-(const typename chashmap<Key, T>::iterator& lhs, typename chashmap<Key, T>::difference_type n);
template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator-(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::iterator& lhs);
template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator+(const typename chashmap<Key, T>::const_iterator& lhs, typename chashmap<Key, T>::difference_type n);
template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator+(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::const_iterator& lhs);
template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator-(const typename chashmap<Key, T>::const_iterator& lhs, typename chashmap<Key, T>::difference_type n);
template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator-(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::const_iterator& lhs);

template<Hashable Key, class T>
class chashmap {
public:
    using key_type = Key;
    using value_type = std::pair<const Key, T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
private:
    using bucket_content = std::pair<bool, value_type>;
    using bucket = std::shared_ptr<bucket_content>;
    std::vector<bucket> buckets;
    int inserted_values = 0;
public:
    class iterator {
        friend class chashmap;
    private:
        typename std::vector<bucket>::iterator current;
    public:
        iterator();
        iterator(const typename std::vector<bucket>::iterator& current);
        iterator(typename std::vector<bucket>::iterator&& current);
        iterator(const iterator& copy);
        iterator(iterator&& move);
        iterator& operator++();
        iterator operator++(int);
        iterator& operator+=(difference_type index);
        friend iterator operator+ <Key, T> (const iterator& lhs, const difference_type n);
        friend iterator operator+ <Key, T> (const difference_type n, const iterator& lhs);
        iterator& operator--();
        iterator operator--(int);
        iterator& operator-=(difference_type index);
        friend iterator operator- <Key, T> (const iterator& lhs, const difference_type n);
        friend iterator operator- <Key, T> (const difference_type n, const iterator& lhs);
        value_type& operator*() const;
        value_type* operator->() const;
        friend auto operator<=> (const iterator& lhs, const iterator& rhs) = default;
    };

    class const_iterator {
        friend class chashmap;
    private:
        typename std::vector<bucket>::const_iterator current;
    public:
        const_iterator();
        const_iterator(const typename std::vector<bucket>::const_iterator& current);
        const_iterator(typename std::vector<bucket>::const_iterator&& current);
        const_iterator(const const_iterator& copy);
        const_iterator(const_iterator&& move);
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator+=(difference_type index);
        friend const_iterator operator+ <Key, T> (const const_iterator& lhs, const difference_type n);
        friend const_iterator operator+ <Key, T> (const difference_type n, const const_iterator& lhs);
        const_iterator& operator--();
        const_iterator operator--(int);
        const_iterator& operator-=(difference_type index);
        friend const_iterator operator- <Key, T> (const const_iterator& lhs, const difference_type n);
        friend const_iterator operator- <Key, T> (const difference_type n, const const_iterator& lhs);
        const value_type& operator*() const;
        const value_type* operator->() const;
        friend auto operator<=> (const const_iterator& lhs, const const_iterator& rhs) = default;
    };

    chashmap(const size_type initial_capacity = 16);
    chashmap(const chashmap<Key, T>& copy);
    chashmap(chashmap<Key, T>&& move);
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    bool empty() const;
    size_type size() const;
    size_type max_size() const;
    void clear();
    std::pair<iterator, bool> insert(const Key& key, const T& value);
    std::pair<iterator, bool> insert(const value_type value);
    void insert(std::initializer_list<value_type> values);
    std::pair<iterator, bool> insert_or_assign(const Key& key, const T& value);
    void erase(iterator pos);
    size_type erase(const Key& key);
    size_type count(const Key& key);
    iterator find(const Key& key);
    const_iterator find(const Key& key) const;
    bool contains(const Key& key) const;
    T* get(const Key& key);
    T& operator[](const Key& key);
private:
    std::pair<iterator, bool> create(const Key& key, const T& value);
};

template<Hashable Key, class T>
chashmap<Key, T>::chashmap(const typename chashmap<Key, T>::size_type initial_capacity)
: buckets{initial_capacity}
{
    if (initial_capacity <= 0) {
        throw std::runtime_error("initial capacity needs to be non-negative");
    }
}

template<Hashable Key, class T>
chashmap<Key, T>::chashmap(const chashmap<Key, T>& copy)
: buckets{copy.buckets}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::chashmap(chashmap<Key, T>&& copy)
: buckets{std::move(copy.buckets)}
{
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator chashmap<Key, T>::begin() {
    return iterator(buckets.begin());
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator chashmap<Key, T>::begin() const {
    return cbegin();
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator chashmap<Key, T>::cbegin() const {
    return const_iterator(buckets.begin());
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator chashmap<Key, T>::end() {
    return iterator(buckets.end());
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator chashmap<Key, T>::end() const {
    return cend();
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator chashmap<Key, T>::cend() const {
    return const_iterator(buckets.end());
}

template<Hashable Key, class T>
bool chashmap<Key, T>::empty() const {
    for (bucket value : buckets) {
        if (value && std::get<0>(*value)) return true;
    }
    return false;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::size_type chashmap<Key, T>::size() const {
    int sum = 0;
    for (bucket value : buckets) {
        if (value && std::get<0>(*value)) ++sum;
    }
    return sum;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::size_type chashmap<Key, T>::max_size() const {
    return std::numeric_limits<size_type>::max();
}

template<Hashable Key, class T>
void chashmap<Key, T>::clear() {
    buckets.clear();
    inserted_values = 0;
}

template<Hashable Key, class T>
std::pair<typename chashmap<Key, T>::iterator, bool> chashmap<Key, T>::create(const Key& key, const T& value)
{
    int buckets_size = buckets.size();
    size_type hash = std::hash<Key>()(key);
    for (size_type i = 0; /*infinite loop*/; i++) {
      size_type idx = (hash + i) % buckets_size;
      if (buckets[idx] == nullptr) {
        // nothing at this position
        // create a new thing
        buckets[idx] = std::make_shared<bucket_content>( false, std::make_pair( key, value ) );
        ++inserted_values;
        return { iterator(buckets.begin()+idx), true };
      }
      auto [ is_removed, kvp ] = *buckets[idx];
      auto [ tkey, tvalue ] = kvp;
      if (is_removed) {
        // if marked for lazy deletion
        buckets[idx] = std::make_shared<bucket_content>( false, std::make_pair( key, value ) );
        ++inserted_values;
        return { iterator(buckets.begin()+idx), true };
      } else if (tkey == key) {
        // if key is already represented
        // no insertion, and no need to resize
        return { iterator(buckets.begin()+idx), false };
      }
      // continue in our linear probing
    }
}


template<Hashable Key, class T>
std::pair<typename chashmap<Key, T>::iterator, bool> chashmap<Key, T>::insert(const Key& key, const T& value) {
    
    int buckets_size = buckets.size();
    // we want to resize our pairs vector when we find that the number of
    // inserted elements is 2/3 of our max capacity
    // (to prevent collisions)
    const float threshold = 2.0 / 3.0;  // 1/3 of the pockets are empty
    const float ratio = (float)inserted_values / (buckets_size + 1);
    if (ratio >= threshold) {
        // we want to resize our pairs vector
        // we will need to rehash our values
        std::vector<bucket> oldbuckets = std::move(buckets);
        buckets = std::vector<bucket>(oldbuckets.size() * 2 + 1);
        inserted_values = 0;
        for (auto bucket : oldbuckets) if (bucket != nullptr) {  // O(N)
            auto [ is_removed, kvp ] = *bucket;
            if (is_removed) continue; // we can ignore deleted values since we are now resetting the hashTable
            auto [ tkey, tvalue ] = kvp;
            // recursive call ALWAYS O(1) due to resize
            // threshold was *barely* met, so since we doubled the length of pairs,
            // the inserted_pairs will always be under the threshold.
            create(tkey, tvalue); 
        }
    }
    return create(key, value);
}

template<Hashable Key, class T>
std::pair<typename chashmap<Key, T>::iterator, bool> chashmap<Key, T>::insert(const typename chashmap<Key, T>::value_type value) {
    return insert(value.first, value.second);
}

template<Hashable Key, class T>
void chashmap<Key, T>::insert(std::initializer_list<typename chashmap<Key, T>::value_type> values)
{
    for (auto [key, value] : values) {
        insert(key, value);
    }
}

template<Hashable Key, class T>
std::pair<typename chashmap<Key, T>::iterator, bool> chashmap<Key, T>::insert_or_assign(const Key& key, const T& value)
{
    auto [ iter, inserted ] = insert(key, value);
    if (!inserted) iter->second = value;
    return {iter, true };
}

template<Hashable Key, class T>
void chashmap<Key, T>::erase(typename chashmap<Key, T>::iterator pos)
{
    (*pos.current)->first = true;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::size_type chashmap<Key, T>::erase(const Key& key)
{
    auto iter = find(key);
    if (iter != end()) {
        erase(iter);
        return 1;
    }
    return 0;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::size_type chashmap<Key, T>::count(const Key& key)
{
    auto iter = find(key);
    return iter == end() ? 0 : 1;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator chashmap<Key, T>::find(const Key& key)
{
    for (auto iter = begin(); iter != end(); ++iter) {
        auto [tkey, _] = *iter;
        if (tkey == key) return iter;
    }
    return end();
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator chashmap<Key, T>::find(const Key& key) const
{
    for (auto iter = cbegin(); iter != cend(); ++iter) {
        auto [tkey, _] = *iter;
        if (tkey == key) return iter;
    }
    return cend();
}

template<Hashable Key, class T>
bool chashmap<Key, T>::contains(const Key& key) const
{
    return count(key) == 1;
}

template<Hashable Key, class T>
T* chashmap<Key, T>::get(const Key& key) {
    int buckets_size = buckets.size();
    size_type hash = std::hash<Key>()(key);
    std::pair<iterator, bool> result;
    for (size_type i = 0; /*infinite loop*/; i++) {
      size_type idx = (hash + i) % buckets_size;
      if (buckets[idx] == nullptr) {
        // nothing at this position
        return nullptr;
      }
      auto [ is_removed, kvp ] = *buckets[idx];
      auto [ tkey, tvalue ] = kvp;
      if (is_removed) {
        // if marked for lazy deletion
        continue; // continue the search
      } else if (tkey == key) {
        // if key is found
        return &tvalue;
      }
      // continue in our linear probing
    }
}

template<Hashable Key, class T>
T& chashmap<Key, T>::operator[](const Key& key) {
    // it will return the reference to the key's
    // value if it exists,
    // if it does not exist, it will create a
    // new KeyValuePair and return a reference
    // to that value.

    auto value = get(key);
    if (value != nullptr) {
        return *value;
    }
    auto [ iterator, inserted ] = this->insert(std::make_pair( key, T{} ));  // call default constructor for value
    return std::get<1>(*iterator);
}

template<Hashable Key, class T>
chashmap<Key, T>::iterator::iterator()
: current{}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::iterator::iterator(const typename std::vector<bucket>::iterator& current)
: current{current}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::iterator::iterator(typename std::vector<bucket>::iterator&& current)
: current{std::move(current)}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::iterator::iterator(const typename chashmap<Key, T>::iterator& copy)
: current{copy.current}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::iterator::iterator(typename chashmap<Key, T>::iterator&& move)
: current{std::move(move.current)}
{
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator++() {
    while (current != typename std::vector<bucket>::iterator()  // not at the end and
       && (!*current || std::get<0>(**current))) // nonexisting or removed node
       ++current;
    return *this;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::iterator chashmap<Key, T>::iterator::operator++(int) {
    iterator result = *this;
    ++(*this);
    return result;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator+=(typename chashmap<Key, T>::difference_type index) {
    difference_type aindex = abs(index);
    for (difference_type i = 0; i < aindex; ++i) {
        if (index > 0) ++(*this);
        else           --(*this);
    }
    return *this;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator+(const typename chashmap<Key, T>::iterator& lhs, typename chashmap<Key, T>::difference_type n) {
    typename chashmap<Key, T>::iterator i = lhs;
    lhs += n;
    return lhs;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator+(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::iterator& lhs) {
    return lhs + n;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator--() {
    while (current != std::vector<bucket>::iterator()  // not at the end and
       && (!*current || std::get<0>(**current))) // nonexisting or removed node
       --current;
    return *this;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::iterator chashmap<Key, T>::iterator::operator--(int) {
    iterator result = *this;
    --(*this);
    return result;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator-=(typename chashmap<Key, T>::difference_type index) {
    difference_type aindex = abs(index);
    for (difference_type i = 0; i < aindex; ++i) {
        if (index > 0) --(*this);
        else           ++(*this);
    }
    return *this;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator-(const typename chashmap<Key, T>::iterator& lhs, typename chashmap<Key, T>::difference_type n) {
    typename chashmap<Key, T>::iterator i = lhs;
    lhs -= n;
    return lhs;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::iterator operator-(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::iterator& lhs) {
    return lhs - n;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::value_type& chashmap<Key, T>::iterator::operator*() const {
    return (*current)->second;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::value_type* chashmap<Key, T>::iterator::operator->() const {
    return &(*current)->second;
}

template<Hashable Key, class T>
chashmap<Key, T>::const_iterator::const_iterator()
: current{}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::const_iterator::const_iterator(const typename std::vector<bucket>::const_iterator& copy)
: current{copy.current}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::const_iterator::const_iterator(typename std::vector<bucket>::const_iterator&& move)
: current{std::move(move.current)}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::const_iterator::const_iterator(const typename chashmap<Key, T>::const_iterator& copy)
: current{copy.current}
{
}

template<Hashable Key, class T>
chashmap<Key, T>::const_iterator::const_iterator(typename chashmap<Key, T>::const_iterator&& move)
: current{std::move(move.current)}
{
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator++() {
    while (current != std::vector<bucket>::iterator()  // not at the end and
       && (!*current || std::get<0>(**current))) // nonexisting or removed node
       ++current;
    return *this;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::const_iterator chashmap<Key, T>::const_iterator::operator++(int) {
    const_iterator result = *this;
    ++(*this);
    return result;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator+=(typename chashmap<Key, T>::difference_type index) {
    difference_type aindex = abs(index);
    for (difference_type i = 0; i < aindex; ++i) {
        if (index > 0) ++(*this);
        else           --(*this);
    }
    return *this;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator+(const typename chashmap<Key, T>::const_iterator& lhs, typename chashmap<Key, T>::difference_type n) {
    typename chashmap<Key, T>::const_iterator i = lhs;
    lhs += n;
    return lhs;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator+(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::const_iterator& lhs) {
    return lhs + n;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator--() {
    while (current != std::vector<bucket>::iterator()  // not at the end and
       && (!*current || std::get<0>(**current))) // nonexisting or removed node
       --current;
    return *this;
}

template<Hashable Key, class T> 
typename chashmap<Key, T>::const_iterator chashmap<Key, T>::const_iterator::operator--(int) {
    const_iterator result = *this;
    --(*this);
    return result;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator-=(typename chashmap<Key, T>::difference_type index) {
    difference_type aindex = abs(index);
    for (difference_type i = 0; i < aindex; ++i) {
        if (index > 0) --(*this);
        else           ++(*this);
    }
    return *this;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator-(const typename chashmap<Key, T>::const_iterator& lhs, typename chashmap<Key, T>::difference_type n) {
    typename chashmap<Key, T>::const_iterator i = lhs;
    lhs -= n;
    return lhs;
}

template<Hashable Key, class T>
typename chashmap<Key, T>::const_iterator operator-(typename chashmap<Key, T>::difference_type n, const typename chashmap<Key, T>::const_iterator& lhs) {
    return lhs - n;
}

template<Hashable Key, class T> 
const typename chashmap<Key, T>::value_type& chashmap<Key, T>::const_iterator::operator*() const {
    return **current;
}

template<Hashable Key, class T> 
const typename chashmap<Key, T>::value_type* chashmap<Key, T>::const_iterator::operator->() const {
    return *current;
}

#endif