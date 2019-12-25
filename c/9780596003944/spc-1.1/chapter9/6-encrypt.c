#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <krb5.h>

int spc_krb5_encrypt(krb5_context ctx, krb5_keyblock *key, void *inbuf,
                     size_t inlen, void **outbuf, size_t *outlen) {
#ifdef KRB5_GENERAL__
  size_t        blksz, newlen;
  krb5_data     in_data;
  krb5_enc_data out_data;

  if (krb5_c_block_size(ctx, key->enctype, &blksz)) return 0;
  if (!(inlen % blksz)) newlen = inlen + blksz;
  else newlen = ((inlen + blksz - 1) / blksz) * blksz;

  in_data.magic  = KV5M_DATA;
  in_data.length = newlen;
  in_data.data   = malloc(newlen);
  if (!in_data.data) return 0;

  memcpy(in_data.data, inbuf, inlen);
  spc_add_padding((unsigned char *)in_data.data + inlen, inlen, blksz);

  if (krb5_c_encrypt_length(ctx, key->enctype, in_data.length, outlen)) {
    free(in_data.data);
    return 0;
  }

  out_data.magic   = KV5M_ENC_DATA;
  out_data.enctype = key->enctype;
  out_data.kvno    = 0;
  out_data.ciphertext.magic  = KV5M_ENCRYPT_BLOCK;
  out_data.ciphertext.length = *outlen;
  out_data.ciphertext.data   = malloc(*outlen);
  if (!out_data.ciphertext.data) {
    free(in_data.data);
    return 0;
  }

  if (krb5_c_encrypt(ctx, key, 0, 0, &in_data, &out_data)) {
    free(in_data.data);
    return 0;
  }

  *outbuf = out_data.ciphertext.data;
  free(in_data.data);
  return 1;
#else
  int           result;
  void          *tmp;
  size_t        blksz, newlen;
  krb5_data     edata;
  krb5_crypto   crypto;

  if (krb5_crypto_init(ctx, key, 0, &crypto) != 0) return 0;

  if (krb5_crypto_getblocksize(ctx, crypto, &blksz)) {
    krb5_crypto_destroy(ctx, crypto);
    return 0;
  }
  if (!(inlen % blksz)) newlen = inlen + blksz;
  else newlen = ((inlen + blksz - 1) / blksz) * blksz;
  if (!(tmp = malloc(newlen))) {
    krb5_crypto_destroy(ctx, crypto);
    return 0;
  }
  memcpy(tmp, inbuf, inlen);
  spc_add_padding((unsigned char *)tmp + inlen, inlen, blksz);

  if (!krb5_encrypt(ctx, crypto, 0, tmp, inlen, &edata)) {
    if ((*outbuf = malloc(edata.length)) != 0) {
      result = 1;
      memcpy(*outbuf, edata.data, edata.length);
      *outlen = edata.length;
    }
    krb5_data_free(&edata);
  }

  free(tmp);
  krb5_crypto_destroy(ctx, crypto);
  return result;
#endif
}
