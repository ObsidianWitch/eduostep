#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

void errno_chk(const char *msg) {
    if (errno != 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

// Return time in seconds.
// NB: clock() uses clock_gettime() w/ CLOCK_PROCESS_CPUTIME_ID
clock_t gettime() {
    clock_t result = clock() / CLOCKS_PER_SEC;
    errno_chk("clock");
    return result;
}

int main(int argc, char *argv[]) {
    // cli args
    if (argc < 3) {
        fprintf(stderr, "usage: %s <memory> <time>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long arg_memory = strtol(argv[1], NULL, 10); // megabytes
    arg_memory *= 1000000; // bytes, can overflow
    if (arg_memory < 1) { errno = ERANGE; }
    errno_chk("strtol");

    long arg_time = strtol(argv[2], NULL, 10); // seconds
    if (arg_time < 0) { errno = ERANGE; }
    errno_chk("strtol");

    // allocate array and touch each entry repeatedly for `arg_time` seconds
    char* array = calloc(arg_memory, sizeof(char)); // memory set to 0
    errno_chk("calloc");

    clock_t begin_t = gettime();
    while (gettime() - begin_t < arg_time) {
        for (size_t i = 0; i < arg_memory - 1; ++i) {
            array[i] = 1;
        }
    }
    free(array);

    return EXIT_SUCCESS;
}
