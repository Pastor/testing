#include <windows.h>
#include <wincrypt.h>

DWORD SpcFingerPrintCert(PCCERT_CONTEXT pCertContext, ALG_ID Algid,
                         BYTE *pbFingerPrint, DWORD *pcbFingerPrint) {
  DWORD cbComputedHash;

  if (!CryptHashCertificate(0, Algid, 0, pCertContext->pbCertEncoded,
                            pCertContext->cbCertEncoded, 0, &cbComputedHash))
    return 0;
  if (*pcbFingerPrint < cbComputedHash) return 0;
  CryptHashCertificate(0, Algid, 0, pCertContext->pbCertEncoded,
                       pCertContext->cbCertEncoded, pbFingerPrint,
                       pcbFingerPrint);
  return *pcbFingerPrint;
}

int SpcFingerPrintEqual(BYTE *pbFingerPrint1, DWORD cbFingerPrint1,
                        BYTE *pbFingerPrint2, DWORD cbFingerPrint2) {
  return (cbFingerPrint1 == cbFingerPrint2 &&
          !memcmp(pbFingerPrint1, pbFingerPrint2, cbFingerPrint1));
}
