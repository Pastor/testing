#include <windows.h>

PSID SpcLookupName(LPCTSTR lpszSystemName, LPCTSTR lpszAccountName) {
  PSID         Sid;
  DWORD        cbReferencedDomainName, cbSid;
  LPTSTR       ReferencedDomainName;
  SID_NAME_USE eUse;

  cbReferencedDomainName = cbSid = 0;
  if (LookupAccountName(lpszSystemName, lpszAccountName, 0, &cbSid,
                        0, &cbReferencedDomainName, &eUse)) {
    SetLastError(ERROR_NONE_MAPPED);
    return 0;
  }
  if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return 0;

  if (!(Sid = (PSID)LocalAlloc(LMEM_FIXED, cbSid))) return 0;
  ReferencedDomainName = (LPTSTR)LocalAlloc(LMEM_FIXED, cbReferencedDomainName);
  if (!ReferencedDomainName) {
    LocalFree(Sid);
    return 0;
  }

  if (!LookupAccountName(lpszSystemName, lpszAccountName, Sid, &cbSid,
                         ReferencedDomainName, &cbReferencedDomainName, &eUse)) {
    LocalFree(ReferencedDomainName);
    LocalFree(Sid);
    return 0;
  }

  LocalFree(ReferencedDomainName);
  return Sid;
}
