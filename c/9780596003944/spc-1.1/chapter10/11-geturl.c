LPSTR SpcGetCertCRLURL(PCCERT_CONTEXT pSubject, PCCERT_CONTEXT pIssuer,
                       BOOL bLookupOnly) {
  LPSTR      lpszURL;
  SPC_CACERT *pCACert;

  if (!bLookupOnly) {
    if (pSubject && (lpszURL = GetDistributionPoint(pSubject)) != 0)
      return lpszURL;
    if (pIssuer && (lpszURL = GetDistributionPoint(pIssuer)) != 0)
      return lpszURL;
  }

  /* Get the fingerprint of the cert's issuer, and look it up in a table */
  if (pIssuer) {
    if (!(pCACert = SpcLookupCACert(pIssuer))) return 0;
    if (pCACert->lpszCRLURL) {
      lpszURL = (LPSTR)LocalAlloc(LMEM_FIXED, lstrlenA(pCACert->lpszCRLURL) + 1);
      if (!lpszURL) return 0;
      lstrcpy(lpszURL, pCACert->lpszCRLURL);
      return lpszURL;
    }
  }

  return 0;
}
