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
#ifndef H_MMP_H_UTILS_H
#define H_MMP_H_UTILS_H

#ifdef __cplusplus
#   define MMP_CEXTERN_BEGIN    extern "C" {
#   define MMP_CEXTERN_END      }
#else
#   define MMP_CEXTERN_BEGIN
#   define MMP_CEXTERN_END
#endif

#ifndef UNREFERENCED_PARAM
#   define UNREFERENCED_PARAM(_P) ((void)(_P))
#endif

#endif /* H_MMP_H_UTILS_H */
