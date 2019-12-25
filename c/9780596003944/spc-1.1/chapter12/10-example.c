#include <stdio.h>

int main(int argc, char *argv[]) {
  int         i, j, *p, val;
  spc_array_t *a_split, *a_merge, *a_flat, *a_fold;

  /* Split arrays */
  a_split = spc_array_split(sizeof(int), 8);
  for (i = 0;  i < a_split->num_elem;  i++) {
    val = i * 10;
    printf("%#.8X ", val);
    spc_array_set(a_split, 0, i, &val);
  }
  putchar('\n');
  for (i = 0;  i < a_split->num_elem;  i++) {
    if (!(p = (int *)spc_array_get(a_split, 0, i))) break;
    printf("%#.8X ", *p);
  }
  putchar('\n');

  /* Merged arrays */
  a_merge = spc_array_merge(sizeof(int), 4, 8);
  for (i = 0;  i < 4;  i++) {
    val = (i * 12) / 3;
    printf("%#.8X ", val);
    spc_array_set(a_merge, 0, i, &val);
  }
  putchar('\n');
  for (i = 0;  i < 8;  i++) {
    val = (i * 2) + 10;
    printf("%#.8X ", val);
    spc_array_set(a_merge, 1, i, &val);
  }
  putchar('\n');
  for (i = 0;  i < 4;  i++) {
    if (!(p = (int *)spc_array_get(a_merge, 0, i))) break;
    printf("%#.8X ", *p);
  }
  putchar('\n');
  for (i = 0;  i < 8;  i++) {
    if (!(p = (int *)spc_array_get(a_merge, 1, i))) break;
    printf("%#.8X ", *p);
  }
  putchar('\n');

  /* Folded arrays */
  a_fold = spc_array_fold(sizeof(int), 32, 4);
  for (i = 0;  i < a_fold->num_elem;  i++) {
    val = ((i * 3) + 2) % 256;
    printf("%#.2X ", val);
    spc_array_set(a_fold, 0, i, &val);
  }
  putchar('\n');
  for (i = 0;  i < a_fold->num_elem;  i++) {
    if (!(p = (int *)spc_array_get(a_fold, 0, i))) break;
    printf("%#.2X ", *p);
  }
  putchar('\n');

  /* Flat arrays */
  a_flat = spc_array_flat(sizeof(int), 6, 4);
  for (i = 0;  i < 4;  i++) {
    printf("Dimension %d: ", i);
    for (j = 0;  j < 6;  j++) {
      val = (i * j) << 2;
      printf("%#.8X ", val);
      spc_array_set(a_flat, i, j, &val);
    }
    putchar('\n');
  }
  for (i = 0;  i < 4;  i++) {
    printf("Dimension %d: ", i );
    for (j = 0;  j < 6;  j++) {
      if (!(p = spc_array_get(a_flat, i, j))) break;
      printf("%#.8X ", *p);
    }
    putchar('\n');
  }

  return 0;
}
