#include <iostream>
#include <mutex>
#include <limits>
#include "shared.hpp"

struct Node {
    int data;
    Node *next;
    std::mutex mutex;
    Node(int data, Node *next): data(data), next(next), mutex() {}
};

class HandOverHandList {
public:
    HandOverHandList(): head_(NULL), size_(0), insert_mutex_() {}

    ~HandOverHandList() {
        Node *current = head_;
        while (current) {
            Node *tmp = current->next;
            delete current;
            current = tmp;
        }
    }

    void insert(int data) {
        auto *node = new Node(data, NULL);
        insert_mutex_.lock();
        node->next = head_;
        head_ = node;
        size_++;
        insert_mutex_.unlock();
    }

    bool lookup(int data) {
        head_->mutex.lock();
        for (Node *e = head_; e != NULL; e = e->next) {
            if (e->data == data) {
                e->mutex.unlock();
                return true;
            }
            if (e->next) {
                e->next->mutex.lock();
            }
            e->mutex.unlock();
        }
        return false;
    }

    int size() {
        insert_mutex_.lock();
        int result = size_;
        insert_mutex_.unlock();
        return result;
    }
private:
    Node *head_;
    int size_;
    std::mutex insert_mutex_;
};

void worker_insert(int threadID, HandOverHandList &list, int nloops) {
    for (int i = 0; i < nloops; ++i) {
        list.insert(i);
    }
}

void worker_lookup(int threadID, HandOverHandList &list, int nloops) {
    for (int i = 0; i < nloops; ++i) {
        list.lookup(nloops - 11);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <nthreads> <nloops>" << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);

    HandOverHandList list;
    worker_insert(0, list, nloops);
    auto elapsed = time_workers(nthreads, worker_lookup, std::ref(list), nloops);
    std::cout << "program=" << argv[0] << " op=lookup-10"
              << " nthreads=" << nthreads << " nloops=" << nloops
              << " size=" << list.size() << " time=" << elapsed  << std::endl;
    return 0;
}
