/* -*- mode: c++ -*- */
/*
 * alloc.h -- Useful allocation function/defintions
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
#ifndef ALLOC_H
#define ALLOC_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if STDC_HEADERS
#  include <stdlib.h>
#else
extern void free (void*);
#endif /* STDC_HEADERS */

extern void
set_alloc_limit (size_t size);
extern size_t
get_alloc_limit();
extern void* 
CHECKED_MALLOC (size_t size);
extern void*
MALLOC (size_t size);
extern void*
CHECKED_CALLOC (size_t num, size_t size);
extern void*
CALLOC (size_t num, size_t size);
#define FREE(_x) free(_x);_x=NULL

#endif /* ALLOC_H */
