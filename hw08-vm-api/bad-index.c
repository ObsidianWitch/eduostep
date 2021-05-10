#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *data = malloc(100 * sizeof(*data));
    data[100] = 0; // access index out of bounds
    free(data);

    return EXIT_SUCCESS;
}
