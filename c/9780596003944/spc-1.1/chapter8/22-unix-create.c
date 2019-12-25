#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Confirmation receipts must be received within one hour (3600 seconds) */
#define SPC_CONFIRMATION_EXPIRE 3600

typedef struct {
  char   *address;
  char   *id;
  time_t expire;
} spc_confirmation_t;

static unsigned long      confirmation_count, confirmation_size;
static spc_confirmation_t *confirmations;

static int new_confirmation(const char *address, const char *id) {
  unsigned long      i;
  spc_confirmation_t *tmp;

  /* first make sure that the address isn't already in the list */
  for (i = 0;  i < confirmation_count;  i++)
    if (!strcmp(confirmations[i].address, address)) return 0;

  if (confirmation_count == confirmation_size) {
    tmp = (spc_confirmation_t *)realloc(confirmations,
          sizeof(spc_confirmation_t) * (confirmation_size + 1));
    if (!tmp) return 0;
    confirmations = tmp;
    confirmation_size++;
  }
  confirmations[confirmation_count].address = strdup(address);
  confirmations[confirmation_count].id = strdup(id);
  confirmations[confirmation_count].expire = time(0) + SPC_CONFIRMATION_EXPIRE;
  if (!confirmations[confirmation_count].address ||
      !confirmations[confirmation_count].id) {
    if (confirmations[confirmation_count].address)
      free(confirmations[confirmation_count].address);
    if (confirmations[confirmation_count].id)
      free(confirmations[confirmation_count].id);
    return 0;
  }
  confirmation_count++;
  return 1;
}

int spc_confirmation_create(const char *address, char **id) {
  unsigned char buf[16];

  if (!spc_rand(buf, sizeof(buf))) return 0;
  if (!(*id = (char *)spc_base64_encode(buf, sizeof(buf), 0))) return 0;
  if (!new_confirmation(address, *id)) {
    free(*id);
    return 0;
  }
  return 1;
}
