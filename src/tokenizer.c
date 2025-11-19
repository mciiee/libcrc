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


static const char * TokenTypeToStr(enum TokenType type){
    switch (type) {
        case TOKENTYPE_UNKNOWN: return "TOKENTYPE_UNKNOWN";
        case TOKENTYPE_COURSE_SUBJECT: return "TOKENTYPE_COURSE_SUBJECT";
        case TOKENTYPE_COURSE_NUMBER: return "TOKENTYPE_COURSE_NUMBER";
        case TOKENTYPE_CONJUNCTION: return "TOKENTYPE_CONJUNCTION";
        case TOKENTYPE_DISJUNCTION: return "TOKENTYPE_DISJUNCTION";
        case TOKENTYPE_ADJOIN: return "TOKENTYPE_ADJOIN";
        case TOKENTYPE_PAREN_OPEN: return "TOKENTYPE_PAREN_OPEN";
        case TOKENTYPE_PAREN_CLOSE: return "TOKENTYPE_PAREN_CLOSE";
        case TOKENTYPE_RANGE_START: return "TOKENTYPE_RANGE_START";
        case TOKENTYPE_RANGE_END: return "TOKENTYPE_RANGE_END";
        default: return nullptr;
    }
}

static const char * TokenFlagsToStr(enum TokenFlags flags) {
    switch (flags) {
        case TOKEN_NO_FLAGS: return "TOKEN_NO_FLAGS";
        case TOKEN_DYNAMICALLY_ALLOCATED: return "TOKEN_DYNAMICALLY_ALLOCATED";
        default: return 0;
    }
}

static void Token_prettyPrint(const Token * const token) {
    puts("Token {");
    printf("  type: %s", TokenTypeToStr(token->type));
    printf("  flags: %s", TokenFlagsToStr(token->flags));
    puts("}");
}

void DynamicTokenArray_prettyPrint(const DynamicTokenArray * const array) {
    puts("DynamicTokenArray {");
    printf("  capacity: %u\n", (unsigned int)array->capacity);
    printf("  occupied: %u\n", (unsigned int)array->occupied);
    puts("}");
    for (size_t i = 0; i < array->occupied; i++) {
        Token_prettyPrint(&array->data[i]);
    }
}

bool isCourseRangeStart(const char *str) {
    constexpr const char SUBJECT[] = "SUBJECT";
    return strncmp(str, SUBJECT, sizeof(SUBJECT)/sizeof(SUBJECT[0]));
}

bool isCourseSubject(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isupper(str[i])) {
            return false;
        }
    }
    return true;
}

bool isQuotedCourseSubject(const char *str) {
    if(str[0] != '"') {
        return false;
    }
    size_t i = 0;
    for (; str[i] != '\0' && str[i] != '"'; i++) {
        if (!isupper(str[i])) {
            return false;
        }
    }
    return str[i] == '"';
}

bool isCourseNumber(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

char *unquoteQuotedCourseSubject(const char *str) {
    char *courseStr = strdup(&str[1]);
    size_t len = strlen(courseStr);
    courseStr[len-1] = '\0';
    return courseStr;
}

DynamicTokenArray * tokenize_string(char *stream) {
    auto array = DynamicTokenArray_new(TOKEN_ARRAY_INITIAL_CAPACITY);
    char *nextToken = nullptr;
    char *tokenStr = strtok_rs(stream, " \t\n", &nextToken);
    Token token = {0};
    for (size_t i = 0; tokenStr != nullptr; i++) {
        (void)i;
        if (tokenStr == nullptr) {
            DynamicTokenArray_free(array);
            return nullptr;
        }
        else if(STR_EQUALS(tokenStr, "/")) {
            token.type = TOKENTYPE_ADJOIN;
            token.flags = TOKEN_NO_FLAGS;
            token.data = "/";
            DynamicTokenArray_append(array, &token);
        }
        else if (STR_EQUALS(tokenStr, "AND")) {
            token.type = TOKENTYPE_CONJUNCTION;
            token.flags = TOKEN_NO_FLAGS;
            token.data = "AND";
            DynamicTokenArray_append(array, &token);
        }
        else if (STR_EQUALS(tokenStr, "OR")) {
            token.type = TOKENTYPE_DISJUNCTION;
            token.flags = TOKEN_NO_FLAGS;
            token.data = "AND";
            DynamicTokenArray_append(array, &token);
        }
        else if (isCourseNumber(tokenStr)) {
            token.type = TOKENTYPE_DISJUNCTION;
            token.flags = TOKEN_DYNAMICALLY_ALLOCATED;
            token.data = strdup(tokenStr);
            DynamicTokenArray_append(array, &token);
        }
        else if (isQuotedCourseSubject(tokenStr)) {
            token.type = TOKENTYPE_COURSE_SUBJECT;
            token.flags = TOKEN_DYNAMICALLY_ALLOCATED;
            token.data = unquoteQuotedCourseSubject(tokenStr);
            DynamicTokenArray_append(array, &token);
        }
        else if (isCourseSubject(tokenStr)){
            token.type = TOKENTYPE_COURSE_SUBJECT;
            token.flags = TOKEN_DYNAMICALLY_ALLOCATED;
            token.data = strdup(tokenStr);
            DynamicTokenArray_append(array, &token);
        }
        else if (isCourseRangeStart(tokenStr)) {
            token.type = TOKENTYPE_RANGE_START;
            token.flags = TOKEN_NO_FLAGS;
            token.data = "";
            DynamicTokenArray_append(array, &token);
        }
        else {
            token.type = TOKENTYPE_COURSE_SUBJECT;
            token.flags = TOKEN_DYNAMICALLY_ALLOCATED;
            token.data = strdup(tokenStr);
            DynamicTokenArray_append(array, &token);
        }
            
        tokenStr = strtok_rs(nullptr, " ", &nextToken);
    }
    return array;
}

