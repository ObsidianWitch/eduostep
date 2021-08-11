#include <iostream>
#include <filesystem>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <sys/stat.h>
#include "shared.hpp"

namespace fs = std::filesystem;

std::_Put_time<char> put_time(time_t time) {
    auto tm = std::localtime(&time);
    return std::put_time(tm, "%b %d %H:%M");
}

void print_one(fs::path path, bool verbose) {
    if (verbose) {
        struct stat statbuf;
        panic_if(lstat(path.c_str(), &statbuf) < 0, "print_one:lstat");
        std::cout << std::oct << statbuf.st_mode << std::dec
            << "\t" << statbuf.st_nlink << "\t" << statbuf.st_uid
            << "\t" << statbuf.st_gid << "\t" << put_time(statbuf.st_mtime)
            << "\t" << path.filename() << std::endl;
    } else {
        std::cout << path << std::endl;
    }
}

// 2. List Files: Write a program that lists files in the given directory. When
// called without any arguments, the program should just print the file names.
// When invoked with the -l flag, the program should print out information about
// each file, such as the owner, group, per- missions, and other information
// obtained from the stat() system call. The program should take one additional
// argument, which is the directory to read, e.g., myls -l directory. If no
// directory is given, the program should just use the current working
// directory. Useful interfaces: stat(), opendir(), readdir(), getcwd().
int main(int argc, char *argv[]) {
    fs::path path = ".";
    auto verbose = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0) {
            std::cerr << "usage: myls [-h] [-l] [path=.]" << std::endl;
            return 1;
        } else if (strcmp(argv[i], "-l") == 0) {
            verbose = true;
        } else {
            path = argv[i];
        }
    }

    auto dir_stream = opendir(path.c_str());
    if (dir_stream != NULL) {
        struct dirent *dir_entry;
        while ((dir_entry = readdir(dir_stream)) != NULL) {
            print_one(path / dir_entry->d_name, verbose);
        }
        panic_if(errno != 0, "main:readdir");
        panic_if(closedir(dir_stream) < 0, "main:closedir");
    } else if (errno == ENOTDIR) {
        print_one(path, verbose);
    } else {
        panic("main:opendir:" + path.string());
    }

    return 0;
}
