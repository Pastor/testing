#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT       "0987654321dcba"
#define TEXT_LEN   sizeof(TEXT)

static int character_compare(const void *ch1, const void *ch2) {
    return (*(char *) ch1) >= (*(char *) ch2);
}

static void bubble_sort(char *text, size_t text_len) {
    char temp;
    for (size_t i = 0; i < text_len - 1; ++i) {
        for (size_t j = 0; j < text_len - i - 1; j++) {
            if (text[j] > text[j + 1]) {
                temp = text[j];
                text[j] = text[j + 1];
                text[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char **argv) {
    char text[TEXT_LEN + 1];

    memcpy(text, TEXT, TEXT_LEN);
    text[TEXT_LEN] = 0;
    fprintf(stdout, "Quick sort.\n");
    fprintf(stdout, "Before    : '%s'\n", text);
    qsort(text, strlen(text), sizeof(text[0]), character_compare);
    fprintf(stdout, "After     : '%s'\n", text);

    memcpy(text, TEXT, TEXT_LEN);
    text[TEXT_LEN] = 0;
    fprintf(stdout, "Bubble sort.\n");
    fprintf(stdout, "Before    : '%s'\n", text);
    bubble_sort(text, strlen(text));
    fprintf(stdout, "After     : '%s'\n", text);
    fflush(stdout);
    return EXIT_SUCCESS;
}
