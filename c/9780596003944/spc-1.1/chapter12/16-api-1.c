#define NULLPAD_START asm volatile ( \
        "pushl %eax      \n"         \
        "movl  %esp, %eax\n")

#define NULLPAD       asm volatile ("addb  %al, (%eax)\n")

#define NULLPAD_END   asm volatile ("popl  %eax\n")

#define NULLPAD_10    NULLPAD_START; \
                      NULLPAD;  NULLPAD;  NULLPAD;  NULLPAD; NULLPAD; \
                      NULLPAD_END
