#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
namespace chrono = std::chrono;

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

void worker(SimpleList &list, int nloops) {
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
    auto start = chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        threads.push_back(std::thread(worker, std::ref(list), nloops));
    }
    for (auto &thread : threads) {
        thread.join();
    }
    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    std::cout << list.size()
              << " " << (float) elapsed / chrono::nanoseconds::period::den
              << std::endl;
    return 0;
}
