/*
 * basename.c -- basename function for platforms without
 *
 * Copyright (C)1999-2003 Mark Simpson <damned@world.std.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can either send email to this
 * program's maintainer or write to: The Free Software Foundation,
 * Inc.; 59 Temple Place, Suite 330; Boston, MA 02111-1307, USA.
 *
 */
#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if STDC_HEADERS
#  include <string.h>
#else
#  ifndef HAVE_STRRCHR
#    define strrchr rindex
#  else
extern char *strrchr (const char*, int);
#  endif /* !HAVE_STRRCHR */
extern char *strrchr();
#endif /* STDC_HEADERS */

#if !HAVE_BASENAME
/* works like basename(1) (NOTE: the returned pointer must not be freed! */
char*
basename (const char* path)
{
    char *ptr = strrchr (path, '/');
    return ptr ? ptr + 1 : (char*)path;
}
#endif /* !HAVE_BASENAME */

