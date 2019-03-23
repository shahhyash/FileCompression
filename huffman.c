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
        printf("Tree height: %d\n", size);
        leaf ** arr = output_driver(root_AVL);
        if (arr == NULL)
        {
                fprintf(stderr, "[build_Codebook] NULL returned from output_driver. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                return NULL;
        }
        for (i = 0; i < size; i++)
        {
                printf("Before heapify: Arr data %d: %s\n", i, arr[i]->data->word);
        }
        heap * h = heapify(arr, size);
        for (i = 0; i < size; i++)
        {
                printf("After heapify: Arr data %d: %s\n", i, arr[i]->data->word);
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
                new->data = (Code *) malloc(sizeof(Code));
                if (new->data == NULL)
                {
                        fprintf(stderr, "[build_Codebook] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                        return NULL;
                }
                new->data->word = NULL;
                new->data->freq = a->data->freq + b->data->freq;
                heap_push(new, h);
        }
        leaf * root_Huff = heap_pop(h);

        /* Assign encoding */
        if(encode_keys(root_Huff, ""))
        {
                fprintf(stderr, "[build_Codebook] encode_keys returned error.\n");
                return NULL;
        }

        return root_AVL;
}

int encode_keys(leaf * root, char * s)
{
        /* Continue traversing tree */
        if (root->data->word == NULL)
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
        }
        /* Found data to encode */
        else
        {
                root->data->encoding = s;
                printf("%s\t%s\n", s, root->data->word);
        }
        return 0;
}

int write_Codebook(char ** tokens, int num_tokens, char * name, leaf * root)
{
        char * file_name = strcat(name, ".hcz");
        int fd = open(file_name, O_WRONLY | O_CREAT, 00666);
        int i;
        for (i = 0; i < num_tokens; i++)
        {
                leaf * cur = lookup(root, tokens[i]);
                int ret = better_write(fd, cur->data->encoding, strlen(cur->data->encoding), __FILE__, __LINE__);
                if (ret <= 0)
                {
                        fprintf(stderr, "[write_Codebook] Error returned by better_write. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
                }
        }
        close(fd);
        return 0;
}
