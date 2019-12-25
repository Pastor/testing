#include <windows.h>
#include <wincrypt.h>

static LPSTR Crypt64Encode(BYTE *pBuffer) {
  int   i;
  DWORD dwTemp;
  LPSTR lpszOut, lpszPtr;

  static LPSTR lpszCrypt64Set = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwyxz";

  if (!(lpszOut = lpszPtr = (char *)LocalAlloc(LMEM_FIXED, 23))) return 0;

#define CRYPT64_ENCODE(x, y, z) \
  for (i = 0, dwTemp = (pBuffer[(x)] << 16) | (pBuffer[(y)] << 8) | \
       pBuffer[(z)];  i++ < 4;  dwTemp >>= 6) \
    *lpszPtr++ = lpszCrypt64Set[dwTemp & 0x3F]

  CRYPT64_ENCODE(0,  6, 12);  CRYPT64_ENCODE(1,  7, 13);
  CRYPT64_ENCODE(2,  8, 14);  CRYPT64_ENCODE(3,  9, 15);
  CRYPT64_ENCODE(4, 10,  5);

  for (i = 0, dwTemp = pBuffer[11];  i++ < 2;  dwTemp >>= 6)
    *lpszPtr++ = lpszCrypt64Set[dwTemp & 0x3F];
  *lpszPtr = 0;

#undef CRYPT64_ENCODE

  return lpszOut;
}

static BOOL ComputeHash(BYTE *pbHash, LPCSTR lpszKey, LPCSTR lpszSalt,
                        DWORD dwSaltLength) {
  int        i, length;
  DWORD      cbHash, dwKeyLength;
  HCRYPTHASH hHash, hHash1;
  HCRYPTPROV hProvider;

  dwKeyLength = lstrlenA(lpszKey);
  if (!CryptAcquireContext(&hProvider, 0, MS_DEF_PROV, 0, CRYPT_VERIFYCONTEXT))
    return FALSE;
  if (!CryptCreateHash(hProvider, CALG_MD5, 0, 0, &hHash)) {
    CryptReleaseContext(hProvider, 0);
    return FALSE;
  }
  CryptHashData(hHash, (BYTE *)lpszKey, dwKeyLength, 0);
  CryptHashData(hHash, (BYTE *)lpszSalt, dwSaltLength, 0);

  if (!CryptCreateHash(hProvider, CALG_MD5, 0, 0, &hHash1)) {
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProvider, 0);
    return FALSE;
  }
  CryptHashData(hHash1, lpszKey, dwKeyLength, 0);
  CryptHashData(hHash1, lpszSalt, dwSaltLength, 0);
  CryptHashData(hHash1, lpszKey, dwKeyLength, 0);
  cbHash = 16;  CryptGetHashParam(hHash1, HP_HASHVAL, pbHash, &cbHash, 0);
  CryptDestroyHash(hHash1);

  for (length = dwKeyLength;  length > 0;  length -= 16)
    CryptHashData(hHash, pbHash, (length > 16 ? 16 : length), 0);
  SecureZeroMemory(pbHash, 16);
  for (i = dwKeyLength;  i;  i >>= 1)
    if (i & 1) CryptHashData(hHash, pbHash, 1, 0);
    else CryptHashData(hHash, lpszKey, 1, 0);
  cbHash = 16;  CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &cbHash, 0);
  CryptDestroyHash(hHash);

  for (i = 0;  i < 1000;  i++) {
    if (!CryptCreateHash(hProvider, CALG_MD5, 0, 0, &hHash)) {
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }
    if (i & 1) CryptHashData(hHash, lpszKey, dwKeyLength, 0);
    else CryptHashData(hHash, pbHash, 16, 0);
    if (i % 3) CryptHashData(hHash, lpszSalt, dwSaltLength, 0);
    if (i & 7) CryptHashData(hHash, lpszKey, dwKeyLength, 0);
    if (i & 1) CryptHashData(hHash, pbHash, 16, 0);
    else CryptHashData(hHash, lpszKey, dwKeyLength, 0);
    cbHash = 16;  CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &cbHash, 0);
    CryptDestroyHash(hHash);
  }

  CryptReleaseContext(hProvider, 0);
  return TRUE;
}

LPSTR SpcMD5Encrypt(LPCSTR lpszKey, LPCSTR lpszSalt) {
  BYTE   pbHash[16], pbRawSalt[8];
  DWORD  dwResultLength, dwSaltLength;
  LPSTR  lpszBase64Out, lpszBase64Salt, lpszResult, lpszTemp;
  LPCSTR lpszSaltEnd;

  lpszBase64Out = lpszBase64Salt = lpszResult = 0;

  if (!lpszSalt) {
    spc_rand(pbRawSalt, (dwSaltLength = sizeof(pbRawSalt)));
    if (!(lpszBase64Salt = Crypt64Encode(pbRawSalt))) goto done;
    if (!(lpszTemp = (LPSTR)LocalReAlloc(lpszBase64Salt, dwSaltLength + 1, 0)))
      goto done;
    lpszBase64Salt = lpszTemp;
  } else {
    if (lpszSalt[0] != '$' || lpszSalt[1] != '1' || lpszSalt[2] != '$') goto done;
    for (lpszSaltEnd = lpszSalt + 3;  *lpszSaltEnd != '$'; lpszSaltEnd++)
      if (!*lpszSaltEnd) goto done;
    dwSaltLength = (lpszSaltEnd - (lpszSalt + 3));
    if (dwSaltLength > 8) dwSaltLength = 8; /* maximum salt is 8 bytes */
    if (!(lpszBase64Salt = (LPSTR)LocalAlloc(LMEM_FIXED, dwSaltLength + 1)))
      goto done;
    CopyMemory(lpszBase64Salt, lpszSalt + 3, dwSaltLength);
  }
  lpszBase64Salt[dwSaltLength] = 0;

  if (!ComputeHash(pbHash, lpszKey, lpszBase64Salt, dwSaltLength)) goto done;

  if (!(lpszBase64Out = Crypt64Encode(pbHash))) goto done;
  dwResultLength = lstrlenA(lpszBase64Out) + lstrlenA(lpszBase64Salt) + 5;
  if (!(lpszResult = (LPSTR)LocalAlloc(LMEM_FIXED, dwResultLength + 1)))
    goto done;
  wsprintfA(lpszResult, "$1$%s$%s", lpszBase64Salt, lpszBase64Out);

done:
  /* cleanup */
  if (lpszBase64Salt) LocalFree(lpszBase64Salt);
  if (lpszBase64Out) LocalFree(lpszBase64Out);
  return lpszResult;
}
