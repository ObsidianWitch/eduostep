#include <iostream>
#include <vector>
#include <mutex>
#include "shared.hpp"

class SloppyCounter {
public:
    SloppyCounter(int ncpus, int threshold) :
        threshold_(threshold),
        countGlobal_(0), mutexGlobal_(),
        countLocal_(ncpus, 0), mutexLocal_(ncpus)
    {}

    void increment(int cpuID) {
        mutexLocal_[cpuID].lock();
        countLocal_[cpuID]++;
        if (countLocal_[cpuID] >= threshold_) {
            update(cpuID);
        }
        mutexLocal_[cpuID].unlock();
    }

    void update(int cpuID) {
        mutexGlobal_.lock();
        countGlobal_ += countLocal_[cpuID];
        countLocal_[cpuID] = 0;
        mutexGlobal_.unlock();
    }

    int get() {
        mutexGlobal_.lock();
        int result = countGlobal_;
        mutexGlobal_.unlock();
        return result;
    }

    friend std::ostream& operator<<(std::ostream&, SloppyCounter&);
private:
    int threshold_;
    int countGlobal_;
    std::mutex mutexGlobal_;
    std::vector<int> countLocal_;
    std::vector<std::mutex> mutexLocal_;
};

std::ostream& operator<<(std::ostream &stream, SloppyCounter &counter) {
    return stream << "threshold=" << counter.threshold_
                  << " count=" << counter.get();
}

void worker(int threadID, int ncpus, SloppyCounter &counter, int nloops) {
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
    auto elapsed = time_workers(nthreads, worker, ncpus, std::ref(counter), nloops);
    assert(counter.get() == nthreads * nloops);
    output(argv[0], "increment", nthreads, nloops, counter, elapsed);
    return 0;
}
