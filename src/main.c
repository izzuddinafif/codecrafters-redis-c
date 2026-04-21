#include "arpa/inet.h"
#include "sys/types.h"
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "encoder.h"
#include "parser.h"
#include "strlib.h"

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

ssize_t receive(int sockfd, char *buf, size_t len) {
  const char *terminator = "\r\n";

  int param_count = 0;
  int count;
  char **tokens;
  ssize_t bytes = 0;
  do {
    bytes = recv(sockfd, buf, len, 0);
    if (bytes == 0)
      return 0; // client disconnected

    if (bytes == -1)
      return -1; // error, check errno
    char *first_line = parse_line(buf, terminator);
    if (str_to_int(first_line + 1, &param_count) == 1) {
      printf("Failed to convert string to int, invalid first line");
    }
    free(first_line);

    tokens = split_str_by_str_terminator(buf, terminator, &count);
  } while ((count - 1) / 2 < param_count);
  return 1;
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
      char *pong = "+PONG\r\n";
      char **tokens;
      int count = 0;
      const char *terminator = "\r\n";
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
        tokens = split_str_by_str_terminator(buf, terminator, &count);
        for (int i = 0; i < count; i++) {
          printf("tokens[%d] = '%s'\n", i, tokens[i]);
        }
        char *to_upper = tokens[2];
        str_to_upper(to_upper);
        if (strcmp(to_upper, "PING") == 0) {
          printf("received ping\n");
          if (send_all(client_fd, pong, strlen(pong)) == -1)
            perror("send_all");
        } else if (strcmp(to_upper, "ECHO") == 0) {
          if (count < 4) {
            char *msg = "ECHO : argument missing";
            printf("%s\n", msg);
            if (send_all(client_fd, msg, strlen(msg)) == -1)
              perror("send_all");
            break;
          }
          char *msg = encode_bulk_str(tokens[4]);
          if (send_all(client_fd, msg, strlen(msg)) == -1)
            perror("send_all");
        }
      }
      free_split(tokens, count);
      close(client_fd);
      exit(0);
    }
    close(client_fd);
  }

  close(server_fd);

  return 0;
}
