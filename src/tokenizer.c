#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "utils.h"
#include "deque.h"

#ifndef TOKENIZER_LOOP_LIMIT 
#define TOKENIZER_LOOP_LIMIT 2048
#endif

#ifndef TOKEN_ARRAY_INITIAL_CAPACITY
#define TOKEN_ARRAY_INITIAL_CAPACITY 128
#endif

#ifndef CHAR_QUEUE_SIZE
#define CHAR_QUEUE_SIZE 128
#endif 

DynamicTokenArray *DynamicTokenArray_new(size_t initialCapacity) {
    DynamicTokenArray *array = malloc(sizeof(DynamicTokenArray));
    array->data = calloc(initialCapacity, sizeof(Token));
    array->capacity = initialCapacity;
    array->occupied = 0;
    return array;
}

void DynamicTokenArray_free(DynamicTokenArray *const array) {
    for (size_t i = 0; i < array->occupied; i++) {
        if (array->data[i].flags & TOKEN_DYNAMICALLY_ALLOCATED) {
            free(array->data[i].data);
        }
    }
    free(array->data);
    free(array);
}


void DynamicTokenArray_append(DynamicTokenArray * const array, const Token * const token) {
    if (array->occupied >= array->capacity) {
        array->capacity = 1.5 * array->capacity + 1;
        array->data = realloc(array->data,  sizeof(Token[array->capacity]));
    }
    array->data[array->occupied] = *token;
    array->occupied++;
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
        case TOKENTYPE_WHITESPACE: return "TOKENTYPE_WHITESPACE";
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
    printf("  type: %s\n", TokenTypeToStr(token->type));
    printf("  flags: %s\n", TokenFlagsToStr(token->flags));
    printf("  data: \"%s\"\n", token->data);
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
    return strncmp(str, SUBJECT, sizeof(SUBJECT)/sizeof(SUBJECT[0])) == 0;
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
        if (!isdigit(str[i]) && i <= 3) {
            return false;
        }
        else if (!isalnum(str[i])) {
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

//static void tokenize_string_cleanup(DynamicTokenArray *array) {
//    DynamicTokenArray_free(array);
//}

struct CharStream {
    char *str;
    size_t index;
};

struct CharStream *CharStream_init(char *str) {   
    struct CharStream *stream = malloc(sizeof(struct CharStream));
    stream->str = str;
    stream->index = 0;
    return stream;
}

void CharStream_free(struct CharStream *stream) {
    free(stream);
}

bool CharStream_isEndOfStream(struct CharStream *stream) {
    return stream->str[stream->index] == '\0';
}

char CharStream_getNext(struct CharStream *stream) {
    if (CharStream_isEndOfStream(stream)) {
        return '\0';
    }
    char ch = stream->str[stream->index];
    stream->index++;
    return ch;
}

static int handleQueue(size_t dequeSize, char * const deque, enum TokenType tokenType, DynamicTokenArray *array) {
    if (dequeIsEmpty(dequeSize, deque)) {
        return 0;
    }
    Token token = {
        .type = tokenType,
        .flags = TOKEN_DYNAMICALLY_ALLOCATED,
        .data = strndup(deque, dequeSize)
    };
    DynamicTokenArray_append(array, &token);
    return dequeClear(dequeSize, deque) == nullptr? -1 : 0;
}


// TODO: Add error handling
DynamicTokenArray * tokenize_string(char *str) {
    auto stream = CharStream_init(str);

    char deque[CHAR_QUEUE_SIZE] = {0};
    constexpr size_t dequeSize = sizeof(deque)/sizeof(deque[0]);
    
    Token token = {0};
    auto array = DynamicTokenArray_new(TOKEN_ARRAY_INITIAL_CAPACITY);
    enum TokenType dequeTokenType = TOKENTYPE_WHITESPACE;
    
    char ch = '\0';
    while (!CharStream_isEndOfStream(stream)) {
        ch = CharStream_getNext(stream);
        switch (ch) {
            case '\0': 
                break;
            case ' ':
            [[fallthrough]];
            case '\t':
            [[fallthrough]];
            case '\n':
                handleQueue(dequeSize, deque, dequeTokenType, array);

                dequeTokenType = TOKENTYPE_WHITESPACE;
                DynamicTokenArray_append(array, &TOKEN_WHITESPACE);
                break;
            case '(':
                handleQueue(dequeSize, deque, dequeTokenType, array);
                DynamicTokenArray_append(array, &TOKEN_PAREN_OPEN);
                break;
            case ')':
                handleQueue(dequeSize, deque, dequeTokenType, array);
                DynamicTokenArray_append(array, &TOKEN_PAREN_CLOSE);
                break;
            case '/':
                handleQueue(dequeSize, deque, dequeTokenType, array);
                DynamicTokenArray_append(array, &TOKEN_ADJOIN);
                break;
            default:
                if (!isalnum(ch)) {
                    token.type = TOKENTYPE_UNKNOWN;
                    token.flags = TOKEN_DYNAMICALLY_ALLOCATED;
                    token.data = calloc(2, sizeof(char));
                    token.data[0] = ch;
                    DynamicTokenArray_append(array, &token);
                    break;
                }
                else if (isdigit(ch)) {
                    if (dequeTokenType == TOKENTYPE_WHITESPACE) {
                        dequeTokenType = TOKENTYPE_COURSE_NUMBER;
                    }
                    (void)dequeEnqueue(CHAR_QUEUE_SIZE, deque, ch);
                    break;
                }
                // Optimization-wise 'isupper' implies 'isalpha', but the overhead is not that big
                // And this is more readable...
                // ... I hope
                else if (isalpha(ch) && isupper(ch)) {
                    if (dequeTokenType != TOKENTYPE_COURSE_SUBJECT && dequeTokenType != TOKENTYPE_COURSE_NUMBER && dequeTokenType != TOKENTYPE_UNKNOWN) {
                        token.type = dequeTokenType;
                        token.flags = TOKEN_DYNAMICALLY_ALLOCATED;
                        token.data = strdup(deque);
                        DynamicTokenArray_append(array, &token);
                        (void)dequeClear(dequeSize, deque);
                        dequeTokenType = TOKENTYPE_COURSE_SUBJECT;
                    }
                    (void)dequeEnqueue(sizeof(deque)/sizeof(deque[0]), deque, ch);
                }
                else if (isalpha(ch) && islower(ch)) {
                    if (dequeTokenType != TOKENTYPE_COURSE_NUMBER) {
                        dequeTokenType = TOKENTYPE_UNKNOWN;
                    }
                    (void)dequeEnqueue(dequeSize, deque, ch);
                }
                else {
                    dequeTokenType = TOKENTYPE_UNKNOWN;
                    (void)dequeEnqueue(dequeSize, deque, ch);
                }
        }
    }
    CharStream_free(stream);
    return array;
}

bool isValidCourseNumberToken(const Token *const token) {
    size_t length = strlen(token->data);
    if (length > 3 && isdigit(token->data[3]) ) {
        return false;
    }
    return true;
}

bool isValidCourseSubjectToken(const Token *const token) {
    size_t length = strlen(token->data);
    if (length < 3 || length > 4) {
        return false;
    }
    for (size_t j = 0; j < length; j++) {
        if (!isupper(token->data[j])) {
            return false;
        }
    }
    return true;
}

bool isValidWhitespaceToken(const Token * const token) {
    if (token->data == nullptr) {
        return true;
    }
    size_t length = strlen(token->data);

    for (size_t i = 0; i < length; i++) {
        if (!isspace(token->data[i])) {
            return false;
        }
    }
    return true;
}

bool isTokensStructuralEqualShallow(const Token * const tokenLeft, const Token * const tokenRight) {
    return tokenLeft->data == tokenRight->data && tokenLeft->type == tokenRight->type && tokenLeft->flags == tokenRight->flags;
}

size_t validate_tokens(DynamicTokenArray *array, size_t start) {
    Token *token = nullptr;
    for (size_t i = start; i < array->occupied; i++) {
        token = &array->data[i];
        switch (token->type) {
            case TOKENTYPE_UNKNOWN: 
                break;
            case TOKENTYPE_COURSE_NUMBER:
                if (!isValidCourseNumberToken(token)) {
                    return i;
                }
                break;
            case TOKENTYPE_COURSE_SUBJECT:
                if (!isValidCourseNumberToken(token)) {
                    return i;
                }
                break;

            case TOKENTYPE_CONJUNCTION:
                if (!isTokensStructuralEqualShallow(token, &TOKEN_CONJUNCTION)) {
                    return i;
                }
                break;

            case TOKENTYPE_DISJUNCTION:
                if (!isTokensStructuralEqualShallow(token, &TOKEN_DISJUNCTION)) {
                    return i;
                }
                break;
            case TOKENTYPE_ADJOIN:
                if (!isTokensStructuralEqualShallow(token, &TOKEN_ADJOIN)) {
                    return i;
                }
                break;
            case TOKENTYPE_PAREN_OPEN:
                if (!isTokensStructuralEqualShallow(token, &TOKEN_PAREN_OPEN)) {
                    return i;
                }
                break;
            case TOKENTYPE_PAREN_CLOSE:
                if (!isTokensStructuralEqualShallow(token, &TOKEN_PAREN_CLOSE)) {
                    return i;
                }
                break;
            case TOKENTYPE_RANGE_START:
                if (!isTokensStructuralEqualShallow(token, &TOKEN_RANGE_START)) {
                    return i;
                }
                break;
            // TODO: Handle this case
            case TOKENTYPE_RANGE_END:
                break;
            case TOKENTYPE_WHITESPACE:
                if (!isValidWhitespaceToken(token)) {
                    return i;
                }
                break;
            }
    }
    return (size_t)-1;
}
