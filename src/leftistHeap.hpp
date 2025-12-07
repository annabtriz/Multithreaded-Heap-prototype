#ifndef LEFTISTHEAP_HPP
#define LEFTISTHEAP_HPP

#include <climits>
#include <functional>
#include <utility>

struct LHNode {
    int key;
    int value;
    int npl;
    LHNode* left;
    LHNode* right;
    LHNode* parent;

    LHNode(int k = INT_MAX, int v = 0)
        : key(k), value(v), npl(0),
          left(nullptr), right(nullptr), parent(nullptr) {}
};

class LeftistHeap {
public:
    LeftistHeap();
    ~LeftistHeap();

    LeftistHeap(const LeftistHeap&) = delete;
    LeftistHeap& operator=(const LeftistHeap&) = delete;

    LeftistHeap(LeftistHeap&& other) noexcept;
    LeftistHeap& operator=(LeftistHeap&& other) noexcept;

    LHNode* insert(int key, int value = 0);
    std::pair<int,int> find_min() const;
    std::pair<int,int> extract_min();
    bool is_empty() const;
    void clear();

    void merge_with(LeftistHeap& other);

private:
    LHNode* root;

    static LHNode* merge_nodes(LHNode* a, LHNode* b);

    static void set_parent(LHNode* child, LHNode* parent);
    static void delete_subtree(LHNode* node);
};

#endif