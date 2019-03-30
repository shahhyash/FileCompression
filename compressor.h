#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "data_structures/code.h"
#include "data_structures/AVL.h"

int compress_file(int filedes, char ** tokens, int num_tokens, leaf * root);
int decompress_file(int filedes, char * buffer, int size, leaf * root_huff);

#endif
