#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // fork(), pipe(), dup2()
#include <err.h> // err()
#include <sys/wait.h> // waitpid()

// OwnQ09. Implement a pipe of two commands using pipe().
// ps | grep --color --extended-regexp 'ps|grep|$'
int main(int argc, char *argv[]) {
    int pipefd[2]; // 0: read end ; 1: write end
    if (pipe(pipefd) < 0) { perror("pipe"); }

    // child: pipefd[0] -> reader -> stdout
    pid_t cpid_reader = fork();
    if (cpid_reader < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid_reader == 0) {
        // close unused write end of pipe
        if (close(pipefd[1]) < 0) { perror("close"); }
        // replace stdin w/ pipefd[0]
        if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) { perror("dup"); }
        // execute rightmost command
        execlp("grep", "grep", "--color", "--extended-regexp", "ps|grep|$",
               (char *) NULL);
        err(EXIT_FAILURE, "execlp");
    }

    // child: stdin -> writer -> pipefd[1]
    pid_t cpid_writer = fork();
    if (cpid_writer < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid_writer == 0) { // child
        // close unused read end of pipe
        if (close(pipefd[0]) < 0) { perror("close"); }
        // replace stdout w/ pipefd[1]
        if (dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO) { perror("dup"); }
        // execute leftmost command
        execlp("ps", "ps", (char *) NULL);
        err(EXIT_FAILURE, "execlp");
    }

    // parent
    if (close(pipefd[0]) < 0) { perror("close"); }
    if (close(pipefd[1]) < 0) { perror("close"); }
    waitpid(cpid_writer, NULL, 0);
    waitpid(cpid_reader, NULL, 0);

    return EXIT_SUCCESS;
}
