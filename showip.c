#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <uchar.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: showip hostname\n");
    return EXIT_FAILURE;
  }
  struct addrinfo hints, *res, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(argv[1], nullptr, &hints, &res)) != 0) {
    fprintf(stderr, "getadrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  printf("IP Addresses for %s:\n\n", argv[1]);

  for (p = res; p != nullptr; p = p->ai_next) {
    void *addr;
    char *ipver;
    struct sockaddr_in *ipv4;
    struct sockaddr_in6 *ipv6;
    if (p->ai_family == AF_INET) {
      ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else {
      ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    printf("%s | ", ipver);

    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf("%s\n", ipstr);
  }
  freeaddrinfo(res);
}
