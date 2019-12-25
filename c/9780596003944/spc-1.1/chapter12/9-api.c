#define SET_FN_PTR(func, num)                  \
        static inline void *get_##func(void) { \
          int  i, j = num / 4;                 \
          long ptr = (long)func + num;         \
          for (i = 0;  i < 2;  i++) ptr -= j;  \
          return (void *)(ptr - (j * 2));      \
        }
#define GET_FN_PTR(func) get_##func()
