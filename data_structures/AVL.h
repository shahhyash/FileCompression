#ifndef AVL_H
#define AVL_H

typedef struct _leaf {
        struct _leaf * right;
        struct _leaf * left;
        char * word;
        int frequency;
} leaf;

int get_balance(leaf * a);
int get_tree_height(leaf * a);
leaf * create_leaf(char * word);
leaf * insert (leaf * root, char * word, char * file, int line);
leaf * rotate_right (leaf * a);
leaf * rotate_left (leaf * a);
void free_tree(leaf * root);
int traverse (leaf * root);

#endif
