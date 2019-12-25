static void socketpool_cleanup(void *arg) {
  SPC_RELEASE_MUTEX(socketpool_mutex);
}

int spc_socketpool_accept(int sd, struct sockaddr *addr, int *addrlen, int block) {
  int avail = 1, new_sd = -1;

  SPC_ACQUIRE_MUTEX(socketpool_mutex);
  SPC_CLEANUP_PUSH(socketpool_cleanup, 0);
  if (socketpool_limit > 0 && socketpool_used >= socketpool_limit) {
    if (!block) {
      avail = 0;
      errno = EMFILE;
    } else {
      while (socketpool_limit > 0 && socketpool_used >= socketpool_limit)
        SPC_WAIT_COND(socketpool_cond, socketpool_mutex);
    }
  }
  if (avail && (new_sd = accept(sd, addr, addrlen)) != -1)
    socketpool_used++;
  SPC_CLEANUP_POP(1);
  return new_sd;
}

int spc_socketpool_socket(int domain, int type, int protocol, int block) {
  int avail = 1, new_sd = -1;

  SPC_ACQUIRE_MUTEX(socketpool_mutex);
  SPC_CLEANUP_PUSH(socketpool_cleanup, 0);
  if (socketpool_limit > 0 && socketpool_used >= socketpool_limit) {
    if (!block) {
      avail = 0;
      errno = EMFILE;
    } else {
      while (socketpool_limit > 0 && socketpool_used >= socketpool_limit)
        SPC_WAIT_COND(socketpool_cond, socketpool_mutex);
    }
  }
  if (avail && (new_sd = socket(domain, type, protocol)) != -1)
    socketpool_used++;
  SPC_CLEANUP_POP(1);
  return new_sd;
}
