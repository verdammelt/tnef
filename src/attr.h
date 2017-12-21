/*
 * attr.h -- Functions for handling tnef attributes
 *
 * Copyright (C)1999-2018 Mark Simpson <damned@theworld.com>
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
#ifndef ATTR_H
#define ATTR_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"
#include "date.h"
#include "tnef_types.h"
#include "tnef_names.h"

#define MINIMUM_ATTR_LENGTH 8

/* Object types */
enum _lvl_type
{
    LVL_MESSAGE		= 0x1,
    LVL_ATTACHMENT	= 0x2,
};
typedef enum _lvl_type level_type;

/* Attr -- storing a structure, formated according to file specification */
typedef struct
{
    level_type lvl_type;
    tnef_type type;
    tnef_name name;
    size_t len;
    unsigned char* buf;
} Attr;

typedef struct
{
    uint16 id;
    uint16 chbgtrp;
    uint16 cch;
    uint16 cb;
} TRP;

typedef struct
{
    TRP trp;
    char* sender_display_name;
    char* sender_address;
} TRIPLE;

extern void attr_dump (Attr* attr);
extern void attr_free (Attr* attr);
extern void copy_date_from_attr (Attr* attr, struct date* dt);
extern Attr* attr_read ();

#endif /* ATTR_H */
