#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>
#include <stddef.h>

enum TokenType {
    TOKENTYPE_COURSE_LITERAL,

    TOKENTYPE_CONJUNCTION, // AND
    TOKENTYPE_DISJUNCTION, // OR
    TOKENTYPE_ADJOIN, // /


    TOKENTYPE_PAREN_OPEN, // (
    TOKENTYPE_PAREN_CLOSE, // )

    TOKENTYPE_RANGE_OPEN, // 
    TOKENTYPE_RANGE_CLOSE, // 

};

typedef struct Token {
    enum TokenType type;
    char *data;
} Token;

typedef struct DynamicTokenArray {
    size_t capacity;
    size_t occupied;
    Token *data;
} DynamicTokenArray;

DynamicTokenArray *DynamicTokenArray_new(size_t initialCapacity);

#endif
