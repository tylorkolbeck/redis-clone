#include "network.h"
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>

int server_listen(uint16_t port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    printf("Error creating socket\n");
    return -1;
  }

  int val = 1;
  int sock_err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  if (sock_err < 0) {
    printf("Error setting socket options\n");
    return -1;
  }

  struct in_addr in_a = {.s_addr = htonl(0)};
  struct sockaddr_in addr = {
      .sin_family = AF_INET, .sin_port = htons(port), .sin_addr = in_a};

  int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv) {
    printf("Failed to bind address to port\n");
    return -1;
  }

  rv = listen(fd, SOMAXCONN);
  if (rv) {
    printf("Failed to listen on socket\n");
  }
  return fd;
}
