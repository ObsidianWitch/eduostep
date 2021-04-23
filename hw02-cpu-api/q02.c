#include <stdlib.h>
#include <unistd.h> // fork()
#include <fcntl.h> // open()
#include <err.h> // err()
#include <string.h> // strlen()

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
    if (fd < 0) { err(EXIT_FAILURE, "open"); }

    pid_t cpid = fork();
    if (cpid < 0) { err(EXIT_FAILURE, "fork"); }

    char *str = (cpid == 0) ? "parent " : "child ";
    size_t len = strlen(str);
    for (uint i = 0; i < 100; i++) {
        if (write(fd, str, len) < 0) { err(EXIT_FAILURE, "write"); }
    }

    if (close(fd) < 0) { err(EXIT_FAILURE, "close"); }

    return EXIT_SUCCESS;
}
