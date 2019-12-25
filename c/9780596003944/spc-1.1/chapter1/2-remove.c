#include <windows.h>

BOOL RemoveBackupAndRestorePrivileges(VOID) {
  BOOL              bResult;
  HANDLE            hProcess, hProcessToken;
  PTOKEN_PRIVILEGES pNewState;

  /* Allocate a TOKEN_PRIVILEGES buffer to hold the privilege change information.
   * Two privileges will be adjusted, so make sure there is room for two
   * LUID_AND_ATTRIBUTES elements in the Privileges field of TOKEN_PRIVILEGES.
   */
  pNewState = (PTOKEN_PRIVILEGES)LocalAlloc(LMEM_FIXED, sizeof(TOKEN_PRIVILEGES) +
                                            (sizeof(LUID_AND_ATTRIBUTES) * 2));
  if (!pNewState) return FALSE;

  /* Add the two privileges that will be removed to the allocated buffer */
  pNewState->PrivilegeCount = 2;
  if (!LookupPrivilegeValue(0, SE_BACKUP_NAME, &pNewState->Privileges[0].Luid) ||
      !LookupPrivilegeValue(0, SE_RESTORE_NAME, &pNewState->Privileges[1].Luid)) {
    LocalFree(pNewState);
    return FALSE;
  }
  pNewState->Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
  pNewState->Privileges[1].Attributes = SE_PRIVILEGE_REMOVED;

  /* Get a handle to the process's primary token.  Request TOKEN_ADJUST_PRIVILEGES
   * access so that we can adjust the privileges.  No other privileges are req'd
   * since we'll be removing the privileges and thus do not care about the previous
   * state.  TOKEN_QUERY access would be required in order to retrieve the previous
   * state information.
   */
  hProcess = GetCurrentProcess();
  if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hProcessToken)) {
    LocalFree(pNewState);
    return FALSE;
  }

  /* Adjust the privileges, specifying FALSE for DisableAllPrivileges so that the
   * NewState argument will be used instead.  Don't request information regarding
   * the token's previous state by specifying 0 for the last three arguments.
   */
  bResult = AdjustTokenPrivileges(hProcessToken, FALSE, pNewState, 0, 0, 0);

  /* Cleanup and return the success or failure of the adjustment */
  CloseHandle(hProcessToken);
  LocalFree(pNewState);
  return bResult;
}
