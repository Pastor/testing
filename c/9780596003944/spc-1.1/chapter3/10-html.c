#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* These are HTML tags that do not take arguments.  We special-case the <a> tag
 * since it takes an argument.  We will allow the tag as-is, or we will allow a
 * closing tag (e.g., </p>).  Additionally, we process tags in a case-
 * insensitive way.  Only letters and numbers are allowed in tags we can allow.
 * Note that we do a linear search of the tags.  A binary search is more
 * efficient (log n time instead of linear), but more complex to implement.
 * The efficiency hit shouldn't matter in practice.
 */
static unsigned char *allowed_formatters[]  = {
  "b", "big", "blink", "i", "s", "small", "strike", "sub", "sup", "tt", "u",
  "abbr", "acronym", "cite", "code", "del", "dfn", "em", "ins", "kbd", "samp",
  "strong", "var", "dir", "li", "dl", "dd", "dt", "menu", "ol", "ul", "hr",
  "br", "p", "h1", "h2", "h3", "h4", "h5", "h6", "center", "bdo", "blockquote",
  "nobr", "plaintext", "pre", "q", "spacer",
  /* include "a" here so that </a> will work */
  "a"
};

#define SKIP_WHITESPACE(p) while (isspace(*p)) p++

static int spc_is_valid_link(const char *input) {
  static const char *href = "href";
  static const char *http = "http://";
  int               quoted_string = 0, seen_whitespace = 0;

  if (!isspace(*input)) return 0;
  SKIP_WHITESPACE(input);
  if (strncasecmp(href, input, strlen(href))) return 0;
  input += strlen(href);
  SKIP_WHITESPACE(input);
  if (*input++ != '=') return 0;
  SKIP_WHITESPACE(input);
  if (*input == '"') {
    quoted_string = 1;
    input++;
  }
  if (strncasecmp(http, input, strlen(http))) return 0;
  for (input += strlen(http);  *input && *input != '>';  input++) {
    switch (*input) {
      case '.': case '/': case '-': case '_':
        break;
      case '"':
        if (!quoted_string) return 0;
        SKIP_WHITESPACE(input);
        if (*input != '>') return 0;
        return 1;
      default:
        if (isspace(*input)) {
          if (seen_whitespace && !quoted_string) return 0;
          SKIP_WHITESPACE(input);
          seen_whitespace = 1;
          break;
        }
        if (!isalnum(*input)) return 0;
        break;
    }
  }
  return (*input && !quoted_string);
}

static int spc_allow_tag(const char *input) {
  int  i;
  char *tmp;

  if (*input == 'a')
    return spc_is_valid_link(input + 1);
  if (*input == '/') {
    input++;
    SKIP_WHITESPACE(input);
  }
  for (i = 0;  i < sizeof(allowed_formatters);  i++) {
    if (strncasecmp(allowed_formatters[i], input, strlen(allowed_formatters[i])))
      continue;
    else {
      tmp = input + strlen(allowed_formatters[i]);
      SKIP_WHITESPACE(tmp);
      if (*input == '>') return 1;
    }
  }
  return 0;
}

/* Note: This interface expects a C-style NULL-terminated string. */
char *spc_escape_html(const char *input) {
  char       *output, *ptr;
  size_t     outputlen = 0;
  const char *c;

  /* This is a worst-case length calculation */
  for (c = input;  *c;  c++) {
    switch (*c) {
      case '<':  outputlen += 4; break; /* &lt; */
      case '>':  outputlen += 4; break; /* &gt; */
      case '&':  outputlen += 5; break; /* &amp; */
      case '\"': outputlen += 6; break; /* &quot; */
      default:   outputlen += 1; break;
    }
  }

  if (outputlen + 1 < strlen(input) || !(output = ptr = (char *)malloc(outputlen + 1))) return 0;
  for (c = input;  *c;  c++) {
    switch (*c) {
      case '<':
        if (!spc_allow_tag(c + 1)) {
          *ptr++ = '&';  *ptr++ = 'l';  *ptr++ = 't';  *ptr++ = ';';
          break;
        } else {
          do {
            *ptr++ = *c;
          } while (*++c != '>');
          *ptr++ = '>';
          break;
        }
      case '>':
        *ptr++ = '&';  *ptr++ = 'g';  *ptr++ = 't';  *ptr++ = ';';
        break;
      case '&':
        *ptr++ = '&';  *ptr++ = 'a';  *ptr++ = 'm';  *ptr++ = 'p';
        *ptr++ = ';';
        break;
      case '\"':
        *ptr++ = '&';  *ptr++ = 'q';  *ptr++ = 'u';  *ptr++ = 'o';
        *ptr++ = 't';  *ptr++ = ';';
        break;
      default:
        *ptr++ = *c;
        break;
    }
  }
  *ptr = 0;
  return output;
}
