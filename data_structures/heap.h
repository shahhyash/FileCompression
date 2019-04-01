#ifndef HEAP_H
#define HEAP_H

#include "code.h"

typedef struct _min_heap {
        leaf ** codes;           /* Frequencies, strings, and encoding */
        int size;               /* Number of strings stored */
} heap;

/*
 *      Transforms the array of arr_length leaves arr into a heap in O(n)
 */
heap * heapify(leaf ** arr, int arr_length);
/*
 *      Pops minimum item from the heap h and updates heap structure.
 */
leaf * heap_pop(heap * h);
/*
 *      Pushes leaf l onto the heap h and sorts it into heap order
 */
void heap_push(leaf * data, heap * h);
/*
 *      Compares root of heap to lower element recursively and swaps if needed
 */
void sift_down(int index, heap * h);
/*
 *      Compares bottom elements of heap recursively to their parents starting at i
 *      and swaps if necessary
 */
void sift_up(int index, heap * h);
/*
 *      Swaps indices a and b in the array stored in the heap h.
 */
void swap(int a, int b, heap * h);

#endif
