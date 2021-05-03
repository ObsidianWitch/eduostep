#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // fork(), pipe(), dup2()
#include <err.h> // err()
#include <sys/wait.h> // waitpid()
#include "common.h" // error(), error_if()

// OwnQ09. Implement a pipe of two commands using pipe().
// ps | grep --color --extended-regexp 'ps|grep|$'
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
        // execute rightmost command
        execlp("grep", "grep", "--color", "--extended-regexp", "ps|grep|$",
               (char *) NULL);
        error("execlp");
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
        // execute leftmost command
        execlp("ps", "ps", (char *) NULL);
        error("execlp");
    }

    // parent
    error_if(close(pipefd[0]) < 0, "close");
    error_if(close(pipefd[1]) < 0, "close");
    /// simulate shell waiting for its current command to finish executing
    /// before executing the next one
    error_if(waitpid(cpid_writer, NULL, 0) < 0, "waitpid");
    error_if(waitpid(cpid_reader, NULL, 0) < 0, "waitpid");

    return EXIT_SUCCESS;
}
