#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int *data = malloc(100 * sizeof(*data));
    data[0] = 1;
    free(data);
    printf("%d\n", data[0]);

    return EXIT_SUCCESS;
}
