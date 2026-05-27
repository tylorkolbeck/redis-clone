#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>
extern int logger_fd;

void logger(const char *msg);
int init_log(char *fileName);
void logger_close();
char *cat(const char *str1, const char *str2);
void bufFree(char *buf);
char *buf_to_str(size_t size);
void log_http(int connfd, const char *raw_req);

#endif
