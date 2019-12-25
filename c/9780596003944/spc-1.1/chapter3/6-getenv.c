#include <stdlib.h>
#include <string.h>

char *spc_getenv(const char *name) {
  char *value;

  if (!(value = getenv(name))) return 0;
  return strdup(value);
}
