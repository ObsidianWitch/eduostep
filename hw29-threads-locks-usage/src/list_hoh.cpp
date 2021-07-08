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

Node END(INT_MIN, NULL);

class HandOverHandList {
public:
    HandOverHandList(): head_(&END), size_(0) {}

    ~HandOverHandList() {
        Node *current = head_;
        while (current != &END) {
            Node *tmp = current->next;
            delete current;
            current = tmp;
        }
    }

    void insert(int data) {
        auto *node = new Node(data, NULL);

        head_->mutex.lock();
        node->mutex.lock();

        node->next = head_;
        head_ = node;
        size_++;

        node->mutex.unlock(); // current head unlock
        node->next->mutex.unlock(); // previous head unlock
    }

    bool lookup(int data) {
        head_->mutex.lock();
        for (Node *e = head_; e != &END; e = e->next) {
            if (e->data == data) {
                e->mutex.unlock();
                return true;
            }
            if (e->next != &END) {
                e->next->mutex.lock();
            }
            e->mutex.unlock();
        }
        return false;
    }

    int size() {
        head_->mutex.lock();
        int result = size_;
        head_->mutex.unlock();
        return result;
    }
private:
    Node *head_;
    int size_;
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
