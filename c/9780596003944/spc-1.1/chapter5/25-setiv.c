BOOL SpcSetIV(HCRYPTPROV hProvider, HCRYPTKEY hKey, BYTE *pbIV) {
  BOOL  bResult;
  BYTE  *pbTemp;
  DWORD dwBlockLen, dwDataLen;

  if (!pbIV) {
    dwDataLen = sizeof(dwBlockLen);
    if (!CryptGetKeyParam(hKey, KP_BLOCKLEN, (BYTE *)&dwBlockLen, &dwDataLen, 0))
      return FALSE;
    dwBlockLen /= 8;
    if (!(pbTemp = (BYTE *)LocalAlloc(LMEM_FIXED, dwBlockLen))) return FALSE;
    bResult = CryptGenRandom(hProvider, dwBlockLen, pbTemp);
    if (bResult)
      bResult = CryptSetKeyParam(hKey, KP_IV, pbTemp, 0);
    LocalFree(pbTemp);
    return bResult;
  }
  return CryptSetKeyParam(hKey, KP_IV, pbIV, 0);
}
