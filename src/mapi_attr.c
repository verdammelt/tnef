/*
 * mapi_attr.c -- Functions for handling MAPI attributes
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

#if !HAVE_LDIV
#  include "replace/ldiv.h"
#endif 

#include "common.h"

#include "mapi_attr.h"
#include "alloc.h"
#include "options.h"
#include "util.h"
#include "write.h"

/* return the length padded to a 4 byte boundary */
static size_t
pad_to_4byte (size_t length)
{
    ldiv_t d = ldiv (length, 4L);
    if (d.rem != 0)
    {
	length += (4 - d.rem);
    }
    return length;
}

/* Copy the GUID data from a character buffer */
static void
copy_guid_from_buf (GUID* guid, char *buf)
{
    int i;
    int idx = 0;
    assert (guid);
    assert (buf);

    guid->data1 = GETINT32(buf + idx); idx += sizeof (uint32);
    guid->data2 = GETINT16(buf + idx); idx += sizeof (uint16);
    guid->data3 = GETINT16(buf + idx); idx += sizeof (uint16);
    for (i = 0; i < 8; i++, idx += sizeof (uint8))
	guid->data4[i] = (uint8)(buf[idx]);
}


/* dumps info about MAPI attributes... useful for debugging */
static void
mapi_attr_dump (MAPI_Attr* attr)
{
    char *name = get_mapi_name_str (attr->name);
    char *type = get_mapi_type_str (attr->type);
    size_t i;

    fprintf (stdout, "(MAPI) %s [type: %s] [num_values = %lu] = \n",
	     name, type, (unsigned long)attr->num_values);
    if (attr->guid)
    {
	fprintf (stdout, "\tGUID: ");
	write_guid (stdout, attr->guid);
	fputc ('\n', stdout);
    }
	
    for (i = 0; i < attr->num_names; i++)
	fprintf (stdout, "\tname #%d: '%s'\n", i, attr->names[i].data);

    for (i = 0; i < attr->num_values; i++)
    {
	fprintf (stdout, "\t#%lu [len: %lu] = ",
		 (unsigned long)i,
		 (unsigned long)attr->values[i].len);

	switch (attr->type)
	{
	case szMAPI_NULL:
	    fprintf (stdout, "NULL");
	    break;

	case szMAPI_SHORT:
	    write_int16 (stdout, (int16)attr->values[i].data.bytes2);
	    break;

	case szMAPI_INT:
	    write_int32 (stdout, (int32)attr->values[i].data.bytes4);
	    break;

	case szMAPI_FLOAT:
	case szMAPI_DOUBLE:
	    write_float (stdout, (float)attr->values[i].data.bytes4);
	    break;

	case szMAPI_BOOLEAN:
	    write_boolean (stdout, attr->values[i].data.bytes4);
	    break;

	case szMAPI_STRING:
	case szMAPI_UNICODE_STRING:
	    write_string (stdout, attr->values[i].data.buf);
	    break;

	case szMAPI_SYSTIME:
	case szMAPI_CURRENCY:
	case szMAPI_INT8BYTE:
	case szMAPI_APPTIME:
	    write_uint64 (stdout, attr->values[i].data.bytes8);
	    break;

	case szMAPI_ERROR:
	    write_uint32 (stdout, attr->values[i].data.bytes4);
	    break;

	case szMAPI_CLSID:
	    write_guid (stdout, &attr->values[i].data.guid);
	    break;

	case szMAPI_OBJECT:
	case szMAPI_BINARY:
	{
	    size_t x;

	    for (x = 0; x < attr->values[i].len; x++)
	    {
		write_byte (stdout, (uint8)attr->values[i].data.buf[x]);
		fputc (' ', stdout);
	    }
	}
	break;

	default:
	    fprintf (stdout, "<unknown type>");
	    break;
	}
	fprintf (stdout, "\n");
    }
}

static MAPI_Value*
alloc_mapi_values (MAPI_Attr* a)
{
    if (a && a->num_values)
    {
	a->values = CHECKED_XCALLOC (MAPI_Value, a->num_values);
	return a->values;
    }
    return NULL;
}

/* parses out the MAPI attibutes hidden in the character buffer */
MAPI_Attr**
mapi_attr_read (size_t len, char *buf)
{
    size_t idx = 0;
    uint32 i;
    uint32 num_properties = GETINT32(buf+idx);
    MAPI_Attr** attrs = CHECKED_XMALLOC (MAPI_Attr*, (num_properties + 1));

    idx += 4;

    if (!attrs) return NULL;
    for (i = 0; i < num_properties; i++)
    {
	MAPI_Attr* a = attrs[i] = CHECKED_XCALLOC(MAPI_Attr, 1);
	MAPI_Value* v = NULL;

	a->type = GETINT16(buf+idx); idx += 2;
	a->name = GETINT16(buf+idx); idx += 2;

	/* handle special case of GUID prefixed properties */
	if (a->name >= 0x8000)
	{
	    /* copy GUID */
	    a->guid = CHECKED_XMALLOC(GUID, 1);
	    copy_guid_from_buf(a->guid, buf+idx);
	    idx += sizeof (GUID);

	    a->num_names = GETINT32(buf+idx); idx += 4;
	    if (a->num_names > 0)
	    {
		/* FIXME: do something useful here! */
		size_t i;

		a->names = CHECKED_XCALLOC(VarLenData, a->num_names);

		for (i = 0; i < a->num_names; i++)
		{
		    size_t j;

		    a->names[i].len = GETINT32(buf+idx); idx += 4;

		    /* read the data into a buffer */
		    a->names[i].data 
			= CHECKED_XMALLOC(char, a->names[i].len);
		    for (j = 0; j < (a->names[i].len >> 1); j++)
			a->names[i].data[j] = (buf+idx)[j*2];

		    /* But what are we going to do with it? */
		    
		    idx += pad_to_4byte(a->names[i].len);
		}
	    }
	    else
	    {
		/* get the 'real' name */
		a->name = GETINT32(buf+idx); idx+= 4;
	    }
	}

	switch (a->type)
	{
	case szMAPI_NULL:
	    a->num_values = 0;
	    v = NULL;
	    break;

	case szMAPI_SHORT:        /* 2 bytes */
	    a->num_values = 1;
	    v = alloc_mapi_values (a);
	    v->len = 2;
	    v->data.bytes2 = GETINT16(buf+idx);
	    idx += 4; /* advance by 4! */
	    break;

	case szMAPI_INT:
	case szMAPI_FLOAT:      /* 4 bytes */
	case szMAPI_ERROR:
	case szMAPI_BOOLEAN:	/* spec says 2 bytes but reality is 4! */
	    a->num_values = 1;
	    v = alloc_mapi_values (a);
	    v->len = 4;
	    v->data.bytes4 = GETINT32(buf+idx);
	    idx += v->len;
	    break;

	case szMAPI_DOUBLE:
	case szMAPI_APPTIME:
	case szMAPI_CURRENCY:
	case szMAPI_INT8BYTE:
	case szMAPI_SYSTIME:         /* 8 bytes */
	    a->num_values = 1;
	    v = alloc_mapi_values (a);
	    v->len = 8;
	    v->data.bytes8[0] = GETINT32(buf+idx);
	    v->data.bytes8[1] = GETINT32(buf+idx+4);
	    idx += v->len;
	    break;

	case szMAPI_CLSID:
	    a->num_values = 1;
	    v = alloc_mapi_values (a);
	    v->len = sizeof (GUID);
	    copy_guid_from_buf(&v->data.guid, buf+idx);
	    idx += v->len;
	    break;

	case szMAPI_STRING:
	case szMAPI_UNICODE_STRING:
	case szMAPI_OBJECT:
	case szMAPI_BINARY:       /* variable length */
	case szMAPI_UNSPECIFIED:
	{
	    size_t val_idx = 0;
	    a->num_values = GETINT32(buf+idx); idx += 4;
	    v = alloc_mapi_values (a);
	    for (val_idx = 0; val_idx < a->num_values; val_idx++)
	    {
		v[val_idx].len = GETINT32(buf+idx); idx += 4;
		v[val_idx].data.buf 
		    = CHECKED_XMALLOC(char, v[val_idx].len);
		memmove (v[val_idx].data.buf,
			 buf+idx,
			 v[val_idx].len);
		idx += pad_to_4byte(v[val_idx].len);
	    }
	}
	break;
	}
	if (DEBUG_ON) mapi_attr_dump (attrs[i]);

    }
    attrs[i] = NULL;


    return attrs;
}

static void
mapi_attr_free (MAPI_Attr* attr)
{
    if (attr)
    {
	size_t i;
	for (i = 0; i < attr->num_values; i++)
	{
	    if ((attr->type == szMAPI_STRING)
		|| (attr->type == szMAPI_UNICODE_STRING)
		|| (attr->type == szMAPI_BINARY))
	    {
		XFREE (attr->values[i].data.buf);
	    }
	}
	XFREE (attr->values);
	memset (attr, '\0', sizeof (MAPI_Attr));
    }
}

void
mapi_attr_free_list (MAPI_Attr** attrs)
{
    int i;
    for (i = 0; attrs && attrs[i]; i++)
    {
	mapi_attr_free (attrs[i]);
	XFREE (attrs[i]);
    }
}
