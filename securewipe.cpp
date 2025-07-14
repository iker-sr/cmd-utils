#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <vector>
#include <iomanip>

namespace fs = std::filesystem;

void show_help(char* argv0) {
    std::cout << "Usage: " << argv0 << " [OPTION]... [FILE]...\n";
    std::cout << "Open a file and fill the content with zeros to remove securely\n";
    std::cout << "\n";
    std::cout << "  -f, --force       do not ask if you are sure to wipe a file\n";
    std::cout << "\n";
    std::cout << "      --help        display this help and exit\n";
    std::cout << "      --version     output version information and exit\n";
}

void show_version() {
    std::cout << "securewipe 1.0\n";
    std::cout << "\n";
    std::cout << "This is free software: you are free to change and redistribute it.\n";
    std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
    std::cout << "Inspired by the Free Software Foundation philosophy.\n";
}

enum class WipeError : unsigned char { None, OpenFailed, WriteFailed };

void wipe(const char* filename, WipeError& error) {
    unsigned long long filesize = fs::file_size(filename);

    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);

    if (!file) {
        error = WipeError::OpenFailed;
        return;
    }

    const unsigned long long buffer_size =
        std::min(std::max(1024ULL, filesize / 1024ULL), 68719476736ULL); // between 1 KB and 64 MB
    
    char *buffer = new char[buffer_size];
    std::memset(buffer, '\0', buffer_size);

    unsigned long long remaining = filesize;

    while (remaining > 0) {
        unsigned long long chunk_size = std::min<unsigned long long>(buffer_size, remaining);
        file.write(buffer, chunk_size);
        if (!file) {
            error = WipeError::WriteFailed;
            file.close();
            delete[] buffer;
            return;
        }
        remaining -= chunk_size;

        std::cout << "\r" << filename << ": "
                  << std::fixed << std::setprecision(2)
                  << ((filesize - remaining) * 100.0 / filesize)
                  << "% " << std::flush;
    }

    file.flush();
    file.close();
    delete[] buffer;

    error = WipeError::None;
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

    bool force = false;
    std::vector<char*> files;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0) {
            force = true;
        } else {
            files.push_back(argv[i]);
        }
    }

    if (files.empty()) {
        std::cout << argv[0] << ": missing operand\n";
        return 1;
    }

    for (const char* file : files) {
        if (!force) {
            std::string response;

            while (true)
            {
                std::cout << argv[0] << ": are you sure to wipe '" << file << "'? (y/n): ";
                std::getline(std::cin, response);
                if (response == "y" || response == "Y" || response == "n" || response == "N") {
                    break;
                }
                std::cout << argv[0] << ": invalid response\n";
            }

            if (response == "n" || response == "N") {
                continue;
            }
        }

        if (fs::is_directory(file)) {
            std::cout << argv[0] << ": cannot wipe '" << file << "': Is a directory\n";
            continue;
        } else if (!fs::is_regular_file(file)) {
            std::cout << argv[0] << ": cannot wipe '" << file << "': No such file or directory\n";
            continue;
        }

        WipeError error;
        wipe(file, error);
        std::cout << "\n";
        
        switch (error) {
        case WipeError::None:
            break;
        case WipeError::OpenFailed:
            std::cout << argv[0] << ": cannot wipe '" << file << "': Failed to open the file\n";
            break;
        case WipeError::WriteFailed:
            std::cout << argv[0] << ": cannot wipe '" << file << "': Failed while wiping file\n";
            break;
        default:
            std::cout << argv[0] << ": cannot wipe '" << file << "': Unknown error\n";
            break;
        }
    }
}
