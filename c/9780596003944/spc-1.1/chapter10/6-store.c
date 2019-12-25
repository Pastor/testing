#include <windows.h>
#include <wincrypt.h>

static PCCERT_CONTEXT FindIssuerInSystemStore(LPCTSTR pszStoreName,
                                             PCCERT_CONTEXT pSubjectContext) {
  HCERTSTORE     hCertStore;
  PCCERT_CONTEXT pIssuerContext

  if (!(hCertStore = CertOpenSystemStore(0, pszStoreName))) return 0;
  pIssuerContext = CertFindCertificateInStore(hCertStore, X509_ASN_ENCODING, 0,
                                         CERT_FIND_ISSUER_OF, pSubjectContext, 0);
  CertCloseStore(hCertStore, 0);
  return pIssuerContext;
}

static LPCTSTR SpcSystemStoreList[] = {
  TEXT("MY"), TEXT("CA"), TEXT("ROOT"), TEXT("SPC"), 0
};

HCERTSTORE SpcNewStoreForCert(PCCERT_CONTEXT pSubjectContext) {
  LPCTSTR        pszStoreName;
  HCERTSTORE     hCertStore;
  PCCERT_CONTEXT pIssuerContext;

  /* First create an in-memory store, and add the subject certificate to it */
  if (!(hCertStore = CertOpenStore(CERT_STORE_PROV_MEMORY, 0, 0, 0, 0))) return 0;
  if (!CertAddCertificateContextToStore(hCertStore, pSubjectContext,
 CERT_STORE_ADD_REPLACE_EXISTING, 0)) {
    CertCloseStore(hCertStore, 0);
    return 0;
  }

  pSubjectContext = CertDuplicateCertificateContext(pSubjectContext);
  while (!CertCompareCertificateName(X509_ASN_ENCODING,
         pSubjectContext->pCertInfo->Issuer, pSubjectContext->pCertInfo->Subject)){
    for (pszStoreName = SpcSystemStoreList;  pszStoreName; pszStoreName++) {
      pIssuerContext = FindIssuerInSystemStore(pszStoreName, pSubjectContext);
      if (pIssuerContext) {
        if (!CertAddCertificateContextToStore(hCertStore, pIssuerContext,
 CERT_STORE_ADD_REPLACE_EXISTING, 0)) {
          CertFreeCertificateContext(pSubjectContext);
          CertFreeCertificateContext(pIssuerContext);
          CertCloseStore(hCertStore, 0);
          return 0;
        }
        CertFreeCertificateContext(pSubjectContext);
        pSubjectContext = pIssuerContext;
        break;
      }
    }
    if (!pszStoreName) {
      CertFreeCertificateContext(pSubjectContext);
      CertCloseStore(hCertStore, 0);
      return 0;
    }
  }
  CertFreeCertificateContext(pSubjectContext);
  return hCertStore;
}
