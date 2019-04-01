#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "data_structures/code.h"

char find_escape_character(char ** tokens, int num_tokens);
char sanitize_tokens(char ** tokens, int num_tokens, char input_esc);
char * sanitize_token(char * token, char esc);
void reverse_sanitize_token(char * token, char * out);
leaf * build_AVL(char ** tokens, int num_tokens, leaf * root_AVL);
int build_Codebook(leaf * root_AVL);
int compress_file(int filedes, char ** tokens, int num_tokens, leaf * root, char esc_char);
int encode_keys(leaf * root, char * s);
void free_huff(leaf * root);
int write_Codebook_Driver(int fd, leaf * root, char esc_char);
int write_Codebook(int fd, leaf * root);
char * lookup_token(leaf * root, char * encoding);
leaf * read_Codebook(int fd, char *esc, int compress);
int decompress_file(int filedes, char * buffer, int size, leaf * root_huff, char esc);

leaf * huffman_insert(leaf * root, leaf * new, char * encoding, int level);

#endif
