// demo of sequential leftist heap

#include "leftistHeap.hpp"
#include <iostream>

int main() {
    LeftistHeap h;
    h.insert(10, 100);
    h.insert(3, 30);
    h.insert(7, 70);
    h.insert(1, 10);

    std::cout << "Extracting values in order:\n";
    while (!h.is_empty()) {
        auto kv = h.extract_min();
        std::cout << "key = " << kv.first << ", value=" << kv.second << "\n";
    }
    return 0;
}