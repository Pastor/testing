typedef struct {
  char x;
  char y;
} spc_bool_t;

#define SPC_TEST_BOOL(b)      ((b).x + (b).y)
#define SPC_SET_BOOL_TRUE(b)  do { (b).x = 1;  (b).y = 0; } while (0)
#define SPC_SET_BOOL_FALSE(b) do { (b).x = -10;  (b).y = 10; } while (0)
