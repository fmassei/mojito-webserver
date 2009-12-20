#include <mjt_types.h>

/* taken from libiberty */
int vasprintf(char **res, const char *format, va_list ap)
{
    const char *p = format;
    int total_width = strlen(format)+1;
    va_list lap;
    va_copy(lap, ap);
    while (*p!='\0') {
        if (*p++=='%') {
            while(strchr("-+ #0", *p))
                ++p;
            if (*p=='*') {
                ++p;
                total_width += abs(va_arg(lap, int));
            } else total_width += strtoul(p, (char**)&p, 10);
            if (*p=='.') {
                ++p;
                if (*p=='*') {
                    ++p;
                    total_width += abs(va_arg(lap, int));
                } else total_width += strtoul(p, (char**)&p, 10);
            }
            while(strchr("hlL", *p))
                ++p;
            total_width += 30;
            switch(*p) {
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
            case 'c':
                (void)va_arg(lap, int);
                break;
            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                (void)va_arg(lap, double);
                total_width += 307;
                break;
            case 's':
                total_width += strlen(va_arg(lap, char*));
                break;
            case 'p':
            case 'n':
                (void)va_arg(lap, char*);
                break;
            }
            p++;
        }
    }
    va_end(lap);
    if ((*result = mjt_malloc(total_width))!=NULL)
        return vsprintf(*result, format, ap);
    else
        return -1;
}

