#include <windows.h>

__declspec(naked) BOOL spc_softice_ispresent(void) {
  __asm {
    mov ah, 0x43
    int 0x68
    cmp ax, 0xF386
    jnz debugger_not_present
    mov eax, 1
    ret
  debugger_not_present:
    xor eax, eax
    ret
  }
}
