#include "huffman.h"
#include "fileIO.h"
#include "data_structures/AVL.h"
#include "compressor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

typedef struct _Token {
	char ** tokens;
	int num_tokens;
} Token;

Token * tokenize(char * s, int len)
{
	Token * out = (Token *) malloc(sizeof(Token));
	out->num_tokens = 0;
	int found_chars = FALSE;
	int i;
	for (i = 0; i < len; i++)
	{
		if (isspace(s[i]))
		{
			if (found_chars)
			{
				out->num_tokens += 2;
				found_chars = FALSE;
			}
			else
				out->num_tokens++;
		}
		else
			found_chars = TRUE;
	}

	if (found_chars)
	{
		out->num_tokens++;
	}

	out->tokens = (char **) malloc(sizeof(char *) * out->num_tokens);
	found_chars = FALSE;
	int start = 0;
	int j = 0;
	//printf("NUM TOKENS %d\n", out->num_tokens);
	for (i = 0; i < len; i++)
	{
		if (isspace(s[i]))
		{
			if (found_chars)
			{
				out->tokens[j] = (char *) malloc(sizeof(char) * (i-start+1));
				strncpy(out->tokens[j], &s[start], i-start);
				out->tokens[j][i-start] = '\0';
				//printf("%s\n", out->tokens[j]);
				j++;
				found_chars = FALSE;
			}
			out->tokens[j] = (char *) malloc(sizeof(char)*2);
			strncpy(out->tokens[j], &s[i], 1);
			out->tokens[j][1] = '\0';
			j++;
			start = i+1;
		}
		else
			found_chars = TRUE;
	}

	if (found_chars)
	{
		out->tokens[j] = (char *) malloc(sizeof(char) * (i-start+1));
		strncpy(out->tokens[j], &s[start], i-start);
		out->tokens[j][i-start] = '\0';
		//printf("%s\n", out->tokens[j]);
		j++;
		found_chars = FALSE;
	}

	return out;
}

int main(int argc, char *argv[])
{
	int build = FALSE, compress = FALSE, decompress = FALSE, recursive = FALSE;
	int num_flags = 0;
	int i;
	/* Possible issue: repeated flags */
	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-b"))
		{
			if(num_flags == 0 || (num_flags == 1 && recursive))
			{
				build = TRUE;
				num_flags++;
			}
			else
				fprintf(stderr, "Incorrect flags. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);

		}
		else if (!strcmp(argv[i], "-c"))
		{
			if(num_flags == 0 || (num_flags == 1 && recursive))
			{
				compress = TRUE;
				num_flags++;
			}
			else
				fprintf(stderr, "Incorrect flags. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);

		}
		else if (!strcmp(argv[i], "-d"))
		{
			if(num_flags == 0 || (num_flags == 1 && recursive))
			{
				decompress = TRUE;
				num_flags++;
			}
			else
				fprintf(stderr, "Incorrect flags. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);

		}
		else if (!strcmp(argv[i], "-R"))
		{
			recursive = TRUE;
			num_flags++;
		}
		/* flags done */
		else
		{
			break;
		}
	}
	if (build)
	{
		if(recursive)
		{
			//char * path = argv[i];
		}
		else
		{
			char * file = argv[i];
			int fd = open(file, O_RDONLY, 00444);
			if (fd == -1)
			{
				fprintf(stderr, "Error opening file %s. FILE: %s. LINE %d.\n", file, __FILE__, __LINE__);
			}
			int size = lseek(fd, 0, SEEK_END);
			printf("%d\n", size);
			char * buffer = (char *) malloc(sizeof(char) * size);
			if (buffer == NULL)
			{
				fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			}
			lseek(fd, 0, SEEK_SET);
			if (better_read(fd, buffer, size, __FILE__, __LINE__) != 0)
			{
				fprintf(stderr, "[main] better_read returned error. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			}
			Token * t = tokenize(buffer, size);
			free(buffer);
			int k;
			for(k = 0; k < t->num_tokens; k++)
			{
				printf("Token #%d: %s\n", k, t->tokens[k]);
			}

			// TODO: no need to return the AVL tree. 
			leaf * root_AVL = build_Codebook(t->tokens, t->num_tokens);
			if (root_AVL == NULL)
			{
				fprintf(stderr, "[main] build_Codebook returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			}

			for(k = 0; k < t->num_tokens; k++)
			{
				free(t->tokens[k]);
			}
			free_tree(root_AVL);
			free(t->tokens);
			free(t);
		}
	}
	else if (compress)
	{
		if(recursive)
		{
			//char * path = argv[i++];
			//char * codebook = argv[i];
		}
		else
		{
			// fetch command arguments
			char * file_path = argv[i++];
			char * codebook_path = argv[i];

			// start by building a list of tokens for file to compress
			int fd = open(file_path, O_RDONLY, 00444);
			if (fd == -1)
			{
				fprintf(stderr, "Error opening file %s. FILE: %s. LINE %d.\n", file_path, __FILE__, __LINE__);
			}
			int size = lseek(fd, 0, SEEK_END);
			printf("%d\n", size);
			char * buffer = (char *) malloc(sizeof(char) * size);
			if (buffer == NULL)
			{
				fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			}
			lseek(fd, 0, SEEK_SET);
			if (better_read(fd, buffer, size, __FILE__, __LINE__) != 0)
			{
				fprintf(stderr, "[main] better_read returned error. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			}
			Token * t = tokenize(buffer, size);
			free(buffer);
			close(fd);

			// build tree from codebook
			fd = open(codebook_path, O_RDONLY, 00444);
			if (fd == -1)
			{
				fprintf(stderr, "Error opening file %s. FILE: %s. LINE %d.\n", codebook_path, __FILE__, __LINE__);
			}

			leaf * codebook = read_Codebook(fd, 1);
			close(fd);

			// finally, create a new file with the same name and hcz extension and compress it.
			char * file_name = strcat(file_path, ".hcz");
    		fd = open(file_name, O_WRONLY | O_CREAT, 00666);

			compress_file(fd, t->tokens, t->num_tokens, codebook);

			// free allocated memory for codebook and tokens
			int k;
			for(k = 0; k < t->num_tokens; k++)
			{
				free(t->tokens[k]);
			}
			free(t->tokens);
			free(t);
			free_tree(codebook);

			close(fd);
		}
	}
	else if (decompress)
	{
		if(recursive)
		{
			//char * path = argv[i++];
			//char * codebook = argv[i];
		}
		else
		{
			char * file_path = argv[i++];
			char * codebook_path = argv[i];

			// start by reading the compressed file
			int fd = open(file_path, O_RDONLY, 00444);
			if (fd == -1)
			{
				fprintf(stderr, "Error opening file %s. FILE: %s. LINE %d.\n", file_path, __FILE__, __LINE__);
			}
			int size = lseek(fd, 0, SEEK_END);
			printf("%d\n", size);
			char * buffer = (char *) malloc(sizeof(char) * size);
			if (buffer == NULL)
			{
				fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			}
			lseek(fd, 0, SEEK_SET);
			if (better_read(fd, buffer, size, __FILE__, __LINE__) != 0)
			{
				fprintf(stderr, "[main] better_read returned error. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			}
			close(fd);

			// read the stored codebook and generate Huffman Tree
			fd = open(codebook_path, O_RDONLY, 00444);
			if (fd == -1)
			{
				fprintf(stderr, "Error opening file %s. FILE: %s. LINE %d.\n", codebook_path, __FILE__, __LINE__);
			}
			leaf * codebook = read_Codebook(fd, 0);
			close(fd);

			// remove the '.hcz' extension from the compressed path
			int decompressed_path_size = strlen(file_path) - 4*sizeof(char);
			char * decompressed_path = malloc(decompressed_path_size+1);
			memcpy(decompressed_path, file_path, decompressed_path_size);
			decompressed_path[decompressed_path_size] = '\0';

			// create/open file at decompressed path
    		fd = open(decompressed_path, O_WRONLY | O_CREAT, 00666);

			// decompress file then write to original location
			decompress_file(fd, buffer, size, codebook);

			// free and close resources
			free_huff(codebook);
			free(buffer);
			close(fd);
		}
	}
	return 0;
}