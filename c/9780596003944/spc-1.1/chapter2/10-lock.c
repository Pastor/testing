#include <windows.h>

HANDLE SpcLockResource(LPCTSTR lpName) {
  HANDLE hResourceLock;

  if (!lpName) {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  if (!(hResourceLock = CreateMutex(0, FALSE, lpName))) return 0;
  if (WaitForSingleObject(hResourceLock, INFINITE) == WAIT_FAILED) {
    CloseHandle(hResourceLock);
    return 0;
  }

  return hResourceLock;
}

BOOL SpcUnlockResource(HANDLE hResourceLock) {
  if (!ReleaseMutex(hResourceLock)) return FALSE;
  CloseHandle(hResourceLock);
  return TRUE;
}
