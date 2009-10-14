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

#include "linear_array.h"

#define SHMNAME  "/mojito_fcgi"

/* create a shared memory mapped area */
struct la_s *lacreate(unsigned int size)
{
    struct la_s *la = NULL;
    int fd = -1;

    if ((la = malloc(sizeof(*la)))==NULL)
        goto error;
    la->size = size;
    if ((fd = shm_open(SHMNAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR))<0)
        goto error;
    if (ftruncate(fd, la->size)==-1)
        goto error;
    la->addr = mmap(NULL, la->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (la->addr == MAP_FAILED)
        goto error;
    return lh;
error:
    if (la) free(la);
    if (fd>=0) shm_unlink(SHMNAME);
    return NULL;
}

/* kill!kill!! */
void ladestroy(struct la_s *la)
{
    if (!la) return;
    if (la->addr)
        munmap(la->addr, la->size);
    shm_unlink(SHMNAME);
    free(la);
}

void *laget(struct la_s *la)
{
    return la->addr;
}

