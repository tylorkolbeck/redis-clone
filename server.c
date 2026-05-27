#include "logger.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void do_something(int connfd);

int main() {
  int running = 1;
  int logErr = init_log("log.txt");
  if (logErr) {
    perror("Failed to initialize logger");
    return 1;
  }

  // AF_INET - IPv4
  // SOCK_STREAM - TCP
  // IPv4+TCP	socket(AF_INET, SOCK_STREAM, 0)
  // IPv6+TCP	socket(AF_INET6, SOCK_STREAM, 0)
  // IPv4+UDP	socket(AF_INET, SOCK_DGRAM, 0)
  // IPv6+UDP	socket(AF_INET6, SOCK_DGRAM, 0)
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  struct in_addr in_a = {.s_addr = htonl(0)};
  struct sockaddr_in addr = {
      .sin_family = AF_INET, .sin_port = htons(1234), .sin_addr = in_a};

  int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv) {
    logger("Failed to bind address to port");
    return 1;
  }

  rv = listen(fd, SOMAXCONN);
  if (rv) {
    logger("Failed to listen on socket");
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

    do_something(connfd);
    close(connfd);
  }

  logger_close();
  return 0;
}

static void do_something(int connfd) {
  log_http(connfd, "");
  // char req_buffer[4096];
  // memset(req_buffer, 0, sizeof(req_buffer));
  // ssize_t bytes_read = read(connfd, req_buffer, sizeof(req_buffer) - 1);
  //
  // if (bytes_read < 0) {
  //   logger("error reading from connection socket");
  //   return;
  // } else if (bytes_read == 0) {
  //   logger("client disconnected before sending request");
  //   return;
  // } else {
  //   logger("Read all incoming bytes");
  // }
  //
  // req_buffer[bytes_read] = '\0';
  // logger(req_buffer);

  // Read the buffer
  // char rbuf[64] = {};
  // ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
  // if (n < 0) {
  //   logger("buffer read() error");
  //   return;
  // }
  //
  // // Log what the client sent
  // char *clientMsg = cat("Client sent: ", rbuf);
  // logger(clientMsg);
  // bufFree(clientMsg);

  // Respond with a message
  char res[] = "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/plain\r\n"
               "Content-Length: 12\r\n"
               "Connection: close\r\n"
               "\r\n"
               "Hello World!";

  // char wbuf[] = "world";
  write(connfd, res, strlen(res));
}
