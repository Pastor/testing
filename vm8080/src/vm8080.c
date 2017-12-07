#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vm8080.h"

#define FAST_SWAP(x, y)  (x) ^= (y); (y) ^= (x); (x) ^= (y)
#define WHIT_IN(x, y, z) (((x) >= (y)) && ((x) <= (z)))

u16 stack_pop();

void stack_push(u16 value);

u8 read_u8() {
    return read_byte(cpu.PC++);
}

u16 read_u16() {
    u16 r = read_u8() | read_u8() << 8;
    return r;
}

void set_flags(u32 reg, int mask) {
    cpu.F &= ~(mask);
    reg &= 0xFFFF;

    if (mask & FLAG_SIGN) {
        if (reg & (1 << 7)) {
            cpu.F |= FLAG_SIGN;
        }
    }

    if (mask & FLAG_ZERO) {
        if (!reg) {
            cpu.F |= FLAG_ZERO;
        }
    }

    if (mask & FLAG_CARRY) {
        if ((reg & 0x100) != 0) {
            cpu.F |= FLAG_CARRY;
        }
    }

    if (mask & FLAG_ACARRY) {
        if ((cpu.A & 0xf) > (reg & 0xf)) {
            cpu.F |= FLAG_ACARRY;
        }
    }

    if (mask & FLAG_PARITY) {
        if (!(reg & 0x1)) {
            cpu.F |= FLAG_PARITY;
        }
    }
}

void write_register(u8 value, int index) {
    switch (index) {
        case 0x00:
            cpu.B = value;
            break;
        case 0x01:
            cpu.C = value;
            break;
        case 0x02:
            cpu.D = value;
            break;
        case 0x03:
            cpu.E = value;
            break;
        case 0x04:
            cpu.H = value;
            break;
        case 0x05:
            cpu.L = value;
            break;
        case 0x06:
            write_byte(value, cpu.HL);
            break;
        case 0x07:
            cpu.A = value;
            break;
        default:
            break;
    }
}

u8 read_register(int index) {
    switch (index) {
        case 0x00:
            return cpu.B;
        case 0x01:
            return cpu.C;
        case 0x02:
            return cpu.D;
        case 0x03:
            return cpu.E;
        case 0x04:
            return cpu.H;
        case 0x05:
            return cpu.L;
        case 0x06:
            return read_byte(cpu.HL);
        case 0x07:
            return cpu.A;
        default:
            break;
    }
    return 0xFF;
}

void CMC(u8 op) {
    cpu.F ^= FLAG_CARRY;
}

void STC(u8 op) {
    cpu.F |= FLAG_CARRY;
}

void DAA(u8 op) {
    int top4 = (cpu.A >> 4) & 0xF;
    int bot4 = (cpu.A & 0xF);

    if ((bot4 > 9) || (cpu.F & FLAG_ACARRY)) {
        set_flags(cpu.A + 6, FLAG_ZERO | FLAG_SIGN | FLAG_PARITY | FLAG_CARRY | FLAG_ACARRY);
        cpu.A += 6;
        top4 = (cpu.A >> 4) & 0xF;
        bot4 = (cpu.A & 0xF);
    }

    if ((top4 > 9) || (cpu.F & FLAG_CARRY)) {
        top4 += 6;
        cpu.A = (u8) ((top4 << 4) | bot4);
    }
}


void INR(u8 op) {
    int dst = (op >> 3) & 0x7;
    u8 tmp = read_register(dst);
    tmp++;
    set_flags(tmp, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY);
    write_register(tmp, dst);
}

void DCR(u8 op) {
    int dst = (op >> 3) & 0x7;
    u8 tmp = read_register(dst);
    tmp--;
    set_flags(tmp, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY);
    write_register(tmp, dst);
}

void CMA(u8 op) {
    cpu.A = ~(cpu.A);
}

void NOP(u8 op) {
}

void MOV(u8 op) {
    int dst = (op >> 3) & 0x7;
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    write_register(src_value, dst);
}

void STAX(u8 op) {
    int src = (op >> 4) & 1;
    u16 address = (!src) ? cpu.BC : cpu.DE;
    write_byte(cpu.A, address);
}

void LDAX(u8 op) {
    int src = (op >> 4) & 1;
    u16 address = (!src) ? cpu.BC : cpu.DE;
    cpu.A = read_byte(address);
}

void ADD(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags(cpu.A + src_value, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A += src_value;
}

void ADC(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags((u32) (cpu.A + src_value + (cpu.F & FLAG_CARRY)),
              FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A += src_value + (cpu.F & FLAG_CARRY);
}

void SUB(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags(cpu.A - src_value, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A -= src_value;
}

void SBB(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags((u32) (cpu.A - (src_value + (cpu.F & FLAG_CARRY))),
              FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A -= (src_value + (cpu.F & FLAG_CARRY));
}

void ANA(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags(cpu.A & src_value, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A &= src_value;
}

void XRA(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags(cpu.A ^ src_value, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A ^= src_value;
}

void ORA(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags(cpu.A | src_value, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A |= src_value;
}

void CMP(u8 op) {
    int src = (op & 0x7);
    u8 src_value = read_register(src);
    set_flags(cpu.A - src_value, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
}

void RLC(u8 op) {
    cpu.F = (u8) ~(FLAG_CARRY);
    if (cpu.A & (1 << 7)) {
        cpu.F |= FLAG_CARRY;
    }
    cpu.A = (u8) ((cpu.A << 1) | (cpu.F & FLAG_CARRY));
}

void RRC(u8 op) {
    cpu.F = (u8) ~(FLAG_CARRY);
    if (cpu.A & 1) {
        cpu.F |= FLAG_CARRY;
    }
    cpu.A = (u8) ((cpu.A >> 1) | ((cpu.F & FLAG_CARRY) << 7));
}

void RAL(u8 op) {
    u8 c = (u8) (cpu.A & (1 << 7));
    cpu.A = (u8) ((cpu.A << 1) | (cpu.F & FLAG_CARRY));
    cpu.F = c;
}

void RAR(u8 op) {
    u8 c = (u8) (cpu.A & 1);
    cpu.A = (u8) ((cpu.A >> 1) | ((cpu.F & FLAG_CARRY) << 7));
    cpu.F = c;
}

void PUSH(u8 op) {
    int src = (op >> 4) & 3;
    switch (src) {
        case 0x00:
            stack_push(cpu.BC);
            break;
        case 0x01:
            stack_push(cpu.DE);
            break;
        case 0x02:
            stack_push(cpu.HL);
            break;
        case 0x03:
            stack_push(cpu.AF);
            break;
        default:
            while (1);
    }
}

void POP(u8 op) {
    int src = (op >> 4) & 3;
    u16 val = stack_pop();
    switch (src) {
        case 0x00:
            cpu.BC = val;
            break;
        case 0x01:
            cpu.DE = val;
            break;
        case 0x02:
            cpu.HL = val;
            break;
        case 0x03:
            cpu.AF = val;
            break;
        default:
            while (1);
    }
}

void DAD(u8 op) {
    int src = (op >> 4) & 3;
    u16 rVal = 0;
    switch (src) {
        case 0x00:
            rVal = cpu.BC;
            break;
        case 0x01:
            rVal = cpu.DE;
            break;
        case 0x02:
            rVal = cpu.HL;
            break;
        case 0x03:
            rVal = cpu.AF;
            break;
    }
    set_flags(cpu.HL + rVal, FLAG_CARRY);
    cpu.HL += rVal;
}

void INX(u8 op) {
    int src = (op >> 4) & 3;
    switch (src) {
        case 0x00:
            cpu.BC++;
            break;
        case 0x01:
            cpu.DE++;
            break;
        case 0x02:
            cpu.HL++;
            break;
        case 0x03:
            cpu.SP++;
            break;
        default:
            while (1);
    }
}

void DCX(u8 op) {
    int src = (op >> 4) & 3;
    switch (src) {
        case 0x00:
            cpu.BC--;
            break;
        case 0x01:
            cpu.DE--;
            break;
        case 0x02:
            cpu.HL--;
            break;
        case 0x03:
            cpu.SP--;
            break;
        default:
            while (1);
    }
}

void XCHG(u8 op) {
    FAST_SWAP(cpu.DE, cpu.HL);
}

void XTHL(u8 op) {
    cpu.H = read_byte(cpu.SP);
    cpu.L = read_byte((u16) (cpu.SP + 1));
}

void SPHL(u8 op) {
    cpu.SP = cpu.HL;
}

void LXI(u8 op) {
    int dst = (op >> 4) & 3;
    switch (dst) {
        case 0x00:
            cpu.BC = read_u16();
            break;
        case 0x01:
            cpu.DE = read_u16();
            break;
        case 0x02:
            cpu.HL = read_u16();
            break;
        case 0x03:
            cpu.SP = read_u16();
            break;
        default:
            while (1);
    }
}

void MVI(u8 op) {
    int dst = (op >> 3) & 0x7;
    write_register(read_u8(), dst);
}

void ADI(u8 op) {
    u8 imm = read_u8();
    set_flags(cpu.A + imm, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A += imm;
}

void ACI(u8 op) {
    u8 imm = read_u8();
    set_flags((u32) (cpu.A + imm + (cpu.F & FLAG_CARRY)),
              FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A += (imm + (cpu.F & FLAG_CARRY));
}

void SUI(u8 op) {
    u8 imm = read_u8();
    set_flags(cpu.A - imm, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A -= imm;
}

void SBI(u8 op) {
    u8 imm = read_u8();
    set_flags((u32) (cpu.A - (imm + (cpu.F & FLAG_CARRY))),
              FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A -= (imm + (cpu.F & FLAG_CARRY));
}

void ANI(u8 op) {
    u8 imm = read_u8();
    set_flags(cpu.A & imm, FLAG_SIGN | FLAG_ZERO | FLAG_PARITY | FLAG_CARRY);
    cpu.A &= imm;
}

void XRI(u8 op) {
    u8 imm = read_u8();
    set_flags(cpu.A ^ imm, FLAG_SIGN | FLAG_ZERO | FLAG_PARITY | FLAG_CARRY);
    cpu.A ^= imm;
}

void ORI(u8 op) {
    u8 imm = read_u8();
    set_flags(cpu.A | imm, FLAG_SIGN | FLAG_ZERO | FLAG_PARITY | FLAG_CARRY);
    cpu.A |= imm;
}

void CPI(u8 op) {
    u8 imm = read_u8();
    set_flags(cpu.A - imm, FLAG_SIGN | FLAG_ZERO | FLAG_ACARRY | FLAG_PARITY | FLAG_CARRY);
    cpu.A -= imm;
}

void STA(u8 op) {
    u16 address = read_u16();
    write_byte(cpu.A, address);
}

void LDA(u8 op) {
    u16 address = read_u16();
    cpu.A = read_byte(address);
}

void SHLD(u8 op) {
    u16 address = read_u16();
    write_byte(cpu.H, address);
    write_byte(cpu.L, (u16) (address + 1));
}

void LHLD(u8 op) {
    u16 address = read_u16();
    cpu.H = read_byte(address);
    cpu.L = read_byte((u16) (address + 1));
}

void PCHL(u8 op) {
    cpu.PC = cpu.HL;
}

void JMP(u8 op) {
    cpu.PC = read_u16();
}

void JC(u8 op) {
    if (cpu.F & FLAG_CARRY) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void JNC(u8 op) {
    if (!(cpu.F & FLAG_CARRY)) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void JZ(u8 op) {
    if (cpu.F & FLAG_ZERO) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void JNZ(u8 op) {
    if (!(cpu.F & FLAG_ZERO)) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void JM(u8 op) {
    if (cpu.F & FLAG_SIGN) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void JP(u8 op) {
    if (!(cpu.F & FLAG_SIGN)) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void JPE(u8 op) {
    if (!(cpu.F & FLAG_PARITY)) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void JPO(u8 op) {
    if (cpu.F & FLAG_PARITY) {
        JMP(op);
    } else {
        cpu.PC += 2;
    }
}

void CALL(u8 op) {
    u16 address = read_u16();
    stack_push(cpu.PC);
    cpu.PC = address;
    /*if (cpu.PC == 0x100)
        die("NOW");*/
}

void CC(u8 op) {
    if (cpu.F & FLAG_CARRY) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void CNC(u8 op) {
    if (!(cpu.F & FLAG_CARRY)) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void CZ(u8 op) {
    if (cpu.F & FLAG_ZERO) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void CNZ(u8 op) {
    if (!(cpu.F & FLAG_ZERO)) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void CM(u8 op) {
    if (cpu.F & FLAG_SIGN) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void CP(u8 op) {
    if (!(cpu.F & FLAG_SIGN)) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void CPE(u8 op) {
    if (!(cpu.F & FLAG_PARITY)) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void CPO(u8 op) {
    if (cpu.F & FLAG_PARITY) {
        CALL(op);
    } else {
        cpu.PC += 2;
    }
}

void RET(u8 op) {
    cpu.PC = stack_pop();
}

void RC(u8 op) {
    if (cpu.F & FLAG_CARRY) {
        RET(op);
    }
}

void RNC(u8 op) {
    if (!(cpu.F & FLAG_CARRY)) {
        RET(op);
    }
}

void RZ(u8 op) {
    if (cpu.F & FLAG_ZERO) {
        RET(op);
    }
}

void RNZ(u8 op) {
    if (!(cpu.F & FLAG_ZERO)) {
        RET(op);
    }
}

void RM(u8 op) {
    if (cpu.F & FLAG_SIGN) {
        RET(op);
    }
}

void RP(u8 op) {
    if (!(cpu.F & FLAG_SIGN)) {
        RET(op);
    }
}

void RPE(u8 op) {
    if (!(cpu.F & FLAG_PARITY)) {
        RET(op);
    }
}

void RPO(u8 op) {
    if (cpu.F & FLAG_PARITY) {
        RET(op);
    }
}

void RST(u8 op) {
    stack_push(cpu.PC);
    cpu.PC = (u16) (((op >> 3) & 0x7) << 3);
}

void EI(u8 op) {
    cpu.IE = 1;
}

void DI(u8 op) {
    cpu.IE = 0;
}

void IN(u8 op) {
    cpu.A = (*cpu.port_in)(read_u8());
}

void OUT(u8 op) {
    (*cpu.port_out)(read_u8(), cpu.A);
}

void HLT(u8 op) {
    cpu.halt = 1;
}

static struct {
    void (*execute)(u8 op);

    int cycles;
} operation_table[0x100] = {
        {NOP,  4},
        {LXI,  10},
        {STAX, 7},
        {INX,  5},
        {INR,  5},
        {DCR,  5},
        {MVI,  7},
        {RLC,  4},
        {NOP,  4},
        {DAD,  10},
        {LDAX, 7},
        {DCX,  5},
        {INR,  5},
        {DCR,  5},
        {MVI,  7},
        {RRC,  4},
        {NOP,  4}, // 0x10
        {LXI,  10},
        {STAX, 7},
        {INX,  5},
        {INR,  5},
        {DCR,  5},
        {MVI,  7},
        {RAL,  4},
        {NOP,  4},
        {DAD,  10},
        {LDAX, 7},
        {DCX,  5},
        {INR,  5},
        {DCR,  5},
        {MVI,  7},
        {RAR,  4},
        {NOP,  4},// 0x20
        {LXI,  10},
        {SHLD, 16},
        {INX,  5},
        {INR,  5},
        {DCR,  5},
        {MVI,  7},
        {DAA,  4},
        {NOP,  4},
        {DAD,  10},
        {LHLD, 16},
        {DCX,  5},
        {INR,  5},
        {DCR,  5},
        {MVI,  7},
        {CMA,  4},
        {NOP,  4}, // 0x30
        {LXI,  10},
        {STA,  13},
        {INX,  5},
        {INR,  10},
        {DCR,  10},
        {MVI,  10},
        {STC,  4},
        {NOP,  4},
        {DAD,  10},
        {LDA,  13},
        {DCX,  5},
        {INR,  5},
        {DCR,  5},
        {MVI,  7},
        {CMC,  4},
        {MOV,  5}, // 0X40
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  7},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  7},
        {MOV,  5},
        {MOV,  5}, // 0X50
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  7},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  7},
        {MOV,  5},
        {MOV,  5}, // 0X60
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  7},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  7},
        {MOV,  5},
        {MOV,  5}, // 0X70
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {HLT,  7},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  5},
        {MOV,  7},
        {MOV,  5},
        {ADD,  4}, // 0X80
        {ADD,  4},
        {ADD,  4},
        {ADD,  4},
        {ADD,  4},
        {ADD,  4},
        {ADD,  7},
        {ADD,  4},
        {ADC,  4},
        {ADC,  4},
        {ADC,  4},
        {ADC,  4},
        {ADC,  4},
        {ADC,  4},
        {ADC,  7},
        {ADC,  4},
        {SUB,  4},
        {SUB,  4},
        {SUB,  4},
        {SUB,  4},
        {SUB,  4},
        {SUB,  4},
        {SUB,  7},
        {SUB,  4},
        {SBB,  4},
        {SBB,  4},
        {SBB,  4},
        {SBB,  4},
        {SBB,  4},
        {SBB,  4},
        {SBB,  7},
        {SBB,  4},
        {ANA,  4},
        {ANA,  4},
        {ANA,  4},
        {ANA,  4},
        {ANA,  4},
        {ANA,  4},
        {ANA,  7},
        {ANA,  4},
        {XRA,  4},
        {XRA,  4},
        {XRA,  4},
        {XRA,  4},
        {XRA,  4},
        {XRA,  4},
        {XRA,  7},
        {XRA,  4},
        {ORA,  4},
        {ORA,  4},
        {ORA,  4},
        {ORA,  4},
        {ORA,  4},
        {ORA,  4},
        {ORA,  7},
        {ORA,  4},
        {CMP,  4},
        {CMP,  4},
        {CMP,  4},
        {CMP,  4},
        {CMP,  4},
        {CMP,  4},
        {CMP,  7},
        {CMP,  4},
        {RNZ,  11},
        {POP,  10},
        {JNZ,  10},
        {JMP,  10},
        {CNZ,  17},
        {PUSH, 11},
        {ADI,  7},
        {RST,  11},
        {RZ,   11},
        {RET,  10},
        {JZ,   10},
        {JMP,  10},
        {CZ,   17},
        {CALL, 17},
        {ACI,  7},
        {RST,  11},
        {RNC,  11},
        {POP,  10},
        {JNC,  10},
        {OUT,  10},
        {CNC,  17},
        {PUSH, 11},
        {SUI,  7},
        {RST,  11},
        {RC,   11},
        {RET,  10},
        {JC,   10},
        {IN,   10},
        {CC,   17},
        {CALL, 17},
        {SBI,  7},
        {RST,  11},
        {RPO,  11},
        {POP,  10},
        {JPO,  10},
        {XTHL, 18},
        {CPO,  17},
        {PUSH, 11},
        {ANI,  7},
        {RST,  11},
        {RPE,  11},
        {PCHL, 5},
        {JPE,  10},
        {XCHG, 5},
        {CPE,  17},
        {CALL, 17},
        {XRI,  7},
        {RST,  11},
        {RP,   11},
        {POP,  10},
        {JP,   10},
        {DI,   4},
        {CP,   17},
        {PUSH, 11},
        {ORI,  7},
        {RST,  11},
        {RM,   11},
        {SPHL, 5},
        {JM,   10},
        {EI,   4},
        {CM,   17},
        {CALL, 17},
        {CPI,  7},
        {RST,  11}
};

void dump_registers() {
    printf("A : %02X F : %02X\nB : %02x C : %02x\nD : %02x E : %02x\nH : %02x L : %02x\nPC : %04x SP : %04x\n",
           cpu.A, cpu.F, cpu.B, cpu.C, cpu.D, cpu.E, cpu.H, cpu.L, cpu.PC, cpu.SP);
}

void die(char *err) {
    printf("FATAL : %s\n", err);
    dump_registers();
    exit(-1);
}

void write_byte(u8 value, u16 offset) {
    int c;

    for (c = 0; c < 4; c++) {
        if (!WHIT_IN(offset, cpu.ram[c].start, cpu.ram[c].start + cpu.ram[c].size))
            continue;
        if (cpu.ram[c].flag == FLAG_MIRROR) {
            offset -= cpu.ram[c].start;
            offset += cpu.ram[c].mirror;
            break;
        }
    }

    for (c = 0; c < 8; c++) {
        if (cpu.ram[c].flag == FLAG_UNUSED)
            continue;
        if (!WHIT_IN(offset, cpu.ram[c].start, cpu.ram[c].start + cpu.ram[c].size))
            continue;
        if (cpu.ram[c].flag == FLAG_ROM)
            die("Write to ROM region");
#ifdef DEBUG
        printf("Write %#x @ %#x\n", value, offset);
#endif
        cpu.ram[c].ptr[offset - cpu.ram[c].start] = value;
        return;
    }
    die("Write out of bounds!");
}

u8 read_byte(u16 offset) {
    int c;

    for (c = 0; c < 4; c++) {
        if (!WHIT_IN(offset, cpu.ram[c].start, cpu.ram[c].start + cpu.ram[c].size))
            continue;
        if (cpu.ram[c].flag == FLAG_MIRROR) {
            offset -= cpu.ram[c].start;
            offset += cpu.ram[c].mirror;
            break;
        }
    }

    for (c = 0; c < 4; c++) {
        if (cpu.ram[c].flag == FLAG_UNUSED)
            continue;
        if (!WHIT_IN(offset, cpu.ram[c].start, cpu.ram[c].start + cpu.ram[c].size))
            continue;
#ifdef DEBUG
        printf("Read @ %#x\n", value, offset);
#endif
        return cpu.ram[c].ptr[offset - cpu.ram[c].start];
    }
    die("Read out of bounds!");
    return 0xFF;
}

static u16 stack[0xFFFF];
static int stack_ptr = 0;

void stack_push(u16 value) {
    if (cpu.SP < 2) {
        die("Stack pointer exhausted");
    }
    stack[stack_ptr++] = value;
}

u16 stack_pop() {
    /*u16 r  =  readByte(cpu.SP++);
        r |= (read_byte(cpu.SP++) << 8);
    return r;*/
    return stack[--stack_ptr];
}


void cause_int(int address) {
    if (cpu.IE) {
        stack_push(cpu.PC);
        cpu.PC = (u16) (address & 0xFFFF);
    }
}

#ifdef DEBUG
static const char* lut_mnemonic[0x100]={
    "nop",     "lxi b,#", "stax b",  "inx b",   "inr b",   "dcr b",   "mvi b,#", "rlc",     "ill",     "dad b",   "ldax b",  "dcx b",   "inr c",   "dcr c",   "mvi c,#", "rrc",
    "ill",     "lxi d,#", "stax d",  "inx d",   "inr d",   "dcr d",   "mvi d,#", "ral",     "ill",     "dad d",   "ldax d",  "dcx d",   "inr e",   "dcr e",   "mvi e,#", "rar",
    "ill",     "lxi h,#", "shld",    "inx h",   "inr h",   "dcr h",   "mvi h,#", "daa",     "ill",     "dad h",   "lhld",    "dcx h",   "inr l",   "dcr l",   "mvi l,#", "cma",
    "ill",     "lxi sp,#","sta $",   "inx sp",  "inr M",   "dcr M",   "mvi M,#", "stc",     "ill",     "dad sp",  "lda $",   "dcx sp",  "inr a",   "dcr a",   "mvi a,#", "cmc",
    "mov b,b", "mov b,c", "mov b,d", "mov b,e", "mov b,h", "mov b,l", "mov b,M", "mov b,a", "mov c,b", "mov c,c", "mov c,d", "mov c,e", "mov c,h", "mov c,l", "mov c,M", "mov c,a",
    "mov d,b", "mov d,c", "mov d,d", "mov d,e", "mov d,h", "mov d,l", "mov d,M", "mov d,a", "mov e,b", "mov e,c", "mov e,d", "mov e,e", "mov e,h", "mov e,l", "mov e,M", "mov e,a",
    "mov h,b", "mov h,c", "mov h,d", "mov h,e", "mov h,h", "mov h,l", "mov h,M", "mov h,a", "mov l,b", "mov l,c", "mov l,d", "mov l,e", "mov l,h", "mov l,l", "mov l,M", "mov l,a",
    "mov M,b", "mov M,c", "mov M,d", "mov M,e", "mov M,h", "mov M,l", "hlt",     "mov M,a", "mov a,b", "mov a,c", "mov a,d", "mov a,e", "mov a,h", "mov a,l", "mov a,M", "mov a,a",
    "add b",   "add c",   "add d",   "add e",   "add h",   "add l",   "add M",   "add a",   "adc b",   "adc c",   "adc d",   "adc e",   "adc h",   "adc l",   "adc M",   "adc a",
    "sub b",   "sub c",   "sub d",   "sub e",   "sub h",   "sub l",   "sub M",   "sub a",   "sbb b",   "sbb c",   "sbb d",   "sbb e",   "sbb h",   "sbb l",   "sbb M",   "sbb a",
    "ana b",   "ana c",   "ana d",   "ana e",   "ana h",   "ana l",   "ana M",   "ana a",   "xra b",   "xra c",   "xra d",   "xra e",   "xra h",   "xra l",   "xra M",   "xra a",
    "ora b",   "ora c",   "ora d",   "ora e",   "ora h",   "ora l",   "ora M",   "ora a",   "cmp b",   "cmp c",   "cmp d",   "cmp e",   "cmp h",   "cmp l",   "cmp M",   "cmp a",
    "rnz",     "pop b",   "jnz $",   "jmp $",   "cnz $",   "push b",  "adi #",   "rst 0",   "rz",      "ret",     "jz $",    "ill",     "cz $",    "call $",  "aci #",   "rst 1",
    "rnc",     "pop d",   "jnc $",   "out p",   "cnc $",   "push d",  "sui #",   "rst 2",   "rc",      "ill",     "jc $",    "in p",    "cc $",    "ill",     "sbi #",   "rst 3",
    "rpo",     "pop h",   "jpo $",   "xthl",    "cpo $",   "push h",  "ani #",   "rst 4",   "rpe",     "pchl",    "jpe $",   "xchg",    "cpe $",   "ill",     "xri #",   "rst 5",
    "rp",      "pop psw", "jp $",    "di",      "cp $",    "push psw","ori #",   "rst 6",   "rm",      "sphl",    "jm $",    "ei",      "cm $",    "ill",     "cpi #",   "rst 7"
};
#endif

int run_vm8080(int cycles) {
    int cycles_done = 0;

    while (cycles_done < cycles) {
        if (cpu.halt) {
            printf("Halted!\n");
            break;
        }
        /**TODO: Wat? */
        if (cpu.PC == 0x0005) {
            if (cpu.C == 9) {
                int i;
                for (i = cpu.DE; cpu.ram->ptr[i] != '$'; i += 1) {
                    putchar(cpu.ram->ptr[i]);
                }
                fflush(stdout);
            }
            if (cpu.C == 2) {
                putchar((char) cpu.E);
                fflush(stdout);
            }
        }
        u8 op = read_u8();
#ifdef DEBUG
        printf("[%04x] %s\n", cpu.PC, lut_mnemonic[op]);
        fflush(stdout);
#endif
        operation_table[op].execute(op);
        cycles_done += operation_table[op].cycles;
    }
    return cycles_done;
}

void reset_vm8080() {
    int c;

    memset(&cpu, 0, sizeof cpu);
    cpu.SP = 0xF000;
    cpu.ram = (RamBank *) &bank;
    for (c = 0; c < 4; c++) {
        if (cpu.ram[c].flag == FLAG_UNUSED)
            continue;
        if (cpu.ram[c].flag == FLAG_MIRROR)
            continue;
        if (!cpu.ram[c].ptr)
            cpu.ram[c].ptr = malloc(cpu.ram[c].size);
        if (!cpu.ram[c].flag != FLAG_ROM)
            memset(cpu.ram[c].ptr, 0, cpu.ram[c].size);
    }
}

int game_vm8080() {
    FILE *rom;
    char *bank_name[] = {"invaders.h", "invaders.g", "invaders.f", "invaders.e"};
    int c;
    u8 *bank_ptr = NULL;

    printf("Resetting the 8080...\n");
    reset_vm8080();
    for (c = 0; c < 4; c++) {
        if (cpu.ram[c].flag == FLAG_ROM) {
            printf("ROM @ %#x (%#x) %p\n", cpu.ram[c].start, cpu.ram[c].size, (void *) cpu.ram[c].ptr);
            bank_ptr = cpu.ram[c].ptr;
            break;
        }
    }
    if (!bank_ptr)
        die("No rom bank defined");
    for (c = 0; c < 4; c++) {
        rom = fopen(bank_name[c], "rb");
        if (!rom) {
            return 0;
        }
        if (fread(&bank_ptr[0x0800 * c], 1, 0x0800, rom) != 0x0800) {
            return 0;
        }
        fclose(rom);
    }
    printf("Loaded the 4 rom banks...\n");
    fflush(stdout);
    return 1;
}
