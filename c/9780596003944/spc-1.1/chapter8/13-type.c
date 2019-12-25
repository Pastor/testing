#include <krb5.h>

typedef struct {
  krb5_context      ctx;
  krb5_auth_context auth_ctx;
  krb5_ticket       *ticket;
} spc_krb5bundle_t;
