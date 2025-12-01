#ifndef DEQUE_H
#define DEQUE_H
#include <stddef.h>

ptrdiff_t firstNullTerm(size_t bufferSize, char buffer[bufferSize]);

[[nodiscard("Check whether it failed")]]
const char * dequeEnqueue(size_t dequeSize, char deque[dequeSize], char appendee);

[[maybe_unused]]
char dequePopLast(size_t dequeSize, char deque[dequeSize]);

[[maybe_unused]]
char dequePeekLast(size_t dequeSize, char deque[dequeSize]);

[[nodiscard("Check whether it failed")]]
const char * dequeClear(size_t dequeSize, char deque[dequeSize]);

[[maybe_unused]]
bool dequeIsEmpty(size_t dequeSize, char deque[dequeSize]);
#endif
