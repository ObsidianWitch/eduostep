#include <stdlib.h>
#include <unistd.h> // fork(), exec()
#include <err.h> // err()

// Q4. Write a program that calls fork() and then calls some form of exec() to
// run the program /bin/ls. See if you can try all of the variants of exec(),
// including (on Linux) execl(), execle(), execlp(), execv(), execvp(), and
// execvpe(). Why do you think there are so many variants of the same basic
// call?
//
// A. l variant: variadic arguments list
//    v variant: array of pointers to strings
//    ---
//    / variant: absolute executable filepath
//    e variant: envp arguments to specify environment
//    p variant: search for executable file in PATH
int main(int argc, char *argv[]) {
    pid_t cpid = fork();
    if (cpid < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid == 0) { // child
        execlp("ls", "ls", "-l", (char *) NULL);
        err(EXIT_FAILURE, "execlp");
    }
    return EXIT_SUCCESS;
}
