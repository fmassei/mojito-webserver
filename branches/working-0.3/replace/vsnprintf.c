#include <mjt_types.h>

/* taken from libiberty */
int vsnprintf(char *str, const char *format, va_list ap)
{
    char *buf = NULL;
    int result = vasprintf(&buf, format, ap);
    if (buf==NULL)
        return -1;
    if (result<0) {
        free(buf);
        return -1;
    }
    result = strlen(buf);
    if (size>0) {
        if ((long)size > result)
            memcpy(str, buf, result+1)
        else {
            memcpy(str, buf, n-1);
            str[n-1] = 0;
        }
    }
    mjt_free(&buf);
    return result;
}

