#include "network.h"
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
struct sockaddr_in client_addr = {};

const uint16_t PORT = 1234;
void print_peer_conn(const struct sockaddr_in *client_add);

int main() {
  int running = 1;
  int fd = server_listen(PORT);
  char buffer[100];
  snprintf(buffer, sizeof(buffer), "Server listening on port: %hu", PORT);
  msg(buffer);

  while (running) {
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    // Waits and listens for a request
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connfd < 0) {
      continue;
    }

    print_peer_conn(&client_addr);

    // Once a connection is made run while
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

  const char reply[] = "PONG";
  char wbuf[4 + sizeof(reply)];
  len = (uint32_t)strlen(reply);
  memcpy(wbuf, &len, 4);
  memcpy(&wbuf[4], reply, len);
  return write_all(connfd, wbuf, 4 + len);
}

void print_peer_conn(const struct sockaddr_in *addr) {

  // Create 16 bit buffer to hold IP address
  char client_ip[INET_ADDRSTRLEN];

  // Convert ip address from network byte order to string - network to
  // presentation
  inet_ntop(AF_INET, &addr->sin_addr, client_ip, INET_ADDRSTRLEN);
  int client_port = ntohs(addr->sin_port);

  // Log ip address of requester
  char log_msg[128];
  snprintf(log_msg, sizeof(log_msg), "Connection Accepted from %s:%d",
           client_ip, client_port);
  msg(log_msg);
}
