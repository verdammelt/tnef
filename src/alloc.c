/*
 * alloc.c -- Useful allocation function/defintions
 *
 * Copyright (C)1999, 2000, 2001, 2002, 2003 Mark Simpson <damned@world.std.com>
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

#include <stdio.h>

#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#else
extern void* memset (void* ptr, int c, size_t size);
extern void* malloc (size_t size);
#endif /* STDC_HEADERS */

#include "alloc.h"

static size_t alloc_limit = 0;

void
set_alloc_limit (size_t size)
{
    alloc_limit = size;
}

size_t
get_alloc_limit()
{
    return alloc_limit;
}

static void
alloc_limit_failure (char *fn_name, size_t size)
{
    fprintf (stderr, 
             "%s: Maximum allocation size exceeded "
             "(maxsize = %lu; size = %lu).\n",
             fn_name,
             (unsigned long)alloc_limit, 
             (unsigned long)size);
}

/* Allocates memory but only up to a limit */
void*
CHECKED_MALLOC (size_t size)
{
    if (alloc_limit && size > alloc_limit)
    {
        alloc_limit_failure ("CHECKED_MALLOC", size);
        exit (-1);
    }
    return MALLOC (size);
}

/* attempts to malloc memory, if fails print error and call abort */
void*
MALLOC (size_t size)
{
    void *ptr = malloc (size);
    if (!ptr 
        && (size != 0))         /* some libc don't like size == 0 */
    {
        perror ("MALLOC: Memory allocation failure");
        abort();
    }
    return ptr;
}

/* CALLOCS memory but only up to a limit */
void*
CHECKED_CALLOC (size_t num, size_t size)
{
    if (alloc_limit && ((num * size) > alloc_limit))
    {
        alloc_limit_failure ("CHECKED_CALLOC", (num*size));
        exit (-1);
    }
    return CALLOC (num, size);
}

/* mallocs memory and clears it out */
void*
CALLOC (size_t num, size_t size)
{
    void *ptr = MALLOC(num * size);
    if (ptr)
    {
        memset (ptr, '\0', (num * size));
    }
    return ptr;
}


