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

#include "mjt_mime.h"

/* return the mime type based on file extension */
const char *mjt_mime_gettype(const char *fname)
{
    char *ext;
    if ((ext = rindex(fname, '.'))==NULL)
        return "text/plain";
    ++ext;
    if (!strcmp(ext, "html"))
        return "text/html";
    if (!strcmp(ext, "htm"))
        return "text/html";
    if (!strcmp(ext, "css"))
        return "text/css";
    if (!strcmp(ext, "rss"))
        return "application/rss+xml";
    if (!strcmp(ext, "jpg"))
        return "image/jpg";
    if (!strcmp(ext, "gif"))
        return "image/gif";
    if (!strcmp(ext, "png"))
        return "image/png";
    if (!strcmp(ext, "ico"))
        return "image/x-icon";
    return "application/octet-stream";
}

