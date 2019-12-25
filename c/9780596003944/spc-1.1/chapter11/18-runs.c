#define FIPS_NUMBYTES  2500
#define FIPS_LONGRUN   34
#define FIPS_RUNS_1_LO 2267
#define FIPS_RUNS_1_HI 2733
#define FIPS_RUNS_2_LO 1079
#define FIPS_RUNS_2_HI 1421
#define FIPS_RUNS_3_LO 502
#define FIPS_RUNS_3_HI 748
#define FIPS_RUNS_4_LO 223
#define FIPS_RUNS_4_HI 402
#define FIPS_RUNS_5_LO 90
#define FIPS_RUNS_5_HI 223
#define FIPS_RUNS_6_LO 90
#define FIPS_RUNS_6_HI 223

/* Perform both the "Runs" test and the "Long Run" test */
int spc_fips_runs(unsigned char data[FIPS_NUMBYTES]) {
  /* We allow a zero-length run size, mainly just to keep the array indexing less
   * confusing.  It also allows us to set cur_val arbitrarily below (if the first
   * bit of the stream is a 1, then runs[0] will be 1; otherwise, it will be 0).
   */
  int           runs[2][7] = {{0,},{0,}};
  int           cur_val, i, j, runsz;
  unsigned char curr;

  for (cur_val = i = runsz = 0;  i < FIPS_NUMBYTES;  i++) {
    curr = data[i];
    for (j = 0;  j < 8;  j++) {
      /* Check to see if the current bit is the same as the last one */
      if ((curr & 0x01) ^ cur_val) {
        /* The bits are different. A run is over, and a new run of 1 has begun */
        if (runsz >= FIPS_LONGRUN) return 0;
        if (runsz > 6) runsz = 6;
        runs[cur_val][runsz]++;
        runsz = 1;
        cur_val = (cur_val + 1) & 1; /* Switch the value. */
      } else runsz++;
      curr >>= 1;
    }
  }

  return (runs[0][1] > FIPS_RUNS_1_LO && runs[0][1] < FIPS_RUNS_1_HI &&
          runs[0][2] > FIPS_RUNS_2_LO && runs[0][2] < FIPS_RUNS_2_HI &&
          runs[0][3] > FIPS_RUNS_3_LO && runs[0][3] < FIPS_RUNS_3_HI &&
          runs[0][4] > FIPS_RUNS_4_LO && runs[0][4] < FIPS_RUNS_4_HI &&
          runs[0][5] > FIPS_RUNS_5_LO && runs[0][5] < FIPS_RUNS_5_HI &&
          runs[0][6] > FIPS_RUNS_6_LO && runs[0][6] < FIPS_RUNS_6_HI &&
          runs[1][1] > FIPS_RUNS_1_LO && runs[1][1] < FIPS_RUNS_1_HI &&
          runs[1][2] > FIPS_RUNS_2_LO && runs[1][2] < FIPS_RUNS_2_HI &&
          runs[1][3] > FIPS_RUNS_3_LO && runs[1][3] < FIPS_RUNS_3_HI &&
          runs[1][4] > FIPS_RUNS_4_LO && runs[1][4] < FIPS_RUNS_4_HI &&
          runs[1][5] > FIPS_RUNS_5_LO && runs[1][5] < FIPS_RUNS_5_HI &&
          runs[1][6] > FIPS_RUNS_6_LO && runs[1][6] < FIPS_RUNS_6_HI);
}
