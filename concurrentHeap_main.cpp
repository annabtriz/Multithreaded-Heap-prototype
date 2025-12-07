// Demo of basic multi-threaded usage of my Concurrent Heap

#include "concurrentHeap.hpp"
#include <thread>
#include <vector>
#include <iostream>
#include <random>
#include <atomic>
#include <chrono>

using namespace std;

int main() {
    const int NUM_THREADS = 2; // I reduced the number of threadsfor cleaner output, it was giving me a very big output!
    const int OPS_PER_THREAD = 20;
    ShardedConcurrentHeap simHeap(8);

    atomic<int> inserts{0}, extracts{0};

    auto worker = [&](int tid) {
        mt19937 rng(1234 + tid);
        uniform_int_distribution<int> keyDist(1, 100);

        for (int i = 0; i < OPS_PER_THREAD; ++i) {
            int op = i % 2;
            if (op == 0) {
                int k = keyDist(rng);
                simHeap.insert(k, 1);
                inserts.fetch_add(1, memory_order_relaxed);
                cout << "[Thread " << tid << "] Inserted key=" << k << "\n";
            } else {
                auto r = simHeap.extract_min();
                if (r) {
                    extracts.fetch_add(1, memory_order_relaxed);
                    cout << "[Thread " << tid << "] Extracted key=" << r->first 
                         << ", value=" << r->second << "\n";
                }
            }
        }
    };

    auto t0 = chrono::high_resolution_clock::now();
    vector<thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) threads.emplace_back(worker, i);
    for (auto &t : threads) t.join();
    auto t1 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = t1 - t0;
    cout << "\nThreads: " << NUM_THREADS 
         << " , inserts=" << inserts.load() 
         << " , extracts=" << extracts.load() << "\n";
    cout << "Elapsed(s): " << elapsed.count() << "\n";

    cout << "\nRemaining heap after simulation:\n";
    if (simHeap.empty()) {
        cout << "Heap is empty.\n";
    } else {
        while (!simHeap.empty()) {
            auto kv = simHeap.extract_min();
            if (kv)
                cout << "key = " << kv->first << ", value = " << kv->second << "\n";
        }
    }


    ShardedConcurrentHeap pq;
    mt19937 rng(12345); 
    uniform_int_distribution<int> key_dist(1, 100);
    uniform_int_distribution<int> value_dist(10, 1000);

    const int N = 10;
    cout << "\n___________________________________________\n";
    cout << "\nInserting " << N << " random key-value pairs:\n";
    for (int i = 0; i < N; ++i) {
        int key = key_dist(rng);
        int value = value_dist(rng);
        pq.insert(key, value);
        cout << "Inserted: key=" << key << ", value=" << value << "\n";
    }

    cout << "\nExtracting values in order:\n";
    while (!pq.empty()) {
        auto kv = pq.extract_min();
        if (kv)
            cout << "key = " << kv->first << ", value = " << kv->second << "\n";
    }
    if (pq.empty()) {
        cout << "Random PQ is now empty.\n";
    }

    return 0;
}