#include "heap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define get_parent(index) ((int)(((index)-1) / 2))
#define get_right(index) (2*(index) + 2)
#define get_left(index) (2*(index) + 1)

/*
 *      Transforms the array of arr_length leaves arr into a heap in O(n)
 */
heap * heapify(leaf ** arr, int arr_length)
{
        heap * h = (heap *) malloc(sizeof(heap));
        if (h == NULL)
        {
                fprintf(stderr, "[heapify] NULL returned from malloc. FILE: %s. LINE: %d.\n", __FILE__, __LINE__);
                return NULL;
        }
        h->size = arr_length;
        h->codes = arr;
        sift_down(0, h);
        return h;
}

/*
 *      Pops minimum item from the heap h and updates heap structure.
 */
leaf * heap_pop(heap * h)
{
        if (h->size == 0)
                return NULL;
        leaf * out = h->codes[0];
        h->codes[0] = h->codes[h->size-1];
        h->codes[h->size-1] = NULL;
        h->size -= 1;
        if (h->size != 0)
                sift_down(0, h);
        return out;
}

/*
 *      Pushes leaf l onto the heap h and sorts it into heap order
 */
void heap_push(leaf * l, heap * h)
{
        h->codes[h->size++] = l;
        if (h->size > 1)
                sift_up(h->size - 1, h);
}

/*
 *      Compares root of heap to lower element recursively and swaps if needed
 */
void sift_down(int index, heap * h)
{
        int root = index;
        if (get_left(index) < h->size)
        {
                if (h->codes[index]->freq > h->codes[get_left(index)]->freq)
                {
                        root = get_left(index);
                }
        }

        if (get_right(index) < h->size)
        {
                if (h->codes[index]->freq > h->codes[get_right(index)]->freq)
                {
                        root = get_right(index);

                }
        }
        if (index != root)
        {
                swap(index, root, h);
                sift_down(root, h);
        }
}

/*
 *      Compares bottom elements of heap recursively to their parents starting at i
 *      and swaps if necessary
 */
void sift_up(int i, heap * h)
{
        while (h->codes[get_parent(i)]->freq > h->codes[i]->freq)
        {
                swap(i, get_parent(i), h);
                i = get_parent(i);
        }
}

/*
 *      Swaps indices a and b in the array stored in the heap h.
 */
void swap(int a, int b, heap * h)
{
        leaf * temp = h->codes[a];
        h->codes[a] = h->codes[b];
        h->codes[b] = temp;
}
