#include "logger.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int logger_fd = -1;

int init_log(char *fileName) {
  logger_fd = open(fileName, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (logger_fd < 0) {
    perror("Failed to open error log File Descriptor");
    return 1;
  } else {
    printf("Error log opened for writing FD: %d\n", logger_fd);
    return 0;
  }

  return 0;
}

void logger_close() { close(logger_fd); }

void logger(const char *msg) {
  time_t raw_time;
  struct tm *time_info;
  char timestamp[32];

  // Get current system time
  time(&raw_time);

  // convert to local time
  time_info = localtime(&raw_time);
  size_t ts_len =
      strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", time_info);

  ssize_t bw;
  if (ts_len > 0) {
    bw = write(logger_fd, timestamp, ts_len);
    if (bw < 0) {
      printf("error writing log\n");
    }
  }

  bw = write(logger_fd, msg, strlen(msg));
  bw = write(logger_fd, "\n", 1);
  fsync(logger_fd);
}

void log_http(int connfd) {
  char req_buffer[4096];
  memset(req_buffer, 0, sizeof(req_buffer));
  ssize_t bytes_read = read(connfd, req_buffer, sizeof(req_buffer) - 1);

  if (bytes_read < 0) {
    logger("error reading from connection socket");
    return;
  } else if (bytes_read == 0) {
    logger("client disconnected before sending request");
    return;
  }
  req_buffer[bytes_read] = '\0';

  ssize_t bw = write(logger_fd, req_buffer, strlen(req_buffer));
  if (bw < 0) {
    printf("error logging http request\n");
  }
}

// Caller is responsible for calling free on the returned string
char *cat(const char *str1, const char *str2) {
  size_t total_length = strlen(str1) + strlen(str2) + 1;
  char *sBuf = malloc(total_length);
  if (sBuf == NULL) {
    logger("Failed to initalize memory for string concatenation");
    return NULL;
  }

  strcpy(sBuf, str1);
  strcat(sBuf, str2);

  return sBuf;
}

void bufFree(char *buf) {
  if (buf) {
    free(buf);
  }
}

// Caller is responsible for calling free on the returned string
char *buf_to_str(size_t size) {
  size_t buffer_size = 32;
  char *str = (char *)malloc(buffer_size);
  if (str == NULL) {
    return NULL;
  }

  snprintf(str, buffer_size, "%zu", size);

  return str;
}
