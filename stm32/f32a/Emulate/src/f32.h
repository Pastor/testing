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

#define P9          0x200    // P9 bit
#define SIGN_BIT    0x20000  // 18 bit sign bit
#define MASK3       0x7
#define MASK8       0xff
#define MASK9       0x1ff
#define MASK10      0x3ff
#define MASK18      0x3ffff
#define MASK19      0x7ffff
#define IMASK       0x15555  // exeucte/compiler mask

#define INS_RETURN     0x00   // ;
#define INS_EXECUTE    0x01   // ex
#define INS_PJUMP      0x02   // jump <10-bit>
#define INS_PCALL      0x03   // call <10-bit>
#define INS_UNEXT      0x04   // micronext
#define INS_NEXT       0x05   // next <10-bit>
#define INS_IF         0x06   //  if  <10-bit>
#define INS_MINUS_IF   0x07   // -if  <10-bit>
#define INS_FETCH_P    0x08   // @p
#define INS_FETCH_PLUS 0x09   // @+
#define INS_FETCH_B    0x0a   // @b
#define INS_FETCH      0x0b   // @
#define INS_STORE_P    0x0c   // !p
#define INS_STORE_PLUS 0x0d   // !+
#define INS_STORE_B    0x0e   // !b
#define INS_STORE      0x0f   // !
#define INS_MULT_STEP  0x10   // +*
#define INS_TWO_STAR   0x11   // 2*
#define INS_TWO_SLASH  0x12   // 2/
#define INS_NOT        0x13   // -
#define INS_PLUS       0x14   // +
#define INS_AND        0x15   // and
#define INS_OR         0x16   // or 	ALU 	1.5 	(exclusive or)
#define INS_DROP       0x17   // drop ALU 	1.5
#define INS_DUP        0x18   // dup 	ALU 	1.5
#define INS_POP        0x19   // pop 	ALU 	1.5
#define INS_OVER       0x1a   // over 	ALU 	1.5
#define INS_A          0x1b   // a 	ALU 	1.5 	(A to T)
#define INS_NOP        0x1c   // . 	ALU 	1.5 	“nop”
#define INS_PUSH       0x1d   // push 	ALU 	1.5 	(from T to R)
#define INS_B_STORE    0x1e   // b! 	ALU 	1.5 	“b-store” (store into B)
#define INS_A_STORE    0x1f   // a! 	ALU 	1.5 	“a-store” (store into A)

#define SIGNED18(v)  (((int32_t)((v)<<14))>>14)

#define FLAG_VERBOSE      0x00001
#define FLAG_TRACE        0x00002
#define FLAG_TERMINATE    0x00004
#define FLAG_DUMP_REG     0x00010
#define FLAG_DUMP_RAM     0x00020
#define FLAG_DUMP_RS      0x00040
#define FLAG_DUMP_DS      0x00080
#define FLAG_RD_BIN_RIGHT 0x00800
#define FLAG_RD_BIN_DOWN  0x00400
#define FLAG_RD_BIN_LEFT  0x00200
#define FLAG_RD_BIN_UP    0x00100
#define FLAG_WR_BIN_RIGHT 0x08000
#define FLAG_WR_BIN_LEFT  0x04000
#define FLAG_WR_BIN_DOWN  0x02000
#define FLAG_WR_BIN_UP    0x01000

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


#define TOKEN_ERROR     -1
#define TOKEN_EMPTY     0
#define TOKEN_MNEMONIC1 1
#define TOKEN_MNEMONIC2 2
#define TOKEN_VALUE     3

int parse_instruction(char **p_pointer, u18 *p_instruction, u18 *p_destination);


#ifdef DEBUG
#define VERBOSE(np, fmt, ...) do {            \
    if ((np)->flags & FLAG_VERBOSE)            \
        print(fmt, __VA_ARGS__);        \
    } while(0)
#define TRACE(np, fmt, ...) do {                \
    if ((np)->flags & FLAG_TRACE)                \
        print(fmt, __VA_ARGS__);        \
    } while(0)
#define DELAY(np) do {               \
    if ((np)->delay)                \
        usleep((np)->delay);        \
    } while(0)
#undef DELAY
#define DELAY(np)
#else
#define VERBOSE(np, fmt, ...)
#define TRACE(np, fmt, ...)
#define DELAY(np)
#endif

#endif //F32A_F32_H
