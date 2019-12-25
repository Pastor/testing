#include <windows.h>
#include <wincrypt.h>

BOOL SpcVerifyCert(HCERTSTORE hCertStore, PCCERT_CONTEXT pSubjectContext) {
  DWORD           dwFlags;
  PCCERT_CONTEXT  pIssuerContext;

  if (!(pSubjectContext = CertDuplicateCertificateContext(pSubjectContext)))
    return FALSE;
  do {
    dwFlags = CERT_STORE_REVOCATION_FLAG | CERT_STORE_SIGNATURE_FLAG |
              CERT_STORE_TIME_VALIDITY_FLAG;
    pIssuerContext = CertGetIssuerCertificateFromStore(hCertStore,
 pSubjectContext, 0, &dwFlags);
    CertFreeCertificateContext(pSubjectContext);
    if (pIssuerContext) {
      pSubjectContext = pIssuerContext;
      if (dwFlags & CERT_STORE_NO_CRL_FLAG)
        dwFlags &= ~(CERT_STORE_NO_CRL_FLAG | CERT_STORE_REVOCATION_FLAG);
      if (dwFlags) break;
    } else if (GetLastError() == CRYPT_E_SELF_SIGNED) return TRUE;
  } while (pIssuerContext);
  return FALSE;
}
