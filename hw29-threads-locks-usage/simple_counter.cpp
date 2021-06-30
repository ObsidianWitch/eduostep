#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
namespace chrono = std::chrono;

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

void worker(SimpleCounter &counter, int nloops) {
    for (int i = 0; i < nloops; ++i) {
        counter.increment();
    }
}

// Usage: simple_counter <nthreads> <nloops>
int main(int argc, char *argv[]) {
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);

    SimpleCounter counter;
    auto start = chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        threads.push_back(std::thread(worker, std::ref(counter), nloops));
    }
    for (auto &thread : threads) {
        thread.join();
    }
    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    std::cout << "IN " << nthreads << " " << nloops
              << " OUT " << counter.get()
              << " " << (float) elapsed / chrono::nanoseconds::period::den
              << std::endl;
    return 0;
}
