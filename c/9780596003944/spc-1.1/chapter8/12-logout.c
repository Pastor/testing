void spc_pam_logout(pam_handle_t *hndl) {
  if (!hndl) return;
  pam_close_session(hndl, 0);
  pam_end(hndl, PAM_SUCCESS);
}
