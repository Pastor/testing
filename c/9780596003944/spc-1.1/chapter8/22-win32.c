#include <windows.h>

/* Confirmation receipts must be received within one hour (3600 seconds) */
#define SPC_CONFIRMATION_EXPIRE 3600

typedef struct {
  LPTSTR        lpszAddress;
  LPSTR         lpszID;
  LARGE_INTEGER liExpire;
} SPC_CONFIRMATION;

static DWORD            dwConfirmationCount, dwConfirmationSize;
static SPC_CONFIRMATION *pConfirmations;

static BOOL NewConfirmation(LPCTSTR lpszAddress, LPCSTR lpszID) {
  DWORD            dwIndex;
  LARGE_INTEGER    liExpire;
  SPC_CONFIRMATION *pTemp;

  /* first make sure that the address isn't already in the list */
  for (dwIndex = 0;  dwIndex < dwConfirmationCount; dwIndex++) {
    if (CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                      pConfirmations[dwIndex].lpszAddress, -1,
                      lpszAddress, -1) == CSTR_EQUAL) return FALSE;
  }

  if (dwConfirmationCount == dwConfirmationSize) {
    if (!pConfirmations)
      pTemp = (SPC_CONFIRMATION *)LocalAlloc(LMEM_FIXED, sizeof(SPC_CONFIRMATION));
    else
      pTemp = (SPC_CONFIRMATION *)LocalReAlloc(pConfirmations,
          sizeof(SPC_CONFIRMATION) * (dwConfirmationSize + 1), 0);
    if (!pTemp) return FALSE;
    pConfirmations = pTemp;
    dwConfirmationSize++;
  }

  pConfirmations[dwConfirmationCount].lpszAddress = (LPTSTR)LocalAlloc(
                      LMEM_FIXED, sizeof(TCHAR) * (lstrlen(lpszAddress) + 1));
  if (!pConfirmations[dwConfirmationCount].lpszAddress) return FALSE;
  lstrcpy(pConfirmations[dwConfirmationCount].lpszAddress, lpszAddress);

  pConfirmations[dwConfirmationCount].lpszID = (LPSTR)LocalAlloc(LMEM_FIXED,
                      lstrlenA(lpszID) + 1);
  if (!pConfirmations[dwConfirmationCount].lpszID) {
 LocalFree(pConfirmations[dwConfirmationCount].lpszAddress);
    return FALSE;
  }
  lstrcpyA(pConfirmations[dwConfirmationCount].lpszID, lpszID);

  /* File Times are 100-nanosecond intervals since January 1, 1601 */
  GetSystemTimeAsFileTime((LPFILETIME)&liExpire);
  liExpire.QuadPart += (SPC_CONFIRMATION_EXPIRE * (__int64)10000000);
  pConfirmations[dwConfirmationCount].liExpire = liExpire;

  dwConfirmationCount++;
  return TRUE;
}

BOOL SpcConfirmationCreate(LPCTSTR lpszAddress, LPSTR *lpszID) {
  BYTE pbBuffer[16];

  if (!spc_rand(pbBuffer, sizeof(pbBuffer))) return FALSE;
  if (!(*lpszID = (LPSTR)spc_base64_encode(pbBuffer, sizeof(pbBuffer), 0)))
    return FALSE;
  if (!NewConfirmation(lpszAddress, *lpszID)) {
    LocalFree(*lpszID);
    return FALSE;
  }
  return TRUE;
}

BOOL SpcConfirmationReceive(LPCTSTR lpszAddress, LPCSTR lpszID) {
  DWORD         dwIndex;
  LARGE_INTEGER liNow;

  /* Before we check the pending list of confirmations, prune the list to
   * remove expired entries.
   */
  GetSystemTimeAsFileTime((LPFILETIME)&liNow);
  for (dwIndex = 0;  dwIndex < dwConfirmationCount; dwIndex++) {
    if (pConfirmations[dwIndex].liExpire.QuadPart <= liNow.QuadPart) {
      LocalFree(pConfirmations[dwIndex].lpszAddress);
      LocalFree(pConfirmations[dwIndex].lpszID);
      if (dwConfirmationCount > 1 && dwIndex < dwConfirmationCount - 1)
        pConfirmations[dwIndex] = pConfirmations[dwConfirmationCount - 1];
      dwIndex--;
      dwConfirmationCount--;
    }
  }

  for (dwIndex = 0;  dwIndex < dwConfirmationCount; dwIndex++) {
    if (CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                      pConfirmations[dwIndex].lpszAddress, -1,
                      lpszAddress, -1) == CSTR_EQUAL) {
      if (lstrcmpA(pConfirmations[dwIndex].lpszID, lpszID) != 0) return FALSE;
      LocalFree(pConfirmations[dwIndex].lpszAddress);
      LocalFree(pConfirmations[dwIndex].lpszID);
      if (dwConfirmationCount > 1 && dwIndex < dwConfirmationCount - 1)
        pConfirmations[dwIndex] = pConfirmations[dwConfirmationCount - 1];
      dwConfirmationCount--;
      return TRUE;
    }
  }
  return FALSE;
}
