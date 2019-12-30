#include <stdlib.h>
#include <stdint.h>
#include "f18.h"

int main(int argc, char **argv) {
    struct Node node = {0};

    f18_initialize(&node, argc, argv);
    f18_emulate(&node);
    return EXIT_SUCCESS;
}

