#include <iostream>
#include <vector>
#include <thread>
#include <semaphore>
#include <chrono>

class Barrier {
public:
    Barrier(int nthreads): counter_(nthreads), mutex_(1), arrived_(0) {}

    // Decrement counter_ by one and block until it reaches 0.
    void wait() {
        mutex_.acquire();
        counter_--;
        if (counter_ <= 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            arrived_.release();
        }
        mutex_.release();
        arrived_.acquire();
        arrived_.release();
    }
private:
    int counter_;
    std::counting_semaphore<1> mutex_;
    std::counting_semaphore<1> arrived_;
};

void worker(int i, Barrier &barrier) {
    std::cout << "w" << i << ": before" << std::endl;
    barrier.wait();
    std::cout << "w" << i << ": after" << std::endl;
}

int main(int argc, char *argv[]) {
    std::cout << "parent: begin" << std::endl;

    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <nthreads>" << std::endl;
        return 1;
    }
    auto nthreads = std::stoi(argv[1]);

    Barrier barrier(nthreads);
    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; ++i) {
        threads.emplace_back(worker, i, std::ref(barrier));
    }
    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << "parent: end" << std::endl;
    return 0;
}
