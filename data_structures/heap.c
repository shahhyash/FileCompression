#include "heap.h"

#include <stdlib.h>
#include <string.h>

#define get_parent(index) ((int)(((index)-1) / 2))
#define get_right(index) (2*(index) + 2)
#define get_left(index) (2*(index) + 1)
#define MAX 1
#define MIN 0

void free_heap(heap * h)
{
        int i;
        for (i = 0; i < h->capacity; i++)
        {
                if(h->data[i] != NULL)
                        free(h->data[i]);
        }
        free(h->data);
        free(h);
}

void grow_heap(heap * h)
{
        int capacity_new = h->capacity * 2;
        char ** data_new = (char **) malloc(sizeof(char *) * capacity_new);
        int i;
        for (i = 0; i < h->capacity; i++)
        {
                data_new[i] = h->data[i];
        }
        free(h->data);
        h->capacity = capacity_new;
        h->data = data_new;
}

heap * heapify(char ** arr, int arr_length, int capacity, int type)
{
        heap * h = (heap *) malloc(sizeof(heap));
        h->capacity = capacity;
        h->heap_type = type;
        h->size = arr_length;
        h->data = (char **) malloc(sizeof(char *) * capacity);
        sift_down(0, h);
        return h;
}

char * heap_peek(heap * h)
{
        if (h->size == 0)
                return NULL;
        return h->data[0];
}

char * heap_pop(heap * h)
{
        if (h->size == 0)
                return NULL;
        char * out = h->data[0];
        h->data[0] = h->data[h->size-1];
        h->data[h->size-1] = NULL;
        h->size -= 1;
        if (h->size != 0)
                sift_down(0, h);
        return out;
}

void heap_push(char * data, heap * h)
{
        if (h->size + 1 > h->capacity)
                grow_heap(h);
        h->data[h->size++] = data;
        if (h->size > 1)
                sift_up(h->size - 2, h);
}

int max_compare(char * a, char * b)
{
        return strcmp(a, b) < 1;
}

int min_compare(char * a, char * b)
{
        return strcmp(a, b) > 1;
}

heap * init(int capacity, int type)
{
        heap * h = (heap *) malloc(sizeof(heap));
        h->capacity = capacity;
        h->size = 0;
        h->heap_type = type;
        h->data = (char **) malloc(sizeof(char *) * capacity);
        return h;
}

void sift_down(int index, heap * h)
{
        int (* comparator)(char *, char *) = &min_compare;
        if (h->heap_type == MAX)
                comparator = &max_compare;
        int root = index;
        if (get_left(index) <= h->size)
        {
                if (comparator(h->data[index], h->data[get_left(index)]))
                {
                        root = get_left(index);
                }
        }

        if (get_right(index) <= h->size)
        {
                if (comparator(h->data[index], h->data[get_right(index)]))
                {
                        root = get_right(index);
                }
        }
        /* Swap */
        if (root != index)
        {
                char * temp = h->data[index];
                h->data[index] = h->data[root];
                h->data[root] = temp;
        }
        if (index+1 < h->size)
                sift_down(index+1, h);
}

/* Needs implementation */
void sift_up(int index, heap * h)
{
        int (* comparator)(char *, char *) = &min_compare;
        if (h->heap_type == MAX)
                comparator = &max_compare;
        /*
        while (index > 0)
        {
                if (comparator(h->data[get_parent(index)], h->data[index]))
                {

                }
        }
        parent := iParent(child)
        if a[parent] < a[child] then (out of max-heap order)
        swap(a[parent], a[child])
        child := parent (repeat to continue sifting up the parent now)
        else
        return
        */
}
