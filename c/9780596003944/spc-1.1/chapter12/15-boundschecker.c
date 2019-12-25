#include <windows.h>

__declspec(naked) BOOL spc_softice_boundschecker(void) {
  __asm {
      push ebp
      mov  ebp, 0x4243484B       ; "BCHK"
      mov  eax, 4                ; function 4: boundschecker interface
      int  3
      test al, al                ; test for zero
      jnz  debugger_not_present
      mov  eax, 1                ; set the return value to 1
      pop  ebp
      ret
    debugger_not_present:
      xor  eax, eax              ; set the return value to 0
      pop  ebp
      ret
  }
}
