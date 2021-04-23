#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // fork()
#include <err.h> // err()
#include <sys/wait.h> // wait(), waitpid()

// Q6. Write a slight modification of the previous program, this time using
// waitpid() instead of wait(). When would waitpid() be useful?
//
// A. It's useful to wait for a specific child to terminate. It also allows
// options to modify its behaviour.
int main(int argc, char *argv[]) {
    pid_t cpid = fork();
    if (cpid < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid == 0) { // child
        printf("child\n");
    } else { // parent
        pid_t wpid = waitpid(cpid, NULL, 0);
        if (wpid < 0) { err(EXIT_FAILURE, "wait"); }
        printf("parent\n");
    }
    return EXIT_SUCCESS;
}
