BOOL           bResult;
HCERTSTORE     hCertStore;
PCCRL_CONTEXT  pCRLContext;
PCCERT_CONTEXT pCertContext = 0;

if (!(hCertStore = SpcNewStoreForCert(pSubjectContext))) {
  /* handle an error condition--could not create the store */
  abort();
}
while ((pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext))) {
  /* do something with the certificate retrieved from the store.
   * if an error occurs, and enumeration must be terminated prematurely, the last
   * certificate retrieved must be freed manually.
   *
   * For example, attempt to retrieve the CRL for the certificate using the code
   * the can be found in Recipe 10.11.  If no CRL can be retrieved, or the CRL
   * cannot be added to the certificate store, consider it a failure and break
   * out of the enumeration.
   */
  if (!(pCRLContext = SpcRetrieveCRL(pCertContext, 0)) ||
      !CertAddCRLContextToStore(hCertStore, pCRLContext,
                                CERT_ADD_USE_EXISTING, 0)) {
    if (pCRLContext) CertFreeCRLContext(pCRLContext);
    break;
  }
  CertFreeCRLContext(pCRLContext);
}
if (pCertContext) {
  CertFreeCertificateContext(pCertContext);
  CertCloseStore(hCertStore, 0);
  abort();
}
bResult = SpcVerifyCert(hCertStore, pSubjectContext);
CertCloseStore(hCertStore, 0);
return bResult;
