#define DISASM_FALSERET asm volatile (               \
        "  pushl %ecx        /* save registers    */\n" \
        "  pushl %ebx                               \n" \
        "  pushl %edx                               \n" \
        "  movl  %esp, %ebx  /* save ebp, esp     */\n" \
        "  movl  %ebp, %esp                         \n" \
        "  popl  %ebp        /* save old %ebp       \n" \
        "  popl  %ecx        /* save return addr  */\n" \
        "  lea   0f, %edx    /* edx = addr of 0:  */\n" \
        "  pushl %edx        /* return addr = edx */\n" \
        "  ret                                      \n" \
        "  .byte 0x0F        /* off-by-one byte   */\n" \
        "0:                                         \n" \
        "  pushl %ecx        /* restore ret addr  */\n" \
        "  pushl %ebp        /* restore old %ebp  */\n" \
        "  movl  %esp, %ebp  /* restore ebp, esp  */\n" \
        "  movl  %ebx, %esp                         \n" \
        "  popl  %edx        /* restore regs      */\n" \
        "  popl  %ebx                               \n" \
        "  popl  %ecx                               \n")
