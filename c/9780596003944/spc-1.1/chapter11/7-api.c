#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define EGD_SOCKET_PATH "/home/egd/socket"

/* NOTE: this needs to be augmented with whatever you need to do in order to seed
 * your application-level generator.  Clearly, seed that generator after you've
 * initialized the connection with the entropy server.
 */

static int spc_egd_fd = -1;

void spc_rand_init(void) {
  struct sockaddr_un a;

  if ((spc_egd_fd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("Entropy server connection failed");
    exit(-1);
  }
  a.sun_len    = sizeof(a);
  a.sun_family = AF_UNIX;
  strncpy(a.sun_path, EGD_SOCKET_PATH, sizeof(a.sun_path));
  a.sun_path[sizeof(a.sun_path) - 1] = 0;
  if (connect(spc_egd_fd, (struct sockaddr *)&a, sizeof(a))) {
    perror("Entropy server connection failed");
    exit(-1);
  }
}

unsigned char *spc_keygen(unsigned char *buf, size_t l) {
  ssize_t              nb;
  unsigned char        nbytes, *p, tbytes;
  static unsigned char cmd[2] = {0x01,};

  if (spc_egd_fd == -1) spc_rand_init();
  for (p = buf;  l;  l -= tbytes) {
    /* Build and send the request command to the EGD server */
    cmd[1] = (l > 255 ? 255 : l);
    do {
      if ((nb = write(spc_egd_fd, cmd, sizeof(cmd))) == -1 && errno != EINTR) {
        perror("Communication with entropy server failed");
        exit(-1);
      }
    } while (nb == -1);

    /* Get the number of bytes in the result */
    do {
      if ((nb = read(spc_egd_fd, &nbytes, 1)) == -1 && errno != EINTR) {
        perror("Communication with entropy server failed");
        exit(-1);
      }
    } while (nb == -1);
    tbytes = nbytes;

    /* Get all of the data from the result */
    while (nbytes) {
      do {
        if ((nb = read(spc_egd_fd, p, nbytes)) == -1) {
          if (errno == -1) continue;
          perror("Communication with entropy server failed");
          exit(-1);
        }
      } while (nb == -1);
      p      += nb;
      nbytes -= nb;
    }

    /* If we didn't get as much entropy as we asked for, the server has no more
     * left, so we must fall back on the application-level generator to avoid
     * blocking.
     */
    if (tbytes != cmd[l]) {
      spc_rand(p, l);
      break;
    }
  }
  return buf;
}

unsigned char *spc_entropy(unsigned char *buf, size_t l) {
  ssize_t              nb;
  unsigned char        *p;
  static unsigned char cmd = 0x02;

  if (spc_egd_fd == -1) spc_rand_init();
  /* Send the request command to the EGD server */
  do {
    if ((nb = write(spc_egd_fd, &cmd, sizeof(cmd))) == -1 && errno != EINTR) {
      perror("Communcation with entropy server failed");
      exit(-1);
    }
  } while (nb == -1);

  for (p = buf;  l;  p += nb, l -= nb) {
    do {
      if ((nb = read(spc_egd_fd, p, l)) == -1) {
        if (errno == -1) continue;
        perror("Communication with entropy server failed");
        exit(-1);
      }
    } while (nb == -1);
  }

  return buf;
}

void spc_egd_write_entropy(unsigned char *data, size_t l) {
  ssize_t              nb;
  unsigned char        *buf, nbytes, *p;
  static unsigned char cmd[4] = { 0x03, 0, 0, 0 };

  for (buf = data;  l;  l -= cmd[3]) {
    cmd[3] = (l > 255 ? 255 : l);
    for (nbytes = 0, p = cmd;  nbytes < sizeof(cmd);  nbytes += nb) {
      do {
        if ((nb = write(spc_egd_fd, cmd, sizeof(cmd) - nbytes)) == -1) {
          if (errno != EINTR) continue;
          perror("Communication with entropy server failed");
          exit(-1);
        }
      } while (nb == -1);
    }

    for (nbytes = 0;  nbytes < cmd[3];  nbytes += nb, buf += nb) {
      do {
        if ((nb = write(spc_egd_fd, data, cmd[3] - nbytes)) == -1) {
          if (errno != EINTR) continue;
          perror("Communication with entropy server failed");
          exit(-1);
        }
      } while (nb == -1);
    }
  }
}
