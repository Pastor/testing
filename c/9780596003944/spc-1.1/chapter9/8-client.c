spc_socket_t *spc_unix_connect(void) {
  spc_socket_t *conn;

  if (!(conn = spc_socket_connect("127.0.0.1", 2222))) {
    printf("Unable to connect to the server!\n");
    return 0;
  }
  if (!spc_send_credentials(conn->sd)) {
    printf("Unable to send credentials to the server!\n");
    spc_socket_close(conn);
    return 0;
  }
  printf("Credentials were successfully sent to the server.\n");
  return conn;
}
