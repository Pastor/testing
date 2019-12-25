#include <windows.h>

BOOL IsPasswordValid(HWND hwndPassword) {
  BOOL   bValid = FALSE;
  DWORD  dwTextLength;
  LPTSTR lpText;

  if (!(dwTextLength = (DWORD)SendMessage(hwndPassword, WM_GETTEXTLENGTH, 0, 0)))
    return FALSE;
  lpText = (LPTSTR)LocalAlloc(LMEM_FIXED, (dwTextLength + 1) * sizeof(TCHAR));
  if (!lpText) return FALSE;
  SendMessage(hwndPassword, WM_GETTEXT, dwTextLength + 1, (LPARAM)lpText);

  /* Do something to validate the password */

  while (dwTextLength--) *(lpText + dwTextLength) = ' ';
  SendMessage(hwndPassword, WM_SETTEXT,  0, (LPARAM)lpText);
  LocalFree(lpText);

  return bValid;
}
