#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "../src/concurrentHeap.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <random>

using namespace std;

TEST_CASE("Concurrent Heap behaves like a priority queue") {
    const int N = 10000;
    const int num_threads = 4;
    ShardedConcurrentHeap sh(4);

    vector<thread> producers;
    for (int t = 0; t < num_threads; ++t) {
        producers.emplace_back([&sh, t, N]() {
            mt19937 rng(1000 + t);
            uniform_int_distribution<int> dist(1, 1000);
            for (int i = 0; i < N; ++i) {
                sh.insert(dist(rng), t);
            }
        });
    }
    for (auto& th : producers) th.join();

    vector<int> collected;
    mutex col_m;
    vector<thread> consumers;

    for (int t = 0; t < num_threads; ++t) {
        consumers.emplace_back([&]() {
            while (true) {
                auto values = sh.extract_min();
                if (!values) break;
                lock_guard<mutex> lk(col_m);
                collected.push_back(values->first);
            }
        });
    }
    for (auto& th : consumers) th.join();

    REQUIRE(collected.size() == num_threads * N);

    sort(collected.begin(), collected.end());
    for (size_t i = 1; i < collected.size(); ++i) {
        REQUIRE(collected[i-1] <= collected[i]);
    }
}