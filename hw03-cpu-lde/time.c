#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // read()
#include <time.h> // clock_getres(), clock_gettime
#include <err.h> // err()

// Objectives: measure the costs of a system call and context switch.
// 1. Repeatedly call a simple system call (0B read) & time how long it takes
// Note: "Applications should use the clock_gettime() function instead of the
// obsolescent gettimeofday() function." -man 3p gettimeofday
int main(int argc, char *argv[]) {
    // clock resolution
    struct timespec resolution;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &resolution) < 0) {
        err(EXIT_FAILURE, "clock_getres");
    }
    printf("clock resolution: %ld.%09ld s\n",
        resolution.tv_sec, resolution.tv_nsec);

    // time system call
    size_t n = 1000000;
    struct timespec ts1;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1) < 0) {
        err(EXIT_FAILURE, "clock_gettime 1");
    }
    for (size_t i = 0 ; i < n ; ++i) {
        read(STDIN_FILENO, NULL, 0);
    }
    struct timespec ts2;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2) < 0) {
        err(EXIT_FAILURE, "clock_gettime 2");
    }

    ts2.tv_sec -= ts1.tv_sec;
    ts2.tv_nsec -= ts1.tv_nsec;
    ts2.tv_nsec += (ts2.tv_sec * 1000000000UL);
    printf("time syscall: %ld ns\n", ts2.tv_nsec / n);

    return EXIT_SUCCESS;
}
