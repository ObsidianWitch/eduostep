#ifndef SHARED_HPP
#define SHARED_HPP

#include <cstdio> // perror()
#include <cerrno> // errno
#include <cstdlib> // exit()

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void error_if(bool failure, const char *msg) {
    if(failure) { error(msg); }
}

#endif // SHARED_HPP
