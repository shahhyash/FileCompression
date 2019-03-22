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
        leaf * root_AVL = (leaf *) malloc(sizeof(leaf));

        /* Count frequencies */
        int i;
        for (i = 0; i < num_tokens; i++)
        {
                root_AVL = insert(root_AVL, tokens[i], __FILE__, __LINE__);
        }

        /* Sort frequencies */
        int size = get_tree_height(root_AVL);
        leaf ** arr = output_driver(root_AVL);
        heap * h = heapify(arr, size);

        /* Create Huffman Code tree */
        while (h->size > 1)
        {
                /* Take top 2 */
                leaf * a = heap_pop(h);
                leaf * b = heap_pop(h);
                leaf * new = (leaf *) malloc(sizeof(leaf));
                new->left = a;
                new->right = b;
                new->data->word = NULL;
                new->data->freq = a->data->freq + b->data->freq;
                heap_push(new, h);
        }
        leaf * root_Huff = heap_pop(h);

        /* Assign encoding */
        encode_keys(root_Huff, "");

        return root_AVL;
}

void encode_keys(leaf * root, char * s)
{
        /* Continue traversing tree */
        if (root->data->word == NULL)
        {
                if (root->left != NULL)
                {
                        int len = strlen(s);
                        char * s1 = (char *) malloc(sizeof(char) * (len+2));
                        strncpy(s1, s, len);
                        s1[len] = '0';
                        s1[len+1] = '\0';
                        encode_keys(root->left, s1);
                }
                if (root->right != NULL)
                {
                        int len = strlen(s);
                        char * s1 = (char *) malloc(sizeof(char) * (len+2));
                        strncpy(s1, s, len);
                        s1[len] = '0';
                        s1[len+1] = '\0';
                        encode_keys(root->right, s1);
                }
        }
        /* Found data to encode */
        else
        {
                root->data->encoding = s;
                printf("%s\t%s\n", s, root->data->word);
        }
}

void write_Codebook(char ** tokens, int num_tokens, char * name, leaf * root)
{
        char * file_name = strcat(name, ".hcz");
        int fd = open(file_name, O_WRONLY | O_CREAT, 00666);
        int i;
        for (i = 0; i < num_tokens; i++)
        {
                leaf * cur = lookup(root, tokens[i]);
                better_write(fd, cur->data->encoding, strlen(cur->data->encoding), __FILE__, __LINE__);
        }
        close(fd);

}
