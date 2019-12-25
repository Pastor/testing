#define FIPS_NUMBYTES      2500
#define FIPS_POKER_LOBOUND 1.03
#define FIPS_POKER_HIBOUND 57.4

int spc_fips_poker(unsigned char data[FIPS_NUMBYTES]) {
  int    i;
  long   counts[16] = {0,}, sum = 0;
  double result;

  for (i = 0;  i < FIPS_NUMBYTES;  i++) {
    counts[data[i] & 0xf]++;
    counts[data[i] >> 4]++;
  }
  for (i = 0;  i < 16;  i++)
    sum += (counts[i] * counts[i]);
  result = (16.0 / 5000) * (double)sum - 5000.0;
  return (result > FIPS_POKER_LOBOUND && result < FIPS_POKER_HIBOUND);
}
