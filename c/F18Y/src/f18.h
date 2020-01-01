#ifndef F18Y_F18_H
#define F18Y_F18_H

#include <stdint.h>
#include <stdio.h>

typedef uint32_t u18;
typedef uint16_t u9;
typedef uint16_t u10;
typedef uint8_t u5;
typedef uint8_t u8;
typedef uint8_t u3;

struct Node {
    u18 ram[64];
    const u18 rom[64];
    u18 io;       // io status register (read)

    unsigned int delay;       // delay between instructions
    u18 flags;         // flags,debug,trace...
    int fd;

    u18 ds_t;            // top of data stack
    u18 ds_s;            // second of data stack
    u18 ds[8];        // data stack
    u3 ds_pointer;           // data stack pointer

    u18 rs_r;            // return top of return stack
    u18 rs[8];        // return stack
    u3 rs_pointer;           // return stack pointer

    u18 i;           // instruction register
    u10 p;           // program counter
    u18 a;           // address register
    u9 b;
    u8 c;           // carry flag

    u18  (*read )(struct Node *node, u18 reg);
    void (*write)(struct Node *node, u18 reg, u18 value);
};

void f18_emulate(struct Node *node);

void f18_initialize(struct Node *node, int argc, char **argv);

#endif //F18Y_F18_H
