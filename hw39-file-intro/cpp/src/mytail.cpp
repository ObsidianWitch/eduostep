#include <iostream>
#include <filesystem>
#include <string_view>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "shared.hpp"

namespace fs = std::filesystem;

void help() {
    std::cerr << "usage: mytail [-h] [-10] <path>" << std::endl;
    exit(1);
}

// Read blocks of data from a file backwards. On success, the number of bytes
// read is returned, and the file position is moved back. Zero indicates
// beginning of file.
ssize_t read_reverse(int fd, char *buffer, ssize_t count) {
    auto start_offset = lseek(fd, 0, SEEK_CUR);
    if (start_offset == 0) { return 0; }
    panic_if(start_offset < 0, "read_reverse:lseek0");
    auto target_offset = (start_offset < count) ? 0 : start_offset - count;
    auto target_count = start_offset - target_offset;

    panic_if(lseek(fd, target_offset, SEEK_SET) < 0, "read_reverse:lseek1");

    auto read_count = 0;
    while (read_count < target_count) {
        auto tmp_count = read(fd, buffer + read_count, target_count);
        panic_if(tmp_count < 0, "read_reverse:read");
        read_count += tmp_count;
    }

    panic_if(lseek(fd, target_offset, SEEK_SET) < 0, "read_reverse:lseek2");

    return read_count;
}

void tail(fs::path path, int target_lines) {
    auto fd = open(path.c_str(), O_RDONLY);
    panic_if(fd < 0, "tail:open");

        std::array<char, 512> buffer;

        // set cursor to beginning of nth line (from EOF)
        auto offset = lseek(fd, 0, SEEK_END);
        panic_if(offset < 0, "tail:lseek0");
        int buffer_count = 0;
        int lines_count = 0;
        while ((buffer_count = read_reverse(fd, buffer.data(), buffer.max_size())) > 0) {
            for (int i = buffer_count - 1; i >= 0; --i) {
                if (buffer[i] == '\n') {
                    lines_count++;
                }
                if (lines_count >= target_lines + 1) {
                    break;
                }
                offset -= 1;
            }
        }
        panic_if(lseek(fd, offset, SEEK_SET) < 0, "tail:lseek1");

        // print last n lines
        while ((buffer_count = read(fd, buffer.data(), buffer.max_size())) > 0) {
            std::cout << std::string_view(buffer.data(), buffer_count) << std::flush;
        }

    panic_if(close(fd) < 0, "tail:close");
}

// 3. Tail: Write a program that prints out the last few lines of a file. The
// program should be efficient, in that it seeks to near the end of the file,
// reads in a block of data, and then goes backwards until it finds the
// requested number of lines; at this point, it should print out those lines
// from beginning to the end of the file. To invoke the program, one should
// type: mytail -n file, where n is the number of lines at the end of the file
// to print. Useful interfaces: stat(), lseek(), open(), read(), close().
int main(int argc, char *argv[]) {
    fs::path path;
    auto nlines = 10;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0) {
            help();
        } else if (argv[i][0] == '-') {
            nlines = -std::stoi(argv[i]);
        } else {
            path = argv[i];
        }
    }
    if (path.empty()) {
        help();
    }

    tail(path, nlines);

    return 0;
}
