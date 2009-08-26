/*
 * util.h -- Utility functions
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
#ifndef UTIL_H
#define UTIL_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

extern uint32 GETINT32(unsigned char*p);
extern uint16 GETINT16(unsigned char*p);
extern uint8 GETINT8(unsigned char*p);

extern unsigned char* getbuf (FILE *fp, unsigned char buf[], size_t n);
extern uint32 geti32(FILE *fp);
extern uint16 geti16(FILE *fp);
extern uint8 geti8(FILE *fp);

extern unsigned char* unicode_to_utf8 (size_t len, unsigned char*buf);

#endif /* UTIL_H */
