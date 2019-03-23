#ifndef AVL_H
#define AVL_H

#include "code.h"

int get_balance(leaf * a);
int get_tree_height(leaf * a);
leaf * create_leaf(char * word);
int get_tree_size(leaf * root);
leaf * insert (leaf * root, char * word, char * file, int line);
leaf * lookup(leaf * root, char * data);
leaf ** output_driver(leaf * root);
int output(leaf * root, leaf ** arr, int i);
leaf * rotate_right (leaf * a);
leaf * rotate_left (leaf * a);
void free_tree(leaf * root);
void traverse (leaf * root, int a, int size);

#endif
