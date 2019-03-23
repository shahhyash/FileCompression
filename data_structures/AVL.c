#include "AVL.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

int get_balance(leaf * a)
{
        return get_tree_height(a->left) - get_tree_height(a->right);
}

int get_tree_height(leaf * a)
{
        if (a == NULL)
                return 0;
        return 1 + max(get_tree_height(a->right), get_tree_height(a->left));
}

leaf * create_leaf(char * word)
{
        leaf * new = (leaf *) malloc(sizeof(leaf));
        if (new == NULL)
        {
                fprintf(stderr, "[create_leaf] Malloc returned NULL. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                return NULL;
        }
        new->right = NULL;
        new->left = NULL;
        new->freq = 1;
        new->word = word;
        return new;
}

leaf * insert(leaf * root, char * word, char * file, int line)
{
        if (strlen(word) <= 0)
                return root;
        if (root == NULL)
        {
                return create_leaf(word);
        }
        int compare = strcmp(root->word, word);
        if (compare == 0)
        {
                root->freq++;
                return root;
        }
        else if (compare < 0)
        {
                root->left = insert(root->left, word, file, line);
        }
        else if (compare > 0)
        {
                root->right = insert(root->right, word, file, line);
        }

        int balance = get_balance(root);
        if (balance > 1 && compare == 1)
        {
                root->left = rotate_left(root->left);
                return rotate_right(root);
        }
        else if (balance > 1 && compare == -1)
        {
                return rotate_right(root);
        }
        else if (balance < -1 && compare == 1)
        {
                return rotate_left(root);
        }
        else if (balance < -1 && compare == -1)
        {
                root->right = rotate_right(root->right);
                return rotate_left(root);
        }
        return root;
}

leaf * lookup(leaf * root, char * data)
{
        int compare = strcmp(root->word, data);
        if (compare == 0)
        {
                return root;
        }
        else if (compare < 0)
        {
                return lookup(root->left, data);
        }
        else if (compare > 0)
        {
                return lookup(root->right, data);
        }
        return NULL;
}

leaf * rotate_right(leaf * a)
{
        leaf * root = a->left;
        a->left = root->right;
        root->right = a;
        return root;

}

leaf * rotate_left(leaf * a)
{
        leaf * root = a->right;
        a->right = root->left;
        root->left = a;
        return root;
}

void free_tree(leaf * root)
{
        if (root != NULL)
        {
                free(root->encoding);
                free_tree(root->left);
                free_tree(root->right);
                free(root);
        }
}

int get_tree_size(leaf * root)
{
        if (root == NULL)
                return 0;
        return 1 + get_tree_size(root->right) + get_tree_size(root->left);
}

leaf ** output_driver(leaf * root)
{
        int size = get_tree_size(root);
        leaf ** arr = (leaf **) malloc(sizeof(leaf *) * size);
        if (arr == NULL)
        {
                fprintf(stderr, "[output_driver] NULL malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                return NULL;
        }
        int i = output(root, arr, 0);
        if (i != size)
        {
                fprintf(stderr, "[output_driver] error %d %d\n", i, size);
        }
        return arr;
}

int output(leaf * root, leaf ** arr, int i)
{
        if (root == NULL)
        {
                fprintf(stderr, "[output] null root passed for index %d. FILE: %s. LINE: %d.\n", i, __FILE__, __LINE__);
                return i;
        }
        printf("root %d data: %s\n", i, root->word);

        arr[i++] = root;
        if (root->right != NULL)
                i = output(root->right, arr, i);
        if (root->left != NULL)
                i = output(root->left, arr, i);
        return i;
}

void traverse(leaf * root, int a, int size)
{
        // Base case
        if (root == NULL)
                return;

        // Increase distance between levels
        a += size;

        // Process right child first
        traverse(root->right, a, size);

        // Print current node after space
        // count
        printf("\n");
        int i;
        for (i = size; i < a; i++)
                printf(" ");
        printf("%s %d\n", root->word, root->freq);

        // Process left child
        traverse(root->left, a, size);
}
