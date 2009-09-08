/*
    Copyright 2009 Francesco Massei

    This file is part of mojito webserver.

        Mojito is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojito is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Mojito.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "linear_hashtab.h"

#define HASHSIZE 101
#define MINSTEP  512    /* minimum step allowed */
#define SHMNAME  "/mojito"

/* the very basic K&R2 hashing function */
static unsigned int hash(char *key)
{
    char *s;
    unsigned int hval;
    for (hval=0, s=key; *s!='\0'; ++s) hval = *s + 31*hval;
    return hval % HASHSIZE;
}

/* create a shared memory mapped area */
struct lh_s *lhcreate(unsigned int size)
{
    struct lh_s *lh = NULL;
    int fd = -1;

    if ((lh = malloc(sizeof(*lh)))==NULL)
        goto error;
    lh->size = size;
    lh->step = (unsigned int)(size / HASHSIZE);
    if (lh->step < MINSTEP) {
        logmsg(LOG_ERROR, "cache size too small!");
        goto error;
    }
    if ((fd = shm_open(SHMNAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR))<0)
        goto error;
    if (ftruncate(fd, lh->size)==-1)
        goto error;
    lh->addr = mmap(NULL, lh->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (lh->addr == MAP_FAILED)
        goto error;
    return lh;
error:
    if (lh) free(lh);
    if (fd>=0) shm_unlink(SHMNAME);
    return NULL;
}

/* kill!kill!! */
void lhdestroy(struct lh_s *lh)
{
    if (!lh) return;
    if (lh->addr)
        munmap(lh->addr, lh->size);
    shm_unlink(SHMNAME);
    free(lh);
}

/* is key present? */
struct entry_s *lhlookup(struct lh_s *lh, char *key)
{
    struct entry_s *ret;
    unsigned char *q, *top, *qkey;
    unsigned int hval, len;
    hval = hash(key);
    q = lh->addr + hval*lh->step;
    top = (hval>=(HASHSIZE-1)) ? (lh->addr+lh->size) : (q+lh->step);
    while(q<top) {
        len = ((struct entry_s *)q)->len;
        if (len==0)
            return NULL;
        qkey = q + (unsigned long)(&((struct entry_s *)0)->key);
        if (!strcmp((char*)qkey, key)) {
            ret = malloc(sizeof(*ret));
            ret->len = len;
            ret->b = ((struct entry_s *)q)->b;
            ret->key = qkey;
            ret->data = qkey + strlen((char*)qkey) + 1;
            return ret;
        }
        q += len;
    }
    return NULL;
}

/* insert an entry into the hash table */
int lhinstall(struct lh_s *lh, char *key, void *data, unsigned int dlen)
{
    struct entry_s *r;
    unsigned char *q, *top;
    unsigned int hval, len;
    hval = hash(key);
    q = lh->addr + hval*lh->step;
    top = (hval>=(HASHSIZE-1)) ? (lh->addr+lh->size) : (q+lh->step);
    while(q<top) {
        r = (struct entry_s *)q;
        if (r->len==0) {
            r->b = 1;
            len = strlen(key) + dlen + sizeof(unsigned int);
            r->len = len;
            q += sizeof(unsigned int);
            memcpy(q, key, strlen(key)+1);
            memcpy(q+strlen(key)+1, data, dlen);
            r->b = 0;
            return 0; 
        }
        q += r->len;
    }
    return -1;
}

