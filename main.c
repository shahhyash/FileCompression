#include "huffman.h"
#include "fileIO.h"
#include "data_structures/AVL.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/stat.h>

#define TRUE 1
#define FALSE 0
#define ERR 1
#define DEBUG 0

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

leaf * build_AVL_Driver(char * file, leaf * root)
{
	int fd = open(file, O_RDONLY, 00600);
	if (fd == -1)
	{
		fprintf(stderr, "[main] Error opening file %s. FILE: %s. LINE %d.\n", file, __FILE__, __LINE__);
		return NULL;
	}
	int size = lseek(fd, 0, SEEK_END);
	//printf("%d\n", size);
	char * buffer = (char *) malloc(sizeof(char) * size);
	if (buffer == NULL)
	{
		fprintf(stderr, "[main] [main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
		return NULL;
	}
	lseek(fd, 0, SEEK_SET);
	if (better_read(fd, buffer, size, __FILE__, __LINE__) != 1)
	{
		fprintf(stderr, "[main] [main] better_read returned error. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
		return NULL;
	}
	Token * t = tokenize(buffer, size);
	free(buffer);
	int k;
	for(k = 0; k < t->num_tokens; k++)
	{
		if(DEBUG) printf("Token #%d: %s\n", k, t->tokens[k]);
	}

	for(k = 0; k < t->num_tokens; k++)
	{
		free(t->tokens[k]);
	}
	free(t->tokens);
	free(t);

	return build_AVL(t->tokens, t->num_tokens, root);
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
			{
				fprintf(stderr, "[main] Error: Incorrect flags. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
		}
		else if (!strcmp(argv[i], "-c"))
		{
			if(num_flags == 0 || (num_flags == 1 && recursive))
			{
				compress = TRUE;
				num_flags++;
			}
			else
			{
				fprintf(stderr, "[main] Error: Incorrect flags. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
		}
		else if (!strcmp(argv[i], "-d"))
		{
			if(num_flags == 0 || (num_flags == 1 && recursive))
			{
				decompress = TRUE;
				num_flags++;
			}
			else
			{
				fprintf(stderr, "[main] Error: Incorrect flags. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}

		}
		else if (!strcmp(argv[i], "-R"))
		{
			if (argc < 4)
			{
				fprintf(stderr, "Incorrect flags. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}

			/* third argument should be file path regardless of operation */
			char * path = argv[3];		
			struct stat path_stat;
			stat(path, &path_stat);

			/* only mark process as recursive if specified path is a directory */
			if (S_ISDIR(path_stat.st_mode))
			{
				recursive = TRUE;
				num_flags++;
			}
		}
		/* flags done */
		else
		{
			break;
		}
	}
	if (num_flags == 0)
	{
		fprintf(stderr, "[main] No flags found\n");
		return ERR;
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
			leaf * root_AVL = build_AVL_Driver(file, NULL);
			int ret = build_Codebook(root_AVL);
			if (ret == 1)
			{
				fprintf(stderr, "[main] build_Codebook returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}			
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
			int fd = open(file_path, O_RDONLY, 00600);
			if (fd == -1)
			{
				fprintf(stderr, "[main] Error opening file %s. FILE: %s. LINE %d.\n", file_path, __FILE__, __LINE__);
				return ERR;
			}
			int size = lseek(fd, 0, SEEK_END);
			//printf("%d\n", size);
			char * buffer = (char *) malloc(sizeof(char) * size);
			if (buffer == NULL)
			{
				fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
			lseek(fd, 0, SEEK_SET);
			if (better_read(fd, buffer, size, __FILE__, __LINE__) != 1)
			{
				fprintf(stderr, "[main] better_read returned error. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
			Token * t = tokenize(buffer, size);
			free(buffer);
			close(fd);

			// build tree from codebook
			fd = open(codebook_path, O_RDONLY, 00600);
			if (fd == -1)
			{
				fprintf(stderr, "[main] Error opening file %s. FILE: %s. LINE %d.\n", codebook_path, __FILE__, __LINE__);
				return ERR;
			}

			char esc;
			leaf * codebook = read_Codebook(fd, &esc, TRUE);
			if (codebook == NULL)
			{
				fprintf(stderr, "[main] Error in read_Codebook. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
			close(fd);

			// finally, create a new file with the same name and hcz extension and compress it.
			char * file_name = strcat(file_path, ".hcz");
    			fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 00600);

			if (compress_file(fd, t->tokens, t->num_tokens, codebook, esc) == 1)
			{
				fprintf(stderr, "[main] compress_file returned error. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
				return ERR;
			}

			// free allocated memory for codebook and tokens
			int k;
			for(k = 0; k < t->num_tokens; k++)
			{
				free(t->tokens[k]);
			}
			free(t->tokens);
			free(t);
			//printf("traverse\n");
			//traverse(codebook);
			free_full_tree(codebook);

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
			int fd = open(file_path, O_RDONLY, 00600);
			if (fd == -1)
			{
				fprintf(stderr, "[main] Error opening file %s. FILE: %s. LINE %d.\n", file_path, __FILE__, __LINE__);
				return ERR;
			}
			int size = lseek(fd, 0, SEEK_END);
			//printf("%d\n", size);
			char * buffer = (char *) malloc(sizeof(char) * size);
			if (buffer == NULL)
			{
				fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
			lseek(fd, 0, SEEK_SET);
			if (better_read(fd, buffer, size, __FILE__, __LINE__) != 1)
			{
				fprintf(stderr, "[main] better_read returned error. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
			close(fd);

			// read the stored codebook and generate Huffman Tree
			fd = open(codebook_path, O_RDONLY, 00600);
			if (fd == -1)
			{
				fprintf(stderr, "[main] Error opening file %s. FILE: %s. LINE %d.\n", codebook_path, __FILE__, __LINE__);
				return ERR;
			}
			char esc;
			leaf * codebook = read_Codebook(fd, &esc, FALSE);
			close(fd);

			// remove the '.hcz' extension from the compressed path
			int path_size = strlen(file_path) - 4*sizeof(char);
			char * path = (char *) malloc(sizeof(char) * path_size+1);
			if (path == NULL)
			{
				fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
			strncpy(path, file_path, path_size);
			path[path_size] = '\0';

			// create/open file at decompressed path
    			fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 00600);
			free(path);
			// decompress file then write to original location
			decompress_file(fd, buffer, size, codebook, esc);

			// free and close resources
			//traverse(codebook);
			free_full_tree(codebook);
			free(buffer);
			close(fd);
		}
	}
	return 0;
}
