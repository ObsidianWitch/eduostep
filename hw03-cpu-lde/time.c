#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // read(), fork(), pipe(), close(), write(), read()
#include <time.h> // clock_getres(), clock_gettime()
#include <sched.h> // sched_setaffinity()

#define NDEBUG
#ifdef NDEBUG
    #define debug_printf(...)
#else
    #define debug_printf printf
#endif

void errexit(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

struct timespec gettime(clockid_t clock_id) {
    struct timespec ts;
    if (clock_gettime(clock_id, &ts) < 0) {
        errexit("clock_gettime");
    }
    return ts;
}

// Return ta - tb in nanoseconds.
long difftime2ns(struct timespec ta, struct timespec tb) {
    return (ta.tv_nsec - tb.tv_nsec)
         + (ta.tv_sec - tb.tv_sec) * 1000000000UL;
}

// ---

// Retrieve and print CLOCK_PROCESS_CPUTIME_ID's resolution.
void clock_resolution() {
    struct timespec resolution;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &resolution) < 0) {
        errexit("clock_getres");
    }
    printf("clock resolution: %ld.%09ld s\n",
        resolution.tv_sec, resolution.tv_nsec);
}

// Repeatedly (`n`) call a simple system call (0B read) & return the average
// execution time of this syscall in nanoseconds.
// NB: "Applications should use the clock_gettime() function instead of the
// obsolescent gettimeofday() function." -man 3p gettimeofday
long time_syscall(size_t n) {
    struct timespec ts1 = gettime(CLOCK_PROCESS_CPUTIME_ID);
    for (size_t i = 0 ; i < n ; ++i) {
        read(STDIN_FILENO, NULL, 0);
    }
    struct timespec ts2 = gettime(CLOCK_PROCESS_CPUTIME_ID);

    long syscall_nsec = difftime2ns(ts2, ts1) / n;
    printf("time syscall: %ld ns\n", syscall_nsec);
    return syscall_nsec;
}

// Measure the time it takes to write and read a token `n` times through a pipe
// on one process (no context switch).
long time_pipe(size_t n) {
    int pipefd[2];
    if (pipe(pipefd) < 0) { errexit("pipe"); }

    char buffer;
    struct timespec ts1 = gettime(CLOCK_PROCESS_CPUTIME_ID);
    for (size_t i = 0 ; i < n ; ++i) {
        write(pipefd[1], "p", 1);
        read(pipefd[0], &buffer, 1);
    }
    struct timespec ts2 = gettime(CLOCK_PROCESS_CPUTIME_ID);

    // close pipes
    if (close(pipefd[0]) < 0) { errexit("close pipefd0"); }
    if (close(pipefd[1]) < 0) { errexit("close pipefd1"); }

    // result
    long pipe_nsec = difftime2ns(ts2, ts1) / n;
    printf("time pipe: %ld ns\n", pipe_nsec);
    return pipe_nsec;
}

// Time context switch between two processes on one processor by using a loop of
// blocking ipc.
void time_ctxswitch(size_t n, long syscall_nsec, long pipe_nsec) {
    // prepare pipes for two-way IPC between parent and child
    // parent -1-pipes[0]-0-> child
    // child  -1-pipes[1]-0-> parent
    int pipes[2][2];
    if (pipe(pipes[0]) < 0) { errexit("pipe0"); }
    if (pipe(pipes[1]) < 0) { errexit("pipe1"); }

    // run current thread on cpu 0
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
        struct timespec ts1 = gettime(CLOCK_PROCESS_CPUTIME_ID);
        for (size_t i = 0 ; i < n ; ++i) {
            write(pipes[0][1], "p", 1);
            read(pipes[1][0], &buffer, 1);
            debug_printf("[p][i=%lu] %c\n", i, buffer);
        }
        struct timespec ts2 = gettime(CLOCK_PROCESS_CPUTIME_ID);

        // close pipes
        if (close(pipes[0][1]) < 0) { errexit("[p] close pipe01"); }
        if (close(pipes[1][0]) < 0) { errexit("[p] close pipe10"); }

        // results
        /// estimate duration of one iteration of the ipc loop from the parent
        long ipc_nsec = difftime2ns(ts2, ts1) / n;
        printf("time ctxsw (w/ overhead): %ld ns\n", ipc_nsec);

        /// estimate duration of context switch w/o 2 syscalls overhead
        long ctxsw1_nsec = ipc_nsec - (2 * syscall_nsec);
        printf("time ctxsw (w/o 2*syscall overhead): %ld ns\n", ctxsw1_nsec);

        /// estimate duration of context switch w/o pipe overhead
        long ctxsw2_nsec = ipc_nsec - pipe_nsec;
        printf("time ctxsw (w/o pipe overhead): %ld ns\n", ctxsw2_nsec);
    }
}

// Objectives: measure the costs of a system call and context switch.
int main(int argc, char *argv[]) {
    clock_resolution();
    long syscall_nsec = time_syscall(1000000);
    long pipe_nsec = time_pipe(1000000);
    time_ctxswitch(1000000, syscall_nsec, pipe_nsec);
    return EXIT_SUCCESS;
}
