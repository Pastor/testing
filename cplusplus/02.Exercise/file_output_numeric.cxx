#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
    char *filename = "file_output_numeric.txt";
    char ch;
    bool is_whitespace = false;
    if (argc == 2) {
        filename = argv[1];
    }
    std::fstream fin(filename, std::fstream::in);

    if (!fin.is_open()) {
        std::cout << "File '%s' can't open" << filename << std::endl;
        return EXIT_FAILURE;
    }

    while (fin >> std::noskipws >> ch) {
        if (isspace(ch)) {
            if (!is_whitespace) {
                std::cout << " ";
                is_whitespace = true;
            }
        } else if (isdigit(ch)) {
            is_whitespace = false;
            std::cout << ch;
        }
    }
    return EXIT_SUCCESS;
}
