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
#include <future>
#include <iterator>
#include <compare>

template<class Key>
concept Hashable = requires (Key k) { std::hash<Key>()(k); };

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
    private:
        friend class chashmap;
        typename std::vector<bucket>::iterator current;
        size_type begin;
        size_type at;
        size_type end;
    public:
        constexpr iterator() = default;
        constexpr iterator(const iterator&) = default;
        constexpr iterator(iterator&&) = default;
        constexpr explicit iterator(typename std::vector<bucket>::iterator c, const size_type at, const size_type n)
            : current{c}, begin{0}, at{at}, end{n} {}
        constexpr auto operator<=>(const iterator&) const = default;
        constexpr bool operator==(const iterator&) const;
        constexpr value_type& operator*();
        constexpr value_type* operator->();
        constexpr value_type& operator[](const difference_type);
        constexpr iterator& operator++();
        constexpr iterator operator++(int);
        constexpr iterator& operator+=(const difference_type);
        constexpr iterator operator+(const difference_type) const;
        constexpr iterator& operator--();
        constexpr iterator operator--(int);
        constexpr iterator& operator-=(const difference_type);
        constexpr iterator operator-(const difference_type) const;
    };

    class const_iterator {
    private:
        friend class chashmap;
        typename std::vector<bucket>::const_iterator current;
        size_type begin;
        size_type at;
        size_type end;
    public:
        constexpr const_iterator() = default;
        constexpr const_iterator(const const_iterator&) = default;
        constexpr const_iterator(const_iterator&&) = default;
        constexpr explicit const_iterator(typename std::vector<bucket>::const_iterator c, const size_type at, const size_type n)
            : current{c}, at{at}, end{n} {}
        constexpr auto operator<=>(const const_iterator&) const = default;
        constexpr bool operator==(const const_iterator&) const;
        constexpr const value_type& operator*();
        constexpr const value_type* operator->();
        constexpr const value_type& operator[](difference_type);
        constexpr const_iterator& operator++();
        constexpr const_iterator operator++(int);
        constexpr const_iterator operator+(const difference_type) const;
        constexpr const_iterator& operator+=(difference_type);
        constexpr const_iterator& operator--();
        constexpr const_iterator operator--(int);
        constexpr const_iterator& operator-=(difference_type);
        constexpr const_iterator operator-(const difference_type) const;
    };

    constexpr chashmap(const size_type initial_capacity = 16);
    constexpr chashmap(const chashmap<Key, T>& copy);
    constexpr chashmap(chashmap<Key, T>&& move);
    constexpr iterator begin();
    constexpr const_iterator begin() const;
    constexpr const_iterator cbegin() const;
    constexpr iterator end();
    constexpr const_iterator end() const;
    constexpr const_iterator cend() const;
    std::future<bool> empty() const;
    std::future<size_type> size() const;
    constexpr size_type max_size() const;
    std::future<void> clear();
    std::future<std::pair<iterator, bool>> insert(Key key, T value);
    std::future<std::pair<iterator, bool>> insert(value_type value);
    std::future<void> insert(std::initializer_list<value_type> values);
    std::future<std::pair<iterator, bool>> insert_or_assign(Key key, T value);
    constexpr void erase(iterator pos);
    std::future<size_type> erase(Key key);
    std::future<size_type> count(Key key) const;
    std::future<iterator> find(Key key);
    std::future<const_iterator> find(Key key) const;
    std::future<bool> contains(Key key) const;
    std::future<T*> get(Key key);
    constexpr T& operator[](const Key& key);
    std::future<size_type> erase_if(std::predicate<const Key&, const T&> auto fn);
    std::future<size_type> erase_if(std::predicate<const Key&> auto fn);
    std::future<size_type> count_if(std::predicate<const Key&, const T&> auto fn) const;
    std::future<size_type> count_if(std::predicate<const Key&> auto fn) const;
    std::future<iterator> find_if(std::predicate<const Key&, const T&> auto fn);
    std::future<iterator> find_if(std::predicate<const Key&> auto fn);
    std::future<const_iterator> find_if(std::predicate<const Key&, const T&> auto fn) const;
    std::future<const_iterator> find_if(std::predicate<const Key&> auto fn) const;
    std::future<bool> contains(std::predicate<const Key&, const T&> auto fn) const;
    std::future<bool> contains(std::predicate<const T&> auto fn) const;
    std::future<std::optional<T>> compute(Key key, std::invocable<const Key&, const T&> auto fn) const;
    std::future<std::optional<T>> compute(Key key, std::invocable<const T&> auto fn) const;
    constexpr chashmap<Key, T>& operator=(const chashmap<Key, T>& copy) = default;
    constexpr chashmap<Key, T>& operator=(chashmap<Key, T>&& move) = default;
private:
    std::future<std::pair<iterator, bool>> create(Key key, T value);
};

template<Hashable Key, class T>
constexpr chashmap<Key, T>::chashmap(const typename chashmap<Key, T>::size_type initial_capacity)
: buckets{initial_capacity}
{
    if (initial_capacity <= 0) {
        throw std::runtime_error("initial capacity needs to be non-negative");
    }
}

template<Hashable Key, class T>
constexpr chashmap<Key, T>::chashmap(const chashmap<Key, T>& copy)
: buckets{copy.buckets}
{
}

template<Hashable Key, class T>
constexpr chashmap<Key, T>::chashmap(chashmap<Key, T>&& copy)
: buckets{std::move(copy.buckets)}
{
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator chashmap<Key, T>::begin() {
    return iterator(buckets.begin(), 0, buckets.size());
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::begin() const {
    return cbegin();
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::cbegin() const {
    return const_iterator(buckets.cbegin(), 0, buckets.size());
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator chashmap<Key, T>::end() {
    return iterator(buckets.end(), 0, buckets.size());
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::end() const {
    return cend();
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::cend() const {
    return const_iterator(buckets.cend(), 0, buckets.size());
}

template<Hashable Key, class T>
std::future<bool> chashmap<Key, T>::empty() const {
    return std::async(std::launch::async, [&]{
        for (bucket value : buckets) {
            if (value && value->first) return true;
        }
        return false;
    });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::size_type> chashmap<Key, T>::size() const {
    return std::async(std::launch::async, [&]{
        int sum = 0;
        for (bucket value : buckets) {
            if (value && value->first) ++sum;
        }
        return sum;
    });
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::size_type chashmap<Key, T>::max_size() const {
    return std::numeric_limits<size_type>::max();
}

template<Hashable Key, class T>
std::future<void> chashmap<Key, T>::clear() {
    auto future = buckets.clear();
    inserted_values = 0;
    return future;
}

template<Hashable Key, class T>
std::future<std::pair<typename chashmap<Key, T>::iterator, bool>> chashmap<Key, T>::create(Key key, T value)
{
    return std::async(std::launch::async, [&, key=std::move(key), value=std::move(value)]{
        int buckets_size = buckets.size();
        size_type hash = std::hash<Key>()(key);
        for (size_type i = 0; /*infinite loop*/; i++) {
            size_type idx = (hash + i) % buckets_size;
            if (buckets[idx] == nullptr) {
                // nothing at this position
                // create a new thing
                buckets[idx] = std::make_shared<bucket_content>( false, std::make_pair( key, value ) );
                ++inserted_values;
                return std::make_pair( iterator(buckets.begin() + idx, idx, buckets.size()), true );
            }
            auto [ is_removed, kvp ] = *buckets[idx];
            auto [ tkey, tvalue ] = kvp;
            if (is_removed) {
                // if marked for lazy deletion
                buckets[idx] = std::make_shared<bucket_content>( false, std::make_pair( key, value ) );
                ++inserted_values;
                return std::make_pair( iterator(buckets.begin() + idx, idx, buckets.size()), true );
            } else if (tkey == key) {
                // if key is already represented
                // no insertion, and no need to resize
                return std::make_pair( iterator(buckets.begin() + idx, idx, buckets.size()), false );
            }
            // continue in our linear probing
        }
    });
}


template<Hashable Key, class T>
std::future<std::pair<typename chashmap<Key, T>::iterator, bool>> chashmap<Key, T>::insert(Key key, T value) {
    return std::async(std::launch::async, [&, key=std::move(key), value=std::move(value)]{
        int buckets_size = buckets.size();
        // we want to resize our pairs vector when we find that the number of
        // inserted elements is 2/3 of our max capacity
        // (to prevent collisions)
        const float threshold = 3.0 / 4.0;  // 1/4 of the pockets are empty
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
        auto future = create(key, value);
        future.wait();
        return future.get();
    });
}

template<Hashable Key, class T>
std::future<std::pair<typename chashmap<Key, T>::iterator, bool>> chashmap<Key, T>::insert(const typename chashmap<Key, T>::value_type value) {
    return insert(value.first, value.second);
}

template<Hashable Key, class T>
std::future<void> chashmap<Key, T>::insert(std::initializer_list<typename chashmap<Key, T>::value_type> values)
{
    return std::async(std::launch::async, [&, values=std::move(values)]{
        std::vector<std::future<std::pair<iterator, bool>>> pool;
        for (auto [key, value] : values) {
            pool.emplace_back(insert(key, value));
        }
        for (auto future : pool) {
            future.wait();
        }
    });
}

template<Hashable Key, class T>
std::future<std::pair<typename chashmap<Key, T>::iterator, bool>> chashmap<Key, T>::insert_or_assign(Key key, T value)
{
    return std::async(std::launch::async, [&, key=std::move(key), value=std::move(value)]{
        auto promise = insert(key, value);
        promise.wait();
        auto [ iter, inserted ] = promise.get();
        if (!inserted) iter->second = value;
        return std::make_pair( iter, true );
    });
}

template<Hashable Key, class T>
constexpr void chashmap<Key, T>::erase(typename chashmap<Key, T>::iterator pos)
{
    (**pos.current).first = true;
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::size_type> chashmap<Key, T>::erase(Key key)
{
    return erase_if([&, key=std::move(key)](const auto& tkey) { return tkey == key; });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::size_type> chashmap<Key, T>::count(Key key) const
{
    return count_if([&, key=std::move(key)](const auto& tkey) { return tkey == key; });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::iterator> chashmap<Key, T>::find(Key key)
{
    return find_if([&, key=std::move(key)](const auto& tkey) { return tkey == key; });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::const_iterator> chashmap<Key, T>::find(Key key) const
{
    return find_if([&, key=std::move(key)](const auto& tkey) { return tkey == key; });
}

template<Hashable Key, class T>
std::future<bool> chashmap<Key, T>::contains(Key key) const
{
    return contains_if([&, key=std::move(key)](const auto& tkey) { return tkey == key; });
}

template<Hashable Key, class T>
std::future<T*> chashmap<Key, T>::get(Key key) {
    return std::async(std::launch::async, [&, key=std::move(key)]{
        int buckets_size = buckets.size();
        size_type hash = std::hash<Key>()(key);
        std::pair<iterator, bool> result;
        for (size_type i = 0; /*infinite loop*/; i++) {
            size_type idx = (hash + i) % buckets_size;
            if (buckets[idx] == nullptr) {
                // nothing at this position
                return (T*)nullptr;
            }
            auto& [ is_removed, kvp ] = *buckets[idx];
            auto& [ tkey, tvalue ] = kvp;
            if (is_removed) {
                // if marked for lazy deletion
                continue; // continue the search
            } else if (tkey == key) {
                // if key is found
                return &tvalue;
            }
            // continue in our linear probing
        }
    });
}

template<Hashable Key, class T>
constexpr T& chashmap<Key, T>::operator[](const Key& key) {
    // it will return the reference to the key's
    // value if it exists,
    // if it does not exist, it will create a
    // new KeyValuePair and return a reference
    // to that value.

    auto promise = get(key);
    promise.wait();
    auto value = promise.get();
    if (value != nullptr) {
        return *value;
    }
    auto promise2 = this->insert(std::make_pair( key, T{} ));  // call default constructor for value
    promise2.wait();
    auto [ iterator, inserted ] = promise2.get();
    return iterator->second;
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::size_type> chashmap<Key, T>::erase_if(std::predicate<const Key&, const T&> auto fn)
{
    return std::async(std::launch::async, [&, fn=std::move(fn)]{
        size_type count = 0;
        for (auto iter = begin(); iter != end(); iter++) {
            if (fn(iter->first, iter->second)) {
                (*iter.current)->first = true;
                count++;
            }
        }
        return count;
    });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::size_type> chashmap<Key, T>::erase_if(std::predicate<const Key&> auto fn)
{
    return erase_if([&, fn=std::move(fn)](Key k, T) { return fn(k); });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::size_type> chashmap<Key, T>::count_if(std::predicate<const Key&, const T&> auto fn) const
{
    return std::async(std::launch::async, [&, fn=std::move(fn)]{
        size_type count = 0;
        for (auto iter = cbegin(); iter != cend(); iter++) {
            if (fn(iter->first, iter->second)) {
                count++;
            }
        }
        return count;
    });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::size_type> chashmap<Key, T>::count_if(std::predicate<const Key&> auto fn) const
{
    return count_if([&, fn=std::move(fn)](Key k, T) { return fn(k); });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::iterator> chashmap<Key, T>::find_if(std::predicate<const Key&, const T&> auto fn)
{
    return std::async(std::launch::async, [&, fn=std::move(fn)]{
        for (auto iter = begin(); iter != end(); iter++) {
            if (fn(iter->first, iter->second)) {
                return iter;
            }
        }
        return end();
    });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::iterator> chashmap<Key, T>::find_if(std::predicate<const Key&> auto fn)
{
    return find_if([&, fn=std::move(fn)](Key k, T) { return fn(k); });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::const_iterator> chashmap<Key, T>::find_if(std::predicate<const Key&, const T&> auto fn) const
{
    return std::async(std::launch::async, [&, fn=std::move(fn)]{
        for (auto iter = cbegin(); iter != cend(); iter++) {
            if (fn(iter->first, iter->second)) {
                return iter;
            }
        }
        return cend();
    });
}

template<Hashable Key, class T>
std::future<typename chashmap<Key, T>::const_iterator> chashmap<Key, T>::find_if(std::predicate<const Key&> auto fn) const
{
    return find_if([&, fn=std::move(fn)](Key k, T) { return fn(k); });
}

template<Hashable Key, class T>
std::future<bool> chashmap<Key, T>::contains(std::predicate<const Key&, const T&> auto fn) const
{
    return std::async(std::launch::async, [&, fn=std::move(fn)]{
        auto p = find_if(fn);
        p.wait();
        return p.get() != cend();
    });
}

template<Hashable Key, class T>
std::future<bool> chashmap<Key, T>::contains(std::predicate<const T&> auto fn) const
{
    return contains([&, fn=std::move(fn)](Key, T k) { return fn(k); });
}

template<Hashable Key, class T>
std::future<std::optional<T>> chashmap<Key, T>::compute(Key key, std::invocable<const Key&, const T&> auto fn) const {
    return std::async(std::launch::async, [&, key=std::move(key), fn=std::move(fn)]{
        auto p = find(key);
        p.wait();
        const_iterator valptr = p.get();
        if (valptr == cend()) return std::optional<T>{};
        auto [tkey, tvalue] = *valptr;
        return std::make_optional(fn(tkey, tvalue));
    });
}


template<Hashable Key, class T>
std::future<std::optional<T>> chashmap<Key, T>::compute(Key key, std::invocable<const T&> auto fn) const {
    return compute(key, [&, key=std::move(key), fn=std::move(fn)](Key, T t) { return fn(t); });
}

template<Hashable Key, class T>
constexpr bool chashmap<Key, T>::iterator::operator==(const typename chashmap<Key, T>::iterator& it) const {
    return current == it.current;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::value_type& chashmap<Key, T>::iterator::operator*()
{
    return (*current)->second;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::value_type* chashmap<Key, T>::iterator::operator->()
{
    return &(*current)->second;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::value_type& chashmap<Key, T>::iterator::operator[](difference_type index)
{
    return current[index];
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator++()
{
    if (at == end) return *this;
    do { ++current; ++at; } while (at != end && (*current == nullptr || (*current)->first == true));
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator chashmap<Key, T>::iterator::operator++(int)
{
    auto res = *this;
    ++*this;
    return res;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator+=(const difference_type n)
{
    if (n < 0) return (*this -= -n);
    for (difference_type i = 0; i < n; ++i) {
        ++*this;
    }
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator chashmap<Key, T>::iterator::operator+(const difference_type n) const
{
    auto res = *this;
    res += n;
    return res;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator--()
{
    if (at == begin) return *this;
    do { --current; --at; } while (at != begin && (*current == nullptr || (*current)->first == true));
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator chashmap<Key, T>::iterator::operator--(int)
{
    auto res = *this;
    --*this;
    return res;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator& chashmap<Key, T>::iterator::operator-=(const difference_type n)
{
    if (n < 0) return (*this += -n);
    for (difference_type i = 0; i < n; ++i) {
        --*this;
    }
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::iterator chashmap<Key, T>::iterator::operator-(const difference_type n) const
{
    auto res = *this;
    res += n;
    return res;
}

template<Hashable Key, class T>
constexpr bool chashmap<Key, T>::const_iterator::operator==(const typename chashmap<Key, T>::const_iterator& it) const {
    return current == it.current;
}

template<Hashable Key, class T>
constexpr const typename chashmap<Key, T>::value_type& chashmap<Key, T>::const_iterator::operator*()
{
    return (*current)->second;
}

template<Hashable Key, class T>
constexpr const typename chashmap<Key, T>::value_type* chashmap<Key, T>::const_iterator::operator->()
{
    return &(*current)->second;
}

template<Hashable Key, class T>
constexpr const typename chashmap<Key, T>::value_type& chashmap<Key, T>::const_iterator::operator[](difference_type index)
{
    return current[index];
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator++()
{
    if (at == end) return *this;
    do { ++current; ++at; } while (at != end && (*current == nullptr || (*current)->first == true));
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::const_iterator::operator++(int)
{
    auto res = *this;
    ++*this;
    return res;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator+=(const difference_type n)
{
    if (n < 0) return (*this -= -n);
    for (difference_type i = 0; i < n; ++i) {
        ++*this;
    }
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::const_iterator::operator+(const difference_type n) const
{
    auto res = *this;
    res += n;
    return res;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator--()
{
    if (at == begin) return *this;
    do { --current; --at; } while (at != begin && (*current == nullptr || (*current)->first == true));
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::const_iterator::operator--(int)
{
    auto res = *this;
    --*this;
    return res;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator& chashmap<Key, T>::const_iterator::operator-=(const difference_type n)
{
    if (n < 0) return (*this += -n);
    for (difference_type i = 0; i < n; ++i) {
        --*this;
    }
    return *this;
}

template<Hashable Key, class T>
constexpr typename chashmap<Key, T>::const_iterator chashmap<Key, T>::const_iterator::operator-(const difference_type n) const
{
    auto res = *this;
    res += n;
    return res;
}

#endif