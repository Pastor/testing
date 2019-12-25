#include <windows.h>

LPTSTR SpcLookupSid(LPCTSTR lpszSystemName, PSID Sid) {
  DWORD        cbName, cbReferencedDomainName;
  LPTSTR       lpszName, ReferencedDomainName;
  SID_NAME_USE eUse;

  cbName = cbReferencedDomainName = 0;
  if (LookupAccountSid(lpszSystemName, Sid, 0, &cbName,
                       0, &cbReferencedDomainName, &eUse)) {
    SetLastError(ERROR_NONE_MAPPED);
    return 0;
  }
  if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return 0;

  if (!(lpszName = (LPTSTR)LocalAlloc(LMEM_FIXED, cbName))) return 0;
  ReferencedDomainName = (LPTSTR)LocalAlloc(LMEM_FIXED, cbReferencedDomainName);
  if (!ReferencedDomainName) {
    LocalFree(lpszName);
    return 0;
  }

  if (!LookupAccountSid(lpszSystemName, Sid, lpszName, &cbName,
                        ReferencedDomainName, &cbReferencedDomainName, &eUse)) {
    LocalFree(ReferencedDomainName);
    LocalFree(lpszName);
    return 0;
  }

  LocalFree(ReferencedDomainName);
  return lpszName;
}
