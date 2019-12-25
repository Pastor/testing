void spc_bcprng_reseed(SPC_BCPRNG_CTX *prng, unsigned char *new_data, size_t l) {
  size_t        i;
  unsigned char m[SPC_MAX_KEYLEN + SPC_BLOCK_SZ];

  SPC_BCPRNG_LOCK();
  if (prng->kl > SPC_MAX_KEYLEN) prng->kl = SPC_MAX_KEYLEN;
  spc_bcprng_rand(prng, m, prng->kl + SPC_BLOCK_SZ);
  while (l > prng->kl) {
    for (i = 0;  i < prng->kl;  i++) m[i] ^= *new_data++;
    l -= prng->kl;
    spc_bcprng_init(prng, m, prng->kl, m + prng->kl, SPC_BLOCK_SZ);
    spc_bcprng_rand(prng, m, prng->kl + SPC_BLOCK_SZ);
  }
  for (i = 0;  i <l;  i++) m[i] ^= *new_data++;
  spc_bcprng_init(prng, m, prng->kl, m + prng->kl, SPC_BLOCK_SZ);
  SPC_BCPRNG_UNLOCK();
}
