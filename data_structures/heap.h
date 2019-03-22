#ifndef HEAP_H
#define HEAP_H

#include "code.h"

typedef struct _min_heap {
        leaf ** codes;           /* Frequencies, strings, and encoding */
        int size;               /* Number of strings stored */
        int capacity;           /* Total number of strings able to be stored */
} heap;

//void free_heap(heap * a);
heap * heapify(leaf ** arr, int arr_length);
leaf * heap_peek(heap * h);
leaf * heap_pop(heap * h);
void heap_push(leaf * data, heap * h);
int min_compare(int a, int b);
void sift_down(int index, heap * h);
void sift_up(int index, heap * h);
void swap(int a, int b, heap * h);

#endif
