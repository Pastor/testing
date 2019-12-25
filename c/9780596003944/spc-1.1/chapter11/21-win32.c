#include <windows.h>
#include <wincrypt.h>
#include <commctrl.h>

#define SPC_ENTROPY_PER_SAMPLE  0.5
#define SPC_MOUSE_DLGID         102
#define SPC_PROGRESS_BARID      1000
#define SPC_MOUSE_COLLECTID     1003
#define SPC_MOUSE_STATIC        1002

typedef struct {
  double     dEntropy;
  DWORD      cbRequested;
  POINT      ptLastPos;
  DWORD      dwLastTime;
  HCRYPTHASH hHash;
} SPC_DIALOGDATA;

typedef struct {
  POINT ptMousePos;
  DWORD dwTickCount;
} SPC_MOUSEPOS;

static BOOL CALLBACK MouseEntropyProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam) {
  SPC_MOUSEPOS    MousePos;
  SPC_DIALOGDATA  *pDlgData;

  switch (uMsg) {
    case WM_INITDIALOG:
      pDlgData = (SPC_DIALOGDATA *)lParam;
      SetWindowLong(hwndDlg, DWL_USER, lParam);
      SendDlgItemMessage(hwndDlg, SPC_PROGRESS_BARID, PBM_SETRANGE32, 0,
                         pDlgData->cbRequested);
      return TRUE;

    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED) {
        EndDialog(hwndDlg, TRUE);
        return TRUE;
      }
      break;

    case WM_MOUSEMOVE:
      pDlgData = (SPC_DIALOGDATA *)GetWindowLong(hwndDlg, DWL_USER);
      if (pDlgData->dEntropy < pDlgData->cbRequested) {
        MousePos.ptMousePos.x = LOWORD(lParam);
        MousePos.ptMousePos.y = HIWORD(lParam);
        MousePos.dwTickCount  = GetTickCount();
        ClientToScreen(hwndDlg, &(MousePos.ptMousePos));
        CryptHashData(pDlgData->hHash, (BYTE *)&MousePos, sizeof(MousePos), 0);
        if ((MousePos.ptMousePos.x != pDlgData->ptLastPos.x ||
             MousePos.ptMousePos.y != pDlgData->ptLastPos.y) &&
            MousePos.dwTickCount - pDlgData->dwLastTime > 100) {
          pDlgData->ptLastPos = MousePos.ptMousePos;
          pDlgData->dwLastTime = MousePos.dwTickCount;
          pDlgData->dEntropy += SPC_ENTROPY_PER_SAMPLE;
          SendDlgItemMessage(hwndDlg, SPC_PROGRESS_BARID, PBM_SETPOS,
                             (WPARAM)pDlgData->dEntropy, 0);
          if (pDlgData->dEntropy >= pDlgData->cbRequested) {
            EnableWindow(GetDlgItem(hwndDlg, IDOK), TRUE);
            SetFocus(GetDlgItem(hwndDlg, IDOK));
            MessageBeep(0xFFFFFFFF);
          }
        }
      }
      return TRUE;
  }

  return FALSE;
}

BOOL SpcGatherMouseEntropy(HINSTANCE hInstance, HWND hWndParent,
                              BYTE *pbOutput, DWORD cbOutput) {
  BOOL           bResult = FALSE;
  BYTE           *pbHashData = 0;
  DWORD          cbHashData, dwByteCount = sizeof(DWORD);
  HCRYPTHASH     hHash = 0;
  HCRYPTPROV     hProvider = 0;
  SPC_DIALOGDATA DialogData;

  if (!CryptAcquireContext(&hProvider, 0, MS_DEF_PROV, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT)) goto done;
  if (!CryptCreateHash(hProvider, CALG_SHA1, 0, 0, &hHash)) goto done;
  if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cbHashData, &dwByteCount,
                         0)) goto done;
  if (cbOutput > cbHashData) goto done;
  if (!(pbHashData = (BYTE *)LocalAlloc(LMEM_FIXED, cbHashData))) goto done;

  DialogData.dEntropy     = 0.0;
  DialogData.cbRequested = cbOutput * 8;
  DialogData.hHash        = hHash;
  DialogData.dwLastTime   = 0;
  GetCursorPos(&(DialogData.ptLastPos));

  bResult = DialogBoxParam(hInstance, MAKEINTRESOURCE(SPC_MOUSE_DLGID),
                           hWndParent, MouseEntropyProc, (LPARAM)&DialogData);

  if (bResult) {
    if (!CryptGetHashParam(hHash, HP_HASHVAL, pbHashData, &cbHashData, 0))
      bResult = FALSE;
    else
      CopyMemory(pbOutput, pbHashData, cbOutput);
  }

done:
  if (pbHashData) LocalFree(pbHashData);
  if (hHash) CryptDestroyHash(hHash);
  if (hProvider) CryptReleaseContext(hProvider, 0);
  return bResult;
}
