//Test Dijkstra using LeftistHeap

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "../src/leftistHeap.hpp"

#include <vector>
#include <limits>
#include <utility>

using namespace std;

vector<int> dijkstra(
    const vector<vector<pair<int,int>>>& graph,
    int source
) {
    const int N = graph.size();
    const int INF = numeric_limits<int>::max();

    vector<int> dist(N, INF);
    dist[source] = 0;

    LeftistHeap pq;

    pq.insert(0, source);

    while (!pq.is_empty()) {
        auto kv = pq.extract_min();
        int d = kv.first;
        int u = kv.second;

        // stale entry?
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


TEST_CASE("Dijkstra small graph") {


    vector<vector<pair<int,int>>> graph = {
        {{1,1}, {2,4}},
        {{2,2}},
        {}
    };

    auto dist = dijkstra(graph, 0);

    REQUIRE(dist[0] == 0);
    REQUIRE(dist[1] == 1);
    REQUIRE(dist[2] == 3);
}

TEST_CASE("Dijkstra larger graph") {

    vector<vector<pair<int,int>>> graph = {
        {{1,2}, {2,5}},
        {{2,1}, {3,3}},
        {{3,2}, {4,10}},
        {{4,1}},
        {}
    };

    auto dist = dijkstra(graph, 0);

    REQUIRE(dist[0] == 0);
    REQUIRE(dist[1] == 2);
    REQUIRE(dist[2] == 3);
    REQUIRE(dist[3] == 5);
    REQUIRE(dist[4] == 6);
}

TEST_CASE("Dijkstra detects unreachable nodes", "[dijkstra]") {

    vector<vector<pair<int,int>>> graph = {
        {{1,5}},
        {},
        {{3,1}},
        {}
    };

    auto dist = dijkstra(graph, 0);

    REQUIRE(dist[0] == 0);
    REQUIRE(dist[1] == 5);
    REQUIRE(dist[2] == numeric_limits<int>::max());
    REQUIRE(dist[3] == numeric_limits<int>::max());
}