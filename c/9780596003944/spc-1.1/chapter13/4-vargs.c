#include <stdarg.h>
#include <stdio.h>

#if defined(__GNUC__) && defined(i386)
/* NOTE: This is valid only using GCC on an x86 machine */

#define spc_next_varg(ap, type, var) \
  do { \
    unsigned int __frame; \
    __frame = *(unsigned int *)__builtin_frame_address(0); \
    if ((unsigned int)(ap) == __frame - 16) { \
      fprintf(stderr, "spc_next_varg() called too many times!\n"); \
      abort(); \
    } \
    (var) = va_arg((ap), (type)); \
  } while (0)

#define VARARG_CALL_1(func, a1)                             \
  func((a1))
#define VARARG_CALL_2(func, a1, a2)                         \
  func((a1), (a2))
#define VARARG_CALL_3(func, a1, a2, a3)                     \
  func((a1), (a2), (a3))
#define VARARG_CALL_4(func, a1, a2, a3, a4)                 \
  func((a1), (a2), (a3), (a4))
#define VARARG_CALL_5(func, a1, a2, a3, a4, a5)             \
  func((a1), (a2), (a3), (a4), (a5))
#define VARARG_CALL_6(func, a1, a2, a3, a4, a5, a6)         \
  func((a1), (a2), (a3), (a4), (a5), (a6))
#define VARARG_CALL_7(func, a1, a2, a3, a4, a5, a6, a7)     \
  func((a1), (a2), (a3), (a4), (a5), (a6), (a7))
#define VARARG_CALL_8(func, a1, a2, a3, a4, a5, a6, a7, a8) \
  func((a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8))

#else
/* NOTE: This should work on any machine with any compiler */

#define VARARG_MAGIC    0xDEADBEEF

#define spc_next_varg(ap, type, var) \
  do { \
    (var) = va_arg((ap), (type)); \
    if ((int)(var) == VARARG_MAGIC) { \
      fprintf(stderr, "spc_next_varg() called too many times!\n"); \
      abort(); \
    } \
  } while (0)

#define VARARG_CALL_1(func, a1)                             \
  func((a1), VARARG_MAGIC)
#define VARARG_CALL_2(func, a1, a2)                         \
  func((a1), (a2), VARARG_MAGIC)
#define VARARG_CALL_3(func, a1, a2, a3)                     \
  func((a1), (a2), (a3), VARARG_MAGIC)
#define VARARG_CALL_4(func, a1, a2, a3, a4)                 \
  func((a1), (a2), (a3), (a4), VARARG_MAGIC)
#define VARARG_CALL_5(func, a1, a2, a3, a4, a5)             \
  func((a1), (a2), (a3), (a4), (a5), VARARG_MAGIC)
#define VARARG_CALL_6(func, a1, a2, a3, a4, a5, a6)         \
  func((a1), (a2), (a3), (a4), (a5), (a6), VARARG_MAGIC)
#define VARARG_CALL_7(func, a1, a2, a3, a4, a5, a6, a7)     \
  func((a1), (a2), (a3), (a4), (a5), (a6), (a7), VARARG_MAGIC)
#define VARARG_CALL_8(func, a1, a2, a3, a4, a5, a6, a7, a8) \
  func((a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8), VARARG_MAGIC)

#endif
