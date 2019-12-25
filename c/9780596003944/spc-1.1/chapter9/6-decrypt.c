#include <stdlib.h>
#include <string.h>
#include <krb5.h>

int spc_krb5_decrypt(krb5_context ctx, krb5_keyblock *key, void *inbuf,
                     size_t inlen, void **outbuf, size_t *outlen) {
#ifdef KRB5_GENERAL__
  int           padding;
  krb5_data     out_data;
  krb5_enc_data in_data;

  in_data.magic   = KV5M_ENC_DATA;
  in_data.enctype = key->enctype;
  in_data.kvno    = 0;
  in_data.ciphertext.magic  = KV5M_ENCRYPT_BLOCK;
  in_data.ciphertext.length = inlen;
  in_data.ciphertext.data   = inbuf;

  out_data.magic  = KV5M_DATA;
  out_data.length = inlen;
  out_data.data   = malloc(inlen);
  if (!out_data.data) return 0;

  if (krb5_c_block_size(ctx, key->enctype, &blksz)) {
    free(out_data.data);
    return 0;
  }
  if (krb5_c_decrypt(ctx, key, 0, 0, &in_data, &out_data)) {
    free(out_data.data);
    return 0;
  }

  if ((padding = spc_remove_padding((unsigned char *)out_data.data +
                               out_data.length - blksz, blksz)) == -1) {
    free(out_data.data);
    return 0;
  }

  *outlen = out_data.length - (blksz - padding);
  if (!(*outbuf = realloc(out_data.data, *outlen))) *outbuf = out_data.data;
  return 1;
#else
  int         padding, result;
  void        *tmp;
  size_t      blksz;
  krb5_data   edata;
  krb5_crypto crypto;

  if (krb5_crypto_init(ctx, key, 0, &crypto) != 0) return 0;
  if (krb5_crypto_getblocksize(ctx, crypto, &blksz) != 0) {
    krb5_crypto_destroy(ctx, crypto);
    return 0;
  }
  if (!(tmp = malloc(inlen))) {
    krb5_crypto_destroy(ctx, crypto);
    return 0;
  }
  memcpy(tmp, inbuf, inlen);
  if (!krb5_decrypt(ctx, crypto, 0, tmp, inlen, &edata)) {
    if ((padding = spc_remove_padding((unsigned char *)edata.data + edata.length -
                                 blksz, blksz)) != -1) {
      *outlen = edata.length - (blksz - padding);
      if ((*outbuf = malloc(*outlen)) != 0) {
        result = 1;
        memcpy(*outbuf, edata.data, *outlen);
      }
    }
    krb5_data_free(&edata);
  }

  free(tmp);
  krb5_crypto_destroy(ctx, crypto);
  return result;
#endif
}
