#define SPC_HOST_ALLOW 1
#define SPC_HOST_DENY  0

typedef struct {
  int       action;
  char      *name;
  in_addr_t addr;
  in_addr_t mask;
} spc_hostrule_t;

static int            spc_host_rulecount;
static spc_hostrule_t *spc_host_rules;

static int add_rule(spc_hostrule_t *rule) {
  spc_hostrule_t *tmp;

  if (!(spc_host_rulecount % 256)) {
    if (!(tmp = (spc_hostrule_t *)realloc(spc_host_rules,
                  sizeof(spc_host_rulecount) * (spc_host_rulecount + 256))))
      return 0;
    spc_host_rules = tmp;
  }
  spc_host_rules[spc_host_rulecount++] = *rule;
  return 1;
}

static void free_rules(void) {
  int i;

  if (spc_host_rules) {
    for (i = 0;  i < spc_host_rulecount;  i++)
      if (spc_host_rules[i].name) free(spc_host_rules[i].name);
    free(spc_host_rules);
    spc_host_rulecount = 0;
    spc_host_rules = 0;
  }
}

static in_addr_t parse_addr(char *str) {
  int       shift = 24;
  char      *tmp;
  in_addr_t addr = 0;

  for (tmp = str;  *tmp;  tmp++) {
    if (*tmp == '.') {
      *tmp = 0;
      addr |= (atoi(str) << shift);
      str = tmp + 1;
      if ((shift -= 8) < 0) return INADDR_NONE;
    } else if (!isdigit(*tmp)) return INADDR_NONE;
  }
  addr |= (atoi(str) << shift);
  return htonl(addr);
}

static in_addr_t make_mask(int bits) {
  in_addr_t mask;

  bits = (bits < 0 ? 0 : (bits > 32 ? 32 : bits));
  for (mask = 0;  bits--;  mask |= (1 << (31 - bits)));

  return htonl(mask);
}

int spc_host_init(const char *filename) {
  int            lineno = 0;
  char           *buf, *p, *slash, *tmp;
  FILE           *f;
  size_t         bufsz, len = 0;
  spc_hostrule_t rule;

  if (!(f = fopen(filename, "r"))) return 0;
  if (!(buf = (char *)malloc(256))) {
    fclose(f);
    return 0;
  }
  while (fgets(buf + len, bufsz - len, f) != 0) {
    len += strlen(buf + len);
    if (buf[len - 1] != '\n') {
      if (!(buf = (char *)realloc((tmp = buf), bufsz += 256))) {
        fprintf(stderr, "%s line %d: out of memory\n", filename, ++lineno);
        free(tmp);
        fclose(f);
        free_rules();
        return 0;
      }
      continue;
    }
    buf[--len] = 0;
    lineno++;
    for (tmp = buf;  *tmp && isspace(*tmp);  tmp++) len--;
    while (len && isspace(tmp[len - 1])) len--;
    tmp[len] = 0;
    len = 0;
    if (!tmp[0] || tmp[0] == '#' || tmp[0] == ';') continue;

    memset(&rule, 0, sizeof(rule));
    if (strncasecmp(tmp, "allow:", 6) && strncasecmp(tmp, "deny:", 5)) {
      fprintf(stderr, "%s line %d: parse error; continuing anyway.\n",
              filename, lineno);
      continue;
    }

    if (!strncasecmp(tmp, "deny:", 5)) {
      rule.action = SPC_HOST_DENY;
      tmp += 5;
    } else {
      rule.action = SPC_HOST_ALLOW;
      tmp += 6;
    }
    while (*tmp && isspace(*tmp)) tmp++;
    if (!*tmp) {
      fprintf(stderr, "%s line %d: parse error; continuing anyway.\n",
              filename, lineno);
      continue;
    }

    for (p = tmp;  *p;  tmp = p) {
      while (*p && !isspace(*p)) p++;
      if (*p) *p++ = 0;
      if ((slash = strchr(tmp, '/')) != 0) {
        *slash++ = 0;
        rule.name = 0;
        rule.addr = parse_addr(tmp);
        rule.mask = make_mask(atoi(slash));
      } else {
        if (inet_addr(tmp) == INADDR_NONE) rule.name = strdup(tmp);
        else {
          rule.name = 0;
          rule.addr = inet_addr(tmp);
          rule.mask = 0xFFFFFFFF;
        }
      }
      if (!add_rule(&rule)) {
        fprintf(stderr, "%s line %d: out of memory\n", filename, lineno);
        free(buf);
        fclose(f);
        free_rules();
        return 0;
      }
    }
  }
  free(buf);
  fclose(f);
  return 1;
}
