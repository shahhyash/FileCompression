#include "AVL.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

int get_balance(leaf * a)
{
        return abs(get_tree_height(a->right) - get_tree_height(a->left));
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
        new->right = NULL;
        new->left = NULL;
        new->frequency = 1;
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
                root->frequency++;
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
                free_tree(root->left);
                free_tree(root->right);
                free(root->word);
                free(root);
        }
}

int traverse(leaf * root)
{
        if (root == NULL)
                return 0;
        return 1;
}
