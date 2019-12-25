#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <openssl/conf.h>
#include <openssl/ocsp.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>

typedef struct {
  char          *name;
  unsigned char *fingerprint;
  unsigned int  fingerprint_length;
  char          *crl_uri;
  char          *ocsp_uri;
} spc_cacert_t;

spc_cacert_t *spc_lookup_cacert(X509 *cert);

static char *get_distribution_point(X509 *cert) {
  int                   extcount, i, j;
  const char            *extstr;
  CONF_VALUE            *nval;
  unsigned char         *data;
  X509_EXTENSION        *ext;
  X509V3_EXT_METHOD     *meth;
  STACK_OF(CONF_VALUE)  *val;

  if ((extcount = X509_get_ext_count(cert)) > 0) {
    for (i = 0; i < extcount; i++) {
      ext = X509_get_ext(cert, i);
      extstr = OBJ_nid2sn(OBJ_obj2nid(X509_EXTENSION_get_object(ext)));
      if (strcasecmp(extstr, "crlDistributionPoints")) continue;

      if (!(meth = X509V3_EXT_get(ext))) break;
      data = ext->value->data;
      val = meth->i2v(meth, meth->d2i(0, &data, ext->value->length), 0);
      for (j = 0;  j < sk_CONF_VALUE_num(val);  j++) {
        nval = sk_CONF_VALUE_value(val, j);
        if (!strcasecmp(nval->name, "URI"))
          return strdup(nval->value);
      }
    }
  }
  return 0;
}

char *spc_getcert_crlurl(X509 *cert, X509 *issuer, int lookup_only) {
  char          *uri;
  spc_cacert_t  *cacert;

  if (!lookup_only) {
    if (cert && (uri = get_distribution_point(cert)) != 0) return uri;
    if (issuer && (uri = get_distribution_point(issuer)) != 0) return uri;
  }

  /* Get the fingerprint of the cert's issuer, and look it up in a table */
  if (issuer) {
    if (!(cacert = spc_lookup_cacert(issuer))) return 0;
    return (cacert->crl_uri ? strdup(cacert->crl_uri) : 0);
  }
  return 0;
}
