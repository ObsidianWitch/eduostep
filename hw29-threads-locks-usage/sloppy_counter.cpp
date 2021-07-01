#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
namespace chrono = std::chrono;

class SloppyCounter {
public:
    SloppyCounter(int ncpus, int threshold) :
        threshold(threshold),
        countGlobal(0), mutexGlobal(),
        countLocal(ncpus, 0), mutexLocal(ncpus)
    {}

    void increment(int cpuID) {
        this->mutexLocal[cpuID].lock();
        this->countLocal[cpuID]++;
        if (this->countLocal[cpuID] >= this->threshold) {
            this->update(cpuID);
        }
        this->mutexLocal[cpuID].unlock();
    }

    void update(int cpuID) {
        this->mutexGlobal.lock();
        this->countGlobal += this->countLocal[cpuID];
        this->countLocal[cpuID] = 0;
        this->mutexGlobal.unlock();
    }

    int get() {
        this->mutexGlobal.lock();
        int result = this->countGlobal;
        this->mutexGlobal.unlock();
        return result;
    }
private:
    int threshold;
    int countGlobal;
    std::mutex mutexGlobal;
    std::vector<int> countLocal;
    std::vector<std::mutex> mutexLocal;
};

void worker(int ncpus, int threadID, SloppyCounter &counter, int nloops) {
    int cpuID = threadID % ncpus;
    for (int i = 0; i < nloops; ++i) {
        counter.increment(cpuID);
    }
    counter.update(cpuID); // make sure nothing remains in local counters
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0]
                  << " <nthreads> <nloops> [threshold=1024]" << std::endl;
        return 1;
    }
    auto ncpus = std::thread::hardware_concurrency();
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);
    auto threshold = argc >= 4 ? std::stoi(argv[3]) : 1024;

    SloppyCounter counter(ncpus, threshold);
    auto start = chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        threads.push_back(std::thread(worker, ncpus, i, std::ref(counter), nloops));
    }
    for (auto &thread : threads) {
        thread.join();
    }
    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    std::cout << counter.get()
              << " " << (float) elapsed / chrono::nanoseconds::period::den
              << std::endl;
    return 0;
}
