#include <windows.h>
#include <wincrypt.h>

#define SPC_WIPE_BUFSIZE 4096

static BOOL RandomPass(HANDLE hFile, HCRYPTPROV hProvider, DWORD dwFileSize)
{
  BYTE  pbBuffer[SPC_WIPE_BUFSIZE];
  DWORD cbBuffer, cbTotalWritten, cbWritten;

  if (SetFilePointer(hFile, 0, 0, FILE_BEGIN) == 0xFFFFFFFF) return FALSE;
  while (dwFileSize > 0) {
    cbBuffer = (dwFileSize > sizeof(pbBuffer) ? sizeof(pbBuffer) : dwFileSize);
    if (!CryptGenRandom(hProvider, cbBuffer, pbBuffer)) return FALSE;
    for (cbTotalWritten = 0;  cbBuffer > 0;  cbTotalWritten += cbWritten)
      if (!WriteFile(hFile, pbBuffer + cbTotalWritten, cbBuffer - cbTotalWritten,
                     &cbWritten, 0)) return FALSE;
    dwFileSize -= cbTotalWritten;
  }
  return TRUE;
}

static BOOL PatternPass(HANDLE hFile, BYTE *pbBuffer, DWORD cbBuffer, DWORD dwFileSize) {
  DWORD cbTotalWritten, cbWrite, cbWritten;

  if (!cbBuffer || SetFilePointer(hFile, 0, 0, FILE_BEGIN) == 0xFFFFFFFF) return FALSE;
  while (dwFileSize > 0) {
    cbWrite = (dwFileSize > cbBuffer ? cbBuffer : dwFileSize);
    for (cbTotalWritten = 0;  cbWrite > 0;  cbTotalWritten += cbWritten)
      if (!WriteFile(hFile, pbBuffer + cbTotalWritten, cbWrite - cbTotalWritten,
                     &cbWritten, 0)) return FALSE;
    dwFileSize -= cbTotalWritten;
  }
  return TRUE;
}

BOOL SpcWipeFile(HANDLE hFile) {
  BYTE       pbBuffer[SPC_WIPE_BUFSIZE];
  DWORD      dwCount, dwFileSize, dwIndex, dwPass;
  HCRYPTPROV hProvider;

  static BYTE  pbSinglePats[16] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
  };
  static BYTE  pbTriplePats[6][3] = {
    { 0x92, 0x49, 0x24 }, { 0x49, 0x24, 0x92 }, { 0x24, 0x92, 0x49 },
    { 0x6d, 0xb6, 0xdb }, { 0xb6, 0xdb, 0x6d }, { 0xdb, 0x6d, 0xb6 }
  };
  static DWORD cbPattern = sizeof(pbTriplePats[0]);

  if ((dwFileSize = GetFileSize(hFile, 0)) == INVALID_FILE_SIZE) return FALSE;
  if (!dwFileSize) return TRUE;

  if (!CryptAcquireContext(&hProvider, 0, 0, 0, CRYPT_VERIFYCONTEXT))
    return FALSE;

  for (dwPass = 0;  dwPass < 4;  dwPass++)
    if (!RandomPass(hFile, hProvider, dwFileSize)) {
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }

  memset(pbBuffer, pbSinglePats[5], sizeof(pbBuffer));
  if (!PatternPass(hFile, pbBuffer, sizeof(pbBuffer), dwFileSize)) {
    CryptReleaseContext(hProvider, 0);
    return FALSE;
  }
  memset(pbBuffer, pbSinglePats[10], sizeof(pbBuffer));
  if (!PatternPass(hFile, pbBuffer, sizeof(pbBuffer), dwFileSize)) {
    CryptReleaseContext(hProvider, 0);
    return FALSE;
  }

  cbPattern = sizeof(pbTriplePats[0]);
  for (dwPass = 0;  dwPass < 3;  dwPass++) {
    dwCount   = sizeof(pbBuffer) / cbPattern;
    for (dwIndex = 0;  dwIndex < dwCount;  dwIndex++)
      CopyMemory(pbBuffer + (dwIndex * cbPattern), pbTriplePats[dwPass],
                  cbPattern);
    if (!PatternPass(hFile, pbBuffer, cbPattern * dwCount, dwFileSize)) {
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }
  }

  for (dwPass = 0;  dwPass < sizeof(pbSinglePats);  dwPass++) {
    memset(pbBuffer, pbSinglePats[dwPass], sizeof(pbBuffer));
    if (!PatternPass(hFile, pbBuffer, sizeof(pbBuffer), dwFileSize)) {
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }
  }

  for (dwPass = 0;  dwPass < sizeof(pbTriplePats) / cbPattern;  dwPass++) {
    dwCount   = sizeof(pbBuffer) / cbPattern;
    for (dwIndex = 0;  dwIndex < dwCount;  dwIndex++)
      CopyMemory(pbBuffer + (dwIndex * cbPattern), pbTriplePats[dwPass],
                  cbPattern);
    if (!PatternPass(hFile, pbBuffer, cbPattern * dwCount, dwFileSize)) {
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }
  }

  for (dwPass = 0;  dwPass < 4;  dwPass++)
    if (!RandomPass(hFile, hProvider, dwFileSize)) {
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }

  CryptReleaseContext(hProvider, 0);
  return TRUE;
}
