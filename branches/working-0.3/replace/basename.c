#if HAVE_CONFIG_H
#   include <config.h>
#endif

#if HAVE_STRING_H
#   include <string.h>
#elif HAVE_STRINGS_H
#   include <strings.h>
#endif

#if !HAVE_STRRCHR
#   ifndef strrchr
#       define strrchr rindex
#   endif
#endif

char *basename(char *path)
{
    char *basename = strrchr(path, '/');
    return basename ? ++basename : path;
}

