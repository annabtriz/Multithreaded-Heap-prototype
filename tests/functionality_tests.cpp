#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "../src/leftistHeap.hpp"
#include "../src/concurrentHeap.hpp"

#include <thread>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

TEST_CASE("LeftistHeap basic operations") {
    LeftistHeap h;
    REQUIRE(h.is_empty());
    h.insert(5, 50);
    h.insert(2, 20);
    h.insert(8, 80);
    h.insert(1, 10);

    REQUIRE_FALSE(h.is_empty());
    auto p = h.find_min();
    REQUIRE(p.first == 1);

    auto e = h.extract_min();
    REQUIRE(e.first == 1);
    REQUIRE(h.find_min().first == 2);

    h.clear();
    REQUIRE(h.is_empty());
}

TEST_CASE("LeftistHeap ordering") {
    LeftistHeap h;
    vector<int> keys = {10, 4, 7, 1, 3, 9, 2};
    for (int k : keys) h.insert(k, k);
    sort(keys.begin(), keys.end());

    for (int k : keys) {
        auto e = h.extract_min();
        REQUIRE(e.first == k);
    }
    REQUIRE(h.is_empty());
}

TEST_CASE("ConcurrentHeap multi-thread basic") {
    ShardedConcurrentHeap sh(4);
    const int N = 10000;

    vector<thread> producers;
    for (int t = 0; t < 4; ++t) {
        producers.emplace_back([&sh, t, N]() {
            mt19937 rng(1000 + t);
            uniform_int_distribution<int> dist(1, 1000);
            for (int i = 0; i < N; ++i) {
                sh.insert(dist(rng), t);
            }
        });
    }
    for (auto &th : producers) th.join();

    vector<int> collected;
    mutex col_m;
    vector<thread> consumers;
    for (int t = 0; t < 4; ++t) {
        consumers.emplace_back([&]() {
            while (true) {
                auto r = sh.extract_min();
                if (!r) break;
                lock_guard<mutex> lk(col_m);
                collected.push_back(r->first);
            }
        });
    }
    for (auto &th : consumers) th.join();

    REQUIRE(collected.size() == 4 * N);

    sort(collected.begin(), collected.end());
    for (size_t i = 1; i < collected.size(); ++i) {
        REQUIRE(collected[i-1] <= collected[i]);
    }
}