#define DISASM_MISALIGN asm volatile ( \
        "  pushl %eax       \n"        \
        "  cmpl  %eax, %eax \n"        \
        "  jz    0f         \n"        \
        "  .byte 0x0F       \n"        \
        "0:                 \n"        \
        "  popl  %eax       \n")
