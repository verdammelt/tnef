/*
 * file.h -- functions for dealing with file output
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
#ifndef FILE_H
#define FILE_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#include "date.h"
#include "attr.h"

typedef struct
{
    char * name;
    size_t len;
    unsigned char * data;
    struct date dt;
    char * mime_type;
} File;

extern void file_write (File *file, const char* directory);
extern void file_add_attr (File* file, const char* directory, Attr* attr);
extern void file_free (File *file);
#endif /* FILE_H */
