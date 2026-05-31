#include "logger.h"
#include "util.h"
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const size_t k_max_msg = 4096;

static int32_t query(int fd, const char *text) {
  uint32_t len = (uint32_t)strlen(text);
  if (len > k_max_msg) {
    logger("client: length of message is too long\n");
    return -1;
  }

  char wbuf[4 + k_max_msg];
  memcpy(wbuf, &len, 4);
  printf("Client writing message length %u\n", len);
  memcpy(&wbuf[4], text, len);

  // Write header
  int32_t err = write_all(fd, wbuf, 4 + len);
  if (err) {
    printf("error write_all()\n");
    return err;
  }

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

  int32_t err = query(fd, "lo1");
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
