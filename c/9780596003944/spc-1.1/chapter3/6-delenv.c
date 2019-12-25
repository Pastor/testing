#include <stdlib.h>
#include <string.h>

int spc_delenv(const char *name) {
  int         del = 0, envc, i, idx = -1;
  size_t      delsz = 0, envsz = 0, namelen;
  char        *envptr, **new_environ;
  extern int  spc_environ;
  extern char **environ;

  /* first compute the size of the new environment */
  namelen = strlen(name);
  for (envc = 0;  envc >= 0 && environ[envc];  envc++) {
    if (!strncmp(environ[envc], name, namelen) && environ[envc][namelen] == '=') {
      if (idx == -1) idx = envc;
      else {
        if (++del <= 0 || delsz > SIZE_T_MAX - strlen(environ[envc])) return 0;
        delsz += strlen(environ[envc]) + 1;
      }
    }
    if (envsz > SIZE_T_MAX - strlen(environ[envc])) return 0;
    envsz += strlen(environ[envc]) + 1;
  }
  if (envc < 0 || idx == -1) return (idx == -1);
  envc  -= del;   /* account for duplicate entries of the same name */
  envsz -= delsz;

  /* allocate memory for the new environment */
  if (envsz > SIZE_T_MAX - (sizeof(char *) * (envc + 1)) + 1) return 0;
  envsz += (sizeof(char *) * (envc + 1));
  if (!(new_environ = (char **)malloc(envsz))) return 0;
  envptr = (char *)new_environ + (sizeof(char *) * (envc + 1));

  /* copy the old environment into the new environment, ignoring any
   * occurrences of the environment variable that we want to delete.
   */
  for (envc = i = 0;  environ[envc];  envc++) {
    if (envc == idx || (del && !strncmp(environ[envc], name, namelen) &&
        environ[envc][namelen] == '=')) continue;
    new_environ[i++] = envptr;
    envsz = strlen(environ[envc]);
    memcpy(envptr, environ[envc], envsz + 1);
    envptr += (envsz + 1);
  }

  /* possibly free the old environment, then replace it with the new one */
  if (spc_environ) free(environ);
  environ = new_environ;
  spc_environ = 1;
  return 1;
}
