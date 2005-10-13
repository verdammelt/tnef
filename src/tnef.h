/* 
 * tnef.h -- extract files from Microsoft TNEF format.
 * 
 * Copyright (C) 1999-2005 by Mark Simpson <damned@theworld.com>
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

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

/* TNEF signature.  Equivalent to the magic cookie for a TNEF file. */
#define TNEF_SIGNATURE   0x223e9f78

/* Main entrance point to tnef processing */
extern int
parse_file(FILE *input, char * output_dir, 
	   char *body_file, char *body_pref,
	   int flags);

#endif /* !TNEF_H */



