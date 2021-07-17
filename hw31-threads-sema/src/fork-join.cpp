#include <iostream>
#include <thread>
#include <semaphore>
#include <chrono>

std::counting_semaphore<1> semaphore(0);

void child() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "child" << std::endl;
    semaphore.release();
}

int main(int argc, char *argv[]) {
    std::cout << "parent: begin" << std::endl;
    std::thread child_thread(child);
    semaphore.acquire();
    std::cout << "parent: end" << std::endl;
    child_thread.join();
    return 0;
}
