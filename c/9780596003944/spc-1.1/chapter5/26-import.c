HCRYPTKEY SpcImportKeyData(HCRYPTPROV hProvider, ALG_ID Algid, BYTE *pbKeyData,
                           DWORD cbKeyData) {
  BOOL       bResult = FALSE;
  BYTE       *pbData = 0;
  DWORD      cbData, cbHeaderLen, cbKeyLen, dwDataLen;
  ALG_ID     *pAlgid;
  HCRYPTKEY  hImpKey = 0, hKey;
  BLOBHEADER *pBlob;

  if (!CryptGetUserKey(hProvider, AT_KEYEXCHANGE, &hImpKey)) {
    if (GetLastError() != NTE_NO_KEY) goto done;
    if (!CryptGenKey(hProvider, AT_KEYEXCHANGE, (1024 << 16), &hImpKey))
      goto done;
  }

  cbData = cbKeyData;
  cbHeaderLen = sizeof(BLOBHEADER) + sizeof(ALG_ID);
  if (!CryptEncrypt(hImpKey, 0, TRUE, 0, 0, &cbData, cbData)) goto done;
  if (!(pbData = (BYTE *)LocalAlloc(LMEM_FIXED, cbData + cbHeaderLen)))
    goto done;
  CopyMemory(pbData + cbHeaderLen, pbKeyData, cbKeyData);
  cbKeyLen = cbKeyData;
  if (!CryptEncrypt(hImpKey, 0, TRUE, 0, pbData + cbHeaderLen, &cbKeyLen, cbData))
    goto done;

  pBlob  = (BLOBHEADER *)pbData;
  pAlgid = (ALG_ID *)(pbData + sizeof(BLOBHEADER));
  pBlob->bType    = SIMPLEBLOB;
  pBlob->bVersion = 2;
  pBlob->reserved = 0;
  pBlob->aiKeyAlg = Algid;
  dwDataLen = sizeof(ALG_ID);
  if (!CryptGetKeyParam(hImpKey, KP_ALGID, (BYTE *)pAlgid, &dwDataLen, 0))
    goto done;

  bResult = CryptImportKey(hProvider, pbData, cbData + cbHeaderLen, hImpKey, 0,
                           &hKey);
  if (bResult) spc_memset(pbKeyData, 0, cbKeyData);

done:
  if (pbData) LocalFree(pbData);
  CryptDestroyKey(hImpKey);
  return (bResult ? hKey : 0);
}
