#ifndef SHARED_HPP
#define SHARED_HPP

#include <vector>
#include <thread>
#include <chrono>
#include <random>
namespace chrono = std::chrono;

template<class Function, class... Args>
float time_workers(
    int nthreads, Function&& worker, Args&&... args
) {
    auto start = chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        threads.emplace_back(worker, i, args...);
    }
    for (auto &thread : threads) {
        thread.join();
    }
    auto end = chrono::steady_clock::now();
    auto elapsed_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    return (float) elapsed_ns / chrono::nanoseconds::period::den;
}

template<class DataStructure>
void output(
    std::string program, std::string worker, int nthreads, int nloops,
    DataStructure &ds, float elapsed
) {
    std::cout << "program=" << program << " op=" << worker
              << " nthreads=" << nthreads << " nloops=" << nloops
              << " " << ds << " time=" << elapsed  << std::endl;
}

template<class Collection>
void worker_insert(int threadID, Collection &collection, int nloops) {
    std::random_device rdev;
    std::mt19937 gen(rdev());
    std::uniform_int_distribution<> distribution;
    for (int i = 0; i < nloops; ++i) {
        collection.insert(distribution(gen));
    }
}

template<class Collection>
void worker_lookup(int threadID, Collection &collection, int nloops) {
    std::random_device rdev;
    std::mt19937 gen(rdev());
    std::uniform_int_distribution<> distribution;
    for (int i = 0; i < nloops; ++i) {
        collection.search(distribution(gen));
    }
}

template<class Collection>
void worker_interleave(int threadID, Collection &collection, int nloops) {
    std::random_device rdev;
    std::mt19937 gen(rdev());
    std::uniform_int_distribution<> distribution;
    for (int i = 0; i < nloops; ++i) {
        collection.insert(distribution(gen));
        collection.search(distribution(gen));
    }
}

template<class Collection>
int main_collection(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <nthreads> <nloops>" << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);
    auto nloops = std::stoi(argv[2]);

    {
        Collection collection;
        auto elapsed = time_workers(nthreads, worker_insert<Collection>,
            std::ref(collection), nloops);
        output(argv[0], "insert_rand", nthreads, nloops, collection, elapsed);
        elapsed = time_workers(nthreads, worker_lookup<Collection>,
            std::ref(collection), nloops);
        output(argv[0], "lookup_rand", nthreads, nloops, collection, elapsed);
    } {
        Collection collection;
        auto elapsed = time_workers(nthreads, worker_interleave<Collection>,
            std::ref(collection), nloops);
        output(argv[0], "interleave_rand", nthreads, nloops, collection, elapsed);
    }

    return 0;
}

#endif // SHARED_HPP
