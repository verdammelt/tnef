/*
 * basename.h -- basename for platforms without.
 *
 * Copyright (C)1999, 2000, 2001, 2002 Mark Simpson <damned@world.std.com>
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
#ifndef BASENAME_H
#define BASENAME_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

extern char *
basename (const char* path);

#if HAVE_BASENAME
#  if HAVE_STRING_H
#    include <string.h>
#  else
#    include <strings.h>
#  endif /* HAVE_STRING_H */
#endif /* HAVE_BASENAME */


#endif /* !BASENAME_H */
