/*
 * alloc.h -- Useful allocation function/defintions
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
#ifndef ALLOC_H
#define ALLOC_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#if !STDC_HEADERS
extern void free (void*);
#endif /* STDC_HEADERS */

extern void set_alloc_limit (size_t size);
extern size_t get_alloc_limit();
extern void alloc_limit_assert (char *fn_name, size_t size);
extern void* checked_xmalloc (size_t num, size_t size);
extern void* xmalloc (size_t num, size_t size);
extern void* checked_xcalloc (size_t num, size_t size);
extern void* xcalloc (size_t num, size_t size);

#define XMALLOC(_type,_num)			                \
        ((_type*)xmalloc((_num), sizeof(_type)))
#define XCALLOC(_type,_num) 				        \
        ((_type*)xcalloc((_num), sizeof (_type)))
#define CHECKED_XMALLOC(_type,_num) 			        \
        ((_type*)checked_xmalloc((_num),sizeof(_type)))
#define CHECKED_XCALLOC(_type,_num) 			        \
        ((_type*)checked_xcalloc((_num),sizeof(_type)))
#define XFREE(_ptr)						\
        do { if (_ptr) { free (_ptr); _ptr = 0; } } while (0)

#endif /* ALLOC_H */
