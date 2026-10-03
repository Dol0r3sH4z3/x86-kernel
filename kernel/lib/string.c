#include <kernel/string.h>

size_t strlen(const char *str, size_t maxlen)
{
    size_t len = 0;
    while (len < maxlen && str[len] != '\0')
    {
        len++;
    }
    return len;
}

bool strcmp(const char *src, const char *cmp)
{
    size_t i = 0;

    while (src[i] == cmp[i])
    {
        if (src[i] == '\0')
        {
            return true;
        }
        i++;
    }

    return false;
}