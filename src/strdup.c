/* strdup.c -- version of strdup for systems without one */
/* Copyright (C) 1999, 2000, 2001 Mark Simpson <damned@world.std.com> */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if !HAVE_STRDUP
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
char *
strdup (const char *str)
{
    size_t len = strlen(str);
    char *out = (char*)malloc ((len + 1) * sizeof (char));
    assert (out);
    if (!out) abort();
    memmove (out, str, (len + 1));
    return out;
}
#endif /* !HAVE_STRDUP */

