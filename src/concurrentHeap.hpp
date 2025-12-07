//My strategy was to make a simple sharded concurrent heap where each shard is a leftist heap protected by one mutex.

#ifndef CONCURRENTHEAP_HPP
#define CONCURRENTHEAP_HPP

#include "leftistHeap.hpp"
#include <vector>
#include <mutex>
#include <memory>
#include <optional>
#include <functional>

using namespace std;

class ShardedConcurrentHeap {
public:
    explicit ShardedConcurrentHeap(size_t shards = 8);
    ~ShardedConcurrentHeap();

    LHNode* insert(int key, int value = 0);
    optional<pair<int,int>> extract_min();
    bool empty() const;

private:
    struct Shard {
        LeftistHeap heap;
        mutable mutex m;
        Shard() = default;
    };

    vector<unique_ptr<Shard>> shard_vec;
    size_t shards_count;

    size_t shard_for_key(int key) const;
};

#endif