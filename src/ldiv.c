/*
 * ldiv.c -- ldiv function if not provided.
 *
 * Copyright (C)1999, 2000, 2001, 2002, 2004 Mark Simpson <damned@world.std.com>
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

#endif /* !HAVE_LDIV */
