#include <windows.h>
#include <wincrypt.h>
#include <commctrl.h>

#define SPC_ENTROPY_PER_SAMPLE  0.5
#define SPC_KEYBOARD_DLGID      101
#define SPC_PROGRESS_BARID      1000
#define SPC_KEYBOARD_STATIC     -1

typedef struct {
  BYTE  bScanCode;
  DWORD dwTickCount;
} SPC_KEYPRESS;

static BOOL CALLBACK KeyboardEntropyProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam) {
  HWND *pHwnd;

  if (uMsg != WM_COMMAND || LOWORD(wParam) != IDOK ||
      HIWORD(wParam) != BN_CLICKED) return FALSE;

  pHwnd = (HWND *)GetWindowLong(hwndDlg, DWL_USER);
  DestroyWindow(hwndDlg);
  *pHwnd = 0;
  return TRUE;
}

BOOL SpcGatherKeyboardEntropy(HINSTANCE hInstance, HWND hWndParent,
                              BYTE *pbOutput, DWORD cbOutput) {
  MSG            msg;
  BOOL           bResult = FALSE;
  BYTE           bLastScanCode = 0, *pbHashData = 0;
  HWND           hwndDlg;
  DWORD          cbHashData, dwByteCount = sizeof(DWORD), dwLastTime = 0;
  double         dEntropy = 0.0;
  HCRYPTHASH     hHash = 0;
  HCRYPTPROV     hProvider = 0;
  SPC_KEYPRESS   KeyPress;

  if (!CryptAcquireContext(&hProvider, 0, MS_DEF_PROV, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT)) goto done;
  if (!CryptCreateHash(hProvider, CALG_SHA1, 0, 0, &hHash)) goto done;
  if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cbHashData, &dwByteCount,
                         0)) goto done;
  if (cbOutput > cbHashData) goto done;
  if (!(pbHashData = (BYTE *)LocalAlloc(LMEM_FIXED, cbHashData))) goto done;

  hwndDlg = CreateDialog(hInstance, MAKEINTRESOURCE(SPC_KEYBOARD_DLGID),
                         hWndParent, KeyboardEntropyProc);
  if (hwndDlg) {
    if (hWndParent) EnableWindow(hWndParent, FALSE);
    SetWindowLong(hwndDlg, DWL_USER, (LONG)&hwndDlg);
    SendDlgItemMessage(hwndDlg, SPC_PROGRESS_BARID, PBM_SETRANGE32, 0,
                       cbOutput * 8);
    while (hwndDlg && GetMessage(&msg, 0, 0, 0) > 0) {
      if ((msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) &&
          dEntropy < cbOutput * 8) {
        KeyPress.bScanCode   = ((msg.lParam >> 16) & 0x0000000F);
        KeyPress.dwTickCount = GetTickCount();
        CryptHashData(hHash, (BYTE *)&KeyPress, sizeof(KeyPress), 0);
        if (msg.message == WM_KEYUP || (bLastScanCode != KeyPress.bScanCode &&
            KeyPress.dwTickCount - dwLastTime > 100)) {
          bLastScanCode = KeyPress.bScanCode;
          dwLastTime = KeyPress.dwTickCount;
          dEntropy += SPC_ENTROPY_PER_SAMPLE;
          SendDlgItemMessage(hwndDlg, SPC_PROGRESS_BARID, PBM_SETPOS,
                             (WPARAM)dEntropy, 0);
          if (dEntropy >= cbOutput * 8) {
            EnableWindow(GetDlgItem(hwndDlg, IDOK), TRUE);
            SetFocus(GetDlgItem(hwndDlg, IDOK));
            MessageBeep(0xFFFFFFFF);
          }
        }
        continue;
      }
      if (!IsDialogMessage(hwndDlg, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    if (hWndParent) EnableWindow(hWndParent, TRUE);
  }

  if (dEntropy >= cbOutput * 8) {
    if (CryptGetHashParam(hHash, HP_HASHVAL, pbHashData, &cbHashData, 0)) {
      bResult = TRUE;
      CopyMemory(pbOutput, pbHashData, cbOutput);
    }
  }

done:
  if (pbHashData) LocalFree(pbHashData);
  if (hHash) CryptDestroyHash(hHash);
  if (hProvider) CryptReleaseContext(hProvider, 0);
  return bResult;
}
