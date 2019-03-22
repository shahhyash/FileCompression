#ifndef CODE_H
#define CODE_H

typedef struct _Code {
        char * word;
        int freq;
        char * encoding;
} Code;

typedef struct _leaf {
        struct _leaf * right;
        struct _leaf * left;
        Code * data;
} leaf;


#endif
