/* strdup.c -- version of strdup for systems without one */
/* Copyright (C) 1999-2003 Mark Simpson <damned@world.std.com> */
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

#include "alloc.h"

char *
strdup (const char *str)
{
    size_t len = strlen(str);
    char *out = CHECKED_MALLOC ((len+1) * sizeof (char));
    memmove (out, str, (len + 1));
    return out;
}
#endif /* !HAVE_STRDUP */

