/*
 * file.c -- functions for dealing with file output
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
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

#include "alloc.h"
#include "date.h"
#include "debug.h"
#include "file.h"
#include "mapi_attr.h"
#include "options.h"
#include "path.h"

/* ask user for confirmation of the action */
static int
confirm_action (const char *prompt, ...)
{
    if (INTERACTIVE)
    {
	int confirmed = 0;
	char buf[BUFSIZ + 1];
	va_list args;
	va_start (args, prompt);

	VPRINTF(stdout, prompt, args);
	fgets (buf, BUFSIZ, stdin);
	if (buf[0] == 'y' || buf[0] == 'Y') confirmed = 1;

	va_end (args);

	return confirmed;
    }
    return 1;
}


void
file_write (File *file, const char* directory)
{
    assert (file);
    if (!file) return;

    if (file->name == NULL)
    {
	char *tmp = concat_fname (directory, "tnef-tmp");
	debug_print ("No file name specified, using default.\n");
	file->name = find_free_number (tmp);
	XFREE (tmp);
	debug_print ("default filename = %s", file->name);
    }

    debug_print ("%sWRITING %s\n",
		 ((LIST_ONLY==0)?"":"NOT "), file->name);

    if (!LIST_ONLY)
    {
	FILE *fp = NULL;
	char *base_fname = basename (file->name);

	if (!confirm_action ("extract %s?", base_fname)) return;
	if (!OVERWRITE_FILES)
	{
	    if (file_exists (file->name))
	    {
		if (!NUMBER_FILES)
		{
		    fprintf (stderr,
			     "tnef: %s: Could not create file: File exists\n",
			     file->name);
		    return;
		}
		else
		{
		    char *tmp = find_free_number (file->name);
		    debug_print ("Renaming %s to %s\n", file->name, tmp);
		    XFREE (file->name);
		    file->name = tmp;
		}
	    }
	}

	fp = fopen (file->name, "wb");
	if (fp == NULL)
	{
	    perror (file->name);
	    exit (1);
	}
	if (fwrite (file->data, 1, file->len, fp) != file->len)
	{
	    perror (file->name);
	    exit (1);
	}
	fclose (fp);
    }

    if (LIST_ONLY || VERBOSE_ON)
    {
	char *base_fname = basename (file->name);

	if (LIST_ONLY && VERBOSE_ON)
	{
	    /* FIXME: print out date and stuff */
	    const char *date_str = date_to_str(&file->dt);
	    fprintf (stdout, "%11lu %s %s\n", 
		     (unsigned long)file->len,
		     date_str+4, /* skip the day of week */
		     base_fname);
	}
	else
	{
	    fprintf (stdout, "%s\n", base_fname);
	}
    }
}

static void
file_add_mapi_attrs (File* file, 
		     const char *directory, 
		     MAPI_Attr** attrs)
{
    int i;
    for (i = 0; attrs[i]; i++)
    {
	MAPI_Attr* a = attrs[i];

	if (a->num_values)
	{

	    switch (a->name)
	    {
	    case MAPI_ATTACH_LONG_FILENAME:
		if (file->name) XFREE(file->name);
		file->name = munge_fname (directory, (char*)a->values[0].data.buf);
		break;

	    case MAPI_ATTACH_DATA_OBJ:
		file->len = a->values[0].len;
		if (file->data) XFREE (file->data);
		file->data = CHECKED_XMALLOC (unsigned char, file->len);
		memmove (file->data, a->values[0].data.buf, file->len);
		break;

	    default:
		break;
	    }
	}
    }
}

void
file_add_attr (File* file, const char *directory, Attr* attr)
{
    assert (file && attr);
    if (!(file && attr)) return;

    /* we only care about some things... we will skip most attributes */
    switch (attr->name)
    {
    case attATTACHMODIFYDATE:
	copy_date_from_attr (attr, &file->dt);
	break;

    case attATTACHMENT:
    {
	MAPI_Attr **mapi_attrs = mapi_attr_read (attr->len, attr->buf);
	if (mapi_attrs)
	{
	    file_add_mapi_attrs (file, directory, mapi_attrs);
	    mapi_attr_free_list (mapi_attrs);
	    XFREE (mapi_attrs);
	}
    }
    break;

    case attATTACHTITLE:
	file->name = munge_fname (directory, (char*)attr->buf);
	break;

    case attATTACHDATA:
	file->len = attr->len;
	file->data = CHECKED_XMALLOC(unsigned char, attr->len);
	memmove (file->data, attr->buf, attr->len);
	break;

    default:
	break;
    }
}

void
file_free (File *file)
{
    if (file)
    {
	XFREE (file->name);
	XFREE (file->data);
	memset (file, '\0', sizeof (File));
    }
}


