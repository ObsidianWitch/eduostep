#include <iostream>
#include <mutex>
#include <vector>
#include "shared.hpp"

struct Node {
    int key;
    int height;
    Node *parent;
    Node *left;
    Node *right;
    Node(int key, Node *parent, Node *left, Node *right)
    : key(key), parent(parent), left(left), right(right) {
        height = parent == NULL ? 0 : parent->height + 1;
    }
};

class BinarySearchTree {
public:
    BinarySearchTree(): root_(NULL), mutex_(), size_(0), height_(0) {}

    ~BinarySearchTree() {
        mutex_.lock();
        if (root_ == NULL) { return; }
        std::vector<Node*> to_process = { root_ };
        while (!to_process.empty()) {
            auto *e = to_process.back();
            to_process.pop_back();
            if (e->left != NULL) { to_process.push_back(e->left); }
            if (e->right != NULL) { to_process.push_back(e->right); }
            delete e;
        }
        mutex_.unlock();
    }

    Node* search(int key) {
        mutex_.lock();
        auto *result = root_;
        while (result != NULL && key != result->key) {
            if (key < result->key) { result = result->left; }
            else { result = result->right; }
        }
        mutex_.unlock();
        return result;
    }

    void insert(int key) {
        mutex_.lock();
        Node *parent = NULL;
        auto *current = root_;
        while (current != NULL) {
            parent = current;
            if (key < current->key) { current = current->left; }
            else { current = current->right; }
        }

        auto *node = new Node(key, parent, NULL, NULL);
        if (parent == NULL) {
            root_ = node;
        } else if (node->key < parent->key) {
            parent->left = node;
        } else {
            parent->right = node;
        }
        size_++;
        height_ = std::max(height_, node->height);
        mutex_.unlock();
    }

    int size() {
        mutex_.lock();
        int result = size_;
        mutex_.unlock();
        return result;
    }

    int height() {
        mutex_.lock();
        int result = height_;
        mutex_.unlock();
        return result;
    }
private:
    Node *root_;
    std::mutex mutex_;
    int size_;
    int height_;
};

std::ostream& operator<<(std::ostream &stream, BinarySearchTree &tree) {
    return stream << "size=" << tree.size() << " height=" << tree.height();
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <nthreads> <nloops>" << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);

    {
        BinarySearchTree tree;
        auto elapsed = time_workers(nthreads, worker_insert<BinarySearchTree>,
            std::ref(tree), nloops);
        output(argv[0], "insert_rand", nthreads, nloops, tree, elapsed);
        elapsed = time_workers(nthreads, worker_lookup<BinarySearchTree>,
            std::ref(tree), nloops);
        output(argv[0], "lookup_rand", nthreads, nloops, tree, elapsed);
    } {
        BinarySearchTree tree;
        auto elapsed = time_workers(nthreads, worker_interleave<BinarySearchTree>,
            std::ref(tree), nloops);
        output(argv[0], "interleave_rand", nthreads, nloops, tree, elapsed);
    }
    return 0;
}
