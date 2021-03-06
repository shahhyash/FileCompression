#include "AVL.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

/*
 *      Returns integer balance of leaf a
 */
int get_balance(leaf * a)
{
        int bal = get_tree_height(a->left) - get_tree_height(a->right);
        /* printf("word: %s BAL %d\n", a->word, bal); */
        return bal;
}

/*
 *      Returns integer value of longest branch length rooted at leaf a
 */
int get_tree_height(leaf * a)
{
        if (a == NULL)
                return 0;
        int height = 1 + max(get_tree_height(a->right), get_tree_height(a->left));
        /* printf("word: %s HEIGHT %d\n", a->word, height-1); */
        return height;
}

/*
 *      Dynamically allocates space for a new leaf with data word. Returns new leaf.
 */
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
        if (word != NULL)
        {
                new->word = (char *) malloc(sizeof(char) * (strlen(word)+1));
                strcpy(new->word, word);
        }
        else
                new->word = NULL;
        return new;
}

/*
 *      inserts new leaf with data word into the tree rooted at root.
 *      Returns root of tree
 */
leaf * insert(leaf * root, char * word)
{
        if (strlen(word) <= 0)
                return root;
        if (root == NULL)
        {
                return create_leaf(word);
        }
        /* int layer  = *inserted; */
        int compare = strcmp(root->word, word);
        if (compare == 0)
        {
                root->freq++;
                return root;
        }
        else if (compare < 0)
        {
                root->left = insert(root->left, word);
        }
        else if (compare > 0)
        {
                root->right = insert(root->right, word);
        }

        int balance = get_balance(root);
        if (balance > 1 && strcmp(root->left->word, word) == 1)
        {
                /* printf("Rotate left then right: %s %s\n", root->word, word); */
                /* traverse(root); */
                root->left = rotate_left(root->left);
                return rotate_right(root);
        }
        else if (balance > 1 && strcmp(root->left->word, word) == -1)
        {
                /* printf("Rotate right: %s %s\n", root->word, word); */
                return rotate_right(root);
        }
        else if (balance < -1 && strcmp(root->right->word, word) == 1)
        {
                /* printf("Rotate left: %s %s\n", root->word, word); */
                return rotate_left(root);
        }
        else if (balance < -1 && strcmp(root->right->word, word) == -1)
        {
                /* printf("Rotate right then left: %s %s\n", root->word, word); */
                /* traverse(root); */
                root->right = rotate_right(root->right);
                return rotate_left(root);
        }
        return root;
}

/*
 *      Returns pointer to leaf containing word data by searching tree starting at leaf root
 */
leaf * lookup(leaf * root, char * data)
{
        if (root == NULL)
        {
                return NULL;
        }
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

/*
 *      AVL right rotation based at leaf a. Returns pointer to leaf closest to the base of the tree1
 */
leaf * rotate_right(leaf * a)
{
        leaf * root = a->left;
        leaf * b = root->right;
        root->right = a;
        a->left = b;
        return root;

}

/*
 *      AVL left rotation based at leaf a. Returns pointer to leaf closest to the base of the tree1
 */
leaf * rotate_left(leaf * a)
{
        leaf * root = a->right;
        leaf * b = root->left;
        root->left = a;
        a->right = b;
        return root;
}

/*
 *      Recursively frees tree based at root inlcuding encoding field
 */
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

/*
 *      Recursively frees all field in leaf tree based at root
 */
void free_full_tree(leaf * root)
{
        if (root != NULL)
        {
                /* printf("freeing: "); */
                /* printf("%s", root->encoding); */
                /* printf(" %s\n", root->word); */
                free_full_tree(root->left);
                free_full_tree(root->right);
                free(root->encoding);
                free(root->word);
                free(root);
        }
}

/*
 *      Returns number of leaves in tree based at root
 */
int get_tree_size(leaf * root)
{
        if (root == NULL)
                return 0;
        return 1 + get_tree_size(root->right) + get_tree_size(root->left);
}

/*
 *      Driver for mapping a tree rooted at root to an array of leaves
 */
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
                return NULL;
        }
        return arr;
}

/*
 *      Recursively maps the tree rooted at leaf root to the array arr of leaves
 *      i represents the current index of the array to store the leaf at
 */
int output(leaf * root, leaf ** arr, int i)
{
        if (root == NULL)
        {
                fprintf(stderr, "[output] null root passed for index %d. FILE: %s. LINE: %d.\n", i, __FILE__, __LINE__);
                return i;
        }
        /* printf("root %d data: %s\n", i, root->word); */

        arr[i++] = root;
        if (root->right != NULL)
                i = output(root->right, arr, i);
        if (root->left != NULL)
                i = output(root->left, arr, i);
        return i;
}

/*
 *      Recursively prints tree rooted at root horizontally with the current
 *      value printed with space spaces
 */
void print2D(leaf * root, int space)
{
        if (root==NULL) return;

        space += 8;

        print2D(root->right, space);
        printf("\n");
        int i;
        for(i = 8; i < space; i++)
        {
                printf(" ");
        }
        /*
        char *a = "";
        if (root->word != NULL && 0)
        {
                a = root->encoding;
        }
        */
        /* printf("word: %s\tencoding: %s\tfreq: %d", root->word, a, root->freq); */
        printf("word: %s", root->word);
        print2D(root->left, space);
}

/*
 *      Driver to print tree rooted at root
 */
void traverse(leaf * root)
{
        print2D(root, 0);
        printf("\n");
}
