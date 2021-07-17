#include <iostream>
#include <vector>
#include <thread>
#include <semaphore>
#include <chrono>

class RWLock {
public:
    RWLock(): read_sem_(1), write_sem_(1), readers_(0) {}

    void read_lock() {
        read_sem_.acquire();
        readers_++;
        if (readers_ == 1) {
            write_sem_.acquire();
        }
        read_sem_.release();
    }

    void read_unlock() {
        read_sem_.acquire();
        readers_--;
        if (readers_ == 0) {
            write_sem_.release();
        }
        read_sem_.release();
    }

    void write_lock() {
        write_sem_.acquire();
    }

    void write_unlock() {
        write_sem_.release();
    }
private:
    std::counting_semaphore<1> read_sem_;
    std::counting_semaphore<1> write_sem_;
    int readers_;
};

int value = 0;
RWLock lock;

void reader(int nloops) {
    for (int i = 0; i < nloops; ++i) {
        lock.read_lock();
        std::cout << "r" << value << std::endl;
        lock.read_unlock();
    }
}

void writer(int nloops) {
    for (int i = 0; i < nloops; ++i) {
        lock.write_lock();
        value++;
        std::cout << "w" << value << std::endl;
        lock.write_unlock();
    }
}

int main(int argc, char *argv[]) {
    std::cout << "parent: begin" << std::endl;
    if (argc != 4) {
        std::cerr << "usage: " << argv[0] << " <nreaders> <nwriters> <nloops>"
                  << std::endl;
        return 1;
    }
    auto nreaders = std::stoi(argv[1]);
    auto nwriters = std::stoi(argv[2]);
    auto nloops = std::stoi(argv[3]);

    std::vector<std::thread> threads;
    for (int i = 0; i < nreaders; ++i) {
        threads.emplace_back(reader, nloops);
    }
    for (int i = 0; i < nwriters; ++i) {
        threads.emplace_back(writer, nloops);
    }
    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << "parent: end: " << value << std::endl;
    return 0;
}
