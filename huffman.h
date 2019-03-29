#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "data_structures/code.h"

leaf * build_Codebook(char ** tokens, int num_tokens);
int encode_keys(leaf * root, char * s);
void free_huff(leaf * root);
int write_Codebook(int fd, leaf * root);
leaf * read_Codebook(int fd);
char * lookup_token(leaf * root, char * encoding);

#endif
