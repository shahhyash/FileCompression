#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>

int better_read (int filedes, char * buf, size_t nbyte, char * file, int line);
int better_write(int filedes, char * buf, size_t nbyte, char * file, int line);

#endif
