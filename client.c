#include "unistd.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <uchar.h>

ssize_t send_all(int sockfd, char *msg, size_t len) {
  size_t total_sent = 0;
  while (total_sent < len) {
    ssize_t bytes = send(sockfd, msg + total_sent, len - total_sent, 0);
    if (bytes == -1)
      return -1;

    total_sent += (size_t)bytes;
  }
  return (ssize_t)total_sent;
}

ssize_t recv_all(int sockfd, char *buf, size_t len) {
  size_t total_received = 0;
  while (total_received < len) {
    ssize_t bytes = recv(sockfd, buf + total_received, len - total_received, 0);
    if (bytes == 0)
      return 0; // client disconnected

    if (bytes == -1)
      return -1; // error, check errno

    total_received += (size_t)bytes;
  }
  return (ssize_t)total_received;
}
int main(int argc, char **argv) {
  if (argc == 2 && (strcmp(argv[1], "-h") == 0)) {
    fprintf(stderr, "Local redis client\nusage: client <optional "
                    "flag>\n\n\t-d\tdebug flag, change port from "
                    "default 6379 to 6380");
    return 1;
  }
  int port = 6379;
  if (argc > 1) {
    if (strcmp(argv[1], "-d") == 0) {
      port = 6380;
    } else {
      fprintf(stderr, "Wrong flag, show help with -h\n");
      return 1;
    }
  }
  char port_str[8];
  sprintf(port_str, "%d", port);
  struct addrinfo *res, hints;
  int status;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if ((status = getaddrinfo("localhost", port_str, &hints, &res)) != 0) {
    fprintf(stderr, "getadrinfo: %s\n", gai_strerror(status));
    return 2;
  }
  int server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    freeaddrinfo(res);
    return 1;
  }

  status = connect(server_fd, res->ai_addr, res->ai_addrlen);
  if (status == -1) {
    printf("Failed to connect to redis server: %s\n", strerror(errno));
    freeaddrinfo(res);
    close(server_fd);
    return 1;
  }
  printf("Connected to redis server\n");
  char buf[4096];
  char *msg = "*1\r\n$4\r\nPING\r\n";
  if (send_all(server_fd, msg, strlen(msg)) == -1) {
    perror("send_all");
    return 1;
  }
  printf("message sent\n");
  ssize_t bytes = recv(server_fd, buf, sizeof buf, 0);
  if (bytes == 0) {
    printf("server disconnected");
    exit(1);
  }
  if (bytes == -1) {
    perror("recv_all");
  }
  printf("received: %s\n", buf);
  printf("sent once\n");
  memset(buf, 0, sizeof buf);

  if (send_all(server_fd, msg, strlen(msg)) == -1) {
    perror("send_all");
    return 1;
  }

  bytes = recv(server_fd, buf, sizeof buf, 0);
  if (bytes == 0) {
    printf("server disconnected\n");
    exit(1);
  }
  if (bytes == -1) {
    perror("recv_all");
  }
  printf("received: %s\n", buf);
  printf("sent twice\n");
  close(server_fd);
  freeaddrinfo(res);
  return 0;
}
