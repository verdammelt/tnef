/* -*- mode: c++ -*- */
/* 
 * tnef.h -- extract files from Microsoft TNEF format.
 * 
 * Copyright (C) 1999, 2000, 2001, 2002 by Mark Simpson <damned@world.std.com>
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
 * 		Various defined values for decoding a TNEF file.
 */
#ifndef TNEF_H
#define TNEF_H

/* TNEF signature.  Equivalent to the magic cookie for a TNEF file. */
#define TNEF_SIGNATURE   0x223e9f78

/* Object types */
#define LVL_MESSAGE      0x01
#define LVL_ATTACHMENT   0x02

/* Defines uint[8,16,32] */
#include "sizes.h"

/* Generated list of attribute types/sizes */
#include "tnef_types.h"
#include "mapi_types.h"

/* Generated list of attribute names */
#include "tnef_names.h"
#include "mapi_names.h"

/* Store a date according to file specification */
struct date
{
    int16 year, month, day;
    int16 hour, min, sec;
    int16 dow;
};

/* flags to modify behaviour of file parsing */
enum { NONE    	= 0x00,
       VERBOSE 	= 0x01,
       DBG_OUT 	= 0x02,
       LIST    	= 0x04,
       PATHS   	= 0x08,
       OVERWRITE= 0x10,
       CONFIRM 	= 0x20,
       NUMBERED = 0x40
};

/* Main entrance point to tnef processing */
extern int
parse_file(FILE *input, char * output_dir, int flags);

#endif /* !TNEF_H */



