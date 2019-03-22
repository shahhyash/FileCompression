#include "huffman.h"
#include "fileIO.h"

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
	out->tokens = (char **) malloc(sizeof(char *) * out->num_tokens);
	found_chars = FALSE;
	int start = 0;
	int j = 0;
	for (i = 0; i < len; i++)
	{
		if (isspace(s[i]))
		{
			if (found_chars)
			{
				out->tokens[j] = (char *) malloc(sizeof(char) * (i-start+1));
				strncpy(out->tokens[j], &s[start], i-start);
				out->tokens[j][i-start] = '\0';
				j++;
			}
			out->tokens[j] = (char *) malloc(sizeof(char)*2);
			strncpy(out->tokens[j], &s[i], 1);
			out->tokens[j][1] = '\0';
			j++;
			start = i+1;
		}
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
			lseek(fd, 0, SEEK_SET);
			better_read(fd, buffer, size, __FILE__, __LINE__);
			Token * t = tokenize(buffer, size);
			leaf * root_AVL = build_Codebook(t->tokens, t->num_tokens);
			write_Codebook(t->tokens, t->num_tokens, file, root_AVL);
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
			//char * file = argv[i++];
			//char * codebook = argv[i];
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
			//char * file = argv[i++];
			//char * codebook = argv[i];
		}
	}
	return 0;
}
