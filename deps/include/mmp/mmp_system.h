/*
    Copyright 2010 Francesco Massei

    This file is part of the MMP library.

        MMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MMP.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file   mmp_system.h
 * \brief   system settings functions wrappers
 * \author  FtM
 * \date    2010-Aug-24
 */
#ifndef H_MMP_SYSTEM_H
#define H_MMP_SYSTEM_H

#include <stdio.h>
#ifndef _WIN32
#   include <unistd.h>
#else
#   include <windows.h>
#endif

/** \brief get the system page size */
long mmp_system_getPageSize(void);
/** \brief get the system page alignment */
long mmp_system_getPageAlignment(void);

#endif /* H_MMP_SYSTEM_H */
