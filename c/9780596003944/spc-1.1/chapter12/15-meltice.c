#include <windows.h>

BOOL spc_softice_meltice(void) {
  HANDLE hFile;

  hFile = CreateFile(TEXT("\\.\\SICE"), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
  if (hFile == INVALID_HANDLE_VALUE)
    hFile = CreateFile(TEXT("\\.\\NTICE"), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
  if (hFile == INVALID_HANDLE_VALUE)
    hFile = CreateFile(TEXT("\\.\\SIWDEBUG"), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
  if (hFile == INVALID_HANDLE_VALUE)
    hFile = CreateFile(TEXT("\\.\\SIWVID"), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
  if (hFile == INVALID_HANDLE_VALUE) return FALSE;
  CloseHandle(hFile);
  return TRUE;
}
