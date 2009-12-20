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

#ifndef H_MJT_INIPARSE_H
#define H_MJT_INIPARSE_H

#include <mjt_types.h>

#if HAVE_SYS_STAT_H
#   include <sys/stat.h>
#endif
#if HAVE_SYS_MMAN_H
#   include <sys/mman.h>
#endif

#if !HAVE_MMAP || !HAVE_MUNMAP
#   error no mmap/munmap found!
#endif

struct inisection_s {
    char_t *name;
    struct kvlist_s *params;
    struct inisection_s *next;
};

BEGIN_C_DECLS

extern char_t *mjt_inigeterror(void);
extern void mjt_inifree(struct inisection_s **ptr);

END_C_DECLS

#endif /* H_MJT_INIPARSE_H */
