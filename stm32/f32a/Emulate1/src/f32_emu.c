#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <io.h>
#include "f32.h"

extern char *f18_instruction_name[];
static int fd;

void print(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}


static void write_io_register(struct Node *node, u18 io_register, u18 value) {
    switch (io_register) {
        case IOREG_USART: {
            fprintf(stderr, "%c", value);
            break;
        }
        default:
            fprintf(stderr, "[%08x] %c", io_register, value);
            break;
    }
}

static u18 read_io_register(struct Node *node, u18 io_register) {
    char buf[256];
    char *ptr;
    u18 instruction;
    u18 instruction_x;
    u18 dest;
    int i;
    size_t r;

    again:
    // read line from fd until '\n' is found or buffer overflow
    // VERBOSE(node,"read line fd=%d\n", fd);
    if (fd != 0 && (node->flags & FLAG_TERMINATE) && io_register != IOREG_USART)
        return 0;


    // Interpreter mode channel
    i = 0;
    while (i < (sizeof(buf) - 1)) {
        r = read(fd, &buf[i], 1);
        if (r == 0) {  // input stream has closed
            if (fd != 0)  // it was stdin !
                node->flags |= FLAG_TERMINATE;  // lets terminate
            break;
        } else if (r < 0)
            goto error;
        else if (buf[i] == '\n')
            break;
        i++;
    }
    buf[i] = '\0';
    VERBOSE(node, "TXT[FD%04d]<--[%s]\n", fd, buf);
    ptr = buf;
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY:
            goto again;
        case TOKEN_MNEMONIC1:
            instruction = (instruction_x << 13);
            break;
        case TOKEN_MNEMONIC2:
            // instruction part is encoded (^IMASK) but dest is not (why)
            instruction = (instruction_x << 13) ^ IMASK;                // encode instruction
            instruction = (instruction & ~MASK10) | (dest & MASK10);  // set address bits
            return instruction;
        case TOKEN_VALUE:
            return (instruction_x & MASK18);  // value not encoded
        default:
            goto error;
    }
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY: // assume rest of opcode are nops (warn?)
            instruction = (instruction | (INS_NOP << 8) | (INS_NOP << 3) | (INS_NOP >> 2)) ^ IMASK;
            return instruction;
        case TOKEN_MNEMONIC1:
            instruction |= (instruction_x << 8);
            break;
        case TOKEN_MNEMONIC2:
            instruction = (instruction | (instruction_x << 8)) ^ IMASK;      // encode instruction
            instruction = (instruction & ~MASK8) | (dest & MASK8);  // set address bits
            return instruction;
        default:
            goto error;
    }
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY:
            instruction = (instruction | (INS_NOP << 3) | (INS_NOP >> 2)) ^ IMASK;
            return instruction;
        case TOKEN_MNEMONIC1:
            instruction |= (instruction_x << 3);
            break;
        case TOKEN_MNEMONIC2:
            instruction = (instruction | (instruction_x << 3)) ^ IMASK;      // encode instruction
            instruction = (instruction & ~MASK3) | (dest & MASK3);  // set address bits
            return instruction;
        default:
            goto error;
    }
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY:
            instruction = (instruction | (INS_NOP >> 2)) ^ IMASK;
            return instruction;
        case TOKEN_MNEMONIC1:
            if ((instruction_x & 3) != 0)
                fprintf(stderr, "scan error: bad slot3 instruction used %s\n",
                        f18_instruction_name[instruction_x]);
            instruction = (instruction | (instruction_x >> 2)) ^ IMASK; // add op and encode
            return instruction;
        default:
            goto error;
    }

    error:
    fprintf(stderr, "io error when reading ioreg=%x, error=%s\n",
            io_register, strerror(errno));
    return 0;
}

void f18_initialize(struct Node *node, int argc, char **argv) {
    memset(node, 0, sizeof(struct Node));
    node->read = read_io_register;
    node->write = write_io_register;
    node->b = IOREG_USART;
    node->p = IOREG_RDLU;
    node->flags = FLAG_VERBOSE | FLAG_TRACE | FLAG_DUMP_DS | FLAG_DUMP_RS | FLAG_DUMP_REG;
    fd = open(argv[1], 0);
}

int main(int argc, char **argv) {
    struct Node node = {0};

    f18_initialize(&node, argc, argv);
    f18_emulate(&node);
    return EXIT_SUCCESS;
}

