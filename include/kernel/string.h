#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

size_t strlen(const char *str, size_t maxlen);
bool strcmp(const char *src, const char *cmp);

#endif