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

    TOKENTYPE_RANGE_START, // 
    TOKENTYPE_RANGE_END, // 
    

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
void DynamicTokenArray_free(DynamicTokenArray *const array);
void DynamicTokenArray_append(DynamicTokenArray * const array, const Token * const token);
Token *DynamicTokenArray_last(const DynamicTokenArray * const array);
Token *DynamicTokenArray_pop(DynamicTokenArray * const array);
#endif
