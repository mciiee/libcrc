#include <string.h>

#include "utils.h"

char *strtok_rs(char *str, const char *delim, char **saveptr) {
    if (!str && !(str = *saveptr)) { 
        return nullptr;
    }
    str += strspn(str, delim);
    if (!str[0]) {
        *saveptr = nullptr;
        return nullptr;
    }
    *saveptr = str + strcspn(str, delim);
    if (**saveptr) {
        **saveptr = 0;
        (*saveptr)++;
    }
    else { 
        *saveptr = nullptr;
    }
    return str;
}


