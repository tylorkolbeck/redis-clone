#include "util.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int32_t write_all(int fd, char *buf, size_t n) {
  while (n > 0) {
    size_t bytes_written = write(fd, buf, n);
    if (bytes_written <= 0) {
      printf("error writing to buffer");
      return -1; // error
    }

    assert((size_t)bytes_written <= n);

    // decrement the byte count
    n -= (size_t)bytes_written;

    // increment the buffer
    buf += bytes_written;
  }

  return 0;
}

int32_t read_full(int fd, char *p_buf, size_t n) {
  while (n > 0) {
    ssize_t bytes_read = read(fd, p_buf, n);
    if (bytes_read <= 0) {
      return -1;
    }

    assert((size_t)bytes_read <= n);

    // Decrement the read pointer
    n -= (size_t)bytes_read;

    // Increment the buffer pointer
    p_buf += bytes_read;
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

void msg(const char *msg) { printf("%s\n", msg); }

void die(const char *msg) {
  int err = errno;
  fprintf(stderr, "[%d] %s\n", err, msg);
  abort();
}
