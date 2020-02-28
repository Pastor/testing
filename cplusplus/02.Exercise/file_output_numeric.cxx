#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>

/**
 * Исправьте код так, чтобы в выводе результата убрать первый пробельный символ
 */

int main(int argc, char **argv) {
    char *filename = "file_output_numeric.txt";
    char ch;
    bool is_whitespace = false;
    if (argc == 2) {
        filename = argv[1];
    }
    std::fstream fin(filename, std::fstream::in);

    if (!fin.is_open()) {
        std::cerr << "File '" << filename << "' can't open" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "File '" << filename << "' was opened." << std::endl;

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
