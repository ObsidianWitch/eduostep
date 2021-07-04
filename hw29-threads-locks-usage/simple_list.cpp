#include <iostream>
#include <mutex>
#include "shared.hpp"

struct Node {
    int data;
    Node *next;
    Node(int data, Node *next): data(data), next(next) {}
};

class SimpleList {
public:
    SimpleList(): head_(NULL), mutex_(), size_(0) {}

    ~SimpleList() {
        Node *current = head_;
        while (current) {
            Node *tmp = current->next;
            delete current;
            current = tmp;
        }
    }

    void insert(int data) {
        auto *node = new Node(data, NULL);
        mutex_.lock();
        node->next = head_;
        head_ = node;
        size_++;
        mutex_.unlock();
    }

    bool lookup(int data) {
        bool result = false;
        mutex_.lock();
        for (Node *e = head_; e != NULL; e = e->next) {
            if (e->data == data) {
                result = true;
                break;
            }
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

void worker(int threadID, SimpleList &list, int nloops) {
    for (int i = 0; i < nloops; ++i) {
        list.insert(i);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <nthreads> <nloops>" << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);

    SimpleList list;
    auto elapsed_s = time_workers(nthreads, worker, std::ref(list), nloops);
    std::cout << list.size() << " " << elapsed_s << std::endl;
    return 0;
}
