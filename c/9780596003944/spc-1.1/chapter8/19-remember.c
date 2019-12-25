int spc_remember_key(char *filename, char *ipnum, EVP_PKEY *key) {
  int   keytype, result = 0;
  char  *keydata;
  FILE  *fp = 0;

  keytype = get_keydata(key, &keydata);
  if (keytype == EVP_PKEY_NONE || !keydata) goto end;
  if (!(fp = fopen(filename, "a"))) goto end;
  fprintf(fp, "%s %d %s\n", ipnum, keytype, keydata);
  result = 1;

end:
  if (keydata) free(keydata);
  if (fp) fclose(fp);
  return result;
}

int spc_accept_key(char *filename, char *ipnum, EVP_PKEY *key) {
  int   result;
  char  answer[80];

  result = spc_lookup_key(filename, ipnum, key);
  if (result == 1) return 1;
  if (result == -1) {
    for (;;) {
      printf("FATAL ERROR!  A different key has been received from the server "
              "%s\nthan we have on record.  Do you wish to continue? ", ipnum);
      if (!fgets(answer, sizeof(answer), stdin)) continue;
      if (answer[0] == 'Y' || answer[0] == 'y') return 1;
      if (answer[0] == 'N' || answer[0] == 'n') return 0;
    }
  }

  for (;;) {
    printf("WARNING!  The server %s has presented has presented a key for which "
           "we have no\nprior knowledge.  Do you want to [r]eject the key, "
           "[a]ccept and remember it,\nor allow its use for only this [o]ne "
           "time? ", ipnum);
    if (!fgets(answer, sizeof(answer), stdin)) continue;
    if (answer[0] == 'r' || answer[0] == 'R') return 0;
    if (answer[0] == 'o' || answer[0] == 'O') return 1;
    if (answer[0] == 'a' || answer[0] == 'A') break;
  }

  if (!spc_remember_key(filename, ipnum, key))
    printf("Error remembering the key!  It will be accepted this one time only "
           "instead.\n");
  return 1;
}
