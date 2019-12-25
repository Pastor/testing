#include <windows.h>
#include <wincrypt.h>

static LPWSTR fold_wide(LPWSTR str) {
  int     len;
  LPWSTR  wstr;

  if (!(len = FoldStringW(MAP_PRECOMPOSED, str, -1, 0, 0))) return 0;
  if (!(wstr = (LPWSTR)LocalAlloc(LMEM_FIXED, len * sizeof(WCHAR))))
    return 0;
  if (!FoldStringW(MAP_PRECOMPOSED, str, -1, wstr, len)) {
    LocalFree(wstr);
    return 0;
  }

  return wstr;
}

static LPWSTR make_wide(LPCTSTR str) {
#ifndef UNICODE
  int     len;
  LPWSTR  wstr;

  if (!(len = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0)))
    return 0;
  if (!(wstr = (LPWSTR)LocalAlloc(LMEM_FIXED, len * sizeof(WCHAR))))
    return 0;
  if (!MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len)) {
    LocalFree(wstr);
    return 0;
  }

  return wstr;
#else
  return fold_wide(str);
#endif
}

BOOL SpcVerifyCertHostName(PCCERT_CONTEXT pCertContext, LPCTSTR hostname) {
  BOOL               bResult = FALSE;
  DWORD              cbStructInfo, dwCommonNameLength, i;
  LPSTR              szOID;
  LPVOID             pvStructInfo;
  LPWSTR             lpszCommonName, lpszDNSName, lpszHostName, lpszTemp;
  CERT_EXTENSION     *pExtension;
  CERT_ALT_NAME_INFO *pNameInfo;

  if (!(lpszHostName = make_wide(hostname))) return FALSE;

  /* Try SUBJECT_ALT_NAME2 first - it supercedes SUBJECT_ALT_NAME */
  szOID = szOID_SUBJECT_ALT_NAME2;
  pExtension = CertFindExtension(szOID, pCertContext->pCertInfo->cExtension,
 pCertContext->pCertInfo->rgExtension);
  if (!pExtension) {
    szOID = szOID_SUBJECT_ALT_NAME;
    pExtension = CertFindExtension(szOID, pCertContext->pCertInfo->cExtension,
 pCertContext->pCertInfo->rgExtension);
  }

  if (pExtension && CryptDecodeObject(X509_ASN_ENCODING, szOID,
      pExtension->Value.pbData, pExtension->Value.cbData, 0, 0, &cbStructInfo)) {
    if ((pvStructInfo = LocalAlloc(LMEM_FIXED, cbStructInfo)) != 0) {
      CryptDecodeObject(X509_ASN_ENCODING, szOID, pExtension->Value.pbData,
                        pExtension->Value.cbData, 0, pvStructInfo, &cbStructInfo);
      pNameInfo = (CERT_ALT_NAME_INFO *)pvStructInfo;
      for (i = 0;  !bResult && i < pNameInfo->cAltEntry; i++) {
        if (pNameInfo->rgAltEntry[i].dwAltNameChoice == CERT_ALT_NAME_DNS_NAME) {
          if (!(lpszDNSName = fold_wide(pNameInfo->rgAltEntry[i].pwszDNSName)))
            break;
          if (CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, lpszDNSName,
                             -1, lpszHostName, -1) == CSTR_EQUAL)
            bResult = TRUE;
          LocalFree(lpszDNSName);
        }
      }
      LocalFree(pvStructInfo);
      LocalFree(lpszHostName);
      return bResult;
    }
  }

  /* No subjectAltName extension -- check commonName */
  dwCommonNameLength = CertGetNameStringW(pCertContext, CERT_NAME_ATTR_TYPE, 0,
                                         szOID_COMMON_NAME, 0, 0);
  if (!dwCommonNameLength) {
    LocalFree(lpszHostName);
    return FALSE;
  }
  lpszTemp = (LPWSTR)LocalAlloc(LMEM_FIXED, dwCommonNameLength * sizeof(WCHAR));
  if (lpszTemp) {
    CertGetNameStringW(pCertContext, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME,
                       lpszTemp, dwCommonNameLength);
    if ((lpszCommonName = fold_wide(lpszTemp)) != 0) {
      if (CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, lpszCommonName,
                         -1, lpszHostName, -1) == CSTR_EQUAL)
        bResult = TRUE;
      LocalFree(lpszCommonName);
    }
    LocalFree(lpszTemp);
  }

  LocalFree(lpszHostName);
  return bResult;
}
