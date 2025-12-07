//My strategy was to make a simple sharded concurrent heap where each shard is a leftist heap protected by one mutex.

#include "concurrentHeap.hpp"
#include <limits>
#include <algorithm>
#include <functional>

using namespace std;

ShardedConcurrentHeap::ShardedConcurrentHeap(size_t shards)
    : shard_vec(), shards_count(shards)
{
    shard_vec.reserve(shards_count);
    for (size_t i = 0; i < shards_count; ++i) {
        shard_vec.emplace_back(make_unique<Shard>());
    }
}

ShardedConcurrentHeap::~ShardedConcurrentHeap() = default;

size_t ShardedConcurrentHeap::shard_for_key(int key) const {
    return (hash<int>()(key) % shards_count);
}

LHNode* ShardedConcurrentHeap::insert(int key, int value) {
    size_t s = shard_for_key(key);
    auto &sh = shard_vec[s];
    lock_guard<mutex> lock(sh->m);
    return sh->heap.insert(key, value);
}

optional<pair<int,int>> ShardedConcurrentHeap::extract_min() {
    const int INF = numeric_limits<int>::max();
    int bestKey = INF;
    int bestShard = -1;

    for (size_t i = 0; i < shards_count; ++i) {
        auto &sh = shard_vec[i];
        lock_guard<mutex> lock(sh->m);
        if (!sh->heap.is_empty()) {
            auto kv = sh->heap.find_min();
            if (kv.first < bestKey) {
                bestKey = kv.first;
                bestShard = static_cast<int>(i);
            }
        }
    }

    if (bestShard == -1) return nullopt;

    auto &sh = shard_vec[bestShard];
    lock_guard<mutex> lock(sh->m);
    if (sh->heap.is_empty()) return nullopt;
    return sh->heap.extract_min();
}

bool ShardedConcurrentHeap::empty() const {
    for (size_t i = 0; i < shards_count; ++i) {
        auto &sh = shard_vec[i];
        lock_guard<mutex> lock(sh->m);
        if (!sh->heap.is_empty()) return false;
    }
    return true;
}