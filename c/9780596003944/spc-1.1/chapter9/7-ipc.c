#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET  -1
#define closesocket(x)  close((x))
#else
#include <windows.h>
#include <winsock2.h>
#endif

#define SPC_SOCKETFLAG_BOUND  0x1
#define SPC_SOCKETFLAG_DGRAM  0x2

typedef struct {
#ifdef WIN32
  SOCKET            sd;
#else
  int               sd;
#endif
  int               domain;
  struct sockaddr   *addr;
  int               addrlen;
  int               flags;
} spc_socket_t;

void spc_socket_close(spc_socket_t *);

static int make_sockaddr(int *domain, struct sockaddr **addr, char *host,
                         int port) {
  int                 addrlen;
  in_addr_t           ipaddr;
  struct hostent      *he;
  struct sockaddr_in  *addr_inet;

  if (!host) ipaddr = INADDR_ANY;
  else {
    if (!(he = gethostbyname(host))) {
      if ((ipaddr = inet_addr(host)) == INADDR_NONE) return 0;
    } else ipaddr = *(in_addr_t *)he->h_addr_list[0];
    endhostent();
  }

#ifndef WIN32
  if (inet_addr("127.0.0.1") == ipaddr) {
    struct sockaddr_un  *addr_unix;

    *domain = PF_LOCAL;
    addrlen = sizeof(struct sockaddr_un);
    if (!(*addr = (struct sockaddr *)malloc(addrlen))) return 0;
    addr_unix = (struct sockaddr_un *)*addr;
    addr_unix->sun_family = AF_LOCAL;
    snprintf(addr_unix->sun_path, sizeof(addr_unix->sun_path),
             "/tmp/127.0.0.1:%d", port);
#ifndef linux
    addr_unix->sun_len = SUN_LEN(addr_unix) + 1;
#endif
    return addrlen;
  }
#endif

  *domain = PF_INET;
  addrlen = sizeof(struct sockaddr_in);
  if (!(*addr = (struct sockaddr *)malloc(addrlen))) return 0;
  addr_inet = (struct sockaddr_in *)*addr;
  addr_inet->sin_family = AF_INET;
  addr_inet->sin_port = htons(port);
  addr_inet->sin_addr.s_addr = ipaddr;
  return addrlen;
}

static spc_socket_t *create_socket(int type, int protocol, char *host, int port) {
  spc_socket_t  *sock;

  if (!(sock = (spc_socket_t *)malloc(sizeof(spc_socket_t)))) return 0;
  sock->sd    = INVALID_SOCKET;
  sock->addr  = 0;
  sock->flags = 0;
  if (!(sock->addrlen = make_sockaddr(&sock->domain, &sock->addr, host, port)))
    goto error_exit;
  if ((sock->sd = socket(sock->domain, type, protocol)) == INVALID_SOCKET)
    goto error_exit;
  return sock;

error_exit:
  if (sock) spc_socket_close(sock);
  return 0;
}

void spc_socket_close(spc_socket_t *sock) {
  if (!sock) return;
  if (sock->sd != INVALID_SOCKET) closesocket(sock->sd);
  if (sock->domain == PF_LOCAL && (sock->flags & SPC_SOCKETFLAG_BOUND))
    remove(((struct sockaddr_un *)sock->addr)->sun_path);
  if (sock->addr) free(sock->addr);
  free(sock);
}

spc_socket_t *spc_socket_listen(int type, int protocol, char *host, int port) {
  int           opt = 1;
  spc_socket_t  *sock = 0;

  if (!(sock = create_socket(type, protocol, host, port))) goto error_exit;
  if (sock->domain == PF_INET) {
    if (setsockopt(sock->sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
      goto error_exit;
    if (bind(sock->sd, sock->addr, sock->addrlen) == -1) goto error_exit;
  } else {
    if (bind(sock->sd, sock->addr, sock->addrlen) == -1) {
      if (errno != EADDRINUSE) goto error_exit;
      if (connect(sock->sd, sock->addr, sock->addrlen) != -1) goto error_exit;
      remove(((struct sockaddr_un *)sock->addr)->sun_path);
      if (bind(sock->sd, sock->addr, sock->addrlen) == -1) goto error_exit;
    }
  }
  sock->flags |= SPC_SOCKETFLAG_BOUND;
  if (type == SOCK_STREAM && listen(sock->sd, SOMAXCONN) == -1) goto error_exit;
  else sock->flags |= SPC_SOCKETFLAG_DGRAM;
  return sock;

error_exit:
  if (sock) spc_socket_close(sock);
  return 0;
}

spc_socket_t *spc_socket_accept(spc_socket_t *sock) {
  spc_socket_t  *new_sock = 0;

  if (!(new_sock = (spc_socket_t *)malloc(sizeof(spc_socket_t))))
    goto error_exit;
  new_sock->sd      = INVALID_SOCKET;
  new_sock->domain  = sock->domain;
  new_sock->addrlen = sock->addrlen;
  new_sock->flags   = 0;
  if (!(new_sock->addr = (struct sockaddr *)malloc(sock->addrlen)))
    goto error_exit;

  if (!(new_sock->sd = accept(sock->sd, new_sock->addr, &(new_sock->addrlen))))
    goto error_exit;
  return new_sock;

error_exit:
  if (new_sock) spc_socket_close(new_sock);
  return 0;
}

spc_socket_t *spc_socket_connect(char *host, int port) {
  spc_socket_t  *sock = 0;

  if (!(sock = create_socket(SOCK_STREAM, 0, host, port))) goto error_exit;
  if (connect(sock->sd, sock->addr, sock->addrlen) == -1) goto error_exit;
  return sock;

error_exit:
  if (sock) spc_socket_close(sock);
  return 0;
}

int spc_socket_sendto(spc_socket_t *sock, const void *msg, int len, int flags,
                      char *host, int port) {
  int             addrlen, domain, result = -1;
  struct sockaddr *addr = 0;

  if (!(addrlen = make_sockaddr(&domain, &addr, host, port))) goto end;
  result = sendto(sock->sd, msg, len, flags, addr, addrlen);

end:
  if (addr) free(addr);
  return result;
}

int spc_socket_recvfrom(spc_socket_t *sock, void *buf, int len, int flags,
                        spc_socket_t **src) {
  int result;

  if (!(*src = (spc_socket_t *)malloc(sizeof(spc_socket_t)))) goto error_exit;
  (*src)->sd      = INVALID_SOCKET;
  (*src)->domain  = sock->domain;
  (*src)->addrlen = sock->addrlen;
  (*src)->flags   = 0;
  if (!((*src)->addr = (struct sockaddr *)malloc((*src)->addrlen)))
    goto error_exit;
  result = recvfrom(sock->sd, buf, len, flags, (*src)->addr, &((*src)->addrlen));
  if (result == -1) goto error_exit;
  return result;

error_exit:
  if (*src) {
    spc_socket_close(*src);
    *src = 0;
  }
  return -1;
}

int spc_socket_send(spc_socket_t *sock, const void *buf, int buflen) {
  int nb, sent = 0;

  while (sent < buflen) {
    nb = send(sock->sd, (const char *)buf + sent, buflen - sent, 0);
    if (nb == -1 && (errno == EAGAIN || errno == EINTR)) continue;
    if (nb <= 0) return nb;
    sent += nb;
  }

  return sent;
}

int spc_socket_recv(spc_socket_t *sock, void *buf, int buflen) {
  int nb, recvd = 0;

  while (recvd < buflen) {
    nb = recv(sock->sd, (char *)buf + recvd, buflen - recvd, 0);
    if (nb == -1 && (errno == EAGAIN || errno == EINTR)) continue;
    if (nb <= 0) return nb;
    recvd += nb;
  }

  return recvd;
}
