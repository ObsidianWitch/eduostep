#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *x = malloc(sizeof *x);
    *x = 10;
    // forgot to free(x)

    return EXIT_SUCCESS;
}
