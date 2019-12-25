#include <windows.h>

PSID SpcLookupSidByName(LPCTSTR lpAccountName, PSID_NAME_USE peUse) {
  PSID         pSid;
  DWORD        cbSid, cchReferencedDomainName;
  LPTSTR       ReferencedDomainName;
  SID_NAME_USE eUse;

  cbSid = cchReferencedDomainName = 0;
  if (!LookupAccountName(0, lpAccountName, 0, &cbSid, 0, &cchReferencedDomainName,
                         &eUse)) return 0;
  if (!(pSid = LocalAlloc(LMEM_FIXED, cbSid))) return 0;
  ReferencedDomainName = LocalAlloc(LMEM_FIXED,
                                    (cchReferencedDomainName + 1) * sizeof(TCHAR));
  if (!ReferencedDomainName) {
    LocalFree(pSid);
    return 0;
  }
  if (!LookupAccountName(0, lpAccountName, pSid, &cbSid, ReferencedDomainName,
                         &cchReferencedDomainName, &eUse)) {
    LocalFree(ReferencedDomainName);
    LocalFree(pSid);
    return 0;
  }
  LocalFree(ReferencedDomainName);
  if (peUse) *peUse = eUse;
  return 0;
}
