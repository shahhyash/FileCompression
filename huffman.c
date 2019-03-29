#include "huffman.h"
#include "data_structures/AVL.h"
#include "data_structures/heap.h"
#include "fileIO.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

leaf * build_Codebook(char ** tokens, int num_tokens)
{
        leaf * root_AVL = NULL;

        /* Count frequencies */
        int i;
        for (i = 0; i < num_tokens; i++)
        {
                printf("Inserting %s\n", tokens[i]);
                root_AVL = insert(root_AVL, tokens[i], __FILE__, __LINE__);
                if (root_AVL == NULL)
                {
                        fprintf(stderr, "[build_Codebook] NULL returned from insert. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                        return NULL;
                }
        }

        /* Sort frequencies */
        int size = get_tree_size(root_AVL);
        printf("Tree size: %d\n", size);
        leaf ** arr = output_driver(root_AVL);
        if (arr == NULL)
        {
                fprintf(stderr, "[build_Codebook] NULL returned from output_driver. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                return NULL;
        }
        for (i = 0; i < size; i++)
        {
                printf("Before heapify: Arr data %d: %s\n", i, arr[i]->word);
        }
        heap * h = heapify(arr, size);
        for (i = 0; i < size; i++)
        {
                printf("After heapify: Arr data %d: %s\n", i, arr[i]->word);
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
                        return NULL;
                }
                new->left = a;
                new->right = b;
                if (new == NULL)
                {
                        fprintf(stderr, "[build_Codebook] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                        return NULL;
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
                return NULL;
        }

        /* write codebook to file using huffman tree */
        char * file_name = "HuffmanCodebook";
        int fd = open(file_name, O_WRONLY | O_CREAT, 00666);
        write_Codebook(fd, root_Huff);
        close(fd);

        free(arr);
        free(h);
        free_huff(root_Huff);

        return root_AVL;
}

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
                printf("%s\t%s\n", s, root->word);
        }
        return 0;
}

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

leaf * read_Codebook(int fd)
{
        int size = lseek(fd, 0, SEEK_END);
        char * buffer = (char *) malloc(sizeof(char) * size);
        if (buffer == NULL)
        {
                fprintf(stderr, "[read_Codebook] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
        }

        lseek(fd, 0, SEEK_SET);
        if (better_read(fd, buffer, size, __FILE__, __LINE__) != 0)
        {
                fprintf(stderr, "[main] better_read returned error. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
        }

        

        free(buffer);

        return NULL;
}

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
        }

        return 0;
}
