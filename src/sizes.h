/* -*- mode: c++ -*- */
/* 
 * sizes.h -- defines types for 8, 16 and 32 bit integer values.
 * 
 * Copyright (C) 1999, 2000 by Mark Simpson <damned@world.std.com>
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
 * Commentary: 
 * 		Defines the sizes appropriately.  oonfig.h must be inculded before
 * this file.
 */
#ifndef SIZES_H
#define SIZES_H

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

#endif /* !SIZES_H */
