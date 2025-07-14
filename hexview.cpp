/*
Build:
    g++ hexview.cpp -std=c++17 -O2 -s -o hexview
*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>

namespace fs = std::filesystem;

void show_help(char* argv0) {
    std::cout << "Usage: " << argv0 << " [OPTION]... [FILE]...\n";
    std::cout << "Output the bytes of one file in hexadecimal format.\n";
    std::cout << "\n";
    std::cout << "  -c NUM              show NUM columns in the output\n";
    std::cout << "  -r NUM              show NUM rows in the output\n";
    std::cout << "  -o NUM              start reading NUM bytes after the beginning of the file\n";
    std::cout << "\n";
    std::cout << "      --help          display this help and exit\n";
    std::cout << "      --version       output version information and exit\n";
}

void show_version() {
    std::cout << "hexview 1.0\n";
    std::cout << "\n";
    std::cout << "This is free software: you are free to change and redistribute it.\n";
    std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
    std::cout << "Inspired by the Free Software Foundation philosophy.\n";
}

void hexview(char* filepath, unsigned short columns, unsigned long long rows, unsigned long long offset, bool& error) {
    static const char hex_chars[17] = "0123456789ABCDEF";

    std::ifstream file(filepath, std::ios::binary);

    error = false;
    if (!file) {
        error = true;
        return;
    }

    unsigned short used_columns = 0;
    unsigned long long used_rows = 0;
    char byte;
    char hex[3] = "  ";

    file.seekg(offset, std::ios::beg);

    while (file.get(byte) && used_rows < rows) {
        std::cout << hex_chars[(byte >> 4) & 0x0F] << hex_chars[byte & 0x0F];

        if (++used_columns == columns) {
            used_columns = 0;
            used_rows++;
            std::cout << '\n';
        } else {
            std::cout << ' ';
        }
    }
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

    unsigned short columns = 16;
    unsigned long long rows = 8;
    unsigned long long offset = 0;
    char* file = nullptr;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            if (argc == i+1) {
                std::cout << argv[0] << ": option requires an argument -- 'c'\n";
                return 1;
            }

            int n;

            try {
                n = std::stoi(argv[i+1]);
            } catch (const std::invalid_argument& e) {
                std::cout << argv[0] << ": invalid number of columns: '" << argv[i+1] << "'\n";
                return 1;
            } catch (const std::out_of_range& e) {
                std::cout << argv[0] << ": invalid number of columns: '" << argv[i+1] << "': Value too large for defined data type\n";
                return 1;
            }
            if (n < 0 || n > 65535) {
                std::cout << argv[0] << ": invalid number of columns: '" << argv[i+1] << "': Value too large for defined data type\n";
                return 1;
            }

            columns = n;
            i++;
        } else if (strcmp(argv[i], "-r") == 0) {
            if (argc == i+1) {
                std::cout << argv[0] << ": option requires an argument -- 'r'\n";
                return 1;
            }

            try {
                rows = std::stoull(argv[i+1]);
            } catch (const std::invalid_argument& e) {
                std::cout << argv[0] << ": invalid number of rows: '" << argv[i+1] << "'\n";
                return 1;
            } catch (const std::out_of_range& e) {
                std::cout << argv[0] << ": invalid number of rows: '" << argv[i+1] << "': Value too large for defined data type\n";
                return 1;
            }

            i++;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (argc == i+1) {
                std::cout << argv[0] << ": option requires an argument -- 'o'\n";
                return 1;
            }

            try {
                offset = std::stoull(argv[i+1]);
            } catch (const std::invalid_argument& e) {
                std::cout << argv[0] << ": invalid offset: '" << argv[i+1] << "'\n";
                return 1;
            } catch (const std::out_of_range& e) {
                std::cout << argv[0] << ": invalid offset: '" << argv[i+1] << "': Value too large for defined data type\n";
                return 1;
            }

            i++;
        } else {
            if (file) {
                std::cout << argv[0] << ": extra file operand '" << argv[i] << "'\n";
                return 1;
            }
            file = argv[i];
        }
    }

    if (!file) {
        std::cout << argv[0] << ": missing operand\n";
    }

    if (fs::is_directory(file)) {
        std::cout << argv[0] << ": cannot hexview '" << file << "': Is a directory\n";
        return 1;
    } else if (!fs::is_regular_file(file)) {
        std::cout << argv[0] << ": cannot hexview '" << file << "': No such file or directory\n";
        return 1;
    }

    bool error = false;
    hexview(file, columns, rows, offset, error);
    if (error) {
        std::cout << argv[0] << ": cannot hexview '" << file << "': Failed to open the file\n";
    }

    return 0;
}