#include <stdlib.h>
#include <stdio.h> // printf()
#include <unistd.h> // getpid(), getppid(), fork(), pause()
#include <err.h> // err()
#include <signal.h> // sigaction()

void sigusr1_handler(int signum) {}

// Q3. Write another program using fork(). The child process should print
// “hello”; the parent process should print “goodbye”. You should try to ensure
// that the child process always prints first; can you do this without calling
// wait() in the parent?
//
// A. It's possible to do this without wait() by using signal() or sigaction().
int main(int argc, char *argv[]) {
    struct sigaction action = { .sa_handler=sigusr1_handler };
    if (sigaction(SIGUSR1, &action, NULL) < 0) {
        err(EXIT_FAILURE, "sigaction");
    }

    pid_t cpid = fork();
    if (cpid < 0) { // error
        err(EXIT_FAILURE, "fork");
    } else if (cpid == 0) { // child
        printf("hello %d\n", getpid());
        kill(getppid(), SIGUSR1);
    } else { // parent
        pause(); // sleep until a **handled** signal is delivered
        printf("goodbye %d\n", getpid());
    }
    return EXIT_SUCCESS;
}
