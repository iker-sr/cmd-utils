/*
Build:
    g++ countlines.cpp -std=c++17 -O2 -s -o countlines
*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>

namespace fs = std::filesystem;

void show_help(char* argv0) {
    std::cout << "Usage: " << argv0 << " [OPTION]... [FILE]...\n";
    std::cout << "Count the amount of lines in a file and output the result\n";
    std::cout << "\n";
    std::cout << "      --help        display this help and exit\n";
    std::cout << "      --version     output version information and exit\n";
}

void show_version() {
    std::cout << "countlines 1.0\n";
    std::cout << "\n";
    std::cout << "This is free software: you are free to change and redistribute it.\n";
    std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
    std::cout << "Inspired by the Free Software Foundation philosophy.\n";
}

unsigned long long count_lines(const char* path, bool& error) {
    std::ifstream file(path, std::ios::binary);

    error = false;
    if (!file) {
        error = true;
        return 0;
    }

    unsigned long long count = 0;
    std::string line;
    while (std::getline(file, line)) {
        count++;
    }

    file.close();

    return count;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << argv[0] << ": missing operand\n";
        return 1;
    } else if (strcmp(argv[1], "--help") == 0) {
        show_help(argv[0]);
        return 0;
    } else if (strcmp(argv[1], "--version") == 0) {
        show_version();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (fs::is_directory(argv[i])) {
            std::cout << argv[0] << ": cannot count lines of '" << argv[i] << "': Is a directory\n";
            continue;
        }
        else if (!fs::is_regular_file(argv[i])) {
            std::cout << argv[0] << ": cannot count lines of '" << argv[i] << "': No such file or directory\n";
            continue;
        }

        bool error = false;
        unsigned long long lines = count_lines(argv[i], error);
        if (error) {
            std::cout << argv[0] << ": cannot count lines of '" << argv[i] << "': Failed to open the file\n";
            continue;
        }
        std::cout << argv[i] << ": " << lines << " lines\n";
    }

    return 0;
}