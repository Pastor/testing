#ifdef WIN32
#include <windows.h>
#define SPC_C_RUNTIME              "msvcrt.dll"
#define SPC_LIBRARY_TYPE           HMODULE
#define SPC_LOAD_LIBRARY(name)     LoadLibrary((name))
#define SPC_RESOLVE_SYM(lib, name) GetProcAddress((lib), (name))
#else
#include <dlfcn.h>
#define SPC_C_RUNTIME              "libc.so"
#define SPC_LIBRARY_TYPE           void *
#define SPC_LOAD_LIBRARY(name)     dlopen((name), RTLD_LAZY);
#define SPC_RESOLVE_SYM(lib, name) dlsym((lib), (name))
#endif

enum file_op_enum {
  fileop_open, fileop_close, fileop_read, fileop_write, fileop_seek
};

void *file_op(enum file_op_enum op) {
  static SPC_LIBRARY_TYPE lib = 0;
  static struct FILEOP {
    void *open, *close, *read, *write, *seek;
  } s = {0};

  if (!lib) lib = SPC_LOAD_LIBRARY(SPC_C_RUNTIME);
  switch (op) {
    case fileop_open:
      if (!s.open) s.open = SPC_RESOLVE_SYM(lib, "open");
      return s.open;
    case fileop_close:
      if (!s.close) s.close = SPC_RESOLVE_SYM(lib, "close");
      return s.close;
    case fileop_read:
      if (!s.read) s.read = SPC_RESOLVE_SYM(lib, "read");
      return s.read;
    case fileop_write:
      if (!s.write) s.write = SPC_RESOLVE_SYM(lib, "write");
      return s.write;
    case fileop_seek:
      if (!s.seek) s.seek = SPC_RESOLVE_SYM(lib, "seek");
      return s.seek;
  }
  return 0;
}
