/*
 * rtf.c -- utility function for dealing with RTF content
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
 * Commentary:
 *     Entry point is save_rtf_data.  All other functions are internal. 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#include "alloc.h"
#include "file.h"
#include "mapi_attr.h"
#include "path.h"
#include "util.h"

static const uint32 rtf_uncompressed_magic = 0x414c454d;
static const uint32 rtf_compressed_magic = 0x75465a4c;
static const char* rtf_prebuf = "{\\rtf1\\ansi\\mac\\deff0\\deftab720{\\fonttbl;}{\\f0\\fnil \\froman \\fswiss \\fmodern \\fscript \\fdecor MS Sans SerifSymbolArialTimes New RomanCourier{\\colortbl\\red0\\green0\\blue0\n\r\\par \\pard\\plain\\f0\\fs20\\b\\i\\u\\tab\\tx";


static int
is_rtf_data (unsigned char *data)
{
    size_t compr_size = 0L;
    size_t uncompr_size = 0L;
    uint32 magic;
    size_t idx = 0;

    compr_size = GETINT32(data + idx); idx += 4;
    uncompr_size = GETINT32(data + idx); idx += 4;
    magic = GETINT32(data + idx); idx += 4;
    
    if ((magic == rtf_uncompressed_magic) 
	|| (magic == rtf_compressed_magic))
	return 1;
    return 0;
}

static unsigned char*
decompress_rtf_data (unsigned char *src, size_t len)
{
    const size_t rtf_prebuf_len = strlen(rtf_prebuf);

    int in = 0;
    int out = 0;
    int flag_count = 0;
    int flags = 0;
    unsigned char *ret = NULL;
    unsigned char *dest = CHECKED_XCALLOC(unsigned char, rtf_prebuf_len + len);

    memmove (dest, rtf_prebuf, rtf_prebuf_len);

    out = rtf_prebuf_len;

    while (out < len + rtf_prebuf_len)
    {
	/* each flag byte flags 8 literals/references, 1 per bit */
	flags = (((flag_count++ % 8) == 0) ? src[in++] : flags >> 1);
	if ((flags & 1) == 1)	/* 1 == reference */
	{
	    int offset = src[in++];
	    int length = src[in++];
	    int end;
	    /* offset relative to block start */
	    offset = (offset << 4) | (length >> 4);
	    /* number of bytes to copy */
	    length = (length & 0xF) + 2;
	    /* decompression buffer is supposed to wrap around back to
	     * the beginning when the end is reached.  we save the
	     * need for this by pointing straight into the data
	     * buffer, and simulating this behaviour by modifying the
	     * pointeers appropriately */ 
	    offset = (out / 4096) * 4096 + offset;
	    if (offset >= (int)out) offset -= 4096; /* from previous block */
	    end = offset + length;
	    while (offset < end) dest[out++] = dest[offset++];
	}
	else			/* 0 == literal  */
	{
	    dest[out++] = src[in++];
	}
	
    }
    
    ret = CHECKED_XCALLOC(unsigned char, len);
    memmove (ret, dest+rtf_prebuf_len, len);
    XFREE(dest);
    
    return ret;
}

static void
get_rtf_data_from_buf (size_t len, unsigned char *data, 
		       size_t *out_len, unsigned char **out_data)
{
    size_t compr_size = 0L;
    size_t uncompr_size = 0L;
    uint32 magic;
    uint32 checksum;
    size_t idx = 0;
    
    compr_size = GETINT32(data + idx); idx += 4;
    uncompr_size = GETINT32(data + idx); idx += 4;
    magic = GETINT32(data + idx); idx += 4;
    checksum = GETINT32 (data + idx); idx += 4;

    /* sanity check */
    /* assert (compr_size + 4 == len); */

    (*out_len) = uncompr_size;

    if (magic == rtf_uncompressed_magic) /* uncompressed rtf stream */
    {
	(*out_data) = CHECKED_XCALLOC(unsigned char, (*out_len));
	memmove ((*out_data), data+4, uncompr_size);
    }
    else if (magic == rtf_compressed_magic) /* compressed rtf stream */
    {
	(*out_data)
	    = decompress_rtf_data (data+idx, uncompr_size);
    }
}

VarLenData**
get_rtf_data (MAPI_Attr *a)
{
    VarLenData** body 
	= (VarLenData**)CHECKED_XCALLOC(VarLenData*, a->num_values + 1);

    int j;
    for (j = 0; j < a->num_values; j++)
    {
	if (is_rtf_data (a->values[j].data.buf))
	{
	    body[j] = (VarLenData*)XMALLOC(VarLenData, 1);

	    get_rtf_data_from_buf (a->values[j].len,
				   a->values[j].data.buf,
				   &body[j]->len, &body[j]->data);
	}
    }
    return body;
}

