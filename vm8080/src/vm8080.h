#ifndef VM8080_H
#define VM8080_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define BIT(x)       (1 << x)

#define FLAG_UNUSED  0x00
#define FLAG_USED    BIT(0)
#define FLAG_ROM     BIT(1)
#define FLAG_MIRROR  BIT(2)

struct RamBank {
    u32 start;
    u32 size;
    u32 mirror;
    u8  flag;
    u8 *ptr;
};
typedef struct RamBank RamBank;

extern RamBank bank[4];

struct {
    /* Processor frequency MhZ */
    int clock;
    /* Halt flag */
    u8 halt;
    /* 8080 registers */
    union {
        struct {
            u8 C;
            u8 B;
            u8 E;
            u8 D;
            u8 L;
            u8 H;
            u8 F;
            u8 A;
        };
        struct {
            u16 BC;
            u16 DE;
            u16 HL;
            u16 AF;
        };
    };
    /* Program Counter & Stack Pointer */
    u16 PC;
    u16 SP;
    /* Working ram */
    RamBank *ram;
    /* Interrupts enabled */
    u8 IE;
    u8 iPending;

    /* Ports IO */
    u8 (*port_in)(int port);

    void (*port_out)(int port, u8 value);
} cpu;

#define FLAG_SIGN     (1 << 7)
#define FLAG_ZERO     (1 << 6)
#define FLAG_ACARRY   (1 << 4)
#define FLAG_PARITY   (1 << 2)
#define FLAG_CARRY    (1 << 0)

void dump_registers();

extern void exit(int);

void die(char *err);

int load_game_vm8080();

void reset_vm8080();

int run_vm8080(int cycles);

void cause_int(int address);

u8 read_byte(u16 offset);

void write_byte(u8 value, u16 offset);

#endif
