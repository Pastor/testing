typedef enum {
  SPC_ARRAY_SPLIT, SPC_ARRAY_MERGE, SPC_ARRAY_FOLD, SPC_ARRAY_FLAT
} spc_array_type;

typedef struct {
  spc_array_type type;
  int            sz_elem;
  int            num_elem;
  int            split;
  unsigned char  data[1];
} spc_array_t;
