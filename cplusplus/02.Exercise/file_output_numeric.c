#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char **argv) {
    char *filename = "file_output_numeric.txt";
    FILE *fd;
    int ch;
    int is_whitespace = 0;

    if (argc == 2) {
        filename = argv[1];
    }
    if ((fd = fopen(filename, "rt")) == 0) {
        fprintf(stderr, "File '%s' can't open\n", filename);
        return EXIT_FAILURE;
    }
    while ((ch = fgetc(fd)) != EOF) {
        if (isspace(ch)) {
            if (0 == is_whitespace) {
                fprintf(stdout, " ");
                is_whitespace = 1;
            }
        } else if (isdigit(ch)) {
            is_whitespace = 0;
            fprintf(stdout, "%c", ch);
        }
    }
    fclose(fd);
    return EXIT_SUCCESS;
}
