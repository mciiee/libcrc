#ifndef UTILS_H
#define UTILS_H

#ifndef STRN_EQUALS
#define STRN_EQUALS(left, right, n) strncmp(left, right, n) == 0
#endif

#ifndef STR_EQUALS
#define STR_EQUALS(left, right) strcmp(left, right) == 0
#endif

char *strtok_rs(char * restrict str, const char *restrict delim, char **restrict saveptr);

#endif
