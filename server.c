#include "logger.h"
#include "util.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int32_t one_request(int connfd);
const size_t k_max_msg = 4096;

const int port = 1234;

int main() {
  int running = 1;

  // AF_INET - IPv4
  // SOCK_STREAM - TCP
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    printf("Error creating socket\n");
  }

  int val = 1;
  int sock_err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  if (sock_err < 0) {
    printf("Error setting socket options\n");
  }

  struct in_addr in_a = {.s_addr = htonl(0)};
  struct sockaddr_in addr = {
      .sin_family = AF_INET, .sin_port = htons(1234), .sin_addr = in_a};

  int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv) {
    printf("FAILED TO BIND ADDRESS\n");
    logger("Failed to bind address to port\n");
    return 1;
  }

  printf("Server listening on port: %d\n", port);
  rv = listen(fd, SOMAXCONN);
  if (rv) {
    logger("Failed to listen on socket\n");
  }

  while (running) {
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connfd < 0) {
      continue;
    }

    // Create 16 bit buffer to hold IP address
    char client_ip[INET_ADDRSTRLEN];

    // Convert ip address from network byte order to string - network to
    // presentation
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr.sin_port);

    // Log ip address of requester
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Connection Accepted from %s:%d",
             client_ip, client_port);
    logger(log_msg);

    while (1) {
      int32_t err = one_request(connfd);
      if (err) {
        break;
      }
    }

    // do_something(connfd);
    close(connfd);
  }

  printf("Server shutting down");

  logger_close();
  return 0;
}

static int32_t one_request(int connfd) {
  char rbuf[4 + k_max_msg];

  // errno is set to the error code if the syscall failed. Must initialize to 0
  // before making a syscall because libc will not reset it to 0 if it
  // succeeds
  errno = 0;

  // Read the 4 bytes header to get the body length
  int32_t err = read_full(connfd, rbuf, 4);
  if (err) {
    msg(errno == 0 ? "EOF" : "read() error");
    return err;
  }

  uint32_t len = 0;
  memcpy(&len, rbuf, 4);
  if (len > k_max_msg) {
    printf("message too big %u\n", len);
    return -1;
  }

  // Read the request body
  err = read_full(connfd, &rbuf[4], len);
  if (err) {
    printf("read() error\n");
    return -1;
  }

  printf("Client sent %.*s\n", len, &rbuf[4]);

  const char reply[] = "world";
  char wbuf[4 + sizeof(reply)];
  len = (uint32_t)strlen(reply);
  memcpy(wbuf, &len, 4);
  memcpy(&wbuf[4], reply, len);
  return write_all(connfd, wbuf, 4 + len);
}
