#include <windows.h>

static LPTSTR lpszFilenameCharacters = TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

static BOOL MakeTempFilename(LPTSTR lpszBuffer, DWORD dwBuffer) {
  int   i;
  DWORD dwCharacterRange, dwTempPathLength;
  TCHAR cCharacter;

  dwTempPathLength = GetTempPath(dwBuffer, lpszBuffer);
  if (!dwTempPathLength) return FALSE;
  if (++dwTempPathLength > dwBuffer || dwBuffer - dwTempPathLength < 12) {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;
  }
  dwCharacterRange = lstrlen(lpszFilenameCharacters) - 1;
  for (i = 0;  i < 8;  i++) {
    cCharacter = lpszFilenameCharacters[spc_rand_range(0, dwCharacterRange)];
    lpszBuffer[dwTempPathLength++ - 1] = cCharacter;
  }
  lpszBuffer[dwTempPathLength++ - 1] = '.';
  lpszBuffer[dwTempPathLength++ - 1] = 'T';
  lpszBuffer[dwTempPathLength++ - 1] = 'M';
  lpszBuffer[dwTempPathLength++ - 1] = 'P';
  lpszBuffer[dwTempPathLength++ - 1] = 0;
  return TRUE;
}

HANDLE SpcMakeTempFile(LPTSTR lpszBuffer, DWORD dwBuffer) {
  HANDLE hFile;

  do {
    if (!MakeTempFilename(lpszBuffer, dwBuffer)) {
      hFile = INVALID_HANDLE_VALUE;
      break;
    }
    hFile = CreateFile(lpszBuffer, GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                       0, CREATE_NEW,
                       FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, 0);
    if (hFile == INVALID_HANDLE_VALUE && GetLastError() != ERROR_ALREADY_EXISTS)
      break;
  } while (hFile == INVALID_HANDLE_VALUE);

  return hFile;
}
