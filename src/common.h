/*
 * common.h -- 'common' declarations etc.
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
#ifndef COMMON_H
#define COMMON_H 1

#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include <assert.h>
#include <stdio.h>

#if STDC_HEADERS
#  include <stdarg.h>
#  include <stdlib.h>
#  include <memory.h>
#  if HAVE_STRING_H
#    include <string.h>
#  endif
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#else
extern int strcmp (const char *, const char *);
extern char *strcpy (char *, const char *);
extern char *strcat (char *, const char *);
extern void abort (void);
extern void exit (int);
extern void* memset (void* ptr, int c, size_t size);
extern void* malloc (size_t size);
#  if !HAVE_MEMMOVE
#    define memmove (d,s,n) bcopy((s),(d),(n))
#  else
extern void *memmove (void *, const void*, size_t)
#  endif
#endif

extern char* xstrdup (const char* str);

/* ********** SIZES ********** */

/* 
 * typedefs for the types specified in the grammar:
 * BYTE  -- 1 byte  -- char
 * WORD  -- 2 bytes -- short/int
 * DWORD -- 4 bytes -- int/long
 */
typedef signed char    int8;
typedef unsigned char  uint8;
#if (SIZEOF_INT == 4)
typedef short          int16;
typedef unsigned short uint16;
typedef int            int32;
typedef unsigned int   uint32;
#else
typedef int            int16;
typedef unsigned int   uint16;
typedef long           int32;
typedef unsigned long  uint32;
#endif /* SIZEOF_INT == 4 */

/* ********** SIZES ********** */

/* ********** REPLACED FUNCS ********** */
#if !HAVE_DECL_BASENAME
extern char *basename (char *path);
#endif

#if HAVE_VPRINTF
#  define VPRINTF(s,f,a) vfprintf((s),(f),(a))
#elif HAVE_DOPRNT
#  define VPRINTF(s,f,a) _doprnt((f),(a),(s))
#else
#  error Neither vpritnf no _doprnt defined
#endif
/* ********** REPLACED FUNCS ********** */

#endif /* COMMON_H */
