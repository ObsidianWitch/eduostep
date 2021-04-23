#include <stdlib.h>
#include <stdio.h> // printf(), BUFSIZ
#include <unistd.h> // fork(), pipe(), dup2(), read()
#include <err.h> // err()
#include <sys/wait.h> // waitpid()

// Q8. Write a program that creates two children, and connects the standard
// output of one to the standard input of the other, using the pipe() system
// call.
int main(int argc, char *argv[]) {
    int pipefd[2]; // 0: read end ; 1: write end
    if (pipe(pipefd) < 0) { err(EXIT_FAILURE, "pipe"); }

    // child: pipefd[0] -> reader -> stdout
    pid_t cpid_reader = fork();
    if (cpid_reader < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid_reader == 0) {
        // close unused write end of pipe
        if (close(pipefd[1]) < 0) { err(EXIT_FAILURE, "close"); }
        // replace stdin w/ pipefd[0]
        if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) {
            err(EXIT_FAILURE, "dup");
        }
        // read from /proc/self/fd/0 (pipe)
        char buffer[BUFSIZ];
        read(STDIN_FILENO, buffer, BUFSIZ);
        printf("child_reader: %s", buffer);
        exit(EXIT_SUCCESS);
    }

    // child: stdin -> writer -> pipefd[1]
    pid_t cpid_writer = fork();
    if (cpid_writer < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid_writer == 0) { // child
        // close unused read end of pipe
        if (close(pipefd[0]) < 0) { err(EXIT_FAILURE, "close"); }
        // replace stdout w/ pipefd[1]
        if (dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO) {
            err(EXIT_FAILURE, "dup");
        }
        // print to /proc/self/fd/1 (pipe)
        printf("child_writer: Hello, World!\n");
        exit(EXIT_SUCCESS);
    }

    // parent
    if (close(pipefd[0]) < 0) { err(EXIT_FAILURE, "close"); }
    if (close(pipefd[1]) < 0) { err(EXIT_FAILURE, "close"); }
    waitpid(cpid_writer, NULL, 0);
    waitpid(cpid_reader, NULL, 0);

    return EXIT_SUCCESS;
}
