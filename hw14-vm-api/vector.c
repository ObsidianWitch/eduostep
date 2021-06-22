#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct vector {
    int *data;
    size_t size;
    size_t capacity;
};

void vector_init(struct vector *vec) {
    vec->size = 0;
    vec->capacity = 0;
    vec->data = NULL;
}

void vector_dump(struct vector *vec) {
    printf("vector_dump: size=%lu capacity=%lu data=[",
           vec->size, vec->capacity);
    for (size_t i = 0; i < vec->size; ++i) {
        printf("%d ", vec->data[i]);
    }
    printf("]\n");
}

void vector_push(struct vector *vec, int value) {
    printf("vector_push:before: "); vector_dump(vec);

    assert(vec->size >= 0);
    assert(vec->capacity >= vec->size);

    if (vec->capacity == vec->size) {
        vec->capacity = (vec->capacity == 0) ? 1 : vec->capacity * 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(*(vec->data)));
    }

    vec->data[vec->size] = value;
    vec->size += 1;

    printf("vector_push:after:  "); vector_dump(vec);
}

int vector_pop(struct vector *vec) {
    printf("vector_pop:before:  "); vector_dump(vec);

    assert(vec->size > 0);
    assert(vec->capacity >= vec->size);

    int value = vec->data[vec->size - 1];
    vec->size -= 1;

    if (vec->capacity >= vec->size * 2) {
        vec->capacity = vec->size;
        vec->data = realloc(vec->data, vec->capacity * sizeof(*(vec->data)));
    }

    printf("vector_pop:after:   "); vector_dump(vec);

    return value;
}

void vector_clear(struct vector *vec) {
    if (vec->data != NULL) {
        free(vec->data);
    }
    vector_init(vec);
}

int main(int argc, char *argv[]) {
    struct vector vec;
    vector_init(&vec);
    vector_clear(&vec);
    vector_push(&vec, 1);
    vector_push(&vec, 2);
    vector_push(&vec, 3);
    vector_push(&vec, 4);
    vector_push(&vec, 5);
    vector_pop(&vec);
    vector_pop(&vec);
    vector_pop(&vec);
    vector_pop(&vec);
    vector_pop(&vec);
    vector_clear(&vec);

    return EXIT_SUCCESS;
}
