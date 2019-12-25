int   xy = MERGE_SHORT(0x1010, 0xFEEF);
char  i;
short ij = MERGE_CHAR(1, 12);

for (i = GET_CHAR(ij, 0);  i < GET_CHAR(ij, 1);  i++) {
  xy =  SET_SHORT(xy, 0, (GET_SHORT(xy, 0) + i));
  printf("x %#04hX y %#04hX\n", GET_SHORT(xy, 0), GET_SHORT(xy, 1));
}
