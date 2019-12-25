#include <windows.h>

char *sice_cmd = "hboot";

BOOL spc_softice_command(char *cmd) {
  __asm {
    push esi
    mov  esi, 0x4647     ; "FG"
    push edi
    mov  edi, 0x4A4D     ; "JM"
    push edx
    mov  edx, [cmd]      ; command (string) to execute
    mov  ax, 0x0911      ; function 911: execute SOFTICE command
    int  3
    pop  edx
    pop  edi
    pop  esi
  }
}
