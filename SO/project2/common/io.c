#include "io.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//pthread_mutex_t server_fifo_mutex = PTHREAD_MUTEX_INITIALIZER;

// Check if all the bytes have been written
int check_write(int fd, void* buffer, size_t totalBytesToWrite){
  size_t bytesWritten = 0;
  while(bytesWritten < totalBytesToWrite){
    ssize_t bytesWritten_aux = write(fd, buffer + bytesWritten, totalBytesToWrite - bytesWritten);
    if(bytesWritten_aux == -1) {
      return 0;
    }
    else {
      bytesWritten += (size_t) bytesWritten_aux;
    }
  }
  return 1;
}

// Check if all the bytes have been read
int check_read(int fd, void* buffer, size_t totalBytesToRead){
  size_t bytesRead = 0;
  while(bytesRead < totalBytesToRead){
    ssize_t bytesRead_aux = read(fd, buffer + bytesRead, totalBytesToRead - bytesRead);
    if(bytesRead_aux == -1) {
      return 0;
    }
    else if(bytesRead_aux == 0 && bytesRead == 0) {
      return 2; // No client to read
    }
    else {
      bytesRead += (size_t) bytesRead_aux;
    }
    
  }
  return 1;
}

int parse_uint(int fd, unsigned int *value, char *next) {
  char buf[16];

  int i = 0;
  while (1) {
    ssize_t read_bytes = read(fd, buf + i, 1);
    if (read_bytes == -1) {
      return 1;
    } else if (read_bytes == 0) {
      *next = '\0';
      break;
    }

    *next = buf[i];

    if (buf[i] > '9' || buf[i] < '0') {
      buf[i] = '\0';
      break;
    }

    i++;
  }

  unsigned long ul = strtoul(buf, NULL, 10);

  if (ul > UINT_MAX) {
    return 1;
  }

  *value = (unsigned int)ul;

  return 0;
}

int print_uint(int fd, unsigned int value) {
  char buffer[16];
  size_t i = 16;

  for (; value > 0; value /= 10) {
    buffer[--i] = '0' + (char)(value % 10);
  }

  if (i == 16) {
    buffer[--i] = '0';
  }

  while (i < 16) {
    ssize_t written = write(fd, buffer + i, 16 - i);
    if (written == -1) {
      return 1;
    }

    i += (size_t)written;
  }

  return 0;
}

int print_str(int fd, const char *str) {
  size_t len = strlen(str);
  while (len > 0) {
    ssize_t written = write(fd, str, len);
    if (written == -1) {
      return 1;
    }

    str += (size_t)written;
    len -= (size_t)written;
  }

  return 0;
}
