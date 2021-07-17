#include <iostream>
#include <vector>
#include <thread>
#include <semaphore>
#include <chrono>

std::vector<std::unique_ptr<std::counting_semaphore<1>>> semaphores;

void worker(int i, int nthreads) {
    std::cout << "w" << i << ": before" << std::endl;
    if (i != nthreads - 1) {
        semaphores[i]->acquire();
        semaphores[(i + 1) % nthreads]->release();
    } else {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        semaphores[(i + 1) % nthreads]->release();
        semaphores[i]->acquire();
    }
    std::cout << "w" << i << ": after" << std::endl;
}

int main(int argc, char *argv[]) {
    std::cout << "parent: begin" << std::endl;

    int nthreads = 2;
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        semaphores.push_back(std::make_unique<std::counting_semaphore<1>>(0));
        threads.emplace_back(worker, i, nthreads);
    }
    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << "parent: end" << std::endl;
    return 0;
}
