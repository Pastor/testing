#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SPC_ERROR_NOREVERSE   1 /* IP address does not map to a hostname */
#define SPC_ERROR_NOHOSTNAME  2 /* Reversed hostname does not exist      */
#define SPC_ERROR_BADHOSTNAME 3 /* IP addresses do not match */
#define SPC_ERROR_HOSTDENIED  4 /* TCP/SPC Wrappers denied host access   */

static int check_spoofdns(int sockfd, struct sockaddr_in *addr, char **name) {
  int            addrlen, i;
  char           *hostname;
  struct hostent *he;

  *name = 0;
  for (;;) {
    addrlen = sizeof(struct sockaddr_in);
    if (getpeername(sockfd, (struct sockaddr *)addr, &addrlen) != -1) break;
    if (errno != EINTR && errno != EAGAIN) return -1;
  }

  for (;;) {
    he = gethostbyaddr((char *)&addr->sin_addr, sizeof(addr->sin_addr), AF_INET);
    if (he) break;
    if (h_errno == HOST_NOT_FOUND) {
      endhostent();
      return SPC_ERROR_NOREVERSE;
    }
    if (h_errno != TRY_AGAIN) {
      endhostent();
      return -1;
    }
  }

  hostname = strdup(he->h_name);
  for (;;) {
    if ((he = gethostbyname(hostname)) != 0) break;
    if (h_errno == HOST_NOT_FOUND) {
      endhostent();
      free(hostname);
      return SPC_ERROR_NOHOSTNAME;
    }
    if (h_errno != TRY_AGAIN) {
      endhostent();
      free(hostname);
      return -1;
    }
  }

  /* Check all IP addresses returned for the hostname.  If one matches, return
   * 0 to indicate that the address is not likely being spoofed.
   */
  for (i = 0;  he->h_addr_list[i];  i++)
    if (*(in_addr_t *)he->h_addr_list[i] == addr->sin_addr.s_addr) {
      *name = hostname;
      endhostent();
      return 0;
    }

  /* No matches.  Spoofing very likely */
  free(hostname);
  endhostent();
  return SPC_ERROR_BADHOSTNAME;
}
