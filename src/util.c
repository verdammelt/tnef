/*
 * util.c -- Utility functions
 *
 * Copyright (C)1999-2005 Mark Simpson <damned@theworld.com>
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
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#include "util.h"
#include "options.h"

/* Needed to transform char buffers into little endian numbers */
uint32 GETINT32(char *p)
{
    return (uint32)((uint8)(p)[0]           \
		    +((uint8)(p)[1]<<8)     \
		    +((uint8)(p)[2]<<16)    \
		    +((uint8)(p)[3]<<24));
}

uint16 GETINT16 (char* p)
{
    return (uint16)((uint8)(p)[0]+((uint8)(p)[1]<<8));
}

uint8 GETINT8 (char *p)
{
    return (uint8)(p)[0];
}

unsigned char*
getbuf (FILE *fp, unsigned char buf[], size_t n)
{
    if (fread (buf, 1, n, fp) != n)
    {
	perror ("Unexpected end of input");
	exit (1);
    }
    return buf;
}

uint32
geti32 (FILE *fp)
{
    unsigned char buf[4];
    return (uint32)GETINT32(getbuf(fp, buf, 4));
}
uint16
geti16 (FILE *fp)
{
    unsigned char buf[2];
    return (uint16)GETINT16(getbuf(fp, buf, 2));
}

uint8
geti8(FILE *fp)
{
    unsigned char buf[1];
    return (uint8)GETINT8(getbuf(fp, buf, 1));
}

