static LPSTR make_thin(LPWSTR wstr) {
  int   len;
  DWORD dwFlags;
  LPSTR str;

  dwFlags = WC_COMPOSITECHECK | WC_DISCARDNS;
  if (!(len = WideCharToMultiByte(CP_OEMCP, dwFlags, wstr, -1, 0, 0, 0, 0)))
    return 0;
  if (!(str = (LPSTR)LocalAlloc(LMEM_FIXED, len))) return 0;
  WideCharToMultiByte(CP_OEMCP, dwFlags, wstr, -1, str, len, 0, 0);
  return str;
}

static LPSTR GetDistributionPoint(PCCERT_CONTEXT pCertContext) {
  DWORD                 cbStructInfo, i, j;
  LPSTR                 lpszURL;
  LPVOID                pvStructInfo;
  CERT_EXTENSION        *pExtension;
  CERT_ALT_NAME_INFO    *pNameInfo;
  CRL_DIST_POINTS_INFO  *pInfo;

  pExtension = CertFindExtension(szOID_CRL_DIST_POINTS,
 pCertContext->pCertInfo->cExtension,
 pCertContext->pCertInfo->rgExtension);
  if (!pExtension) return 0;

  if (!CryptDecodeObject(X509_ASN_ENCODING, szOID_CRL_DIST_POINTS,
      pExtension->Value.pbData, pExtension->Value.cbData, 0, 0, &cbStructInfo))
    return 0;
  if (!(pvStructInfo = LocalAlloc(LMEM_FIXED, cbStructInfo))) return 0;
  CryptDecodeObject(X509_ASN_ENCODING, szOID_CRL_DIST_POINTS,
                    pExtension->Value.pbData, pExtension->Value.cbData, 0,
                    pvStructInfo, &cbStructInfo);
  pInfo = (CRL_DIST_POINTS_INFO *)pvStructInfo;
  for (i = 0;  i < pInfo->cDistPoint;  i++) {
    if (pInfo->rgDistPoint[i].DistPointName.dwDistPointNameChoice ==
        CRL_DIST_POINT_FULL_NAME) {
      pNameInfo = &pInfo->rgDistPoint[i].DistPointName.FullName;
      for (j = 0;  j < pNameInfo->cAltEntry;  i++) {
        if (pNameInfo->rgAltEntry[j].dwAltNameChoice == CERT_ALT_NAME_URL) {
          if (!(lpszURL = make_thin(pNameInfo->rgAltEntry[i].pwszURL))) break;
          LocalFree(pvStructInfo);
          return lpszURL;
        }
      }
    }
  }

  LocalFree(pvStructInfo);
  return 0;
}
