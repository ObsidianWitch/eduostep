#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // fork()
#include <err.h> // err()
#include <sys/wait.h> // wait()

// Q5. Now write a program that uses wait() to wait for the child process to
// finish in the parent. What does wait() return? What happens if you use wait()
// in the child?
//
// A. "wait(): on success, returns the process ID of the terminated child; on
// failure, -1 is returned." -man 2 wait
// Using wait() in the child process fails: wait() returns -1 and sets errno to
// ECHILD 10 No child processes.
int main(int argc, char *argv[]) {
    pid_t cpid = fork();
    if (cpid < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid == 0) { // child
        pid_t wpid = wait(NULL);
        if (wpid < 0) { perror("wait"); } // wait: No child processes
        printf("child\n");
    } else { // parent
        pid_t wpid = wait(NULL);
        if (wpid < 0) { err(EXIT_FAILURE, "wait"); }
        printf("parent\n");
    }
    return EXIT_SUCCESS;
}
