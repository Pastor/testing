#include <stdlib.h>
#include <string.h>

char *spc_escape_sql(const char *input, char quote, int wildcards) {
  char       *out, *ptr;
  const char *c;

  /* If every character in the input needs to be escaped, the resulting string
   * would at most double in size.  Also, include room for the surrounding quotes.
   */
  if (quote != '\'' && quote != '\"') return 0;
  if (strlen(input) > ((~(unsigned int)0)>>1)-3) return 0;
  if (!(out = ptr = (char *)malloc(strlen(input) * 2 + 2 + 1))) return 0;
  *ptr++ = quote;
  for (c = input;  *c;  c++) {
    switch (*c) {
      case '\'': case '\"':
        if (quote == *c) *ptr++ = *c;
        *ptr++ = *c;
        break;
      case '%': case '_': case '[': case ']':
        if (wildcards) *ptr++ = '\\';
        *ptr++ = *c;
        break;
      case '\\': *ptr++ = '\\'; *ptr++ = '\\'; break;
      case '\b': *ptr++ = '\\'; *ptr++ = 'b';  break;
      case '\n': *ptr++ = '\\'; *ptr++ = 'n';  break;
      case '\r': *ptr++ = '\\'; *ptr++ = 'r';  break;
      case '\t': *ptr++ = '\\'; *ptr++ = 't';  break;
      default:
        *ptr++ = *c;
        break;
    }
  }
  *ptr++ = quote;
  *ptr = 0;
  return out;
}
