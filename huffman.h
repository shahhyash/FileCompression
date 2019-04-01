#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "data_structures/code.h"

/*
 *      Takes a whitespace token and sanitizes it according to the escape character.
 *      Returns the sanitized token.
 */
char * sanitize_token(char * token, char esc);
/*
 *      Returns origin whitespace token from sanitized token.
 */
void reverse_sanitize_token(char * token, char * out);
/*
 *      Replaces whitespace with corresponding escape character
 */
char sanitize_tokens(char ** tokens, int num_tokens, char input_esc);
/*
 *      Builds HuffmanCodebook and writes it to a file based on the AVL tree of tokens. 
 *      Returns 0 on success, 1 otherwise.
 */
int build_Codebook(leaf * root_AVL);
/*
 *      Builds AVL tree and returns the root based on the array of string tokens.
 *      Returns root on success, NULL on failure.
 */
leaf * build_AVL(char ** tokens, int num_tokens, leaf * root_AVL);
/*
 *      Writes compressed file with file descriptor filedes. The encoding for each token in tokens
 *      is looked up in the Huffman tree rooted at root. There are num_tokens tokens and the
 *      escape character for the codebook is esc.
 *      Returns 0 on success, 1 otherwise.
 */
int compress_file(int filedes, char ** tokens, int num_tokens, leaf * root, char esc_char);
/*
 *      Decompressed file according to word and encodings in looked up in root_huff (root of
 *      huffman tree) There are num_tokens tokens and the escape character for the codebook is esc.
 *      Returns 0 on success, 1 otherwise.
 */
int decompress_file(int filedes, char * buffer, int size, leaf * root_huff, char esc);
/*
 *      Encodes keys in huffman tree rooted at root. s is the current encoding.
 *      Returns 0 on success, 1 otherwise.
 */
int encode_keys(leaf * root, char * s);
/*
 *      Recursively frees huffman tree rooted and root and no data fields within
 */
void free_huff(leaf * root);
/*
 *      Compresses if compress=TRUE, else decompresses according to codebook from filedescriptor fd
 */
leaf * read_Codebook(int fd, char *esc, int compress);
/*
 *      Inserts leaf new into huffman tree rooted at root according the encoding.
 *      level represents the current encoding up to the current level of the tree.
 *      Returns root of tree.
 */
leaf * huffman_insert(leaf * root, leaf * new, char * encoding, int level);
/*
 *      Driver to write HuffmanCodebook at file descriptor fd. Writes escape character into
 *      file and writes huffman tree rooted at root.
 *      Returns 0 on success, 1 otherwise.
 */
int write_Codebook_Driver(int fd, leaf * root, char esc_char);
/*
 *      Writes HuffmanCodebook at file descriptor fd. Writes huffman tree rooted at root.
 *      Returns 0 on success, 1 otherwise.
 */
int write_Codebook(int fd, leaf * root);

#endif
