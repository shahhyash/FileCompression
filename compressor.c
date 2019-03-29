#include "compressor.h"
#include "data_structures/code.h"
#include "fileIO.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int compress_file(int filedes, char ** tokens, int num_tokens, leaf * root)
{
    int i;
    for (i = 0; i < num_tokens; i++)
    {
            leaf * cur = lookup(root, tokens[i]);
            int ret = better_write(filedes, cur->encoding, strlen(cur->encoding), __FILE__, __LINE__);
            if (ret <= 0)
            {
                    fprintf(stderr, "[compress] Error returned by better_write. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
            }
    }
    return 0;
}

int decompress_file(int filedes, char * buffer, int size, leaf * root_huff)
{
        int i;
        
        leaf * ptr = root_huff;

        for(i=0; i<size; i++)
        {
                char move = buffer[i];
                if(move == '0')
                        ptr = ptr->left;

                if(move == '1')
                        ptr = ptr->right;

                if(ptr->word)
                {
                        int ret = better_write(filedes, ptr->word, strlen(ptr->word), __FILE__, __LINE__);
                        if (ret <= 0)
                        {
                                fprintf(stderr, "[decompress_file] Error returned by better_write. FILE: %s, LINE: %d\n", __FILE__, __LINE__);
                        }
                        ptr = root_huff;
                }
        }
        return 0;
}