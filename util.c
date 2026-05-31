#include "util.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int32_t write_all(int fd, char *buf, size_t n) {
  while (n > 0) {
    size_t rv = write(fd, buf, n);
    if (rv <= 0) {
      printf("error writing to buffer");
      return -1; // error
    }

    // decrement the byte count
    n -= (size_t)rv;

    // increment the buffer
    buf += rv;
  }

  return 0;
}

int32_t read_full(int fd, char *p_buf, size_t n) {
  while (n > 0) {
    ssize_t rv = read(fd, p_buf, n);
    if (rv <= 0) {
      return -1;
    }
    // if (rv == 0) {
    //   printf("read value is less than n bytes");
    //   // logger("readvalue is less than n bytes");
    //   return -1; // error, or unexpected EOF
    // }

    // Decrement the read pointer
    n -= (size_t)rv;

    // Increment the buffer pointer
    p_buf += rv;
  }

  return 0;
}

int open_file(char *fileName) {
  int fd = open(fileName, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (fd <= 0) {
    return -1;
  }
  return fd;
}
