#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // fork()
#include "common.h" // error(), error_if()

// Q1. Write a program that calls fork(). Before calling fork(), have the main
// process access a variable (e.g., x) and set its value to something (e.g.,
// 100). What value is the variable in the child process? What happens to the
// variable when both the child and parent change the value of x?
//
// A. The parent and child processes have duplicate but separate memory spaces.
// After the fork, both the parent and child have x=100. If both of them modify
// the variable to different values, then x will have a different value in both.
int main(int argc, char *argv[]) {
    int x = 100;
    pid_t cpid = fork();
    if (cpid < 0) { // error
        error("fork");
    } else if (cpid == 0) { // child
        printf("child: x=%d\n", x); // child: x=100
        x += 1;
        printf("child: x=%d\n", x); // child x=101
    } else { // parent
        printf("parent: x=%d\n", x); // parent: x=100
        x -= 1;
        printf("parent: x=%d\n", x); // parent: x=99
    }
    return EXIT_SUCCESS;
}
