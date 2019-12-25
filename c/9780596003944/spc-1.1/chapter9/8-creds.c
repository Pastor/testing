#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#if !defined(linux) && !defined(__NetBSD__)
#include <sys/ucred.h>
#endif

#ifndef SCM_CREDS
#define SCM_CREDS SCM_CREDENTIALS
#endif

#ifndef linux
#  ifndef __NetBSD__
#    define SPC_PEER_UID(c)   ((c)->cr_uid)
#    define SPC_PEER_GID(c)   ((c)->cr_groups[0])
#  else
#    define SPC_PEER_UID(c)   ((c)->sc_uid)
#    define SPC_PEER_GID(c)   ((c)->sc_gid)
#  endif
#else
#  define SPC_PEER_UID(c)   ((c)->uid)
#  define SPC_PEER_GID(c)   ((c)->gid)
#endif

#ifdef __NetBSD__
typedef struct sockcred spc_credentials;
#else
typedef struct ucred spc_credentials;
#endif

spc_credentials *spc_get_credentials(int sd) {
  int             nb, sync;
  char            ctrl[CMSG_SPACE(sizeof(struct ucred))];
  size_t          size;
  struct iovec    iov[1] = { { 0, 0 } };
  struct msghdr   msg = { 0, 0, iov, 1, ctrl, sizeof(ctrl), 0 };
  struct cmsghdr  *cmptr;
  spc_credentials *credentials;

#ifdef LOCAL_CREDS
  nb = 1;
  if (setsockopt(sd, 0, LOCAL_CREDS, &nb, sizeof(nb)) == -1) return 0;
#else
#ifdef SO_PASSCRED
  nb = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_PASSCRED, &nb, sizeof(nb)) == -1) return 0;
#endif
#endif

  do {
    msg.msg_iov->iov_base = (void *)&sync;
    msg.msg_iov->iov_len  = sizeof(sync);
    nb = recvmsg(sd, &msg, 0);
  } while (nb == -1 && (errno == EINTR || errno == EAGAIN));
  if (nb == -1) return 0;

  if (msg.msg_controllen < sizeof(struct cmsghdr)) return 0;
  cmptr = CMSG_FIRSTHDR(&msg);
#ifndef __NetBSD__
  size = sizeof(spc_credentials);
#else
  if (cmptr->cmsg_len < SOCKCREDSIZE(0)) return 0;
  size = SOCKCREDSIZE(((cred *)CMSG_DATA(cmptr))->sc_ngroups);
#endif
  if (cmptr->cmsg_len != CMSG_LEN(size)) return 0;
  if (cmptr->cmsg_level != SOL_SOCKET) return 0;
  if (cmptr->cmsg_type != SCM_CREDS) return 0;

  if (!(credentials = (spc_credentials *)malloc(size))) return 0;
  *credentials = *(spc_credentials *)CMSG_DATA(cmptr);
  return credentials;
}

int spc_send_credentials(int sd) {
  int             sync = 0x11223344;
  struct iovec    iov[1] = { { 0, 0, } };
  struct msghdr   msg = { 0, 0, iov, 1, 0, 0, 0 };

#if !defined(linux) && !defined(__NetBSD__)
  char            ctrl[CMSG_SPACE(sizeof(spc_credentials))];
  struct cmsghdr  *cmptr;

  msg.msg_control    = ctrl;
  msg.msg_controllen = sizeof(ctrl);

  cmptr = CMSG_FIRSTHDR(&msg);
  cmptr->cmsg_len   = CMSG_LEN(sizeof(spc_credentials));
  cmptr->cmsg_level = SOL_SOCKET;
  cmptr->cmsg_type  = SCM_CREDS;
  memset(CMSG_DATA(cmptr), 0, sizeof(spc_credentials));
#endif

  msg.msg_iov->iov_base = (void *)&sync;
  msg.msg_iov->iov_len  = sizeof(sync);

  return (sendmsg(sd, &msg, 0) != -1);
}
