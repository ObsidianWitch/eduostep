#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *x = NULL;
    *x = 10; // undefined behavior

    return EXIT_SUCCESS;
}
