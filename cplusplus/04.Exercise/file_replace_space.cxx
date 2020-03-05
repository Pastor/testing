#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <set>

int main(int argc, char **argv) {
    char *filename = "file_output_numeric.txt";
    std::string replace_from = ";";
    std::string replace_to = "$";
    if (argc >= 2) {
        filename = argv[1];
    }
    if (argc >= 3) {
        replace_from = argv[2];
    }
    if (argc >= 4) {
        replace_to = argv[3];
    }
    std::fstream fin(filename, std::fstream::in);

    if (!fin.is_open()) {
        std::cerr << "File '" << filename << "' can't open" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "File '" << filename << "' was opened." << std::endl;
    std::cout << "Replace from '" << replace_from << "'." << std::endl;
    std::cout << "Replace to   '" << replace_to << "'." << std::endl;

    std::string line_read;
    auto len = replace_from.length();
    while (getline(fin, line_read)) {
        while (true) {
            size_t pos = line_read.find(replace_from);
            if (pos != std::string::npos) {
                line_read.replace(pos, len, replace_to);
            } else {
                break;
            }
        }
        std::cout << line_read << std::endl;
    }
    return EXIT_SUCCESS;
}