#include <unistd.h>
#include <iostream>
#include <cmath>
#include <chrono>
namespace chrono = std::chrono;

int main(int argc, char *argv[]) {
    auto npages = std::stoi(argv[2]);
    auto ntrials = std::stoi(argv[1]);
    ntrials = ntrials <= 0
            ? (int) std::pow(10, 9 - (int) std::log10(npages))
            : ntrials;
    auto pagesize = sysconf(_SC_PAGESIZE);
    auto clkfreq = chrono::steady_clock::period::den;
    auto *array = new char[npages * pagesize](); // zero initialized

    // overhead loop
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < ntrials; ++i) {
        for (int j = 0; j < npages; ++j) { ; }
    }
    auto end = chrono::steady_clock::now();
    auto t1 = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    // access loop
    start = chrono::steady_clock::now();
    for (int i = 0; i < ntrials; ++i) {
        for (int j = 0; j < npages; ++j) {
            array[j * pagesize] += 1;
        }
    }
    end = chrono::steady_clock::now();
    auto t2 = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    delete[] array;

    std::cout << "CST " << pagesize << " " << clkfreq
              << " IN " << ntrials << " " << npages
              << " OUT " << (float) t1 / chrono::nanoseconds::period::den
              << "+" << (float) t2 / chrono::nanoseconds::period::den
              << " " << (float) (t2 - t1) / ntrials / npages
              << std::endl;

    return EXIT_SUCCESS;
}
