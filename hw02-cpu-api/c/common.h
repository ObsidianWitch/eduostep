#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void error_if(bool failure, const char *msg) {
    if(failure) { error(msg); }
}

#endif // COMMON_H
