#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <unordered_map>
#include <list>
#include <tuple>
#include <utility>
#include <stdexcept>
#include <cstdint>
#include <functional>

// Define a custom hash for std::pair
struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
    }
};


class LRUCache {
public:
    using Key = std::pair<uint64_t, int>;
    using Value = std::tuple<uint64_t, uint>;

    LRUCache(size_t capacity) : capacity(capacity) {}

    Value get(const Key& key) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            throw std::runtime_error("Key not found");
        }
        // Move accessed key to the front of the list
        usage.splice(usage.begin(), usage, it->second.second);
        return it->second.first;
    }

    void put(const Key& key, const Value& value) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            // Update value and move to the front
            it->second.first = value;
            usage.splice(usage.begin(), usage, it->second.second);
        } else {
            if (cache.size() == capacity) {
                // Evict least recently used item
                const Key& lru = usage.back();
                usage.pop_back();
                cache.erase(lru);
            }
            usage.push_front(key);
            cache[key] = {value, usage.begin()};
        }
    }

    bool contains(const Key& key) {
        return cache.find(key) != cache.end();
    }

private:
    size_t capacity;

    // List to store the usage order
    std::list<Key> usage;

    // Map to store key-value pairs and list iterators
    std::unordered_map<Key, std::pair<Value, std::list<Key>::iterator>, PairHash> cache;
};

LRUCache lruCache(1000);

#endif // LRUCACHE_H