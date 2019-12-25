PCCRL_CONTEXT SpcRetrieveCRL(PCCERT_CONTEXT pSubject, PCCERT_CONTEXT pIssuer) {
  BYTE          *pbData;
  DWORD         cbData;
  LPSTR         lpszURL, lpszSecondURL;
  PCCRL_CONTEXT pCRL;

  if (!(lpszURL = SpcGetCertCRLURL(pSubject, pIssuer, FALSE))) return 0;
  if (!(pbData = RetrieveWebData(lpszURL, &cbData))) {
    lpszSecondURL = SpcGetCertCRLURL(pSubject, pIssuer, TRUE);
    if (!lpszSecondURL || !lstrcmpA(lpszURL, lpszSecondURL)) {
      if (lpszSecondURL) LocalFree(lpszSecondURL);
      LocalFree(lpszURL);
      return 0;
    }
    pbData = RetrieveWebData(lpszSecondURL, &cbData);
    LocalFree(lpszSecondURL);
  }

  if (pbData) {
    pCRL = CertCreateCRLContext(X509_ASN_ENCODING, pbData, cbData);
    LocalFree(pbData);
  }
  LocalFree(lpszURL);
  return pCRL;
}
