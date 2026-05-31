#include "util.h"
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const size_t k_max_msg = 4096;

static int32_t query(int fd, const char *text) {
  uint32_t len = (uint32_t)strlen(text);

  // Check length of message
  if (len > k_max_msg) {
    msg("client: length of message is too long");
    return -1;
  }

  // copy header length and messge to buffer
  // send request
  char wbuf[4 + k_max_msg];
  memcpy(wbuf, &len, 4);
  memcpy(&wbuf[4], text, len);

  // Write header and data to server connection
  int32_t err = write_all(fd, wbuf, 4 + len);
  if (err) {
    msg("error write_all()");
    return err;
  }

  // Reply
  char rbuf[4 + k_max_msg];
  errno = 0;
  err = read_full(fd, rbuf, 4);
  if (err) {
    msg(errno == 0 ? "EOF" : "read() error");
    return err;
  }
  memcpy(&len, rbuf, 4);
  if (len > k_max_msg) {
    msg("too long");
    return -1;
  }

  // reply body
  err = read_full(fd, &rbuf[4], len);
  if (err) {
    msg("read() error");
    return err;
  }

  printf("server says: %.*s\n", len, &rbuf[4]);

  return 0;
}

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    return 1;
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
  int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv) {
    return 1;
  }

  int32_t err = query(fd, "PING");
  if (err) {
    close(fd);
    return 0;
  }

  // char msg[] = "ping";
  // write(fd, msg, strlen(msg));
  //
  // char rbuf[64] = {};
  // ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
  // if (n < 0) {
  //   return 1;
  // }
  //
  // printf("server says: %s\n", rbuf);
  close(fd);
}
