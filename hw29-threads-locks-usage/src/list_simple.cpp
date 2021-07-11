#include <iostream>
#include <mutex>
#include "shared.hpp"

struct Node {
    int key;
    Node *next;
    Node(int key, Node *next): key(key), next(next) {}
};

class List {
public:
    List(): head_(NULL), mutex_(), size_(0) {}

    ~List() {
        mutex_.lock();
        Node *current = head_;
        while (current) {
            Node *tmp = current->next;
            delete current;
            current = tmp;
        }
        mutex_.unlock();
    }

    void insert(int key) {
        auto *node = new Node(key, NULL);
        mutex_.lock();
        node->next = head_;
        head_ = node;
        size_++;
        mutex_.unlock();
    }

    Node* search(int key) {
        mutex_.lock();
        auto *result = head_;
        while (result != NULL && key != result->key) {
            result = result->next;
        }
        mutex_.unlock();
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
    std::mutex mutex_;
    int size_;
};

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
        std::cout << "program=" << argv[0] << " op=insert_rand"
                  << " nthreads=" << nthreads << " nloops=" << nloops
                  << " size=" << list.size() << " time=" << elapsed  << std::endl;
        elapsed = time_workers(nthreads, worker_lookup<List>,
            std::ref(list), nloops);
        std::cout << "program=" << argv[0] << " op=lookup_rand"
                  << " nthreads=" << nthreads << " nloops=" << nloops
                  << " size=" << list.size() << " time=" << elapsed  << std::endl;
    } {
        List list;
        auto elapsed = time_workers(nthreads, worker_interleave<List>,
            std::ref(list), nloops);
        std::cout << "program=" << argv[0] << " op=interleave_rand"
                  << " nthreads=" << nthreads << " nloops=" << nloops
                  << " size=" << list.size() << " time=" << elapsed  << std::endl;
    }
    return 0;
}
