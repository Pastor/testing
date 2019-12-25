#include <stdlib.h>
#include <limits.h>

/* Create a split array of num_elem elements, each of size sz_elem */
spc_array_t *spc_array_split(int sz_elem, int num_elem) {
  double      size;
  spc_array_t *a;

  size = (((double)sz_elem * (double)num_elem) / 2) + (double)sizeof(spc_array_t);
  if (size > (double)INT_MAX) return 0;
  if (!(a = (spc_array_t *)calloc((size_t)size, 1))) return 0;
  a->type     = SPC_ARRAY_SPLIT;
  a->sz_elem  = sz_elem;
  a->num_elem = num_elem;
  a->split    = 2; /* array is split into 2 arrays */
  return a;
}

/* Create two merged arrays with num_first elements in array 1 and num_second
 * elements in array 2
 */
spc_array_t *spc_array_merge(int sz_elem, int num_first, int num_second) {
  double      size;
  spc_array_t *a;

  size = (((double)num_first + (double)num_second) * (double)sz_elem) +
         (double)sizeof(spc_array_t);
  if (!num_first || size > (double)INT_MAX) return 0;
  if (!(a =  (spc_array_t *)calloc((size_t)size, 1))) return 0;
  a->type     = SPC_ARRAY_MERGE;
  a->sz_elem  = sz_elem;
  a->num_elem = num_first + num_second;
  a->split    = num_first / num_second;
  if (!a->split) a->split = (num_second / num_first) * -1;
  return a;
}

/* Create an array folded 'layers' times, with num_elem elements */
spc_array_t *spc_array_fold(int sz_elem, int num_elem, int layers) {
  double      size = (sz_elem * num_elem) + sizeof(spc_array_t);
  spc_array_t *a;

  size = ((double)sz_elem * (double)num_elem) + (double)sizeof(spc_array_t);
  if (size > (double)INT_MAX) return 0;
  if (!(a = (spc_array_t *)calloc((size_t)size, 1))) return 0;
  a->type     = SPC_ARRAY_FOLD;
  a->sz_elem  = sz_elem;
  a->num_elem = num_elem;
  a->split    = layers;

  return a;
}

/* Create a flattened array of num_dimen dimensions with num_elem elements per
 * dimension, flattened to a single dimension
 */
spc_array_t *spc_array_flat(int sz_elem, int num_elem, int num_dimen) {
  double      size;
  spc_array_t *a;

  size = ((double)sz_elem * (double)num_elem * (double)num_dimen) +
         (double)sizeof(spc_array_t);
  if (size > (double)INT_MAX) return 0;
  if (!(a = (spc_array_t *)calloc((size_t)size, 1))) return 0;
  a->type     = SPC_ARRAY_FLAT;
  a->sz_elem  = sz_elem;
  a->num_elem = num_elem * num_dimen;
  a->split    = num_dimen;

  return a;
}

/* return the real index of element 'idx' in array 'subarray' */
static int array_index(spc_array_t *a, int subarray, int idx) {
  int index = -1, num_row, diff;

  num_row = a->num_elem / a->split;
  switch (a->type) {
    case SPC_ARRAY_SPLIT:
      if (idx % a->split) index = idx / a->split;
      else index = (a->num_elem / a->split) + (idx / a->split);
      break;
    case SPC_ARRAY_MERGE:
      /* a->split == size diff between array 1 and 2 */
      if (a->split < 0) {
        subarray = !subarray;
        diff = a->split * -1;
      } else diff = a->split;
      if (!subarray) index = idx + idx / diff;
      else index = diff + (idx * (diff + 1));
      break;
    case SPC_ARRAY_FOLD:
      index = (idx / num_row) + (a->split * (idx % num_row) );
      break;
    case SPC_ARRAY_FLAT:
      index = subarray + (a->split * (idx % num_row));
      break;
  }
  return (index >= a->num_elem ? -1 : index);
}

/* Get a pointer to element 'idx' in array 'subarray' */
void *spc_array_get(spc_array_t *a, int subarray, int idx) {
  int index;

  if (!a || (index = array_index(a, subarray, idx)) == -1) return 0;
  return (void *)(a->data + (a->sz_elem * index));
}

/* Set element 'idx' in array 'subarray' to the data pointed to by 'src' --
 * note that the sz_elem used to initialize the array is used here to copy
 * the correct amount of data.
 */
int spc_array_set(spc_array_t *a, int subarray, int idx, void *src) {
  int index;

  if (!a || !src || (index = array_index(a, subarray, idx)) == -1)
    return 0;
  memcpy(a->data + (a->sz_elem * index), src, a->sz_elem);
  return 1;
}

/* Free an spc_array_t, including its table of elements */
int spc_array_free(spc_array_t *a) {
  if (a) free(a);
  return !!a;
}
