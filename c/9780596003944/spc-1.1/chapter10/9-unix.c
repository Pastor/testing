#include <string.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

int spc_fingerprint_cert(X509 *cert, EVP_MD *digest, unsigned char *fingerprint,
                        int *fingerprint_length) {

  if (*fingerprint_length < EVP_MD_size(digest))
    return 0;
  if (!X509_digest(cert, digest, fingerprint, fingerprint_length))
    return 0;
  return *fingerprint_length;
}

int spc_fingerprint_equal(unsigned char *fp1, int fp1len, unsigned char *fp2,
                         int fp2len) {
  return (fp1len == fp2len && !memcmp(fp1, fp2, fp1len));
}
