#include <security/pam_appl.h>
#include <security/pam_misc.h>

static struct pam_conv spc_pam_conv = { misc_conv, 0 };

pam_handle_t *spc_pam_login(const char *service, const char *user, int *rc) {
  pam_handle_t *hndl;

  if (!service || !user || !rc) {
    if (rc) *rc = PAM_ABORT;
    return 0;
  }
  if ((*rc = pam_start(service, user, &spc_pam_conv, &hndl)) != PAM_SUCCESS) {
    pam_end(hndl, *rc);
    return 0;
  }

  if ((*rc = pam_authenticate(hndl, PAM_DISALLOW_NULL_AUTHTOK)) != PAM_SUCCESS) {
    pam_end(hndl, *rc);
    return 0;
  }

  *rc = pam_acct_mgmt(hndl, 0);
  if (*rc == PAM_NEW_AUTHTOK_REQD) {
    pam_chauthtok(hndl, PAM_CHANGE_EXPIRED_AUTHTOK);
    *rc = pam_acct_mgmt(hndl, 0);
  }
  if (*rc != PAM_SUCCESS) {
    pam_end(hndl, *rc);
    return 0;
  }

  if ((*rc = pam_setcred(hndl, PAM_ESTABLISH_CRED)) != PAM_SUCCESS) {
    pam_end(hndl, *rc);
    return 0;
  }

  if ((*rc = pam_open_session(hndl, 0)) != PAM_SUCCESS) {
    pam_end(hndl, *rc);
    return 0;
  }

  /* no need to set *rc to PAM_SUCCESS; we wouldn't be here if it weren't */
  return hndl;
}
