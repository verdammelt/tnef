/*
 * path.c -- Utility functions for dealing with pathnames
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
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <assert.h>
#include <stdio.h>

#if STDC_HEADERS
#  include <string.h>
#else
extern char *strcpy (char *, const char *);
extern char *strcat (char *, const char *);
#endif

#include "strdup.h"

#include "alloc.h"

#include "path.h"

/* concatenates fname1 with fname2 to make a pathname, adds '/' as needed
 */ 
char *
concat_fname (const char *fname1, const char* fname2)
{
    char *filename = NULL;

    assert (fname1 || fname2);

    if (!fname1)
    {
        filename = strdup (fname2);
    }
    else
    {
        int len = strlen (fname1);
        if (fname2) len += strlen (fname2);

        filename = (char *)CHECKED_MALLOC ((len + 2) * sizeof (char));
        strcpy (filename, fname1);

        if (fname2)
        {
            if ((filename[strlen(filename)-1] != '/')
                && (fname2[0] != '/'))
            {
                strcat (filename, "/");
            }
            strcat (filename, fname2);
        }
    }

    return filename;
}

