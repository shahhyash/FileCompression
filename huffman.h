#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "data_structures/code.h"

leaf * build_Codebook(char ** tokens, int num_tokens);
int encode_keys(leaf * root, char * s);
int write_Codebook(char ** tokens, int num_tokens, char * name, leaf * root);

#endif
