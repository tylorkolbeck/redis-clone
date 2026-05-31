#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

int32_t write_all(int fd, char *buf, size_t n);
int32_t read_full(int fd, char *buf, size_t n);
int open_file(char *fileName);

#endif // !UTIL_H
