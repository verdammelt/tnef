/*
 * attr.h -- Functions for handling tnef attributes
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
#include "attr.h"
#include "tnef_types.h"
#include "tnef_names.h"
#include "date.h"
#include "options.h"
#include "util.h"
#include "write.h"
#include "debug.h"

/* Copy the date data from the attribute into a struct date */
void
copy_date_from_attr (Attr* attr, struct date* dt)
{
    assert (attr);
    assert (dt);
    assert (attr->type == szDATE);
    assert (attr->len >= 14);

    date_read (dt, attr->buf);
}

void
copy_triple_from_attr (Attr* attr, TRIPLE *t)
{
    assert (attr);
    assert (t);
    assert (attr->type == szTRIPLES);
    assert (attr->len > 8);

    t->trp.id = GETINT16 (attr->buf);
    t->trp.chbgtrp = GETINT16 (attr->buf+2);
    t->trp.cch = GETINT16 (attr->buf+4);
    t->trp.cb = GETINT16 (attr->buf+6);
    t->sender_display_name = (char*)(attr->buf+8);

    assert (attr->len > 8+t->trp.cch);

    t->sender_address = (char*)(attr->buf+8+t->trp.cch);
}

/* attr_dump
   print attr to stdout.  Assumes that the Debug flag has been set and
   already checked */
void
attr_dump (Attr* attr)
{
    char *name = get_tnef_name_str (attr->name);
    char *type = get_tnef_type_str (attr->type);
    size_t i;

    fprintf (stdout, "(%s) %s [type: %s] [len: %lu] =",
             ((attr->lvl_type == LVL_MESSAGE) ? "MESS" : "ATTA"),
             name, type, (unsigned long)attr->len);

    switch (attr->type)
    {
    case szBYTE:
        for (i=0; i < attr->len; i++)
        {
            fputc (' ', stdout);
            write_byte(stdout, (uint8)attr->buf[i]);
        }
        break;

    case szSHORT:
        if (attr->len < sizeof(uint16))
        {
            fprintf (stdout, "Not enough data for szSHORT");
            abort();
        }
        fputc (' ', stdout);
        write_uint16 (stdout, GETINT16(attr->buf));
        if (attr->len > sizeof(uint16))
        {
            fprintf (stdout, " [extra data:");
            for (i = sizeof(uint16); i < attr->len; i++)
            {
                fputc (' ', stdout);
                write_uint8 (stdout, (uint8)attr->buf[i]);
            }
            fprintf (stdout, " ]");
        }
        break;

    case szLONG:
        if (attr->len < sizeof(uint32))
        {
            fprintf (stdout, "Not enough data for szLONG");
            abort();
        }
        fputc (' ', stdout);
        write_uint32 (stdout, GETINT32(attr->buf));
        if (attr->len > sizeof(uint32))
        {
            fprintf (stdout, " [extra data:");
            for (i = sizeof(uint32); i < attr->len; i++)
            {
                fputc (' ', stdout);
                write_uint8 (stdout, (uint8)attr->buf[i]);
            }
            fprintf (stdout, " ]");
        }
        break;


    case szWORD:
        for (i=0; i < attr->len; i+=2)
        {
            fputc (' ', stdout);
            write_word(stdout, GETINT16(attr->buf+i));
        }
        break;

    case szDWORD:
        for (i=0; i < attr->len; i+=4)
        {
            fputc (' ', stdout);
            write_dword (stdout, GETINT32(attr->buf+i));
        }
        break;

    case szDATE:
    {
        struct date dt;
        copy_date_from_attr (attr, &dt);
        fputc (' ', stdout);
        write_date (stdout, &dt);
    }
    break;

    case szTEXT:
    case szSTRING:
    {
        char* buf = CHECKED_XMALLOC (char, (attr->len + 1));
        strncpy (buf, (char*)attr->buf, attr->len);
        buf[attr->len] = '\0';
        write_string (stdout, buf);
        XFREE (buf);
    }
    break;

    case szTRIPLES:
    {
        TRIPLE triple;
        copy_triple_from_attr (attr, &triple);
        write_triple (stdout, &triple);
    }
    break;

    default:
        fprintf (stdout, "<unknown type>");
        break;
    }
    fprintf (stdout, "\n");
    fflush( NULL );
}

void
attr_free (Attr* attr)
{
    if (attr)
    {
        XFREE (attr->buf);
        memset (attr, '\0', sizeof (Attr));
    }
}


/* Validate the checksum against attr.  The checksum is the sum of all the
   bytes in the attribute data modulo 65536 */
static int
check_checksum (Attr* attr, uint16 checksum)
{
    size_t i;
    uint32 sum = 0;

    for (i = 0; i < attr->len; i++)
    {
        sum = ( sum + (uint8)attr->buf[i] ) & 0xffff;
    }

    if (DEBUG_ON)
    {
        if ( sum != checksum )
        {
            /* for grins, figure out if it *ever* matched */

            int match = -1;
            uint32 mysum = 0;

            for ( i=0; i < attr->len; i++ )
            {
                mysum = ( mysum + (uint8)attr->buf[i] ) & 0xffff;

                if ( mysum == checksum ) match = i;
            }

            debug_print( "!!checksum error: length=%d sum=%04x checksum=%04x match=%d\n", attr->len, mysum, checksum, match );
        }
    }

    return (sum == checksum);
}

Attr*
attr_read (FILE* in)
{
    uint32 type_and_name;
    uint16 checksum;

    Attr *attr = CHECKED_XCALLOC (Attr, 1);

    attr->lvl_type = geti8(in);

    assert ((attr->lvl_type == LVL_MESSAGE)
            || (attr->lvl_type == LVL_ATTACHMENT));

    type_and_name = geti32(in);

    attr->type = (type_and_name >> 16);
    attr->name = ((type_and_name << 16) >> 16);
    attr->len = geti32(in);
    attr->buf = CHECKED_XCALLOC (unsigned char, attr->len);

    (void)getbuf(in, attr->buf, attr->len);

    checksum = geti16(in);
    if (!check_checksum(attr, checksum))
    {
        if ( CHECKSUM_SKIP )
        {
            fprintf (stderr,
                 "WARNING: invalid checksum, input file may be corrupted\n");
        }
        else
        {
            fprintf (stderr,
                 "ERROR: invalid checksum, input file may be corrupted\n");
            exit( 1 );
        }
    }

    if (DEBUG_ON) attr_dump (attr);

    return attr;
}
