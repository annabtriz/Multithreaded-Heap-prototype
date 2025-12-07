#include "leftistHeap.hpp"
#include <vector>
#include <limits>
#include <utility>
#include <iostream>

using Graph = std::vector<std::vector<std::pair<int,int>>>;

std::vector<int> dijkstra_leftist(const Graph& graph, int start) {
    const int INF = std::numeric_limits<int>::max();
    int n = graph.size();

    std::vector<int> dist(n, INF);
    dist[start] = 0;

    LeftistHeap pq;
    pq.insert(0, start);

    while (!pq.is_empty()) {
        auto kv = pq.extract_min();
        int d = kv.first;
        int u = kv.second;

        if (d != dist[u]) continue;

        for (auto& edge : graph[u]) {
            int v = edge.first;
            int w = edge.second;

            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.insert(dist[v], v);
            }
        }
    }

    return dist;
}

int main() {
    Graph g(3);
    g[0] = { {1, 1}, {2, 4} };
    g[1] = { {2, 2} };

    auto dist = dijkstra_leftist(g, 0);

    std::cout << "Dijkstra (LeftistHeap) distances from node 0:\n";
    for (int i = 0; i < dist.size(); ++i) {
        std::cout << "  to " << i << " = "
                  << (dist[i] == std::numeric_limits<int>::max() ? -1 : dist[i])
                  << "\n";
    }
}