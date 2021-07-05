#include <iostream>
#include <mutex>
#include "shared.hpp"

class SimpleCounter {
public:
    SimpleCounter() {
        value = 0;
    }

    void increment() {
        mutex.lock();
        value++;
        mutex.unlock();
    }

    int get() {
        mutex.lock();
        int result = value;
        mutex.unlock();
        return result;
    }
private:
    int value;
    std::mutex mutex;
};

void worker(int threadID, SimpleCounter &counter, int nloops) {
    for (int i = 0; i < nloops; ++i) {
        counter.increment();
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <nthreads> <nloops>" << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);

    SimpleCounter counter;
    auto elapsed = time_workers(nthreads, worker, std::ref(counter), nloops);
    std::cout << "program=" << argv[0] << " op=increment"
              << " nthreads=" << nthreads << " nloops=" << nloops
              << " count=" << counter.get() << " time=" << elapsed  << std::endl;
    return 0;
}
