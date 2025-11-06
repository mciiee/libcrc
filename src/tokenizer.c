#include <stdlib.h>
#ifndef TOKENIZER_H
#include "tokenizer.h"
#endif

DynamicTokenArray *DynamicTokenArray_new(size_t initialCapacity) {
    DynamicTokenArray *array = malloc(sizeof(DynamicTokenArray));
    array->data = calloc(initialCapacity, sizeof(Token));
    array->capacity = initialCapacity;
    array->occupied = 0;
    return array;
}

void DynamicTokenArray_free(DynamicTokenArray *array) {
    free(array->data);
    free(array);
}

