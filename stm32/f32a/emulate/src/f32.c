#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "f32.h"

#if defined(WIN32)

#include <windows.h>
#include <io.h>

#define usleep(p) Sleep((p) / 1000)
#else
#include <unistd.h>
#include <fcntl.h>

#endif

extern void print(char *fmt, ...);

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


struct Instruction {
    union {
        u18 base;
        struct {
            u8 in03: 3;
            u8 in02: 5;
            u8 in01: 5;
            u8 in00: 5;
            unsigned int padding: 14;
        } parts;
        struct {
            u8 b17: 1;
            u8 b16: 1;
            u8 b15: 1;
            u8 b14: 1;
            u8 b13: 1;
            u8 b12: 1;
            u8 b11: 1;
            u8 b10: 1;
            u8 b09: 1;
            u8 b08: 1;
            u8 b07: 1;
            u8 b06: 1;
            u8 b05: 1;
            u8 b04: 1;
            u8 b03: 1;
            u8 b02: 1;
            u8 b01: 1;
            u8 b00: 1;
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

// I do not think it matters in the emulator which way the posh and pop goes
#define PUSH_ds(np, val) (np)->ds[(np)->ds_pointer++ & 0x7] = (val)
#define POP_ds(np)      (np)->ds[--(np)->ds_pointer & 0x7]

#define PUSH_s(np, val) do {          \
    PUSH_ds((np), (np)->ds_s);       \
    (np)->ds_s = (np)->ds_t;         \
    (np)->ds_t = (val);              \
    } while(0)

#define POP_s(np) do {                \
    (np)->ds_t = (np)->ds_s;         \
    (np)->ds_s = POP_ds(np);         \
    } while(0)

#define PUSH_rs(np, val) (np)->rs[(np)->rs_pointer++ & 0x7] = (val)
#define POP_rs(np)       (np)->rs[--(np)->rs_pointer & 0x7]

#define PUSH_r(np, val) do {          \
    PUSH_rs((np), (np)->rs_r);       \
    (np)->rs_r = (val);              \
    } while(0)

#define POP_r(np) do {                \
    (np)->rs_r = POP_rs(np);         \
    } while(0)

#define swap18(a, b) do {             \
    u18 _swap18_t1 = (a);            \
    (a) = (b);                       \
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

char *f18_instruction_name[] = {
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
        {"usart",  IOREG_USART},
        {"i2c1",   IOREG_I2C1},
        {"spi1",   IOREG_SPI1},

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

static void dump_ram(struct Node *np) {
    int i;
    for (i = RAM_START; i <= RAM_END; i++)
        fprintf(stderr, "ram[%d]=%05x\n", i, np->ram[i]);
}

static void dump_ds(struct Node *node) {
    int i;
    VERBOSE(node, "DSS[      ]---[%05X, %05X", node->ds_t, node->ds_s);
    for (i = 0; i < 8; i++) {
        VERBOSE(node, ", %05X", node->ds[(node->ds_pointer + i - 1) & 0x7]);
    }
    VERBOSE(node, "]%c", '\n');
}

static void dump_rs(struct Node *node) {
    int i;
    VERBOSE(node, "RSS[      ]---[%05X", node->rs_r);
    for (i = 0; i < 8; i++) {
        VERBOSE(node, ", %05X", node->rs[(node->rs_pointer + i - 1) & 0x7]);
    }
    VERBOSE(node, "]%c", '\n');
}

static void dump_reg(struct Node *node) {
    VERBOSE(node, "REG[      ]---[A=%05X, B=%05X, C=%05X, P=%05X, I=%05X]\n", node->a, node->b, node->c, node->p, node->i);
}

// read value of P return the current value and
// perform auto increment if needed.

static inline u9 p_auto(struct Node *node) {
    u9 p = node->p & MASK9;  // strip P(9)
    if ((p >= RAM_START) && (p <= RAM_END2)) {
        node->p = ((p + 1) & 0x7f) | (node->p & P9);
    } else if ((p >= ROM_START) && (p <= ROM_END2)) {
        node->p = (ROM_START + (((p - ROM_START) + 1) & 0x7f)) | (node->p & P9);
    }
    return p;
}

static inline u9 a_auto(struct Node *np) {
    u9 a = np->a & MASK9;

    if ((a >= RAM_START) && (a <= RAM_END2))
        np->a = ((a + 1) & 0x7f);
    else if ((a >= ROM_START) && (a <= ROM_END2))
        np->a = (ROM_START + (((a - ROM_START) + 1) & 0x7f));
    return a;
}

static u18 read_mem(struct Node *np, u18 reg) {
    u18 value;
    if (reg <= RAM_END2) {
        value = np->ram[reg & 0x3f];
        put_instruction(value);
        VERBOSE(np, "RAM[%06x]<--[%s]\n", reg, instruction_text);
    } else if (reg <= ROM_END2) {
        value = np->rom[(reg - ROM_START) & 0x3f];
        put_instruction(value);
        VERBOSE(np, "ROM[%06x]<--[%s]\n", reg, instruction_text);
    } else {
        value = (*np->read)(np, reg);
        put_instruction(value);
        VERBOSE(np, "VAL[%06x]<--[%s]\n", reg, instruction_text);
    }
    return value;
}

static void write_mem(struct Node *node, u18 reg, u18 value) {
    if (reg <= RAM_END2) {
        node->ram[reg & 0x3f] = value;
        put_instruction(value);
        VERBOSE(node, "RAM[%06x]-->[%s]\n", reg, instruction_text);
    } else if (reg <= ROM_END2) {
        put_instruction(value);
        VERBOSE(node, "ROM[%06x]-->[%s]\n", reg, instruction_text);
        fprintf(stderr, "warning: try to write in ROM area %x, value=%d\n",
                reg, value);
        // node->rom[(reg-ROM_START) & 0x3f] = value;
    } else {
        put_instruction(value);
        (*node->write)(node, reg, value);
        VERBOSE(node, "VAL[%06x]-->[%s]\n", reg, instruction_text);
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
            node->p = node->rs_r;
            POP_r(node);
            goto next;

        case INS_EXECUTE:
            swap18(node->p, node->rs_r);
            node->p &= MASK10;   // maske sure P is 10 bits
            goto next;

        case INS_PJUMP:
            goto load_p;

        case INS_PCALL:
            PUSH_r(node, node->p);
            goto load_p;

        case INS_UNEXT:
            if (node->rs_r == 0)
                POP_r(node);
            else {
                node->rs_r--;
                goto restart;
            }
            break;

        case INS_NEXT:
            if (node->rs_r == 0)
                POP_r(node);
            else {
                node->rs_r--;
                goto load_p;
            }
            goto next;

        case INS_IF:  // if   ( x -- x ) jump if x == 0
            if (node->ds_t == 0)
                goto load_p;
            goto next;

        case INS_MINUS_IF:  // -if  ( x -- x ) jump if x >= 0
            if (SIGNED18(node->ds_t) >= 0)
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
            write_mem(node, p_auto(node), node->ds_t);
            POP_s(node);
            break;

        case INS_STORE_PLUS: // !+ ( x -- ) \ write T in [A] pop data stack, inc A
            write_mem(node, a_auto(node), node->ds_t);
            POP_s(node);
            break;

        case INS_STORE_B:  // !b ( x -- ) \ store T into [B], pop data stack
            write_mem(node, node->b, node->ds_t);
            POP_s(node);
            break;

        case INS_STORE:    // ! ( x -- ) \ store T info [A], pop data stack
            write_mem(node, node->a, node->ds_t);
            POP_s(node);
            break;

        case INS_MULT_STEP: { // ds_t:a * ds_s
            int32_t t = SIGNED18(node->ds_t);
            if (node->a & 1) { // sign-extend and add ds_s and ds_t
                t += SIGNED18(node->ds_s);
                if (node->p & P9) {
                    t += node->c;
                    node->c = (t >> 18) & 1;
                }
            }
            node->a = (node->a >> 1) | ((t & 1) << 17);
            node->ds_t = ((t >> 1) | (t & SIGN_BIT)) & MASK18;
            break;
        }

        case INS_TWO_STAR:
            node->ds_t = (node->ds_t << 1) & MASK18;
            break;

        case INS_TWO_SLASH:
            node->ds_t = (node->ds_t >> 1) | (node->ds_t & SIGN_BIT);
            break;

        case INS_NOT:
            node->ds_t = (~node->ds_t) & MASK18;
            break;

        case INS_PLUS: {  // + or +c  ( x y -- (x+y) ) | ( x y -- (x+y+c) )
            int32_t t = SIGNED18(node->ds_t) + SIGNED18(node->ds_s);
            if (node->p & P9) {
                t += node->c;
                node->c = (t >> 18) & 1;
            }
            node->ds_t = t & MASK18;
            node->ds_s = POP_ds(node);
            break;
        }

        case INS_AND: // ( x y -- ( x & y) )
            node->ds_t &= node->ds_s;
            node->ds_s = POP_ds(node);
            break;

        case INS_OR:  // ( x y -- ( x ^ y) )  why not named XOR????
            node->ds_t ^= node->ds_s;
            node->ds_s = POP_ds(node);
            break;

        case INS_DROP:
            node->ds_t = node->ds_s;
            node->ds_s = POP_ds(node);
            break;

        case INS_DUP:  // ( x -- x x )
            PUSH_ds(node, node->ds_s);
            node->ds_s = node->ds_t;
            break;

        case INS_POP:  // push R onto data stack and pop return stack
            PUSH_s(node, node->rs_r);
            node->rs_r = POP_rs(node);
            break;

        case INS_OVER:  // ( x y -- x y x )
            PUSH_ds(node, node->ds_s);
            swap18(node->ds_t, node->ds_s);
            break;

        case INS_A:  // ( -- A )  push? A onto data stack
            PUSH_s(node, node->a);
            break;

        case INS_NOP:
            break;

        case INS_PUSH:  // push T onto return stack and pop data stack
            PUSH_r(node, node->ds_t);
            POP_s(node);
            break;

        case INS_B_STORE:  // b! ( x -- ) store into B
            node->b = node->ds_t;
            POP_s(node);
            break;

        case INS_A_STORE:  // a! ( x -- ) store into A
            node->a = node->ds_t;
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
