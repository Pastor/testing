#include <windows.h>
#include <wincrypt.h>

BYTE *SpcDigestMessage(ALG_ID Algid, BYTE *pbIn, DWORD cbIn, DWORD *cbOut) {
  BYTE       *pbOut;
  DWORD      cbData = sizeof(DWORD);
  HCRYPTHASH hHash;
  HCRYPTPROV hProvider;

  CryptAcquireContext(&hProvider, 0, MS_DEF_PROV, PROV_RSA_FULL, 0);
  CryptCreateHash(hProvider, Algid, 0, 0, &hHash);
  CryptHashData(hHash, pbIn, cbIn, 0);
  CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)cbOut, &cbData, 0);
  pbOut = (BYTE *)LocalAlloc(LMEM_FIXED, *cbOut);
  CryptGetHashParam(hHash, HP_HASHVAL, pbOut, cbOut, 0);
  CryptDestroyHash(hHash);
  CryptReleaseContext(hProvider, 0);
  return pbOut;
}
