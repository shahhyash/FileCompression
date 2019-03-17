#ifndef HEAP_H
#define HEAP_H

typedef struct _heap {
        char ** data;
        int size;               /* Number of strings stored */
        int capacity;           /* Total number of strings able to be stored */
        int heap_type;          /* min = 0, max = 1 */
} heap;


void free_heap(heap * a);
void grow_heap(heap * h);
heap * heapify(char ** arr, int arr_length, int capacity, int type);
char * heap_peek(heap * h);
char * heap_pop(heap * h);
void heap_push(char * data, heap * h);
int max_compare(char * a, char * b);
int min_compare(char * a, char * b);
heap * init(int capacity, int type);
void sift_down(int index, heap * h);
void sift_up(int index, heap * h);

#endif
