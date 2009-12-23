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

#ifndef H_MJT_DAEMON_H
#define H_MJT_DAEMON_H

#include <mjt_types.h>
#include "mjt_rw.h"

#if HAVE_SYS_STAT_H
#   include <sys/stat.h>
#endif
#if HAVE_SIGNAL_H
#   include <signal.h>
#endif
#if HAVE_FCNTL_H
#   include <fcntl.h>
#endif

#ifndef F_TLOCK
#   define F_TLOCK 2    /* FIXME: this is crap */
#endif

#if !HAVE_FORK || !HAVE_SIGNAL || !HAVE_ALARM || !HAVE_PAUSE || \
        !HAVE_GETPID || !HAVE_GETPPID || !HAVE_UMASK || !HAVE_SETSID || \
        !HAVE_CHDIR
#   error some unix-functions not found
#endif

#if !HAVE_LOCKF
#   error no lockf found!
#endif

BEGIN_C_DECLS

extern int_t fork_to_background(char_t *pidfile, uid_t uid, gid_t gid, 
                                char_t *rootdir, void(*termfunc)(int_t));

END_C_DECLS

#endif /* H_MJT_DATE_H */
