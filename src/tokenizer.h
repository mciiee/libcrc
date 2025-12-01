#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>


enum TokenType: uint32_t {
    TOKENTYPE_UNKNOWN = 0,

    TOKENTYPE_COURSE_SUBJECT = 0x1,
    TOKENTYPE_COURSE_NUMBER = 0x2,

    TOKENTYPE_CONJUNCTION = 0x4, // AND
    TOKENTYPE_DISJUNCTION = 0x5, // OR
    TOKENTYPE_ADJOIN = 0x6, // /


    TOKENTYPE_PAREN_OPEN = 0x8, // (
    TOKENTYPE_PAREN_CLOSE = 0x9, // )

    TOKENTYPE_RANGE_START = 0x10, // 
    TOKENTYPE_RANGE_END = 0x11, // 
    
    TOKENTYPE_WHITESPACE = 0x1A,

};

enum TokenFlags: uint32_t {
    TOKEN_NO_FLAGS = 0,
    TOKEN_DYNAMICALLY_ALLOCATED = 1
};

typedef struct Token {
    enum TokenType type;
    enum TokenFlags flags;
    char *data;
} Token;


constexpr Token TOKEN_CONJUNCTION = {
    .type = TOKENTYPE_CONJUNCTION,
    .flags = TOKEN_NO_FLAGS,
    .data = nullptr,
};


constexpr Token TOKEN_DISJUNCTION = {
    .type = TOKENTYPE_DISJUNCTION,
    .flags = TOKEN_NO_FLAGS,
    .data = nullptr,
};

constexpr Token TOKEN_ADJOIN = {
    .type = TOKENTYPE_ADJOIN,
    .flags = TOKEN_NO_FLAGS,
    .data = nullptr,
};

constexpr Token TOKEN_PAREN_OPEN = {
    .type = TOKENTYPE_PAREN_OPEN,
    .flags = TOKEN_NO_FLAGS,
    .data = nullptr,
};

constexpr Token TOKEN_PAREN_CLOSE = {
    .type = TOKENTYPE_PAREN_CLOSE,
    .flags = TOKEN_NO_FLAGS,
    .data = nullptr,
};

constexpr Token TOKEN_RANGE_START = {
    .type = TOKENTYPE_RANGE_START,
    .flags = TOKEN_NO_FLAGS,
    .data = nullptr
};

constexpr Token TOKEN_WHITESPACE = {
    .type = TOKENTYPE_WHITESPACE,
    .flags = TOKEN_NO_FLAGS,
    .data = nullptr
};

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
void DynamicTokenArray_prettyPrint(const DynamicTokenArray * const array);

// static void Token_prettyPrint(const Token * const token);
// static const char * TokenTypeToStr(enum TokenType type);
// static const char * TokenFlagsToStr(enum TokenFlags flags);


[[nodiscard("Dynamically allocated")]] DynamicTokenArray * tokenize_string(char *stream);
#endif
