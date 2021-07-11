#include <iostream>
#include <mutex>
#include <climits>
#include "shared.hpp"

struct Node {
    int data;
    Node *next;
    std::mutex mutex;
    Node(int data, Node *next): data(data), next(next), mutex() {}
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

    void insert(int data) {
        mutex_.lock();
            head_->mutex.lock();
                auto *node = new Node(data, NULL);
                node->mutex.lock();
                    node->next = head_;
                    head_ = node;
                    size_++;
                node->mutex.unlock();
            node->next->mutex.unlock();
        mutex_.unlock();
    }

    Node* search(int data) {
        mutex_.lock();
        head_->mutex.lock();
        mutex_.unlock();
        auto *result = head_;
        while (result != &END && data != result->data) {
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
