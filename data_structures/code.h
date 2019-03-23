#ifndef CODE_H
#define CODE_H

typedef struct _leaf {
        struct _leaf * right;
        struct _leaf * left;
        char * word;
        char * encoding;
        int freq;
} leaf;


#endif
