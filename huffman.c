#include "huffman.h"
#include "data_structures/AVL.h"
#include "data_structures/code.h"
#include "data_structures/heap.h"
#include "fileIO.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define DEBUG 0
#define TRUE 1
#define FALSE 0

/*
 *      Takes a whitespace token and sanitizes it according to the escape character.
 *      Returns the sanitized token.
 */
char * sanitize_token(char * token, char esc)
{
        char old = token[0];
        free(token);
        // Convert ascii dec to string
        char dec[3];
        dec[0] = old / 100 + 48;
        dec[1] = (old / 10) % 10 + 48;
        dec[2] = (old % 10) + 48;
        //printf("code: %c%c%c\n", dec[0], dec[1], dec[2]);
        token = (char *) malloc(sizeof(char) * 5);
        token[0] = esc;
        strncpy(&token[1], dec, 3);
        token[4] = '\0';
        return token;
}

/*
 *      Returns origin whitespace token from sanitized token.
 */
void reverse_sanitize_token(char * token, char * out)
{
        out[0] = (char)(token[1] - 48)*100 + (token[2] - 48) * 10 + (token[3] - 48);
        out[1] = '\0';
}

/*
 *      Replaces whitespace with corresponding escape character
 */
char sanitize_tokens(char ** tokens, int num_tokens, char input_esc)
{
        char esc;
        if (input_esc == '\0')
                esc = '!';
        else
                esc = input_esc;
        int i;
        for (i = 0; i < num_tokens; i++)
        {
                if (tokens[i][0] < 33 || isspace(tokens[i][0]) || iscntrl(tokens[i][0]))
                {
                        tokens[i] = sanitize_token(tokens[i], esc);
                }
                else if (tokens[i][0] == '!')
                {
                        char * new = (char *) malloc(sizeof(char)*(4+strlen(tokens[i])));
                        if (new == NULL)
                        {
                                fprintf(stderr, "[decompress_file] malloc returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                                //return '\0';
                        }
                        strncpy(new, "!033", 4);
                        strncpy(&new[4], &tokens[i][1], strlen(&tokens[i][1]) );
                        new[3+strlen(tokens[i])] = '\0';
                        free(tokens[i]);
                        tokens[i] = new;
                        //printf("%s\n", new);
                }
        }
        return esc;
}

leaf * build_AVL(char ** tokens, int num_tokens, leaf * root_AVL)
{
        sanitize_tokens(tokens, num_tokens, '\0');
        
        /* Count frequencies */
        int i;
        for (i = 0; i < num_tokens; i++)
        {
                if (DEBUG) printf("Inserting %s\n", tokens[i]);
                root_AVL = insert(root_AVL, tokens[i]);
                if (root_AVL == NULL)
                {
                        fprintf(stderr, "[build_AVL] NULL returned from insert. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                        return NULL;
                }
        }

        return root_AVL;
}

/*
 *      Builds HuffmanCodebook and writes it to a file based on the array of strings in
 *      token inputted of quantity num_tokens. Returns 0 on success, 1 otherwise.
 */
int build_Codebook(leaf * root_AVL)
{
        if(!root_AVL)
        {
                fprintf(stderr, "[build_Codebook] received a NULL input as the AVL tree. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
        }

        /* If both left and right of the AVL tree is NULL, then assume there's only one token to write */
        if (!(root_AVL->left || root_AVL->right))
        {
                char * file_name = "HuffmanCodebook";
                int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 00600);
                char * buffer = (char *) malloc(sizeof(char)*(6+strlen(root_AVL->word)));
                if (buffer == NULL)
                {
                        fprintf(stderr, "[build_Codebook] malloc returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                        return 1;
                }
                strncpy(buffer, "!\n0\t", 4);
                strncpy(&buffer[4], root_AVL->word, strlen(root_AVL->word));
                strncpy(&buffer[4+strlen(root_AVL->word)], "\n", 1);
                buffer[5+strlen(root_AVL->word)] = '\0';
                int ret = better_write(fd, buffer, strlen(buffer), __FILE__, __LINE__);
                if (ret <= 0)
                {
                        fprintf(stderr, "[build_Codebook] Error returned by better_write. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                }
                close(fd);
                return 0;
        }
        
        //traverse(root_AVL);
        /* Sort frequencies */
        int i, size = get_tree_size(root_AVL);
        if (DEBUG) printf("Tree size: %d\n", size);
        leaf ** arr = output_driver(root_AVL);
        if (arr == NULL)
        {
                fprintf(stderr, "[build_Codebook] NULL returned from output_driver. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                return 1;
        }
        for (i = 0; i < size; i++)
        {
                if (DEBUG) printf("Before heapify: Arr data %d: %s\n", i, arr[i]->word);
        }
        heap * h = heapify(arr, size);
        for (i = 0; i < size; i++)
        {
                if (DEBUG) printf("After heapify: Arr data %d: %s\n", i, arr[i]->word);
        }

        /* Create Huffman Code tree */
        while (h->size > 1)
        {
                /* Take top 2 */
                leaf * a = heap_pop(h);
                leaf * b = heap_pop(h);
                leaf * new = (leaf *) malloc(sizeof(leaf));
                if (new == NULL)
                {
                        fprintf(stderr, "[build_Codebook] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                        return 1;
                }
                new->left = a;
                new->right = b;
                if (new == NULL)
                {
                        fprintf(stderr, "[build_Codebook] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                        return 1;
                }
                new->word = NULL;
                new->freq = a->freq + b->freq;
                heap_push(new, h);
        }
        leaf * root_Huff = heap_pop(h);

        /* Assign encoding */
        if(encode_keys(root_Huff, ""))
        {
                fprintf(stderr, "[build_Codebook] encode_keys returned error.\n");
                return 1;
        }
        //traverse(root_Huff);

        /* write codebook to file using huffman tree */
        char * file_name = "HuffmanCodebook";
        int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 00600);

        /* assuming that ! is the only esc character */
        write_Codebook_Driver(fd, root_Huff, '!');
        close(fd);

        free(arr);
        free(h);
        free_huff(root_Huff);
        free_tree(root_AVL);

        return 0;
}

/*
 *      Writes compressed file with file descriptor filedes. The encoding for each token in tokens
 *      is looked up in the Huffman tree rooted at root. There are num_tokens tokens and the
 *      escape character for the codebook is esc.
 *      Returns 0 on success, 1 otherwise.
 */
int compress_file(int filedes, char ** tokens, int num_tokens, leaf * root, char esc)
{
        sanitize_tokens(tokens, num_tokens, esc);
        int i;
        for (i = 0; i < num_tokens; i++)
        {
                leaf * cur = lookup(root, tokens[i]);
                if (DEBUG) printf("token %s\t%s\n", tokens[i], cur->encoding);
                if (cur == NULL)
                {
                        fprintf(stderr, "[compress_file] lookup returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                        return 1;
                }

                int ret = better_write(filedes, cur->encoding, strlen(cur->encoding), __FILE__, __LINE__);
                if (ret <= 0)
                {
                        fprintf(stderr, "[compress] Error returned by better_write. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                }
        }
        return 0;
}

/*
 *      Decompressed file according to word and encodings in looked up in root_huff (root of
 *      huffman tree) There are num_tokens tokens and the escape character for the codebook is esc.
 *      Returns 0 on success, 1 otherwise.
 */
int decompress_file(int filedes, char * buffer, int size, leaf * root_huff, char esc)
{
        int i;

        leaf * ptr = root_huff;

        for(i = 0; i < size; i++)
        {
                char move = buffer[i];
                if (move == '0')
                        ptr = ptr->left;

                else if (move == '1')
                        ptr = ptr->right;

                if(strlen(ptr->word) > 0)
                {
                        int malloc_flag = FALSE;
                        char * word = ptr->word;
                        if (ptr->word[0] == esc)
                        {
                                if (strncmp(ptr->word, "!033", 4) == 0)
                                {
                                        word = (char *) malloc(sizeof(char)*(strlen(ptr->word)+1-3));
                                        if (word == NULL)
                                        {
                                                fprintf(stderr, "[decompress_file] malloc returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                                                return 1;
                                        }
                                        word[0] = '!';
                                        strcpy(&word[1], &ptr->word[4]);
                                        malloc_flag = TRUE;
                                }
                                else
                                {
                                        /* TODO: Something to do with reverse santize and read permissions */
                                        char out[2];
                                        reverse_sanitize_token(ptr->word, out);
                                        word = out;
                                }
                        }
                        int ret = better_write(filedes, word, strlen(word), __FILE__, __LINE__);
                        if (ret <= 0)
                        {
                                fprintf(stderr, "[decompress_file] Error returned by better_write. FILE: %s, LINE: %d\n", __FILE__, __LINE__);
                        }
                        ptr = root_huff;
                        if (malloc_flag)
                                free(word);
                }
        }
        return 0;
}

/*
 *      Encodes keys in huffman tree rooted at root. s is the current encoding.
 *      Returns 0 on success, 1 otherwise.
 */
int encode_keys(leaf * root, char * s)
{
        /* Continue traversing tree */
        if (root->word == NULL)
        {
                if (root->left != NULL)
                {
                        int len = strlen(s);
                        char * s1 = (char *) malloc(sizeof(char) * (len+2));
                        if (s1 == NULL)
                        {
                                fprintf(stderr, "[encode_keys] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                                return 1;
                        }
                        strncpy(s1, s, len);
                        s1[len] = '0';
                        s1[len+1] = '\0';
                        if (encode_keys(root->left, s1))
                        {
                                return 1;
                        }
                }
                if (root->right != NULL)
                {
                        int len = strlen(s);
                        char * s1 = (char *) malloc(sizeof(char) * (len+2));
                        if (s1 == NULL)
                        {
                                fprintf(stderr, "[encode_keys] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                                return 1;
                        }
                        strncpy(s1, s, len);
                        s1[len] = '1';
                        s1[len+1] = '\0';
                        if (encode_keys(root->right, s1))
                        {
                                return 1;
                        }
                }
                if (strlen(s) > 0)
                        free(s);
        }
        /* Found data to encode */
        else
        {
                root->encoding = s;
                if (DEBUG) printf("%s\t%s\n", s, root->word);
        }
        return 0;
}

/*
 *      Recursively frees huffman tree rooted and root and no data fields within
 */
void free_huff(leaf * root)
{

        if (root != NULL)
        {
                free_huff(root->left);
                free_huff(root->right);
                if (root->word == NULL)
                        free(root);
        }
}

/*
 *      Compresses if compress=TRUE, else decompresses according to codebook from filedescriptor fd
 */
leaf * read_Codebook(int fd, char *esc, int compress)
{
        int size = lseek(fd, 0, SEEK_END);

        char * buffer = (char *) malloc(sizeof(char) * size);
        if (buffer == NULL)
        {
                fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
        }

        lseek(fd, 0, SEEK_SET);
        int ret = better_read(fd, buffer, size, __FILE__, __LINE__);
        if (ret == 0)
        {
                fprintf(stderr, "[main] better_read returned error code %s. FILE: %s. LINE: %d.\n", strerror(ret), __FILE__, __LINE__);
        }

        leaf * root = NULL;

        int start = 2, i = 2;
        // Find escape character
        *esc = buffer[0];
        while (i < size) {
                // read line by line
                while(buffer[i] != '\t')
                        i++;
                char * cur_encoding = (char *) malloc(sizeof(char) * (i - start + 1));
                if (cur_encoding == NULL)
                {
                        fprintf(stderr, "[codebook_to_AVL] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                }
                strncpy(cur_encoding, &buffer[start], i - start);
                cur_encoding[i - start] = '\0';
                i++;
                start = i;
                while(buffer[i] != '\n')
                {
                        if (DEBUG) printf("buffer cur: %c\n", buffer[i]);
                        i++;
                }
                char * cur_word = (char *) malloc(sizeof(char) * (i - start + 1));
                if (cur_word == NULL)
                {
                        fprintf(stderr, "[codebook_to_AVL] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                }
                strncpy(cur_word, &buffer[start], i - start);
                cur_word[i - start] = '\0';
                if (compress)
                {
                        root = insert(root, cur_word);
                        leaf * cur_leaf = lookup(root, cur_word);
                        if (cur_leaf == NULL)
                        {
                                fprintf(stderr, "[read_Codebook] Error: lookup returned NULL. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                                return NULL;
                        }
                        cur_leaf->encoding = cur_encoding;
                }


                else
                {
                        leaf * cur_leaf = create_leaf(cur_word);
                        cur_leaf->encoding = cur_encoding;
                        root = huffman_insert(root, cur_leaf, cur_encoding, 0);
                        /*
                        int encoding_size = strlen(cur_encoding);
                        leaf * parent_ptr = root;
                        int j;
                        for(j = 0; j < encoding_size-1; j++)
                        {
                                char move = cur_encoding[j];
                                if(move == '0')
                                {
                                        if(parent_ptr->left == NULL)
                                        {
                                                parent_ptr->left = create_leaf(NULL);
                                        }
                                        parent_ptr = parent_ptr->left;
                                }
                                if(move == '1')
                                {
                                        if(parent_ptr->right == NULL)
                                        {
                                                parent_ptr->right = create_leaf(NULL);
                                        }
                                        parent_ptr = parent_ptr->right;
                                }
                        }

                        char move = cur_encoding[j];

                        if(move == '0')
                        {
                                parent_ptr->left = cur_leaf;
                        }

                        if(move == '1') {
                                parent_ptr->right = cur_leaf;
                        }
                        */

                }
                i++;
                start = i;
        }

        free(buffer);
        return root;
}

/*
 *      Inserts leaf new into huffman tree rooted at root according the encoding.
 *      level represents the current encoding up to the current level of the tree.
 *      Returns root of tree.
 */
leaf * huffman_insert(leaf * root, leaf * new, char * encoding, int level)
{
        if (root == NULL)
        {
                root = create_leaf(NULL);
                root->word = (char *) malloc(sizeof(char));
                root->word[0] = '\0';
                root->encoding = (char *) malloc(sizeof(char) * (level+2));
                strncpy(root->encoding, encoding, level+1);
                root->encoding[level] = '\0';
        }
        if (strlen(encoding) == level + 1)
        {
                if (encoding[level] == '0')
                {
                        root->left = new;
                }
                else if (encoding[level] == '1')
                {
                        root->right = new;
                }
        }
        else if (encoding[level] == '0')
        {
                root->left = huffman_insert(root->left, new, encoding, level+1);
        }
        else if (encoding[level] == '1')
        {
                root->right = huffman_insert(root->right, new, encoding, level+1);
        }
        return root;
}

/*
 *      Driver to write HuffmanCodebook at file descriptor fd. Writes escape character into
 *      file and writes huffman tree rooted at root.
 *      Returns 0 on success, 1 otherwise.
 */
int write_Codebook_Driver(int fd, leaf * root, char esc_char)
{
        char esc[3] = {esc_char, '\n', '\0'};
        int ret = better_write(fd, esc, strlen(esc), __FILE__, __LINE__);
        if (ret <= 0)
        {
                fprintf(stderr, "[compress] Error returned by better_write. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
        }
        return write_Codebook(fd, root);
}

/*
 *      Writes HuffmanCodebook at file descriptor fd. Writes huffman tree rooted at root.
 *      Returns 0 on success, 1 otherwise.
 */
int write_Codebook(int fd, leaf * root)
{
        if (root->word == NULL)
        {
                if (root->left != NULL)
                {
                        write_Codebook(fd, root->left);
                }

                if (root->right != NULL)
                {
                        write_Codebook(fd, root->right);
                }
        } else {
                char * tab = "\t";
                char * newline = "\n";

                int word_size = strlen(root->word);
                int encoding_size = strlen(root->encoding);

                better_write(fd, root->encoding, encoding_size, __FILE__, __LINE__);
                better_write(fd, tab, sizeof(char), __FILE__, __LINE__);
                better_write(fd, root->word, word_size, __FILE__, __LINE__);
                better_write(fd, newline, sizeof(char), __FILE__, __LINE__);
                return 1;
        }

        return 0;
}
