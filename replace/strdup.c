#include <mjt_types.h>

char *strdup(const char *s)
{
    char *ptr;
    if ((ptr = malloc(strlen(s)+1))==NULL)
        return NULL;
    return strcpy(ptr, s);
}

