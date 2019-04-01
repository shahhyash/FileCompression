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
#include <dirent.h>

#define TRUE 1
#define FALSE 0
#define ERR 1
#define DEBUG 0

typedef struct _Token {
	char ** tokens;
	int num_tokens;
} Token;

/*
 *	Tokenizes the string array s of length len by whitespace delimiters.
 *	Returns Token containing the number of tokens and the array of tokens.
 */
Token * tokenize(char * s, int len)
{
	Token * out = (Token *) malloc(sizeof(Token));
	out->num_tokens = 0;
	int found_chars = FALSE;
	int i;
	for (i = 0; i < len; i++)
	{
		if (isspace(s[i]) || iscntrl(s[i]) || s[i] < 33)
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
		if (isspace(s[i]) || iscntrl(s[i]) || s[i] < 33)
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
	
	leaf * ret_AVL = build_AVL(t->tokens, t->num_tokens, root);

	free(t->tokens);
	free(t);

	return ret_AVL;
}

typedef struct _FileNode {
	char * file_path;
	void * next;
} FileNode;

/* Insert new FileNode into the list for a new file */
FileNode * insert_fileNode(FileNode * root, char * file_path)
{
	FileNode * file_node = (FileNode*)malloc(sizeof(FileNode));
	file_node->file_path=file_path;
	file_node->next=NULL;

	if (root)
	{
		FileNode * ptr = root;
		while(ptr->next)
			ptr = ptr->next;
		ptr->next = file_node;
		return root;
	}

	return file_node;
}

/* Free all allocated memory for linked list */
int free_FileNodeList(FileNode * root)
{
	while (root)
	{
		free(root->file_path);
		FileNode * next = root->next;
		free(root);
		root = next;
	}

	return 0;
}

/* 
 * This function takes in a path to a directory and recursively builds a linked list of child paths 
 * 
 * The mode paramater assumes a value of either 0 or 1, where 0 -> build & compress, 1 -> decompress
 * In build & compress mode (0), the function appends all child files to the linked list
 * In decompress mode (1), the function only appends files with a '.hcz' extension to the linked list. 
 */
int fetch_files_recursively(char * dirpath, FileNode * root, int mode)
{
	/* open directory and skip over the first two relative paths */
	DIR * dirdes = opendir(dirpath);
	readdir(dirdes);
	readdir(dirdes);
	
	/* memory used to store current directory item */
	struct dirent * item = readdir(dirdes);
	
	/* loop through directory until no further items are left */
	while (item)
	{
		/* compute full relative path of child item */
		int dirpath_len = strlen(dirpath);
		int name_len = strlen(item->d_name);
		char * child_dirpath;
		if (dirpath[dirpath_len-1] == '/')
		{
			child_dirpath = (char*)malloc(sizeof(char) * (dirpath_len + name_len + 1));
			memcpy(child_dirpath, dirpath, dirpath_len);
			memcpy(&child_dirpath[dirpath_len], item->d_name, name_len);
			child_dirpath[dirpath_len + name_len] = '\0';
		}
		else
		{
			child_dirpath = (char*)malloc(sizeof(char) * (dirpath_len + name_len + 2));
			memcpy(child_dirpath, dirpath, dirpath_len);
			child_dirpath[dirpath_len] = '/';
			memcpy(&child_dirpath[dirpath_len+1], item->d_name, name_len);
			child_dirpath[dirpath_len + name_len + 1] = '\0';
		}

		/* if item is a directory, recursively search all of it's children */
		if (item->d_type == DT_DIR)
			fetch_files_recursively(child_dirpath, root, mode);

		/* if item is a regular file, append it to the linked list as a file */
		if (item->d_type == DT_REG)
		{
			if (!mode || strcmp(&(item->d_name[strlen(item->d_name)-4]), ".hcz")==0)
			{
				printf("mode: %d, file: %s\n", mode, child_dirpath);
				/* root node for parent call of this function should have a null pointer for file path */
				if(root->file_path)
					root = insert_fileNode(root, child_dirpath);
				else
					root->file_path = child_dirpath;
			}
		}

		/* fetch next item in directory */
		item = readdir(dirdes);
	}

	/* close directory stream after traversing through all files */
	closedir(dirdes);

	return 0;
}

int compress_file_Driver(char * file_path, leaf * codebook, char esc)
{
	/* Open and read contents of files, storing tokens into an array to make compression easier */
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

	/* Create a new file with the same name and the hcz extension and store compressed data in it. */
	char * file_name = (char*)malloc(sizeof(char)*(strlen(file_path)+5));
	memcpy(file_name, file_path, strlen(file_path));
	memcpy(&file_name[strlen(file_path)], ".hcz", 4);
	file_name[strlen(file_path)+4] = '\0';	

	fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 00600);

	if (compress_file(fd, t->tokens, t->num_tokens, codebook, esc) == 1)
	{
		fprintf(stderr, "[main] compress_file returned error. FILE: %s. LINE: %d\n", __FILE__, __LINE__);
		return ERR;
	}

	/* Free allocated memory for tokens */
	free(file_name);
	
	int k;
	for(k = 0; k < t->num_tokens; k++)
	{
		free(t->tokens[k]);
	}
	free(t->tokens);
	free(t);
	
	close(fd);

	return 0;
}

int decompress_file_Driver(char * file_path, leaf * codebook, char esc)
{
	/* Begin by reading the compressed file */
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

	/* remove the '.hcz' extension from the compressed path */
	int path_size = strlen(file_path) - 4*sizeof(char);
	char * decompressed_path = (char *) malloc(sizeof(char) * path_size+1);
	if (decompressed_path == NULL)
	{
		fprintf(stderr, "[main] NULL returned by malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
		return ERR;
	}
	strncpy(decompressed_path, file_path, path_size);
	decompressed_path[path_size] = '\0';

	/* create/open file at decompressed path */
	fd = open(decompressed_path, O_WRONLY | O_CREAT | O_TRUNC, 00600);
	free(decompressed_path);

	/* decompress file then write to original location */
	decompress_file(fd, buffer, size, codebook, esc);

	/* free and close resources */
	free(buffer);
	close(fd);

	return 0;
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
			/* fetch dirpath and create empty file node to serve as head of linked list */
			char * path = argv[i];
			FileNode * root_FileNode = insert_fileNode(NULL, NULL);
			fetch_files_recursively(path, root_FileNode, 0);

			/* for each file node, append tokens to AVL tree */
			FileNode * ptr = root_FileNode;
			leaf * root_AVL = NULL;
			while (ptr)
			{
				root_AVL = build_AVL_Driver(ptr->file_path, root_AVL);
				ptr = ptr->next;
			}
			
			/* free linked list data */
			free_FileNodeList(root_FileNode);

			/* build codebook using AVL tree */
			int ret = build_Codebook(root_AVL);
			if (ret == 1)
			{
				fprintf(stderr, "[main] build_Codebook returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
				return ERR;
			}
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
		/* fetch command arguments for compress path and codebook path */
		char * path = argv[i++];
		char * codebook_path = argv[i];
		
		/* build tree from codebook file - only one so can be used across recursive and non-recursive modes */
		int fd = open(codebook_path, O_RDONLY, 00600);
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
		
		if(recursive)
		{
			/* fetch dirpath and create empty file node to serve as head of linked list */
			FileNode * root_FileNode = insert_fileNode(NULL, NULL);
			fetch_files_recursively(path, root_FileNode, 0);

			/* for each file node, compress the file */
			FileNode * ptr = root_FileNode;
			while (ptr)
			{
				if (compress_file_Driver(ptr->file_path, codebook, esc) != 0)
				{
					fprintf(stderr, "[main] Error compressing file %s. FILE: %s. LINE: %d.\n", ptr->file_path, __FILE__, __LINE__);
					return ERR;
				}
				ptr = ptr->next;
			}
			
			/* free linked list data */
			free_FileNodeList(root_FileNode);
		}
		else
		{
			if (compress_file_Driver(path, codebook, esc) != 0)
			{
				fprintf(stderr, "[main] Error compressing file %s. FILE: %s. LINE: %d.\n", path, __FILE__, __LINE__);
				return ERR;
			}
		}
		
		/* Free allocated memory for codebook */
		free_full_tree(codebook);
	}
	else if (decompress)
	{
		/* fetch command arguments for decompress path and codebook path */
		char * path = argv[i++];
		char * codebook_path = argv[i];
		
		/* build tree from codebook file - only one so can be used across recursive and non-recursive modes */
		int fd = open(codebook_path, O_RDONLY, 00600);
		if (fd == -1)
		{
			fprintf(stderr, "[main] Error opening file %s. FILE: %s. LINE %d.\n", codebook_path, __FILE__, __LINE__);
			return ERR;
		}

		char esc;
		leaf * codebook = read_Codebook(fd, &esc, FALSE);
		if (codebook == NULL)
		{
			fprintf(stderr, "[main] Error in read_Codebook. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
			return ERR;
		}
		close(fd);

		if(recursive)
		{
			/* fetch dirpath and create empty file node to serve as head of linked list */
			FileNode * root_FileNode = insert_fileNode(NULL, NULL);
			fetch_files_recursively(path, root_FileNode, 1);

			/* for each file node, decompress the file */
			FileNode * ptr = root_FileNode;
			while (ptr)
			{
				if (decompress_file_Driver(ptr->file_path, codebook, esc) != 0)
				{
					fprintf(stderr, "[main] Error decompressing file %s. FILE: %s. LINE: %d.\n", ptr->file_path, __FILE__, __LINE__);
					return ERR;
				}
				ptr = ptr->next;
			}
			
			/* free linked list data */
			free_FileNodeList(root_FileNode);
		}
		else
		{
			if (decompress_file_Driver(path, codebook, esc) != 0)
			{
				fprintf(stderr, "[main] Error decompressing file %s. FILE: %s. LINE: %d.\n", path, __FILE__, __LINE__);
				return ERR;
			}
		}

		/* Free allocated memory for codebook */
		free_huff(codebook);
	}
	return 0;
}
