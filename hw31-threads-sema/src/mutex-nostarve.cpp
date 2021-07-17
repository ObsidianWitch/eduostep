#include <iostream>
#include <vector>
#include <thread>
#include <semaphore>
#include <barrier>

class Mutex {
public:
    Mutex(int nthreads): sem_(1), barrier_(nthreads) {}

    void lock() {
        sem_.acquire();
    }

    void unlock() {
        sem_.release();
        barrier_.arrive_and_wait();
    }

    void drop() {
        barrier_.arrive_and_drop();
    }
private:
    std::binary_semaphore sem_;
    std::barrier<> barrier_;
};

std::vector<int> counters;
int nloops;

void worker(int id, Mutex &mutex) {
    mutex.lock();
    while (nloops > 0) {
        counters[id]++;
        nloops--;
        mutex.unlock();
        mutex.lock();
    }
    mutex.unlock();
    mutex.drop();
}

int main(int argc, char *argv[]) {
    std::cout << "parent: begin" << std::endl;
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " <nthreads> <nloops>"
                  << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);
    nloops = std::stoi(argv[2]);

    Mutex mutex(nthreads);
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        counters.push_back(0);
        threads.emplace_back(worker, i, std::ref(mutex));
    }
    for (auto &thread : threads) {
        thread.join();
    }
    for (int i = 0; i < nthreads; ++i) {
        std::cout << "t[" << i << "]=" << counters[i] << std::endl;
    }
    std::cout << "parent: end" << std::endl;
    return 0;
}
