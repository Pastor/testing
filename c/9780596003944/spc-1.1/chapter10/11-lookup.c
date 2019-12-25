SPC_CACERT *SpcLookupCACert(PCCERT_CONTEXT pCertContext) {
  SPC_CACERT  *pCACert;
  BYTE        pbFingerPrint[16];  /* MD5 is 128 bits or 16 bytes */
  DWORD       cbFingerPrint;

  /* Compute the fingerprint of the certificate */
  cbFingerPrint = sizeof(pbFingerPrint);
  CryptHashCertificate(0, CALG_MD5, 0, pCertContext->pbCertEncoded,
                       pCertContext->cbCertEncoded, pbFingerPrint,
                       &cbFingerPrint);

  /* Compare the computed certificate against those in our lookup table */
  for (pCACert = rgLookupTable;  pCACert->pbFingerPrint; pCACert++) {
    if (!memcmp(pCACert->pbFingerPrint, pbFingerPrint, cbFingerPrint))
      return pCACert;
  }
  return 0;
}
