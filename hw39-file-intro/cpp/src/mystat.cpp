#include <iostream>
#include <sys/stat.h>
#include "shared.hpp"

// 1. Stat: Write your own version of the command line program stat, which
// simply calls the stat() system call on a given file or directory. Print out
// file size, number of blocks allocated, reference (link) count, and so forth.
// What is the link count of a directory, as the number of entries in the
// directory changes? Useful interfaces: stat().
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <path>" << std::endl;
        return 1;
    }

    struct stat statbuf;
    error_if(lstat(argv[1], &statbuf) < 0, "main:lstat");
    std::cout << "File:" << argv[1]
              << " Size:" << statbuf.st_size
              << " Blocks:" << statbuf.st_blocks
              << " Inode:" << statbuf.st_ino
              << " Links:" << statbuf.st_nlink
              << std::endl;

    return 0;
}
