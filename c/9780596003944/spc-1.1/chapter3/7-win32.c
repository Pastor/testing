#include <windows.h>

LPTSTR SpcResolvePath(LPCTSTR lpFileName) {
  DWORD  dwLastError, nBufferLength;
  LPTSTR lpBuffer, lpFilePart;

  if (!(nBufferLength = GetFullPathName(lpFileName, 0, 0, &lpFilePart))) return 0;
  if (!(lpBuffer = (LPTSTR)LocalAlloc(LMEM_FIXED, sizeof(TCHAR) * nBufferLength)))
    return 0;
  if (!GetFullPathName(lpFileName, nBufferLength, lpBuffer, &lpFilePart)) {
    dwLastError = GetLastError();
    LocalFree(lpBuffer);
    SetLastError(dwLastError);
    return 0;
  }

  return lpBuffer;
}
