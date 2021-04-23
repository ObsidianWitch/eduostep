#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // fork()
#include <err.h> // err()

// Q7. Write a program that creates a child process, and then in the child
// closes standard output (STDOUT FILENO). What happens if the child calls
// printf() to print some output after closing the descriptor?
//
// A. Closing stdout(->'/proc/self/fd/1') and not opening a new file after that
// means that printing anything in the child will have no effect, since the file
// descriptor 1 isn't assigned anymore.
int main(int argc, char *argv[]) {
    pid_t cpid = fork();
    if (cpid < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid == 0) { // child
        if (close(STDOUT_FILENO) < 0) { perror("close"); }
        printf("child\n");
    } else { // parent
        printf("parent\n");
    }
    return EXIT_SUCCESS;
}
