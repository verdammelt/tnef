/*
 * path.c -- Utility functions for dealing with pathnames
 *
 * Copyright (C)1999-2018 Mark Simpson <damned@theworld.com>
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
#include <ctype.h>

#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif

#include "alloc.h"
#include "options.h"
#include "path.h"
#include "debug.h"

#if !HAVE_RINDEX && HAVE_STRRCHR
#  define rindex strrchr
#endif

#if !HAVE_INDEX
#  define index strchr
#endif

/* concatenates fname1 with fname2 to make a pathname, adds '/' as needed */
/* strips trailing '/' */

char *
concat_fname (const char *fname1, const char* fname2)
{
    char *filename;
    int len;

    if ( ( fname1 == NULL ) && ( fname2 == NULL ) ) return NULL;

    if ( ( fname1 == NULL ) || ( *fname1 == '\0' ) )
    {
        filename = xstrdup (fname2);
    }
    else
    {
        len = strlen (fname1);
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

    /* strip trailing '/' */

    while ((len=strlen(filename)) > 0)
    {
        if ( filename[len-1] == '/' )
        {
            filename[len-1] = '\0';
        }
        else
        {
            break;
        }
    }

    if ( *filename == '\0' ) filename = NULL;		/* nothing left */

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

/* windows pathname manipulation routines */

/* per windows file manager, these aren't allowed in filenames */

static char unsanitary_windows_chars[] = {
        '\\', '/', ':', '*', '?', '"', '<', '>', '|', '\0'
};

/* these aren't welcomed in unix filenames */

static char unsavory_unix_chars[] = {
        ' ', ';', '`', '\'', '[', ']', '{', '}', '(', ')', '\0'
};

static int
could_be_a_windows_path( const char *fname )
{
    const char *up;

    if ( ( fname == NULL ) || ( *fname == '\0' ) ) return 0;

    /*
        we might be a windows path if...
        we have at least one path separator and
        we have no unsanitary windows chars and
        we are reasonably printable
    */

    up = rindex( fname, '\\' );

    if ( up )
    {
        up++;
        if ( *up == '\0' ) return 0;	/* trailing backslash doesn't cut it */
    }
    else
    {
        return 0;			/* no backslash */
    }

    for ( up=unsanitary_windows_chars; *up; up++ )
    {
        if ( *up == '\\' ) continue;	/* ignore backslashes */

        if ( index( fname, (int)*up ) )
        {
            return 0;			/* found something we can't stomach */
        }
    }

    for ( up=fname; *up; up++ )
    {
        if ( iscntrl( (int)*up ) )
        {
            return 0;			/* found something we can't see */
        }
    }

    /* found nothing to the contrary, so we might just be a path */

    if (DEBUG_ON) debug_print( "!!windows path possible: %s\n", fname );

    return 1;
}

static unsigned char hex_digits[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

#define SLOP 4		/* a minimum rational buffer size */

static char *
sanitize_filename( const char *fname )
{
    char *buf, *bp;
    const char *cp, *up;
    int flag, stet;

    if ( ( fname == NULL ) || ( *fname == '\0' ) )
    {
        /* nothing to see here -- return small, zeroed buffer */
        buf = CHECKED_XCALLOC( char, SLOP );
        return buf;
    }

    /*
        sanitize the filename by modifying unsanitary or unsavory
        characters using the URL escape technique of c => %XX
        return a "fresh and freeable" buffer with the sanitary filename
    */

    buf = CHECKED_XCALLOC( char, 3*strlen(fname)+SLOP );
    bp = buf;

    for ( cp=fname; *cp; cp++ )
    {
        flag = 0;

        while (1)
        {
            /* control chars */

            if ( iscntrl( (int)*cp ) )
            {
                stet = 0;
                break;
            }

            /* unsanitary windows chars */

            for ( up=unsanitary_windows_chars; *up; up++ )
            {
                if ( *cp == *up )
                {
                    flag = 1;
                    stet = 0;
                    break;		/* for loop... */
                }
            }

            if ( flag ) break;		/* while loop... */

            if ( UNIX_FS )
            {
                /* non-ascii chars */

                if ( !isascii( (int)*cp ) )
                {
                    stet = 0;
                    break;
                }

                /* unsavory unix chars */

                for ( up=unsavory_unix_chars; *up; up++ )
                {
                    if ( *cp == *up )
                    {
                        flag = 1;
                        stet = 0;
                        break;		/* for loop... */
                    }
                }

                if ( flag ) break;		/* while loop... */
            }

            /* escape the escaper */

            if ( *cp == '%' )
            {
                stet = 0;
                break;
            }

            /* nothing obvious */

            stet = 1;
            break;
        }

        /* handle the char */

        if ( stet )
        {
            *bp++ = *cp;		/* keep it */
        }
        else
        {
            *bp++ = '%';		/* escape it */
            *bp++ = hex_digits[ ((*cp)>>4)&0xf ];
            *bp++ = hex_digits[  (*cp)    &0xf ];
        }
    }

    return buf;
}

/*
  pathname generator

  takes input filename and "sanitizes" it for filesystem use
  understands windows paths and unixisms
  returns NULL if totally unsavory filename
  returns (freeable) pointer to sanitized filename if at all palatable
*/

char *
munge_fname( const char *fname )
{
    char *dir, *base, *p, *fpd, *fpb;

    /* If we were not given a filename give up */
    if (!fname || *fname == '\0')
    {
      return NULL;
    }

    if ( USE_PATHS )
    {
        /* evaluate windows path potential */

        if ( could_be_a_windows_path( (char *)fname ) )
        {
            /* split fname after last path separator */

            dir =  strdup( fname );	fpd = dir;
            base = rindex( dir, (int)'\\' );
            base++;
            *base = '\0';

            base = strdup( fname );	fpb = base;
            base = rindex( base, (int)'\\' );
            base++;

            /* flip path separators */

            for ( p=dir; *p; p++ )
            {
                if ( *p == '\\' ) *p = '/';
            }

            /* handle absolute path separators */

            if ( *dir == '/' )
            {
                if ( ABSOLUTE_OK )
                {
                    if (VERBOSE_ON) debug_print( "WARNING: absolute path: %s", fname );
                    if (DEBUG_ON) debug_print( "!!absolute path: %s", fname );
                }
                else
                {
                    if (VERBOSE_ON) debug_print( "WARNING: absolute path stripped: %s", fname );
                    if (DEBUG_ON) debug_print( "!!absolute path stripped: %s", fname );

                    while ( *dir == '/' ) dir++;

                    if ( *dir == '\0' ) dir = NULL;	/* nothing left */
                }
            }
        }
        else
        {
            /* not recognized as a windows path */

            dir = NULL;			fpd = NULL;
            base = strdup( fname );	fpb = base;
        }
    }
    else
    {
        /* no paths allowed */

        dir = NULL;			fpd = NULL;
        base = strdup( fname );		fpb = base;
    }

    /* cleanup the basename */

    base = sanitize_filename( base );

    /* build a pathname out of the pieces */

    p = concat_fname( dir, base );

    /* cleanup and return */

    XFREE( fpd );			/* free what we may have allocated */
    XFREE( fpb );			/* free what we may have allocated */
    XFREE( base );			/* free what the sanitizer allocated */

    if ( p && ( *p == '\0' ) )
    {
        XFREE( p );			/* free what we allocated and is no longer required */
        p = NULL;
    }

    return p;
}
