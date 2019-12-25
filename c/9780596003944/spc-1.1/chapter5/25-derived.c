HCRYPTKEY SpcGetDerivedKey(HCRYPTPROV hProvider, ALG_ID Algid, LPTSTR password) {
  BOOL       bResult;
  DWORD      cbData;
  HCRYPTKEY  hKey;
  HCRYPTHASH hHash;

  if (!CryptCreateHash(hProvider, CALG_SHA1, 0, 0, &hHash)) return 0;
  cbData = lstrlen(password) * sizeof(TCHAR);
  if (!CryptHashData(hHash, (BYTE *)password, cbData, 0)) {
    CryptDestroyHash(hHash);
    return 0;
  }
  bResult = CryptDeriveKey(hProvider, Algid, hHash, CRYPT_EXPORTABLE, &hKey);
  CryptDestroyHash(hHash);
  return (bResult ? hKey : 0);
}
