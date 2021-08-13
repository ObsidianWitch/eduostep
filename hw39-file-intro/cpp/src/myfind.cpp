#include <iostream>
#include <filesystem>
#include <queue>
#include <cstring>
#include <dirent.h>
#include "shared.hpp"

namespace fs = std::filesystem;

void walk(fs::path root) {
    std::queue<fs::path> queue;
    queue.push(root);
    while (!queue.empty()) {
        auto path = queue.front(); queue.pop();
        auto dir_stream = opendir(path.c_str());
        if (dir_stream != NULL) {
            struct dirent *dir_entry;
            while ((dir_entry = readdir(dir_stream)) != NULL) {
                if (strcmp(dir_entry->d_name, ".") == 0
                    || strcmp(dir_entry->d_name, "..") == 0)
                { continue; }
                queue.push(path / dir_entry->d_name);
            }
            panic_if(errno != 0, "walk:readdir");
            panic_if(closedir(dir_stream) < 0, "walk:closedir");
        } else if (errno == ENOTDIR) {
            errno = 0;
            std::cout << path << std::endl;
        } else {
            panic("walk:opendir:" + path.string());
        }
    }
}

// 4. Recursive Search: Write a program that prints out the names of each file
// and directory in the file system tree, starting at a given point in the tree.
// For example, when run without arguments, the program should start with the
// current working directory and print its contents, as well as the contents of
// any sub-directories, etc., until the entire tree, root at the CWD, is
// printed. If given a single argument (of a directory name), use that as the
// root of the tree instead. Refine your recursive search with more fun options,
// similar to the powerful find command line tool. Useful interfaces: figure it
// out.
int main(int argc, char *argv[]) {
    fs::path root = (argc < 2) ? "." : argv[1];

    walk(root);

    return 0;
}
