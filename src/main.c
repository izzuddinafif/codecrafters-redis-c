#include "arpa/inet.h"
#include "sys/types.h"
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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

int main() {
  // Disable output buffering
  setbuf(stdout, nullptr);
  setbuf(stderr, nullptr);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  printf("Logs from your program will appear here!\n");

  int server_fd, client_addr_len;
  struct sockaddr_in client_addr;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(6379),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }

  printf("Waiting for a client to connect...\n");
  client_addr_len = sizeof client_addr;
  while (1) {
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                           (socklen_t *)&client_addr_len);
    if (client_fd == -1) {

      if (errno == EINTR)
        // retry on signal interrupt
        continue;

      fprintf(stderr, "accept: %s\n", strerror(errno));
      break;
    }
    char addr[INET6_ADDRSTRLEN];
    inet_ntop(client_addr.sin_family, &(client_addr.sin_addr.s_addr), addr,
              sizeof addr);
    printf("Client connected from: %s\n", addr);

    if (!fork()) {
      close(server_fd);
      char buf[4096];
      char *msg = "+PONG\r\n";
      while (1) {
        ssize_t status = recv(client_fd, buf, sizeof buf, 0);
        if (status == 0) {
          printf("Client %s disconnected\n", addr);
          break;
        }
        if (status == -1) {
          perror("recv_all");
          continue;
        }
        printf("received: %s", buf);

        if (send_all(client_fd, msg, strlen(msg)) == -1)
          perror("send_all");
      }
      close(client_fd);
      exit(0);
    }
    close(client_fd);
  }

  close(server_fd);

  return 0;
}
