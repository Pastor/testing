int spc_host_check(int sockfd, int strict, int action) {
  int                i, rc;
  char               *hostname;
  struct sockaddr_in addr;

  if ((rc = check_spoofdns(sockfd, &addr, &hostname)) == -1) return -1;
  if (rc && (rc != SPC_ERROR_NOREVERSE || strict)) return rc;

  for (i = 0;  i < spc_host_rulecount;  i++) {
    if (spc_host_rules[i].name) {
      if (hostname && !strcasecmp(hostname, spc_host_rules[i].name)) {
        free(hostname);
        return (spc_host_rules[i].action == SPC_HOST_ALLOW);
      }
    } else {
      if ((addr.sin_addr.s_addr & spc_host_rules[i].mask) ==
          spc_host_rules[i].addr) {
        free(hostname);
        return (spc_host_rules[i].action == SPC_HOST_ALLOW);
      }
    }
  }

  if (hostname) free(hostname);
  return (action == SPC_HOST_ALLOW);
}
