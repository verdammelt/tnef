/* -*- mode: c++ -*- */
/* 
 * ldiv.h
 * 
 * Copyright (C) 2001 by Mark Simpson <damned@world.std.com>
 * 
 * Commentary: 
 */
#ifndef LDIV_H
#define LDIV_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if HAVE_LDIV
#  include <stdlib.h>
#else

typedef struct
{
    long int quot;              /* Quotient. */
    long int rem;               /* Remainder */
} ldiv_t;

extern ldiv_t ldiv (long int numer, long int denom);

#endif /* HAVE_LDIV */

#endif
