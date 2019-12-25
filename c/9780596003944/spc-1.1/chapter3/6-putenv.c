#include <stdlib.h>
#include <string.h>

static int spc_environ;

int spc_putenv(const char *name, const char *value) {
  int         del = 0, envc, i, mod = -1;
  char        *envptr, **new_environ;
  size_t      delsz = 0, envsz = 0, namelen, valuelen;
  extern char **environ;

  /* First compute the amount of memory required for the new environment */
  namelen  = strlen(name);
  valuelen = strlen(value);
  for (envc = 0;  envc >= 0 && environ[envc];  envc++) {
    if (!strncmp(environ[envc], name, namelen) && environ[envc][namelen] == '=') {
      if (mod == -1) mod = envc;
      else {
        if (++del <= 0 || delsz > SIZE_T_MAX - strlen(environ[envc])) return 0;
        delsz += strlen(environ[envc]) + 1;
      }
    }
    if (envsz > SIZE_T_MAX - strlen(environ[envc])) return 0;
    envsz += strlen(environ[envc]) + 1;
  }
  if (envc < 0) return 0;
  if (mod == -1) {
    if (++envc <= 0 || envsz > SIZE_T_MAX - namelen - valuelen - 1) return 0;
    envsz += (namelen + valuelen + 1 + 1);
  }
  envc  -= del;   /* account for duplicate entries of the same name */
  envsz -= delsz;

  /* allocate memory for the new environment */
  if (envsz > SIZE_T_MAX - (sizeof(char *) * (envc + 1)) + 1) return 0;
  envsz += (sizeof(char *) * (envc + 1));
  if (!(new_environ = (char **)malloc(envsz))) return 0;
  envptr = (char *)new_environ + (sizeof(char *) * (envc + 1));

  /* copy the old environment into the new environment,  eplacing the named
   * environment variable if it already exists; otherwise, add it at the end.
   */
  for (envc = i = 0;  environ[envc];  envc++) {
    if (del && !strncmp(environ[envc], name, namelen) &&
        environ[envc][namelen] == '=') continue;
    new_environ[i++] = envptr;
    if (envc != mod) {
      envsz = strlen(environ[envc]);
      memcpy(envptr, environ[envc], envsz + 1);
      envptr += (envsz + 1);
    } else {
      memcpy(envptr, name, namelen);
      memcpy(envptr + namelen + 1, value, valuelen);
      envptr[namelen] = '=';
      envptr[namelen + valuelen + 1] = 0;
      envptr += (namelen + valuelen + 1 + 1);
    }
  }
  if (mod == -1) {
    new_environ[i++] = envptr;
    memcpy(envptr, name, namelen);
    memcpy(envptr + namelen + 1, value, valuelen);
    envptr[namelen] = '=';
    envptr[namelen + valuelen + 1] = 0;
  }
  new_environ[i] = 0;

  /* possibly free the old environment, then replace it with the new one */
  if (spc_environ) free(environ);
  environ = new_environ;
  spc_environ = 1;
  return 1;
}
