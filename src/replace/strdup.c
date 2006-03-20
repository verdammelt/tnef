/*
 * strdup.c -- version of strdup for systems without one
 *
 * Copyright (C)1999-2006 Mark Simpson <damned@theworld.com>
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

#if !HAVE_STRDUP
#include <assert.h>
#include <stdio.h>

#if STDC_HEADERS
#  include <stdlib.h>
#else
extern size_t strlen (const char *);

#  if !HAVE_MEMMOVE
#    define memmove(d,s,n) bcopy((s),(d),(n));
#  else
extern void* memmove (void *, const void *, size_t);
#  endif
#endif

char *
strdup (const char *str)
{
    size_t len = strlen(str);
    char *out = malloc ((len+1) * sizeof (char));
    memmove (out, str, (len + 1));
    return out;
}
#endif /* !HAVE_STRDUP */

