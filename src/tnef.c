/*
 * tnef.c -- extract files from microsoft TNEF format
 *
 * Copyright (C)1999-2005 Mark Simpson <damned@theworld.com>
 * Copyright (C)1998 Thomas Boll  <tb@boll.ch>	[ORIGINAL AUTHOR]
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
 *       scans tnef file and extracts all attachments
 *       attachments are written to their original file-names if possible
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#include "tnef.h"

#include "alloc.h"
#include "attr.h"
#include "debug.h"
#include "file.h"
#include "mapi_attr.h"
#include "options.h"
#include "path.h"
#include "rtf.h"
#include "util.h"

/* Reads and decodes a object from the stream */

static Attr*
read_object (FILE *in)
{
    Attr *attr = NULL;

    /* peek to see if there is more to read from this stream */
    int tmp_char = fgetc(in);
    if (tmp_char == -1) return NULL;
    ungetc(tmp_char, in);

    attr = attr_read (in);

    return attr;
}


/* The entry point into this module.  This parses an entire TNEF file. */
int
parse_file (FILE* input_file, char* directory, char *rtf_file, int flags)
{
    uint32 d;
    uint16 key;
    File *file = NULL;
    Attr *attr = NULL;

    /* store the program options in our file global variables */
    /* g_file = input_file; */
    /* g_directory = directory; */
    g_flags = flags;

    /* check that this is in fact a TNEF file */
    d = geti32(input_file);
    if (d != TNEF_SIGNATURE)
    {
	fprintf (stdout, "Seems not to be a TNEF file\n");
	return 1;
    }

    /* Get the key */
    key = geti16(input_file);
    debug_print ("TNEF Key: %hx\n", key);

    /* The rest of the file is a series of 'messages' and 'attachments' */
    for (attr = read_object(input_file);
	 attr && !feof (input_file);
	 attr = read_object(input_file))
    {
	/* This signals the beginning of a file */
	if (attr->name == attATTACHRENDDATA)
	{
	    if (file)
	    {
		file_write (file, directory);
		file_free (file);
	    }
	    else
	    {
		file = CHECKED_XCALLOC (File, 1);
	    }
	}
	/* Add the data to our lists. */
	switch (attr->lvl_type)
	{
	case LVL_MESSAGE:
	    /* We currently have no use for these attributes */
 	    if (attr->name == attMAPIPROPS) 
 	    { 
 		MAPI_Attr **mapi_attrs 
		    = mapi_attr_read (attr->len, attr->buf); 
 		if (mapi_attrs) 
 		{ 
		    /* save the rtf if wanted */
		    if (flags & SAVERTF)
		    {
			if (!rtf_file) 
			{
			    char *tmp = concat_fname (directory, 
						      "tnef-rtf-tmp");
			    rtf_file = find_free_number (tmp);
			    XFREE (tmp);
			}
			save_rtf_data (rtf_file, directory, mapi_attrs);
		    }

		    /* cannot save attributes to file, since they are 
		       not attachment attributes */
		    /* file_add_mapi_attrs (file, mapi_attrs); */
 		    mapi_attr_free_list (mapi_attrs); 
 		    XFREE (mapi_attrs); 
 		} 
 	    } 
	    break;
	case LVL_ATTACHMENT:
	    file_add_attr (file, directory, attr);
	    break;
	default:
	    fprintf (stderr, "Invalid lvl type on attribute: %d\n",
		     attr->lvl_type);
	    return 1;
	    break;
	}
	attr_free (attr);
	XFREE (attr);
    }
    if (file)
    {
	file_write (file, directory);
	file_free (file);
	XFREE (file);
    }
    return 0;
}
