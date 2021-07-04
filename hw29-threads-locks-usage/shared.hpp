#ifndef SHARED_HPP
#define SHARED_HPP

#include <vector>
#include <thread>
#include <chrono>
namespace chrono = std::chrono;

template<class Function, class... Args>
float time_workers(
    int nthreads, Function&& worker, Args&&... args
) {
    auto start = chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        threads.push_back(std::thread(worker, i, args...));
    }
    for (auto &thread : threads) {
        thread.join();
    }
    auto end = chrono::steady_clock::now();
    auto elapsed_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    return (float) elapsed_ns / chrono::nanoseconds::period::den;
}

#endif // SHARED_HPP
