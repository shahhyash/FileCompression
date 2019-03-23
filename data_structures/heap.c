#include "heap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define get_parent(index) ((int)(((index)-1) / 2))
#define get_right(index) (2*(index) + 2)
#define get_left(index) (2*(index) + 1)

/*
void free_heap(heap * h)
{
        int i;
        for (i = 0; i < h->capacity; i++)
        {
                if(h->codes[i] != NULL)
                {
                        free_code(h->codes[i]);
                }

        }
        free(h->codes);
        free(h);
}
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

leaf * heap_peek(heap * h)
{
        if (h->size == 0)
                return NULL;
        return h->codes[0];
}

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

void heap_push(leaf * l, heap * h)
{
        h->codes[h->size++] = l;
        if (h->size > 1)
                sift_up(h->size - 1, h);
}

void sift_down(int index, heap * h)
{
        int root = index;
        if (get_left(index) < h->size)
        {
                if (h->codes[index]->data->freq > h->codes[get_left(index)]->data->freq)
                {
                        root = get_left(index);
                }
        }

        if (get_right(index) < h->size)
        {
                if (h->codes[index]->data->freq > h->codes[get_right(index)]->data->freq)
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

void sift_up(int i, heap * h)
{
        while (h->codes[get_parent(i)]->data->freq > h->codes[i]->data->freq)
        {
                swap(i, get_parent(i), h);
                i = get_parent(i);
        }
}

void swap(int a, int b, heap * h)
{
        leaf * temp = h->codes[a];
        h->codes[a] = h->codes[b];
        h->codes[b] = temp;
}
