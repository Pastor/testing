#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

static int resources[] = {
  RLIMIT_CPU, RLIMIT_DATA, RLIMIT_STACK, RLIMIT_FSIZE,
#ifdef RLIMIT_NPROC
  RLIMIT_NPROC,
#endif
#ifdef RLIMIT_NOFILE
  RLIMIT_NOFILE,
#endif
#ifdef RLIMIT_OFILE
  RLIMIT_OFILE,
#endif
  -1
};

void spc_rsrclimit(int max_cpu, int max_data, int max_stack, int max_fsize,
                   int max_proc, int max_files) {
  int           limit, *resource;
  struct rlimit r;

  for (resource = resources;  *resource >= 0;  resource++) {
    switch (*resource) {
      case RLIMIT_CPU:    limit = max_cpu;    break;
      case RLIMIT_DATA:   limit = max_data;   break;
      case RLIMIT_STACK:  limit = max_stack;  break;
      case RLIMIT_FSIZE:  limit = max_fsize;  break;
#ifdef RLIMIT_NPROC
      case RLIMIT_NPROC:  limit = max_proc;   break;
#endif
#ifdef RLIMIT_NOFILE
      case RLIMIT_NOFILE: limit = max_files;  break;
#endif
#ifdef RLIMIT_OFILE
      case RLIMIT_OFILE:  limit = max_files;  break;
#endif
    }
    getrlimit(*resource, &r);
    r.rlim_cur = (limit < r.rlim_max ? limit : r.rlim_max);
    setrlimit(*resource, &r);
  }
}
