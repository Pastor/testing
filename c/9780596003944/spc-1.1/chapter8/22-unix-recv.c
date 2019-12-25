int spc_confirmation_receive(const char *address, const char *id) {
  time_t        now;
  unsigned long i;

  /* Before we check the pending list of confirmations, prune the list to
   * remove expired entries.
   */
  now = time(0);
  for (i = 0;  i < confirmation_count;  i++) {
    if (confirmations[i].expire <= now) {
      free(confirmations[i].address);
      free(confirmations[i].id);
      if (confirmation_count > 1 && i < confirmation_count - 1)
        confirmations[i] = confirmations[confirmation_count - 1];
      i--;
      confirmation_count--;
    }
  }

  for (i = 0;  i < confirmation_count;  i++) {
    if (!strcmp(confirmations[i].address, address)) {
      if (strcmp(confirmations[i].id, id) != 0) return 0;
      free(confirmations[i].address);
      free(confirmations[i].id);
      if (confirmation_count > 1 && i < confirmation_count - 1)
        confirmations[i] = confirmations[confirmation_count - 1];
      confirmation_count--;
      return 1;
    }
  }
  return 0;
}
