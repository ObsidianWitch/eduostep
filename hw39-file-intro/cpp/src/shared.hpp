#ifndef SHARED_HPP
#define SHARED_HPP

#include <cstdio> // perror()
#include <cerrno> // errno
#include <cstdlib> // exit()

void panic(std::string msg) {
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}

void panic_if(bool failure, std::string msg) {
    if(failure) { panic(msg); }
}

#endif // SHARED_HPP
