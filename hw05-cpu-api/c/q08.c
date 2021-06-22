#include <stdlib.h>
#include <stdio.h> // printf(), BUFSIZ
#include <unistd.h> // fork(), pipe(), dup2(), read()
#include <sys/wait.h> // waitpid()
#include "common.h" // error(), error_if()

// Q8. Write a program that creates two children, and connects the standard
// output of one to the standard input of the other, using the pipe() system
// call.
int main(int argc, char *argv[]) {
    int pipefd[2]; // 0: read end ; 1: write end
    error_if(pipe(pipefd) < 0, "pipe");

    // child: pipefd[0] -> reader -> stdout
    pid_t cpid_reader = fork();
    if (cpid_reader < 0) { // error
        error("fork");
    } else if (cpid_reader == 0) {
        // close unused write end of pipe
        error_if(close(pipefd[1]) < 0, "close");
        // replace stdin w/ pipefd[0]
        error_if(dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO, "dup");
        // read from /proc/self/fd/0 (pipe)
        char buffer[BUFSIZ];
        error_if(read(STDIN_FILENO, buffer, BUFSIZ) < 0, "read");
        printf("child_reader: %s", buffer);
        exit(EXIT_SUCCESS);
    }

    // child: stdin -> writer -> pipefd[1]
    pid_t cpid_writer = fork();
    if (cpid_writer < 0) { // error
        error("fork");
    } else if (cpid_writer == 0) { // child
        // close unused read end of pipe
        error_if(close(pipefd[0]) < 0, "close");
        // replace stdout w/ pipefd[1]
        error_if(dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO, "dup");
        // print to /proc/self/fd/1 (pipe)
        printf("child_writer: Hello, World!\n");
        exit(EXIT_SUCCESS);
    }

    // parent
    error_if(close(pipefd[0]) < 0, "close");
    error_if(close(pipefd[1]) < 0, "close");
    error_if(waitpid(cpid_writer, NULL, 0) < 0, "waitpid");
    error_if(waitpid(cpid_reader, NULL, 0) < 0, "waitpid");

    return EXIT_SUCCESS;
}
