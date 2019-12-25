int spc_socketpool_close(int sd) {
  if (closesocket(sd) == SOCKET_ERROR) return -1;
  SPC_ACQUIRE_MUTEX(socketpool_mutex);
  if (socketpool_limit > 0 && socketpool_used == socketpool_limit)
    SPC_SIGNAL_COND(socketpool_cond);
  socketpool_used--;
  SPC_RELEASE_MUTEX(socketpool_mutex);
  return 0;
}
