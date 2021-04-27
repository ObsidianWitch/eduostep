#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // read(), fork(), pipe(), close(), write(), read()
#include <time.h> // clock_getres(), clock_gettime()
#include <sched.h> // sched_setaffinity()

// #define NDEBUG
#ifdef NDEBUG
    #define debug_printf(...)
#else
    #define debug_printf printf
#endif

void errexit(char* msg) { perror(msg); exit(EXIT_FAILURE); }

// Retrieve and print CLOCK_PROCESS_CPUTIME_ID resolution.
void clock_resolution() {
    struct timespec resolution;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &resolution) < 0) {
        errexit("clock_getres");
    }
    printf("clock resolution: %ld.%09ld s\n",
        resolution.tv_sec, resolution.tv_nsec);
}

// Repeatedly call a simple system call (0B read) & time how long it takes.
// Note: "Applications should use the clock_gettime() function instead of the
// obsolescent gettimeofday() function." -man 3p gettimeofday
void time_syscall(size_t n) {
    struct timespec ts1;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1) < 0) {
        errexit("clock_gettime 1");
    }
    for (size_t i = 0 ; i < n ; ++i) {
        read(STDIN_FILENO, NULL, 0);
    }
    struct timespec ts2;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2) < 0) {
        errexit("clock_gettime 2");
    }

    ts2.tv_sec -= ts1.tv_sec;
    ts2.tv_nsec -= ts1.tv_nsec;
    ts2.tv_nsec += (ts2.tv_sec * 1000000000UL);
    printf("time syscall: %ld ns\n", ts2.tv_nsec / n);
}

// Time context switch between two processes on one processor by using blocking
// inter-process communication.
// parent -1-pipes[0]-0-> child
// child  -1-pipes[1]-0-> parent
void time_ctxswitch(size_t n) {
    // prepare pipes for two-way IPC between parent and child
    int pipes[2][2];
    if (pipe(pipes[0]) < 0) { errexit("pipe0"); }
    if (pipe(pipes[1]) < 0) { errexit("pipe1"); }

    // run current thread on cpu 0, will be inherited by child process
    // "A child created via fork(2) inherits its parent's CPU affinity mask."
    // -man 2 sched_getaffinity
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) < 0) {
        errexit("[c] sched_setaffinity");
    }

    pid_t cpid = fork();
    if (cpid < 0) { // error
        errexit("fork");
    } else if (cpid == 0) { // child
        // close unused pipes
        if (close(pipes[0][1]) < 0) { errexit("[c] close pipe01"); }
        if (close(pipes[1][0]) < 0) { errexit("[c] close pipe10"); }

        // blocking ipc loop to trigger context switch
        char buffer;
        for (size_t i = 0 ; i < n ; ++i) {
            read(pipes[0][0], &buffer, 1);
            debug_printf("[c][i=%lu] %c\n", i, buffer);
            write(pipes[1][1], "c", 1);
        }

        // close pipes
        if (close(pipes[0][0]) < 0) { errexit("[c] close pipe00"); }
        if (close(pipes[1][1]) < 0) { errexit("[c] close pipe11"); }
    } else { // parent
        // close unused pipes
        if (close(pipes[0][0]) < 0) { errexit("[p] close pipe00"); }
        if (close(pipes[1][1]) < 0) { errexit("[p] close pipe11"); }

        // blocking ipc loop to trigger context switch
        char buffer;
        for (size_t i = 0 ; i < n ; ++i) {
            write(pipes[0][1], "p", 1);
            read(pipes[1][0], &buffer, 1);
            debug_printf("[p][i=%lu] %c\n", i, buffer);
        }

        // close pipes
        if (close(pipes[0][1]) < 0) { errexit("[p] close pipe01"); }
        if (close(pipes[1][0]) < 0) { errexit("[p] close pipe10"); }
    }
}

// Objectives: measure the costs of a system call and context switch.
int main(int argc, char *argv[]) {
    clock_resolution();
    time_syscall(1000000);
    time_ctxswitch(5);
    return EXIT_SUCCESS;
}
