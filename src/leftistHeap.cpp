#include "leftistHeap.hpp"
#include <stdexcept>

LeftistHeap::LeftistHeap() : root(nullptr) {}

LeftistHeap::~LeftistHeap() {
    delete_subtree(root);
    root = nullptr;
}

LeftistHeap::LeftistHeap(LeftistHeap&& other) noexcept : root(other.root) {
    other.root = nullptr;
}

LeftistHeap& LeftistHeap::operator=(LeftistHeap&& other) noexcept {
    if (this != &other) {
        delete_subtree(root);
        root = other.root;
        other.root = nullptr;
    }
    return *this;
}

void LeftistHeap::set_parent(LHNode* child, LHNode* parent) {
    if (child) child->parent = parent;
}

void LeftistHeap::delete_subtree(LHNode* node) {
    if (!node) return;
    delete_subtree(node->left);
    delete_subtree(node->right);
    delete node;
}

LHNode* LeftistHeap::merge_nodes(LHNode* a, LHNode* b) {
    if (!a) return b;
    if (!b) return a;

    if (a->key > b->key) std::swap(a, b);

    a->right = merge_nodes(a->right, b);
    set_parent(a->right, a);

    int leftNpl = (a->left ? a->left->npl : -1);
    int rightNpl = (a->right ? a->right->npl : -1);

    if (leftNpl < rightNpl) {
        std::swap(a->left, a->right);
        set_parent(a->left, a);
        set_parent(a->right, a);
    }

    a->npl = (a->right ? a->right->npl : -1) + 1;
    set_parent(a, nullptr);
    return a;
}

LHNode* LeftistHeap::insert(int key, int value) {
    LHNode* node = new LHNode(key, value);
    root = merge_nodes(root, node);
    set_parent(root, nullptr);
    return node;
}

std::pair<int,int> LeftistHeap::find_min() const {
    if (!root) throw std::runtime_error("find_min on empty heap");
    return std::make_pair(root->key, root->value);
}

std::pair<int,int> LeftistHeap::extract_min() {
    if (!root) throw std::runtime_error("extract_min on empty heap");
    LHNode* minNode = root;
    std::pair<int,int> out{minNode->key, minNode->value};
    LHNode* left = root->left;
    LHNode* right = root->right;
    if (left) left->parent = nullptr;
    if (right) right->parent = nullptr;
    root = merge_nodes(left, right);
    delete minNode;
    return out;
}

bool LeftistHeap::is_empty() const {
    return root == nullptr;
}

void LeftistHeap::clear() {
    delete_subtree(root);
    root = nullptr;
}

void LeftistHeap::merge_with(LeftistHeap& other) {
    root = merge_nodes(root, other.root);
    other.root = nullptr;
    set_parent(root, nullptr);
}