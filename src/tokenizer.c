#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "utils.h"

#ifndef TOKENIZER_LOOP_LIMIT 
#define TOKENIZER_LOOP_LIMIT 2048
#endif


#ifndef TOKEN_ARRAY_INITIAL_CAPACITY
#define TOKEN_ARRAY_INITIAL_CAPACITY 128
#endif

DynamicTokenArray *DynamicTokenArray_new(size_t initialCapacity) {
    DynamicTokenArray *array = malloc(sizeof(DynamicTokenArray));
    array->data = calloc(initialCapacity, sizeof(Token));
    array->capacity = initialCapacity;
    array->occupied = 0;
    return array;
}

void DynamicTokenArray_free(DynamicTokenArray *const array) {
    free(array->data);
    free(array);
}


void DynamicTokenArray_append(DynamicTokenArray * const array, const Token * const token) {
    if (array->occupied >= array->capacity) {
        array->capacity = 1.5 * array->capacity + 1;
        array->data = realloc(array->data,  sizeof(Token[array->capacity]));
    }
    array->data[array->occupied] = *token;
}

Token *DynamicTokenArray_last(const DynamicTokenArray * const array) {
    return &array->data[array->occupied-1];
}

Token *DynamicTokenArray_pop(DynamicTokenArray * const array) {
    if (array->occupied == 0) {
        return nullptr;
    }
    Token *token = DynamicTokenArray_last(array);
    array->occupied--;
    return token;
}


bool isSubject(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isupper(str[i])) {
            return false;
        }
    }
    return true;
}

DynamicTokenArray *tokenize_string(char *stream) {
    auto array = DynamicTokenArray_new(TOKEN_ARRAY_INITIAL_CAPACITY);
    char *nextToken;
    char *token = strtok_rs(stream, " ", &nextToken);
    for (size_t i = 0; token != nullptr && i < TOKENIZER_LOOP_LIMIT ; i++) {
        if (isSubject(token)) {
            
        }
        token = strtok_rs(nullptr, " ", &nextToken);
    }

    return array;
}
