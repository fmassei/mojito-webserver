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

#ifndef H_LINEAR_ARRAY_H
#define H_LINEAR_ARRAY_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../logger.h"

struct la_s {
    unsigned char *addr;
    unsigned int size:31;
    unsigned int b:1;
};

/* standard hashtab operations: create/destroy/lookup/install */
struct la_s *lacreate(unsigned int size);
void ladestroy(struct la_s *la);
void *laget(struct la_s *la);

#endif /* H_LINEAR_ARRAY_H */
