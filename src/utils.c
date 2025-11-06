#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#include <string.h>
#endif

#include "utils.h"

char *strtok_rs(char * restrict str, const char *restrict delim, char **restrict saveptr) {
     #if !(defined (WIN32)) && !(defined (__WIN32))
    return strtok_r(str, delim, saveptr);
    #else 
    // TODO: Finish this implementation for Windows
    if(saveptr == nullptr) {
        size_t i = 0;
        for (; str[i] != '\0' && strcmp(str + i, delim) != 0 ; i++) {
            
        }   
    }
    else {

    }
    #endif
}
