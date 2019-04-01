#ifndef AVL_H
#define AVL_H

#include "code.h"

/*
 *      Returns integer balance of leaf a
 */
int get_balance(leaf * a);
/*
 *      Returns integer value of longest branch length rooted at leaf a
 */
int get_tree_height(leaf * a);
/*
 *      Dynamically allocates space for a new leaf with data word. Returns new leaf.
 */
leaf * create_leaf(char * word);
/*
 *      inserts new leaf with data word into the tree rooted at root.
 *      Returns root of tree
 */
leaf * insert(leaf * root, char * word);
/*
 *      Returns pointer to leaf containing word data by searching tree starting at leaf root
 */
leaf * lookup(leaf * root, char * data);
/*
 *      AVL right rotation based at leaf a. Returns pointer to leaf closest to the base of the tree1
 */
leaf * rotate_right (leaf * a);
/*
 *      AVL left rotation based at leaf a. Returns pointer to leaf closest to the base of the tree1
 */
leaf * rotate_left (leaf * a);
/*
 *      Recursively frees tree based at root inlcuding encoding field
 */
void free_tree(leaf * root);
/*
 *      Driver for mapping a tree rooted at root to an array of leaves
 */
leaf ** output_driver(leaf * root);
/*
 *      Returns number of leaves in tree based at root
 */
int get_tree_size(leaf * root);
/*
 *      Recursively frees all field in leaf tree based at root
 */
void free_full_tree(leaf * root);
/*
 *      Recursively maps the tree rooted at leaf root to the array arr of leaves
 *      i represents the current index of the array to store the leaf at
 */
int output(leaf * root, leaf ** arr, int i);
/*
 *      Recursively prints tree rooted at root horizontally with the current
 *      value printed with space spaces
 */
void print2D(leaf * root, int space);
/*
 *      Driver to print tree rooted at root
 */
void traverse(leaf * root);

#endif
