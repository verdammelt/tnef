/* strdup.h -- version of strdup for systems without it */
/* Copyright (C) 1999, 2000, 2001, 2002 by Mark Simpson <damned@world.std.com> */
#ifndef STRDUP_H
#define STRDUP_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if HAVE_STRDUP
#  if HAVE_STRING_H
#    include <string.h>
#  else
#    include <strings.h>
#  endif
#else
extern char *
strdup (const char *);
#endif /* HAVE_STRDUP */

#endif /* !STRDUP_H */

