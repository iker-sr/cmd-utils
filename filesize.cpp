#include <iostream>
#include <filesystem>
#include <cstring>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

void show_help(char* argv0) {
    std::cout << "Usage: " << argv0 << " [OPTION]... [FILE]...\n";
    std::cout << "Get the size of a file and output the result on bytes\n";
    std::cout << "\n";
    std::cout << "      --help        display this help and exit\n";
    std::cout << "      --version     output version information and exit\n";
}

void show_version() {
    std::cout << "filesize 1.0\n";
    std::cout << "\n";
    std::cout << "This is free software: you are free to change and redistribute it.\n";
    std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
    std::cout << "Inspired by the Free Software Foundation philosophy.\n";
}

std::string human_readable_size(unsigned long long bytes) {
    static const char* units[6] = {"bytes", "KB", "MB", "GB", "TB", "PB"};
    unsigned char unit_index = 0;

    double result = static_cast<double>(bytes);

    for (int i = 1; i < 6 && result >= 1024.0; i++) {
        result /= 1024;
        unit_index = i;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << result << " " << units[unit_index];
    
    return ss.str();
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
            std::cout << argv[0] << ": cannot get size of '" << argv[i] << "': Is a directory\n";
            continue;
        } else if (!fs::is_regular_file(argv[i])) {
            std::cout << argv[0] << ": cannot get size of '" << argv[i] << "': No such file or directory\n";
            continue;
        }

        unsigned long long size;
        try {
            size = fs::file_size(argv[i]);
        } catch (const fs::filesystem_error& e) {
            std::cout << argv[0] << ": cannot get size of '" << argv[i] << "': " << e.what() << "\n";
            continue;
        }

        if (size >= 1024) {
            std::cout << argv[i] << ": " << human_readable_size(size) << " (" << size << " bytes)\n";
        } else {
            std::cout << argv[i] << ": " << size << " bytes\n";
        }
    }

    return 0;
}