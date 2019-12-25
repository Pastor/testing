void spc_generate_cleanup(void) {
  if (spc_wordlist_file) fclose(spc_wordlist_file);
  if (spc_wordlist_offsets) free(spc_wordlist_offsets);

  spc_wordlist_file     = 0;
  spc_wordlist_offsets  = 0;
  spc_wordlist_count    = 0;
  spc_wordlist_shortest = 0;
}
