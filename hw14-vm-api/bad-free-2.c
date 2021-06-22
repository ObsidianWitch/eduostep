#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *data = malloc(100 * sizeof(*data));
    free(&data[50]);

    return EXIT_SUCCESS;
}
