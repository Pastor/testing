typedef void (*spc_client_fn)(spc_socket_t *, spc_credentials *, void *);

void spc_unix_server(spc_client_fn callback, void *arg) {
  spc_socket_t    *client, *listener;
  spc_credentials *credentials;

  listener = spc_socket_listen(SOCK_STREAM, 0, "127.0.0.1", 2222);
  while ((client = spc_socket_accept(listener)) != 0) {
    if (!(credentials = spc_get_credentials(client->sd))) {
      printf("Unable to get credentials from connecting client!\n");
      spc_socket_close(client);
    } else {
      printf("Client credentials:\n\tuid: %d\n\tgid: %d\n",
             SPC_PEER_UID(credentials), SPC_PEER_GID(credentials));
      /* do something with the credentials and the connection ... */
      callback(client, credentials, arg);
    }
  }
}
