#include <iostream>
#include <mutex>
#include <climits>
#include "shared.hpp"

struct Node {
    int key;
    Node *next;
    std::mutex mutex;
    Node(int key, Node *next): key(key), next(next), mutex() {}
};
Node END(INT_MAX, NULL);

class List {
public:
    List(): head_(&END), size_(0), mutex_() {}

    ~List() {
        mutex_.lock();
        Node *current = head_;
        while (current != &END) {
            Node *tmp = current->next;
            delete current;
            current = tmp;
        }
        mutex_.unlock();
    }

    void insert(int key) {
        mutex_.lock();
            head_->mutex.lock();
                auto *node = new Node(key, NULL);
                node->mutex.lock();
                    node->next = head_;
                    head_ = node;
                    size_++;
                node->mutex.unlock();
            node->next->mutex.unlock();
        mutex_.unlock();
    }

    Node* search(int key) {
        mutex_.lock();
        head_->mutex.lock();
        mutex_.unlock();
        auto *result = head_;
        while (result != &END && key != result->key) {
            result->next->mutex.lock();
            result->mutex.unlock();
            result = result->next;
        }
        result->mutex.unlock();
        result = result != &END ? result : NULL;
        return result;
    }

    int size() {
        mutex_.lock();
        int result = size_;
        mutex_.unlock();
        return result;
    }
private:
    Node *head_;
    int size_;
    std::mutex mutex_;
};

std::ostream& operator<<(std::ostream &stream, List &list) {
    return stream << "size=" << list.size();
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <nthreads> <nloops>" << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);

    {
        List list;
        auto elapsed = time_workers(nthreads, worker_insert<List>,
            std::ref(list), nloops);
        output(argv[0], "insert_rand", nthreads, nloops, list, elapsed);
        elapsed = time_workers(nthreads, worker_lookup<List>,
            std::ref(list), nloops);
        output(argv[0], "lookup_rand", nthreads, nloops, list, elapsed);
    } {
        List list;
        auto elapsed = time_workers(nthreads, worker_interleave<List>,
            std::ref(list), nloops);
        output(argv[0], "interleave_rand", nthreads, nloops, list, elapsed);
    }
    return 0;
}
