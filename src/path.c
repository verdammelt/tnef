/*
 * path.c -- Utility functions for dealing with pathnames
 *
 * Copyright (C)1999-2005 Mark Simpson <damned@theworld.com>
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

#include "common.h"

#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif

#include "alloc.h"
#include "options.h"
#include "path.h"
#include "debug.h"

/* concatenates fname1 with fname2 to make a pathname, adds '/' as needed
 */ 
char *
concat_fname (const char *fname1, const char* fname2)
{
    char *filename = NULL;

    assert (fname1 || fname2);

    if (!fname1)
    {
        filename = xstrdup (fname2);
    }
    else
    {
        int len = strlen (fname1);
        if (fname2) len += strlen (fname2);

        filename = CHECKED_XMALLOC (char, (len + 2));
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

int
file_exists (const char *fname)
{
    static struct stat buf;
    return (stat (fname, &buf) == 0);
}

/* finds a filename fname.N where N >= 1 and is not the name of an existing
   filename.  Assumes that fname does not already have such an extension */
char *
find_free_number (const char *fname)
{
    size_t len = (strlen(fname) 
		  + 1	/* '.' */
		  + 5	/* big enough for our purposes (i hope) */
		  + 1);	/* NULL */
    char *tmp = CHECKED_XMALLOC (char, len);
    int counter = 1;
    do
    {
	sprintf (tmp, "%s.%d", fname, counter++);
    }
    while (file_exists(tmp));
    return tmp;
}

char *
munge_fname (const char* directory, char *fname)
{
    char *file = NULL;

    /* If we were not given a filename make one up */
    if (!fname || *fname == '\0')
    {
	char *tmp = concat_fname (directory, "tnef-tmp");
	debug_print ("No file name specified, using default.\n");
	file = find_free_number (tmp);
	XFREE (tmp);
    }
    else
    {
	char *buf = NULL;

	if (USE_PATHS)
	{
	    buf = xstrdup (fname);
	}
	else
	{
	    buf = xstrdup (basename (fname));
	    if (strcmp (buf, fname) != 0)
	    {
		debug_print ("!!Filename contains path: '%s'!!\n",
			     fname);
	    }
	}
	file = concat_fname (directory, buf);

	XFREE(buf);
    }

    return file;
}



