#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // getpid(), getppid(), fork(), pause()
#include <signal.h> // sigaction()
#include "common.h" // error(), error_if()

void sigusr1_handler(int signum) {}

// Q3. Write another program using fork(). The child process should print
// “hello”; the parent process should print “goodbye”. You should try to ensure
// that the child process always prints first; can you do this without calling
// wait() in the parent?
//
// A. It's possible to do this without wait() by using signal() or sigaction().
int main(int argc, char *argv[]) {
    struct sigaction action = { .sa_handler=sigusr1_handler };
    error_if(sigaction(SIGUSR1, &action, NULL) < 0, "sigaction");

    pid_t cpid = fork();
    if (cpid < 0) { // error
        error("fork");
    } else if (cpid == 0) { // child
        printf("child %d: hello\n", getpid());
        error_if(kill(getppid(), SIGUSR1) < 0, "kill");
    } else { // parent
        pause(); // sleep until a **handled** signal is delivered
        printf("parent %d: goodbye\n", getpid());
    }
    return EXIT_SUCCESS;
}
