#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "../src/concurrentHeap.hpp"

#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <random>

TEST_CASE("Concurrent Heap behaves like a priority queue") {
    ShardedConcurrentHeap sh(8);
    const int N = 10000;

    std::vector<int> collected;
    std::mutex col_m;

    std::vector<std::thread> producers;
    for (int t = 0; t < 4; ++t) {
        producers.emplace_back([&]() {
            std::mt19937 rng(123 + t);
            std::uniform_int_distribution<int> dist(1, 1000);
            for (int i = 0; i < N; ++i) {
                sh.insert(dist(rng), 0);
            }
        });
    }

    for (auto &th : producers) th.join();

    std::vector<std::thread> consumers;
    for (int t = 0; t < 4; ++t) {
        consumers.emplace_back([&]() {
            while (true) {
                auto kv = sh.extract_min();
                if (!kv) break;
                std::lock_guard<std::mutex> lk(col_m);
                collected.push_back(kv->first);
            }
        });
    }

    for (auto &th : consumers) th.join();

    REQUIRE(collected.size() == 4 * N); 
    for (size_t i = 1; i < collected.size(); ++i) {
        REQUIRE(collected[i-1] <= collected[i]);
    }
}