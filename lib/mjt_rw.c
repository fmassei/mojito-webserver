#include "mjt_rw.h"

/* check if a directory exists and it's writable */
bool_t mjt_isrwdir(const char_t *path)
{
    struct stat sb;
    bzero(&sb, sizeof(sb));
    if (stat(path, &sb)<0)
        return FALSE;
    if ((sb.st_mode&S_IFMT)!=S_IFDIR)
        return FALSE;
    if (access(path, R_OK|W_OK|W_OK)<0)
        return FALSE;
    return TRUE;
}

/* write something somewhere, going on on partial writings.
 * Oh man, what a function!
 * PS. count could not be less-than-zero due to its type.
 * If count is zero, the "write" syscall MUST be called, as it could have been
 * called for error-checking purpouses. */
bool_t mjt_justwrite(int_t fd, const void *buf, sizet_t count)
{
    ssizet_t ret, cur;
    cur = 0;
    do {
        if ((ret = write(fd, buf, count))<0)
            return FALSE;
        cur += ret;
        count -= cur;
    } while(count>0);
    return TRUE;
}

