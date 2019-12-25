BYTE *SpcEncrypt(HCRYPTKEY hKey, BOOL bFinal, BYTE *pbData, DWORD *cbData) {
  BYTE   *pbResult;
  DWORD  dwBlockLen, dwDataLen;
  ALG_ID Algid;

  dwDataLen = sizeof(ALG_ID);
  if (!CryptGetKeyParam(hKey, KP_ALGID, (BYTE *)&Algid, &dwDataLen, 0)) return 0;
  if (GET_ALG_TYPE(Algid) != ALG_TYPE_STREAM) {
    dwDataLen = sizeof(DWORD);
    if (!CryptGetKeyParam(hKey, KP_BLOCKLEN, (BYTE *)&dwBlockLen, &dwDataLen, 0))
      return 0;
    dwDataLen = ((*cbData + (dwBlockLen * 2) - 1) / dwBlockLen) * dwBlockLen;
    if (!(pbResult = (BYTE *)LocalAlloc(LMEM_FIXED, dwDataLen))) return 0;
    CopyMemory(pbResult, pbData, *cbData);
    if (!CryptEncrypt(hKey, 0, bFinal, 0, pbResult, &dwDataLen, *cbData)) {
      LocalFree(pbResult);
      return 0;
    }
    *cbData = dwDataLen;
    return pbResult;
  }

  if (!(pbResult = (BYTE *)LocalAlloc(LMEM_FIXED, *cbData))) return 0;
  CopyMemory(pbResult, pbData, *cbData);
  if (!CryptEncrypt(hKey, 0, bFinal, 0, pbResult, cbData, *cbData)) {
    LocalFree(pbResult);
    return 0;
  }
  return pbResult;
}
