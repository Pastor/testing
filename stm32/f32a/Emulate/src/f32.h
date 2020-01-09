#ifndef F32A_F32_H
#define F32A_F32_H

#include <stdint.h>
#include <stdio.h>

#define IOREG_STDIN   0x100
#define IOREG_STDOUT  0x101
#define IOREG_STDIO   0x102
#define IOREG_USART   0x103
#define IOREG_I2C1    0x104
#define IOREG_SPI1    0x105

#define IOREG_IO      0x15D  // i/o control and status
#define IOREG_DATA    0x141
#define IOREG____U    0x145  // up
#define IOREG___L_    0x175  // left
#define IOREG___LU    0x165  // left or up
#define IOREG__D__    0x115  // down
#define IOREG__D_U    0x105
#define IOREG__DL_    0x135
#define IOREG__DLU    0x125
#define IOREG_R___    0x1D5  // right
#define IOREG_R__U    0x1C5
#define IOREG_R_L_    0x1F5
#define IOREG_R_LU    0x1E5
#define IOREG_RD__    0x195
#define IOREG_RD_U    0x185
#define IOREG_RDL_    0x1B5
#define IOREG_RDLU    0x1A5

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

#endif //F32A_F32_H
