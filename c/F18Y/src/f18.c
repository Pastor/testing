#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "f18.h"

#if defined(WIN32)

#include <windows.h>
#include <io.h>

#define usleep(p) Sleep((p) / 1000)
#else
#include <unistd.h>
#endif

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

// address layout in binary
// 000000000 - 000111111    RAM
// 001000000 - 001111111    RAM*  (repeat)
// 010000000 - 010111111    ROM
// 011000000 - 011111111    ROM*  (repeat)
// 100000000 - 111111111    IOREG

#define RAM_START   0x000
#define RAM_END     0x03f
#define RAM_END2    0x07f
#define ROM_START   0x080
#define ROM_END     0x0BF
#define ROM_END2    0x0FF
#define IOREG_START 0x100
#define IOREG_END   0x1FF

#define IOREG_STDIN   0x100  // test/debug
#define IOREG_STDOUT  0x101  // test/debug
#define IOREG_STDIO   0x102  // test/debug
#define IOREG_TTY     0x103  // test/debug

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

// IO register number coding
#define F18_DIR_BITS      0x105  // Direction pattern
#define F18_DIR_MASK      0x10F  // Direction pattern
#define F18_RIGHT_BIT     0x080  // right when 1
#define F18_DOWN_BIT      0x040  // down when 0
#define F18_LEFT_BIT      0x020  // left when 1
#define F18_UP_BIT        0x010  // up when 0

#define F18_IO_PIN17      0x20000
// port status bits in io register
#define F18_IO_RIGHT_RD   0x10000
#define F18_IO_RIGHT_WR   0x08000
#define F18_IO_DOWN_RD    0x04000
#define F18_IO_DOWN_WR    0x02000
#define F18_IO_LEFT_RD    0x01000
#define F18_IO_LEFT_WR    0x00800
#define F18_IO_UP_RD      0x00400
#define F18_IO_UP_WR      0x00200
// ...
#define F18_IO_PIN5       0x00020
#define F18_IO_PIN3       0x00008
#define F18_IO_PIN1       0x00002

#define P9          0x200    // P9 bit
#define SIGN_BIT    0x20000  // 18 bit sign bit
#define MASK3       0x7
#define MASK8       0xff
#define MASK9       0x1ff
#define MASK10      0x3ff
#define MASK18      0x3ffff
#define MASK19      0x7ffff
#define IMASK       0x15555  // exeucte/compiler mask

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

struct Instruction {
    union {
        u18 base;
        struct {
            unsigned int in03: 3;
            unsigned int in02: 5;
            unsigned int in01: 5;
            unsigned int in00: 5;
            unsigned int padding: 14;
        } parts;
        struct {
            unsigned int b17: 1;
            unsigned int b16: 1;
            unsigned int b15: 1;
            unsigned int b14: 1;
            unsigned int b13: 1;
            unsigned int b12: 1;
            unsigned int b11: 1;
            unsigned int b10: 1;
            unsigned int b09: 1;
            unsigned int b08: 1;
            unsigned int b07: 1;
            unsigned int b06: 1;
            unsigned int b05: 1;
            unsigned int b04: 1;
            unsigned int b03: 1;
            unsigned int b02: 1;
            unsigned int b01: 1;
            unsigned int b00: 1;
            unsigned int padding: 14;
        } bits;
    };
};
static char instruction_text[60];

static void put_instruction(u18 v) {
    struct Instruction i = {0};
    i.base = v ^ IMASK;
    sprintf(instruction_text, "%08X, %u%u%u%u%u %u%u%u%u%u %u%u%u%u%u %u%u%u, %02X %02X %02X %02X", v, i.bits.b00,
            i.bits.b01,
            i.bits.b02, i.bits.b03,
            i.bits.b04, i.bits.b05, i.bits.b06, i.bits.b07, i.bits.b08, i.bits.b09, i.bits.b10, i.bits.b11, i.bits.b12,
            i.bits.b13, i.bits.b14, i.bits.b15, i.bits.b16, i.bits.b17, i.parts.in00, i.parts.in01, i.parts.in02,
            i.parts.in03);
}

#ifdef DEBUG
#define VERBOSE(np, fmt, ...) do {            \
    if ((np)->flags & FLAG_VERBOSE)            \
        fprintf(stderr, fmt, __VA_ARGS__);        \
        fflush(stderr);        \
    } while(0)
#define TRACE(np, fmt, ...) do {                \
    if ((np)->flags & FLAG_TRACE)                \
        fprintf(stderr, fmt, __VA_ARGS__);        \
        fflush(stderr);        \
    } while(0)
#define DELAY(np) do {                \
    if ((np)->delay)            \
        usleep((np)->delay);        \
    } while(0)
#else
#define VERBOSE(np, fmt, ...)
#define TRACE(np, fmt, ...)
#define DELAY(np)
#endif

// I do not think it matters in the emulator which way the posh and pop goes
#define PUSH_ds(np, val) (np)->ds[(np)->dp++ & 0x7] = (val)
#define POP_ds(np)      (np)->ds[--(np)->dp & 0x7]

#define PUSH_s(np, val) do {            \
    PUSH_ds((np), (np)->s);            \
    (np)->s = (np)->t;                \
    (np)->t = (val);                \
    } while(0)

#define POP_s(np) do {                \
    (np)->t = (np)->s;                \
    (np)->s = POP_ds(np);            \
    } while(0)

#define PUSH_rs(np, val) (np)->rs[(np)->rp++ & 0x7] = (val)
#define POP_rs(np)      (np)->rs[--(np)->rp & 0x7]

#define PUSH_r(np, val) do {            \
    PUSH_rs((np), (np)->r);            \
    (np)->r = (val);                \
    } while(0)

#define POP_r(np) do {                \
    (np)->r = POP_rs(np);            \
    } while(0)

#define swap18(a, b) do {            \
    u18 _swap18_t1 = (a);            \
    (a) = (b);                    \
    (b) = _swap18_t1;                \
    } while(0)

#ifdef DEBUG
#define DUMP(np) do {                    \
    if ((np)->flags & FLAG_DUMP_REG) dump_reg((np));    \
    if ((np)->flags & FLAG_DUMP_DS)  dump_ds((np));    \
    if ((np)->flags & FLAG_DUMP_RS)  dump_rs((np));    \
    if ((np)->flags & FLAG_DUMP_RAM) dump_ram((np));    \
    } while(0)
#else
#define DUMP(np)
#endif

static char *f18_instruction_name[] = {
        ";", "ex", "jump", "call", "unext", "next", "if", "-if",
        "@p", "@+", "@b", "@", "!p", "!+", "!b", "!",
        "+*", "2*", "2/", "-", "+", "and", "or", "drop",
        "dup", "pop", "over", "a", ".", "push", "b!", "a!"
};

int parse_mnemonic(char *word, int n) {
    int i;
    for (i = 0; i < 32; i++) {
        size_t len = strlen(f18_instruction_name[i]);
        if ((n == len) && (memcmp(word, f18_instruction_name[i], n) == 0))
            return i;
    }
    return -1;
}

struct {
    char *name;
    u18 value;
} symbol[] = {
        {"stdio",  IOREG_STDIO},
        {"stdin",  IOREG_STDIN},
        {"stdout", IOREG_STDOUT},
        {"tty",    IOREG_TTY},

        {"io",     IOREG_IO},
        {"data",   IOREG_DATA},
        {"---u",   IOREG____U},
        {"--l-",   IOREG___L_},
        {"--lu",   IOREG___LU},
        {"-d--",   IOREG__D__},
        {"-d--u",  IOREG__D_U},
        {"-dl-",   IOREG__DL_},
        {"-dlu",   IOREG__DLU},
        {"r---",   IOREG_R___},
        {"r--u",   IOREG_R__U},
        {"r-l-",   IOREG_R_L_},
        {"r-lu",   IOREG_R_LU},
        {"rd--",   IOREG_RD__},
        {"rd-u",   IOREG_RD_U},
        {"rdl-",   IOREG_RDL_},
        {"rdlu",   IOREG_RDLU},
        {NULL, 0}
};

int parse_symbol(char **p_pointer, u18 *p_register) {
    int i = 0;
    char *ptr = *p_pointer;

    while (symbol[i].name != NULL) {
        size_t n = strlen(symbol[i].name);
        if (strncmp(ptr, symbol[i].name, n) == 0) {
            if ((ptr[n] == '\0') || isblank(ptr[n])) {
                *p_pointer = ptr + n;
                *p_register = symbol[i].value;
                return 1;
            }
        }
        i++;
    }
    return 0;
}

#define TOKEN_ERROR     -1
#define TOKEN_EMPTY     0
#define TOKEN_MNEMONIC1 1
#define TOKEN_MNEMONIC2 2
#define TOKEN_VALUE     3

//
// parse:
//
//   ( '(' .* ')' )* <mnemonic>
//   ( '(' .* ')' )* <mnemonic>':'<dest>
//   ( '(' .* ')' )* <hex>
//   ( '(' .* ')' )* \<blank> .*
//
int parse_instruction(char **p_pointer, u18 *p_instruction, u18 *p_destination) {
    char *ptr = *p_pointer;
    char *word;
    u18 value = 0;
    int n = 0;
    int instruction;
    int has_dest = 0;

    while (isblank(*ptr) || (*ptr == '(')) {
        while (isblank(*ptr)) ptr++;
        if (*ptr == '(') {
            ptr++;
            while (*ptr && (*ptr != ')'))
                ptr++;
            if (*ptr) ptr++;
        }
    }

    if ((*ptr == '\\') && (isblank(*(ptr + 1)) || (*(ptr + 1) == '\0'))) {
        while (*ptr != '\0') ptr++;  // skip rest
    }
    word = ptr;
    while (*ptr && !isblank(*ptr) && (*ptr != ':')) {
        ptr++;
        n++;
    }
    if (n == 0) return TOKEN_EMPTY;
    instruction = parse_mnemonic(word, n);
    switch (instruction) {
        case -1:
            has_dest = 0;
            ptr = word;
            break;
        case INS_PJUMP:
        case INS_PCALL:
        case INS_NEXT:
        case INS_IF:
        case INS_MINUS_IF:
            if (*ptr == ':') { // force?
                has_dest = 1;
                ptr++;
            }
            break;
        default:
            has_dest = 0;
            break;
    }

    if (parse_symbol(&ptr, &value))
        n = 1;
    else {
        n = 0;
        while (isxdigit(*ptr)) {
            value <<= 4;
            if ((*ptr >= '0') && (*ptr <= '9'))
                value += (*ptr - '0');
            else if ((*ptr >= 'A') && (*ptr <= 'F'))
                value += ((*ptr - 'A') + 10);
            else
                value += ((*ptr - 'a') + 10);
            ptr++;
            n++;
        }
    }
    *p_pointer = ptr;
    if (instruction >= 0) {
        *p_instruction = instruction;
        *p_destination = value;
        if (has_dest && (n > 0))
            return TOKEN_MNEMONIC2;
        return TOKEN_MNEMONIC1;
    }
    if ((n == 0) || !(isblank(*ptr) || (*ptr == '\0')))
        return TOKEN_ERROR;
    *p_instruction = value;
    return TOKEN_VALUE;
}

static void write_io_register(struct Node *node, u18 io_register, u18 value) {
    switch (io_register) {
        case IOREG_TTY: {
            fprintf(stdout, "%c", value);
            break;
        }
        default:
            fprintf(stdout, "[%08x] %c", io_register, value);
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
    if (node->fd != 0 && (node->flags & FLAG_TERMINATE))
        return 0;


    // Interpreter mode channel
    i = 0;
    while (i < (sizeof(buf) - 1)) {
        r = read(node->fd, &buf[i], 1);
        if (r == 0) {  // input stream has closed
            if (node->fd != 0)  // it was stdin !
                node->flags |= FLAG_TERMINATE;  // lets terminate
            break;
        } else if (r < 0)
            goto error;
        else if (buf[i] == '\n')
            break;
        i++;
    }
    buf[i] = '\0';
    VERBOSE(node, "TXT[FD%04d]<--[%s]\n", node->fd, buf);
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


static void dump_ram(struct Node *np) {
    int i;
    for (i = RAM_START; i <= RAM_END; i++)
        fprintf(stderr, "ram[%d]=%05x\n", i, np->ram[i]);
}

static void dump_ds(struct Node *np) {
    int i;
    fprintf(stderr, "t=%05x,s=%05x", np->t, np->s);
    for (i = 0; i < 8; i++)
        fprintf(stderr, ",%05x", np->ds[(np->dp + i - 1) & 0x7]);
    fprintf(stderr, "\n");
}


static void dump_rs(struct Node *np) {
    int i;
    fprintf(stderr, "r=%05x", np->r);
    for (i = 0; i < 8; i++)
        fprintf(stderr, ",%05x", np->rs[(np->rp + i - 1) & 0x7]);
    fprintf(stderr, "\n");
}

static void dump_reg(struct Node *np) {
    fprintf(stderr, "t=%05x,a=%05x,b=%03x,c=%x,p=%x,i=%x,s=%05x,r=%05x\n",
            np->t, np->a, np->b, np->c, np->p, np->i, np->s, np->r);
}

// read value of P return the current value and
// perform auto increment if needed.

static u9 p_auto(struct Node *node) {
    u9 p = node->p & MASK9;  // strip P(9)
    if ((p >= RAM_START) && (p <= RAM_END2))
        node->p = ((p + 1) & 0x7f) | (node->p & P9);
    else if ((p >= ROM_START) && (p <= ROM_END2))
        node->p = (ROM_START + (((p - ROM_START) + 1) & 0x7f)) | (node->p & P9);
    return p;
}

static u9 a_auto(struct Node *np) {
    u9 a = np->a & MASK9;

    if ((a >= RAM_START) && (a <= RAM_END2))
        np->a = ((a + 1) & 0x7f);
    else if ((a >= ROM_START) && (a <= ROM_END2))
        np->a = (ROM_START + (((a - ROM_START) + 1) & 0x7f));
    return a;
}

static u18 read_mem(struct Node *np, u18 addr) {
    u18 value;
    if (addr <= RAM_END2) {
        value = np->ram[addr & 0x3f];
        put_instruction(value);
        VERBOSE(np, "RAM[%06x]<--[%s]\n", addr, instruction_text);
    } else if (addr <= ROM_END2) {
        value = np->rom[(addr - ROM_START) & 0x3f];
        put_instruction(value);
        VERBOSE(np, "ROM[%06x]<--[%s]\n", addr, instruction_text);
    } else {
        value = (*np->read)(np, addr);
        put_instruction(value);
        VERBOSE(np, "REG[%06x]<--[%s]\n", addr, instruction_text);
    }
    return value;
}

static void write_mem(struct Node *node, u18 addr, u18 value) {
    if (addr <= RAM_END2) {
        node->ram[addr & 0x3f] = value;
//        VERBOSE(node, "write ram[%04x] = %02x %02x %02x %02x = %x\n",
//                addr & 0x3f,
//                (value >> 13) & 0x1f,
//                (value >> 8) & 0x1f,
//                (value >> 3) & 0x1f,
//                (value << 2) & 0x1f,
//                value);
        put_instruction(value);
        VERBOSE(node, "RAM[%06x]-->[%s]\n", addr, instruction_text);
    } else if (addr <= ROM_END2) {
        put_instruction(value);
        VERBOSE(node, "ROM[%06x]-->[%s]\n", addr, instruction_text);
        fprintf(stderr, "warning: try to write in ROM area %x, value=%d\n",
                addr, value);
        // node->rom[(addr-ROM_START) & 0x3f] = value;
    } else {
        put_instruction(value);
        (*node->write)(node, addr, value);
        VERBOSE(node, "REG[%06x]-->[%s]\n", addr, instruction_text);
    }
}

void f18_emulate(struct Node *node) {
    u18 I;
    uint32_t II;
    int n;
    u5 ins;

    next:
    DUMP(node);
    node->i = read_mem(node, p_auto(node));
    if (node->flags & FLAG_TERMINATE)
        return;
    I = node->i ^ IMASK; // "decode" instruction (why?)

    restart:
    II = I << 2;
    n = 4;
    unext:
    DUMP(node);
    ins = (II >> 15) & 0x1f;
    TRACE(node, "EXE[%06X]<--[%s]\n", ins, f18_instruction_name[ins]);
    DELAY(node);

    switch (ins) {
        case INS_RETURN:
            node->p = node->r;
            POP_r(node);
            goto next;

        case INS_EXECUTE:
            swap18(node->p, node->r);
            node->p &= MASK10;   // maske sure P is 10 bits
            goto next;

        case INS_PJUMP:
            goto load_p;

        case INS_PCALL:
            PUSH_r(node, node->p);
            goto load_p;

        case INS_UNEXT:
            if (node->r == 0)
                POP_r(node);
            else {
                node->r--;
                goto restart;
            }
            break;

        case INS_NEXT:
            if (node->r == 0)
                POP_r(node);
            else {
                node->r--;
                goto load_p;
            }
            goto next;

        case INS_IF:  // if   ( x -- x ) jump if x == 0
            if (node->t == 0)
                goto load_p;
            goto next;

        case INS_MINUS_IF:  // -if  ( x -- x ) jump if x >= 0
            if (SIGNED18(node->t) >= 0)
                goto load_p;
            goto next;

        case INS_FETCH_P:  //  @p ( -- x ) fetch via P auto-increament
            PUSH_s(node, read_mem(node, p_auto(node)));
            break;

        case INS_FETCH_PLUS:  // @+ ( -- x ) fetch via A auto-increament
            PUSH_s(node, read_mem(node, a_auto(node)));
            break;

        case INS_FETCH_B:  // @b ( -- x ) fetch via B
            PUSH_s(node, read_mem(node, node->b));
            break;

        case INS_FETCH:    // @ ( -- x ) fetch via A
            PUSH_s(node, read_mem(node, node->a));
            break;

        case INS_STORE_P:  // !p ( x -- ) store via P auto increment
            write_mem(node, p_auto(node), node->t);
            POP_s(node);
            break;

        case INS_STORE_PLUS: // !+ ( x -- ) \ write T in [A] pop data stack, inc A
            write_mem(node, a_auto(node), node->t);
            POP_s(node);
            break;

        case INS_STORE_B:  // !b ( x -- ) \ store T into [B], pop data stack
            write_mem(node, node->b, node->t);
            POP_s(node);
            break;

        case INS_STORE:    // ! ( x -- ) \ store T info [A], pop data stack
            write_mem(node, node->a, node->t);
            POP_s(node);
            break;

        case INS_MULT_STEP: { // t:a * s
            int32_t t = SIGNED18(node->t);
            if (node->a & 1) { // sign-extend and add s and t
                t += SIGNED18(node->s);
                if (node->p & P9) {
                    t += node->c;
                    node->c = (t >> 18) & 1;
                }
            }
            node->a = (node->a >> 1) | ((t & 1) << 17);
            node->t = ((t >> 1) | (t & SIGN_BIT)) & MASK18;
            break;
        }

        case INS_TWO_STAR:
            node->t = (node->t << 1) & MASK18;
            break;

        case INS_TWO_SLASH:
            node->t = (node->t >> 1) | (node->t & SIGN_BIT);
            break;

        case INS_NOT:
            node->t = (~node->t) & MASK18;
            break;

        case INS_PLUS: {  // + or +c  ( x y -- (x+y) ) | ( x y -- (x+y+c) )
            int32_t t = SIGNED18(node->t) + SIGNED18(node->s);
            if (node->p & P9) {
                t += node->c;
                node->c = (t >> 18) & 1;
            }
            node->t = t & MASK18;
            node->s = POP_ds(node);
            break;
        }

        case INS_AND: // ( x y -- ( x & y) )
            node->t &= node->s;
            node->s = POP_ds(node);
            break;

        case INS_OR:  // ( x y -- ( x ^ y) )  why not named XOR????
            node->t ^= node->s;
            node->s = POP_ds(node);
            break;

        case INS_DROP:
            node->t = node->s;
            node->s = POP_ds(node);
            break;

        case INS_DUP:  // ( x -- x x )
            PUSH_ds(node, node->s);
            node->s = node->t;
            break;

        case INS_POP:  // push R onto data stack and pop return stack
            PUSH_s(node, node->r);
            node->r = POP_rs(node);
            break;

        case INS_OVER:  // ( x y -- x y x )
            PUSH_ds(node, node->s);
            swap18(node->t, node->s);
            break;

        case INS_A:  // ( -- A )  push? A onto data stack
            PUSH_s(node, node->a);
            break;

        case INS_NOP:
            break;

        case INS_PUSH:  // push T onto return stack and pop data stack
            PUSH_r(node, node->t);
            POP_s(node);
            break;

        case INS_B_STORE:  // b! ( x -- ) store into B
            node->b = node->t;
            POP_s(node);
            break;

        case INS_A_STORE:  // a! ( x -- ) store into A
            node->a = node->t;
            POP_s(node);
            break;
    }
    if (--n == 0)
        goto next;
    II <<= 5;
    goto unext;

    load_p:
    // destination addresses are unencoded and must are retrieved
    // from the "original" i register
    switch (n) {
        case 4:
            node->p = node->i & MASK10;
            break;
        case 3:
            node->p = node->i & MASK8;
            break;
        case 2:
            node->p = node->i & MASK3;
            break;
    }
    goto next;
}

void f18_initialize(struct Node *node, int argc, char **argv) {
    memset(node, 0, sizeof(struct Node));
    node->read = read_io_register;
    node->write = write_io_register;
    node->b = IOREG_TTY;
    node->p = IOREG_RDLU;
    node->fd = open(argv[1], 0);
    node->flags = FLAG_VERBOSE | FLAG_TRACE;
}
