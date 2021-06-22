#include <stdlib.h>
#include <unistd.h> // fork()
#include <fcntl.h> // open()
#include <string.h> // strlen()
#include "common.h" // error(), error_if()

// Q2. Write a program that opens a file (with the open() system call) and then
// calls fork() to create a new process. Can both the child and parent access
// the file descriptor returned by open()? What happens when they are writing to
// the file concurrently, i.e., at the same time?
//
// A. "The child inherits copies of the parent's set of open file descriptors.
// Each file descriptor in the child refers to the same open file description
// (see open(2)) as the corresponding file descriptor in the parent. This means
// that the two file descriptors share open file status flags, file offset, and
// signal-driven I/O attributes." -man 2 fork.
// If both processes write concurrently, the output will be non-deterministic.
int main(int argc, char *argv[]) {
    int fd = open("out/q02.out", O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
    error_if(fd < 0, "open");

    pid_t cpid = fork();
    error_if(cpid < 0, "fork");

    char *str = (cpid == 0) ? "parent " : "child ";
    size_t len = strlen(str);
    for (uint i = 0; i < 100; i++) {
        error_if(write(fd, str, len) < 0, "write");
    }

    error_if(close(fd) < 0, "close");

    return EXIT_SUCCESS;
}
