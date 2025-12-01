#include "deque.h"
#include <stddef.h>
#include <string.h>

ptrdiff_t firstNullTerm(size_t bufferSize, char buffer[bufferSize]) {
    char *zeroPtr =  memchr(buffer, '\0', bufferSize);
    if (zeroPtr == nullptr) {
        return (ptrdiff_t)-1;
    }
    return zeroPtr - buffer;
}

const char * dequeEnqueue(size_t dequeSize, char deque[dequeSize], char appendee){
   ptrdiff_t insertOffset = firstNullTerm(dequeSize, deque);
    if (insertOffset == (ptrdiff_t)-1) {
        return nullptr;
    }
    deque[insertOffset] = appendee;
    return deque;
}

char dequePopLast(size_t dequeSize, char deque[dequeSize]) {
    size_t offset = firstNullTerm(dequeSize, deque);
    if (offset == (ptrdiff_t)-1) {
        return -1;
    }
    else if (offset == 0) {
        return 0;
    }
    char ch = deque[offset - 1];
    deque[offset-1] = '\0';
    return ch;
}

char dequePeekLast(size_t dequeSize, char deque[dequeSize]) {
    size_t index = firstNullTerm(dequeSize, deque);
    if (index == (size_t)-1) {
        return -1;
    }
    else if (index == 0) {
        return 0;
    }
    return deque[index - 1];
}

const char * dequeClear(size_t dequeSize, char deque[dequeSize]) {
    memset(deque, '\0', dequeSize);
    return deque;
}

bool dequeIsEmpty(size_t dequeSize, char deque[dequeSize]) {
    return deque[0] == '\0';
}
