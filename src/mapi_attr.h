/*
 * mapi_attr.h -- Functions for handling MAPI attributes
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
#ifndef MAPI_ATTR_H
#define MAPI_ATTR_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#include "mapi_types.h"
#include "mapi_names.h"

#define MULTI_VALUE_FLAG 0x1000
#define GUID_EXISTS_FLAG 0x8000

typedef struct
{
    uint32 data1;
    uint16 data2;
    uint16 data3;
    uint8 data4[8];
} GUID;

typedef struct
{
    size_t len;
    union
    {
        unsigned char *buf;
        uint16 bytes2;
        uint32 bytes4;
        uint32 bytes8[2];
        GUID guid;
    } data;
} MAPI_Value;

typedef struct
{
    size_t len;
    unsigned char* data;
} VarLenData;

typedef struct
{
    mapi_type type;
    mapi_name name;
    size_t num_values;
    MAPI_Value* values;
    GUID *guid;
    size_t num_names;
    VarLenData *names;
} MAPI_Attr;

extern MAPI_Attr** mapi_attr_read (size_t len, unsigned char *buf);
extern void mapi_attr_free_list (MAPI_Attr** attrs);

#endif /* MAPI_ATTR_H */
