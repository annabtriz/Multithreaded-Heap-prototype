// compare_dijkstra_leftist_concurrent.cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "../src/leftistHeap.hpp"
#include "../src/concurrentHeap.hpp"

#include <vector>
#include <limits>
#include <random>
#include <algorithm>

using Graph = std::vector<std::vector<std::pair<int,int>>>;

std::vector<int> dijkstra_leftist(const Graph& graph, int source) {
    const int N = graph.size();
    const int INF = std::numeric_limits<int>::max();

    std::vector<int> dist(N, INF);
    dist[source] = 0;

    LeftistHeap pq;
    pq.insert(0, source);

    while (!pq.is_empty()) {
        auto kv = pq.extract_min();
        int d = kv.first;
        int u = kv.second;

        if (d != dist[u]) continue;

        for (auto &[v, w] : graph[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.insert(dist[v], v);
            }
        }
    }

    return dist;
}


std::vector<int> dijkstra_concurrent(const Graph& graph, int source) {
    const int N = graph.size();
    const int INF = std::numeric_limits<int>::max();

    std::vector<int> dist(N, INF);
    dist[source] = 0;

    ShardedConcurrentHeap pq(8); // 8 shards
    pq.insert(0, source);

    while (!pq.empty()) {
        auto kv_opt = pq.extract_min();
        if (!kv_opt) break;

        auto kv = *kv_opt;
        int d = kv.first;
        int u = kv.second;

        if (d != dist[u]) continue;

        for (auto &[v, w] : graph[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.insert(dist[v], v);
            }
        }
    }

    return dist;
}


static Graph make_random_graph(int n, int edgesPerNode) {
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(1, 10);

    Graph g(n);
    for (int u = 0; u < n; ++u) {
        for (int j = 0; j < edgesPerNode; ++j) {
            int v = rng() % n;
            if (v != u)
                g[u].push_back({v, dist(rng)});
        }
    }
    return g;
}


TEST_CASE("correctness") {
    Graph g = make_random_graph(50, 4);

    auto d1 = dijkstra_leftist(g, 0);
    auto d2 = dijkstra_concurrent(g, 0);

    REQUIRE(d1.size() == d2.size());
    for (size_t i = 0; i < d1.size(); ++i) {
        REQUIRE(d1[i] == d2[i]);
    }
}


TEST_CASE("Dijkstra comparison - performance") {
    Graph g = make_random_graph(2000, 6);

    BENCHMARK("Dijkstra using LeftistHeap") {
        return dijkstra_leftist(g, 0);
    };

    BENCHMARK("Dijkstra using ConcurrentHeap") {
        return dijkstra_concurrent(g, 0);
    };
}