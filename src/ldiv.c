/* Version of ldiv for systems for systems without it */
/* Copyright (C) 2001 Mark Simpson <damned@world.std.com> */
#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if !HAVE_LDIV
#include "ldiv.h"

ldiv_t
ldiv (int numer, int denom)
{
    static ldiv_t ld;

    ld.quot = numer / denom;
    ld.rem = numer % denom;

    return ld;
}
