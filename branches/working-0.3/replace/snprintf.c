#include <mjt_types.h>

/* taken from libiberty */
int snprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    int ret;
    VA_START(ap, format);
    ret = vsnprintf(str, size, format, ap);
    va_end(ap);
    return ret;
}

