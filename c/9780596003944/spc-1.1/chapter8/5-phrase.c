#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPC_WORDLIST_FILE "/usr/share/dict/words"

static FILE         *spc_wordlist_file;
static size_t       *spc_wordlist_offsets;
static size_t       spc_wordlist_shortest;
static unsigned int spc_wordlist_count;

static int load_wordlist(void) {
  char         buf[80];
  FILE         *f;
  size_t       *offsets, shortest, *tmp;
  unsigned int count;

  if (!(f = fopen(SPC_WORDLIST_FILE, "r"))) return 0;
  if (!(offsets = (size_t *)malloc(sizeof(size_t) * 1024))) {
    fclose(f);
    return 0;
  }
  count      = 0;
  shortest   = ~0;
  offsets[0] = 0;

  while (fgets(buf, sizeof(buf), f))
    if (buf[strlen(buf) - 1] == '\n') {
      if (!((count + 1) % 1024)) {
        if (!(offsets = (size_t *)realloc((tmp = offsets),
                                          sizeof(size_t) * (count + 1025)))) {
          fclose(f);
          free(tmp);
          return 0;
        }
      }
      offsets[++count] = ftell(f);
      if (offsets[count] - offsets[count - 1] < shortest)
        shortest = offsets[count] - offsets[count - 1];
    }
  if (!feof(f)) {
    fclose(f);
    free(offsets);
    return 0;
  }

  if (ftell(f) - offsets[count - 1] < shortest)
    shortest = ftell(f) - offsets[count - 1];

  spc_wordlist_file     = f;
  spc_wordlist_offsets  = offsets;
  spc_wordlist_count    = count;
  spc_wordlist_shortest = shortest - 1; /* shortest includes NULL terminator */

  return 1;
}

static int get_wordlist_word(unsigned int num, char *buf, size_t bufsz) {
  size_t end, length;

  if (num >= spc_wordlist_count) return -1;
  if (num == spc_wordlist_count - 1) {
    fseek(spc_wordlist_file, 0, SEEK_END);
    end = ftell(spc_wordlist_file);
  } else end = spc_wordlist_offsets[num + 1];
  length = end - spc_wordlist_offsets[num]; /* includes NULL terminator */
  if (length > bufsz) return 0;
  if (fseek(spc_wordlist_file, spc_wordlist_offsets[num], SEEK_SET) == -1)
    return -1;
  fread(buf, length, 1, spc_wordlist_file);
  buf[length - 1] = 0;
  return 1;
}

char *spc_generate_passphrase(char *buf, size_t bufsz) {
  int          attempts = 0, rc;
  char         *outp;
  size_t       left, len;
  unsigned int idx;

  if (!spc_wordlist_file && !load_wordlist()) return 0;

  outp = buf;
  left = bufsz - 1;
  while (left > spc_wordlist_shortest) {
    idx = spc_rand_range(0, spc_wordlist_count - 1);
    rc  = get_wordlist_word(idx, outp, left + 1);
    if (rc == -1) return 0;
    else if (!rc && ++attempts < 10) continue;
    else if (!rc) break;

    len = strlen(outp) + 1;
    *(outp + len - 1) = ' ';
    outp += len;
    left -= len;
  }

  *(outp - 1) = 0;
  return buf;
}
