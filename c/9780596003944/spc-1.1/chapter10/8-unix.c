#include <string.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>

int spc_verify_cert_hostname(X509 *cert, char *hostname) {
  int                   extcount, i, j, ok = 0;
  char                  name[256];
  X509_NAME             *subj;
  const char            *extstr;
  CONF_VALUE            *nval;
  unsigned char         *data;
  X509_EXTENSION        *ext;
  X509V3_EXT_METHOD     *meth;
  STACK_OF(CONF_VALUE)  *val;

  if ((extcount = X509_get_ext_count(cert)) > 0) {
    for (i = 0;  !ok && i < extcount;  i++) {
      ext = X509_get_ext(cert, i);
      extstr = OBJ_nid2sn(OBJ_obj2nid(X509_EXTENSION_get_object(ext)));
      if (!strcasecmp(extstr, "subjectAltName")) {
        if (!(meth = X509V3_EXT_get(ext))) break;
        data = ext->value->data;

        val = meth->i2v(meth, meth->d2i(0, &data, ext->value->length), 0);
        for (j = 0;  j < sk_CONF_VALUE_num(val);  j++) {
          nval = sk_CONF_VALUE_value(val, j);
          if (!strcasecmp(nval->name, "DNS") && !strcasecmp(nval->value, hostname)) {
            ok = 1;
            break;
          }
        }
      }
    }
  }

  if (!ok && (subj = X509_get_subject_name(cert)) &&
      X509_NAME_get_text_by_NID(subj, NID_commonName, name, sizeof(name)) > 0) {
    name[sizeof(name) - 1] = '\0';
    if (!strcasecmp(name, hostname)) ok = 1;
  }

  return ok;
}
