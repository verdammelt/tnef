/*
 * tnef.c -- extract files from microsoft TNEF format
 *
 * Copyright (C)1999-2003 Mark Simpson <damned@world.std.com>
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

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif

#if STDC_HEADERS
#  include <stdarg.h>
#  include <stdlib.h>
#  include <string.h>
#  include <memory.h>
#else
extern int strcmp (const char *, const char *);
#  if !HAVE_MEMMOVE
#    define memmove(d,s,n) bcopy((s),(d),(n))
#  else
extern void* memmove (void *, const void *, size_t);
#  endif
#endif

#include "basename.h"
#include "ldiv.h"
#include "strdup.h"

#include "tnef.h"

#include "alloc.h"
#include "path.h"

/* To quiet compiler define tempnam */
extern char*
tempnam(const char*, const char*);

/* Needed to transform char buffers into little endian numbers */
#define GETINT32(p)    (uint32)((uint8)(p)[0]           \
                                +((uint8)(p)[1]<<8)     \
                                +((uint8)(p)[2]<<16)    \
                                +((uint8)(p)[3]<<24))
#define GETINT16(p)    (uint16)((uint8)(p)[0]+((uint8)(p)[1]<<8))

/* Global variables, used by all (or nearly all) functions */
static int8 g_flags = NONE;/* program options */
static char *g_directory = NULL; /* output directory */
static FILE *g_file = NULL;     /* input file */

/* Array of days of the week for translating a date */
static char* day_of_week[] = { "Sun", "Mon", "Tue",
                               "Wed", "Thu", "Fri", "Sat" }; 

/* Format Strings for dumping unsigned integers */
#if (SIZEOF_INT == 4)
#define SHORT_INT_FMT "%hu"
#define LONG_INT_FMT  "%u"
#else
#define SHORT_INT_FMT "%u"
#define LONG_INT_FMT  "%ul"
#endif /* SIZEOF_INT == 4 */

/* macros for dealing with program flags */
#define DEBUG_ON ((g_flags)&DBG_OUT)
#define VERBOSE_ON ((g_flags)&VERBOSE)
#define LIST_ONLY ((g_flags)&LIST)
#define USE_PATHS ((g_flags)&PATHS)
#define INTERACTIVE ((g_flags)&CONFIRM)
#define OVERWRITE_FILES ((g_flags)&OVERWRITE)
#define NUMBER_FILES ((g_flags)&NUMBERED)

/* Attr -- storing a structure, formated according to file specification */
typedef struct 
{
    uint8 lvl_type;
    uint16 type;
    uint16 name;
    size_t len;
    char* buf;
} Attr;

typedef struct
{
  unsigned long data1;
  unsigned short data2;
  unsigned short data3;
  unsigned char data4[8];
} MAPI_GUID;

typedef struct
{
    size_t len;
    union 
    { 
        char *buf;
        uint16 bytes2;
        uint32 bytes4;
        uint32 bytes8[2];
        MAPI_GUID guid;
    } data;
} MAPI_Value;

typedef struct
{
    uint16 type;
    uint16 name;
    size_t num_values;
    MAPI_Value* values;
} MAPI_Attr;

typedef struct
{
    char * name;
    size_t len;
    char * data;
    struct date dt;
} File;


/* ********************
   UTILITY FUNCTIONS
   ******************** */
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
        
#if HAVE_VPRINTF
        vfprintf (stdout, prompt, args);
#else
#  if HAVE_DOPRNT
        _doprnt (prompt, args, stdout);
#  endif /* HAVE_DOPRNT */
#endif /* HAVE_VPRINTF */
        fgets (buf, BUFSIZ, stdin);
        if (buf[0] == 'y' || buf[0] == 'Y') confirmed = 1;
        
        va_end (args);
        
        return confirmed;
    }
    return 1;
}

/* print message only when debug on */
static void
debug_print (const char *fmt, ...)
{
    if (DEBUG_ON)
    {
        va_list args;
        va_start (args, fmt);
#if HAVE_VPRINTF
        vfprintf (stdout, fmt, args);
#else 
#  if HAVE_DOPRNT
        _doprnt (fmt, args, stdout);
#  endif /* HAVE_DOPRNT */
#endif /* HAVE_VPRINTF */
        va_end (args);
    }
}

/* finds a filename fname.N where N >= 1 and is not the name of an existing
   filename.  Assumes that fname does not already have such an extension */ 
static char *
find_free_number (const char *fname)
{
    char *tmp = MALLOC (strlen (fname) 
                        + 1 /* '.' */
                        + 5 /* big enough for our purposes (i hope) */
                        + 1 /* NULL */);
    int counter = 1;
    struct stat buf;

    do
    {
        sprintf (tmp, "%s.%d", fname, counter++);
    }
    while (stat (tmp, &buf) == 0);
    return tmp;
}

static void
file_write (File *file)
{
    assert (file);
    if (!file) return;

    if (file->name == NULL)
    {
        char *tmp = concat_fname (g_directory, "tnef-tmp");
        debug_print ("No file name specified, using default.\n");
        file->name = find_free_number (tmp);
        FREE (tmp);
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
            struct stat buf;
            if (stat (file->name, &buf) == 0)
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
                    FREE (file->name);
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
            fprintf (stdout, "%11lu %04u-%02u-%02u %02u:%02u %s\n", 
                     (unsigned long)file->len,
                     file->dt.year, file->dt.month, file->dt.day,
                     file->dt.hour, file->dt.min,
                     base_fname);
        }
        else
        {
            fprintf (stdout, "%s\n", base_fname);
        }
    }
}

static char *
munge_fname (unsigned long attr_len, char *attr_buf)
{
    char *file = NULL;

    /* If we were not given a filename make one up */
    if (attr_len && *attr_buf == '\0')
    {
        char *tmp = concat_fname (g_directory, "tnef-tmp");
        debug_print ("No file name specified, using default.\n");
        file = find_free_number (tmp);
        FREE (tmp);
    }
    else
    {
        char *buf = NULL;

        if (USE_PATHS)
        {
            buf = strdup (attr_buf);
        }
        else
        {
            buf = strdup (basename (attr_buf));
            if (strcmp (buf, attr_buf) != 0)
            {
                debug_print ("!!Filename contains path: '%s'!!\n",
                             attr_buf);
            }
        }
        file = concat_fname (g_directory, buf);

        FREE(buf);
    }

    return file;
}


/* 
   geti32, geti16,
   Get 16 or 32 bits from the file 
*/
static uint32
geti32 ()
{
    unsigned char buf[4];

    if (fread (buf, 4, 1, g_file) != 1) 
    {
        perror ("Unexpected end of input");
        exit (1);
    }
    return (uint32)GETINT32(buf);
}
static uint16
geti16 ()
{
    unsigned char buf[2];

    if (fread (buf, 2, 1, g_file) != 1) 
    {
        perror ("Unexpected end of input");
        exit (1);
    }
    return (uint16)GETINT16(buf);
}

/* Copy the date data from the attribute into a struct date */
static void
copy_date_from_attr (Attr* attr, struct date* dt)
{
    assert (attr);
    assert (dt);
    assert (attr->type == szDATE);

    if (attr->len >= 14)
    {
        memmove (dt, attr->buf, attr->len);
        dt->year = GETINT16((unsigned char*)&dt->year);
        dt->month = GETINT16((unsigned char*)&dt->month);
        dt->day = GETINT16((unsigned char*)&dt->day);
        dt->hour = GETINT16((unsigned char*)&dt->hour);
        dt->min = GETINT16((unsigned char*)&dt->min);
        dt->sec = GETINT16((unsigned char*)&dt->sec);
        dt->dow = GETINT16((unsigned char*)&dt->dow);
    } 
    else 
    {
        char *name = get_tnef_name_str (attr->name);
        fprintf (stderr, "date attribute in %s failed sanity check\n",
                 name);
        FREE(name);
        memset (dt, 0, sizeof(*dt));
    }
}

/* dump_attr
   print attr to stderr.  Assumes that the Debug flag has been set and
   already checked */
static void 
dump_attr (Attr* attr)
{
    char *name = get_tnef_name_str (attr->name);
    char *type = get_tnef_type_str (attr->type);
    struct date dt;
    uint16 s;
    uint32 l;
    size_t i;

    fprintf (stdout, "%s [type: %s] =", name, type);

    FREE(name);
    FREE(type);

    switch (attr->type)
    {
    case szBYTE:
        for (i=0; i < attr->len; i++)
        {
            if (i< 10) fprintf (stdout, " %02x", (uint8)attr->buf[i]);
            else if (i==10) fprintf (stdout, "...");
        }
        break;

    case szSHORT:
        if (attr->len < sizeof(uint16))
        {
            fprintf (stdout, "Not enough data for szSHORT");
            abort();
        }
		s = GETINT16(attr->buf);
        fprintf (stdout, " " SHORT_INT_FMT, s);
        if (attr->len > sizeof(uint16))
        {
            fprintf (stdout, " [extra data:");
            for (i = sizeof(uint16); i < attr->len; i++)
            {
                fprintf (stdout, " %02x", (uint8)attr->buf[i]);
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
		l = GETINT32(attr->buf);
        fprintf (stdout, " " LONG_INT_FMT, l);
        if (attr->len > sizeof(uint32))
        {
            fprintf (stdout, " [extra data:");
            for (i = sizeof(uint32); i < attr->len; i++)
            {
                fprintf (stdout, " %02x", (uint8)attr->buf[i]);
            }
            fprintf (stdout, " ]");
        }
        break;
        

    case szWORD:
        for (i=0; i < attr->len; i+=2) 
        {
            if (i < 6) fprintf (stdout, " %04x", GETINT16(attr->buf));
            else if (i==6) fprintf (stdout, "...");
        }
        break;

    case szDWORD:
        for (i=0; i < attr->len; i+=4) 
        {
            if (i < 4) fprintf (stdout, " %08x", GETINT32(attr->buf));
            else if (i==4) fprintf (stdout, "...");
        }
        break;

    case szDATE:
        copy_date_from_attr (attr, &dt);
        fprintf (stdout, " %s %04d/%02d/%02d %02d:%02d:%02d",
                 day_of_week[dt.dow],
                 dt.year, dt.month, dt.day,
                 dt.hour, dt.min, dt.sec);
        break;

    case szTEXT:
    case szSTRING:
        {
            char* buf = MALLOC ((attr->len + 1) * sizeof (char));
            strncpy (buf, (char*)attr->buf, attr->len);
            buf[attr->len] = '\0';
            fprintf (stdout, "'%s'", buf);
            FREE (buf);
        }
        break;

    default:
        fprintf (stdout, "<unknown type> len=%lu", 
                 (unsigned long)attr->len);
    }
    fprintf (stdout, "\n");
}


/* dumps info about MAPI attributes... useful for debugging */
static void
dump_mapi_attr (MAPI_Attr* attr)
{
    char *name = get_mapi_name_str (attr->name);
    char *type = get_mapi_type_str (attr->type);
    size_t i;

    fprintf (stdout, "%s [type: %s] [num_values = %lu] = \n", 
             name, type, (unsigned long)attr->num_values);

    FREE(name);
    FREE(type);

    for (i = 0; i < attr->num_values; i++)
    {
        fprintf (stdout, "\tvalue #%lu [len: %lu] = ", 
                 (unsigned long)i, 
                 (unsigned long)attr->values[i].len);

        switch (attr->type)
        {
        case szMAPI_NULL:
            fprintf (stdout, "NULL");
            break;
            
        case szMAPI_SHORT:
            fprintf (stdout, SHORT_INT_FMT, 
                     attr->values[i].data.bytes2);
            break;
             
        case szMAPI_INT:
            fprintf (stdout, "%d", 
                     attr->values[i].data.bytes4);
            break;

        case szMAPI_FLOAT:
        case szMAPI_DOUBLE:
            fprintf (stdout, "%f",
                     (float)attr->values[i].data.bytes4);
            break;

        case szMAPI_BOOLEAN:
            fprintf (stdout, "%s",
                     ((attr->values[i].data.bytes2 == 0) ? "false" : "true"));
            break;

        case szMAPI_STRING:
        case szMAPI_UNICODE_STRING:
            fprintf (stdout, "%s",
                     attr->values[i].data.buf);
            break;

        case szMAPI_SYSTIME:
        case szMAPI_CURRENCY:
        case szMAPI_INT8BYTE:
        case szMAPI_APPTIME:
            fprintf (stdout, "%x %x",
                     (int)attr->values[i].data.bytes8[0],
                     (int)attr->values[i].data.bytes8[1]);
            break;

        case szMAPI_ERROR:
            fprintf (stdout, "%x",
                     attr->values[i].data.bytes4);
            break;

        case szMAPI_CLSID:
            {
                int j;
                fprintf (stdout, "{%04lx %02x %02x ",
                         attr->values[i].data.guid.data1,
                         attr->values[i].data.guid.data2,
                         attr->values[i].data.guid.data3);
                fprintf (stdout, "{");
                for (j = 0; i < 8; i++)
                {
                    fprintf (stdout, "%x",
                             attr->values[i].data.guid.data4[j]);
                }
                fprintf (stdout, "}");
            }
            break;

        case szMAPI_OBJECT:
        case szMAPI_BINARY:
            {
                int x;
                for (x = 0; x < attr->values[i].len; x++)
                {
                    if (x < 10) 
                    {
                        fprintf (stdout, "%02x ", 
                                 (uint8)attr->values[i].data.buf[x]);
                    }
                    else if (x == 10) 
                    {
                        fprintf (stdout, "...");
                        break;
                    }
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


/* Validate the checksum against attr.  The checksum is the sum of all the
   bytes in the attribute data modulo 65536 */
static int
check_checksum (Attr* attr, uint16 checksum)
{
  size_t i;
  uint16 sum = 0;

    for (i = 0; i < attr->len; i++)
    {
        sum += (uint8)attr->buf[i];
    }
    sum %= 65536;
    return (sum == checksum);
}

/* Reads and decodes a object from the stream */

static Attr*
decode_object (void) 
{  
    uint32 type_and_name;
    char buf[2];
    size_t bytes_read = 0;
    uint16 checksum = 0;

    /* First we must get the lvl type */
    if (fread (buf, 1, 1, g_file) == 0) 
    { 
        if (!feof(g_file))
        {
            perror ("Unexpected end of input!");
            exit (1);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        Attr *attr = (Attr*)CALLOC (1, sizeof(Attr));
        
        attr->lvl_type = (uint8)buf[0];
        assert ((attr->lvl_type == LVL_MESSAGE) 
                || (attr->lvl_type == LVL_ATTACHMENT));
        
        type_and_name = geti32();
 
        attr->type = (type_and_name >> 16);
        attr->name = ((type_and_name << 16) >> 16);
        attr->len = geti32();
        attr->buf = MALLOC (attr->len);
        
        bytes_read = fread (attr->buf, 1, attr->len, g_file);
        if (bytes_read != attr->len)
        {
            perror ("Unexpected end of input!");
            exit (1);
        }
        
        checksum = geti16();
        if (!check_checksum(attr, checksum))
        {
            fprintf (stderr, 
                     "Invalid checksum, input file may be corrupted\n");
            exit (1);
        }
        
        if (DEBUG_ON) dump_attr (attr);

        return attr;
    }
}

static void
file_free (File *file)
{
    if (file)
    {
        FREE (file->name);
        FREE (file->data);
        memset (file, '\0', sizeof (File));
    }
}

static void
attr_free (Attr* attr)
{
    if (attr)
    {
        FREE (attr->buf);
        memset (attr, '\0', sizeof (Attr));
    }
}

static MAPI_Value*
alloc_mapi_values (MAPI_Attr* a)
{
    if (a && a->num_values)
    {
        a->values = (MAPI_Value*)CALLOC (a->num_values,
                                         sizeof (MAPI_Value));
        return a->values;
    }
    return NULL;
}

/* parses out the MAPI attibutes hidden in the character buffer */
static MAPI_Attr**
decode_mapi (size_t len, char *buf)
{
    size_t idx = 0;
    int i;
    uint32 num_properties = GETINT32(buf+idx);
    MAPI_Attr** attrs= 
        (MAPI_Attr**)MALLOC ((num_properties + 1) * 
                             sizeof (MAPI_Attr*));

    idx += 4;

    if (!attrs) return NULL;
    for (i = 0; i < num_properties; i++)
    {
        MAPI_Attr* a = attrs[i] = 
            (MAPI_Attr*)CALLOC(1, sizeof (MAPI_Attr));
        MAPI_Value* v = NULL;
        
        a->type = GETINT16(buf+idx); idx += 2;
        a->name = GETINT16(buf+idx); idx += 2;

        switch (a->type)
        {
        case szMAPI_SHORT:        /* 2 bytes */
            a->num_values = 1;
            v = alloc_mapi_values (a);
            v->len = 2;
            v->data.bytes2 = GETINT16(buf+idx);
            idx += v->len;
            break;

        case szMAPI_INT:
        case szMAPI_FLOAT:      /* 4 bytes */
        case szMAPI_ERROR:
        case szMAPI_BOOLEAN:
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
          v->len = sizeof (MAPI_GUID);
          memmove (&v->data, buf+idx, v->len);
          idx += v->len;
          break;

        case szMAPI_STRING:
        case szMAPI_UNICODE_STRING:
        case szMAPI_OBJECT:
        case szMAPI_BINARY:       /* variable length */
            {
                int val_idx = 0;
                a->num_values = GETINT32(buf+idx); idx += 4;
                v = alloc_mapi_values (a);
                for (val_idx = 0; val_idx < a->num_values; val_idx++)
                {
                    v[val_idx].len = GETINT32(buf+idx); idx += 4;
                    /* must pad length to 4 byte boundary */
                    {
                        ldiv_t d = ldiv (v[val_idx].len, 4L);
                        if (d.rem != 0)
                        {
                            v[val_idx].len += (4 - d.rem);
                        }
                    }
                    v[val_idx].data.buf = MALLOC(v[val_idx].len * 
                                                 sizeof (char));
                    memmove (v[val_idx].data.buf, 
                             buf+idx, 
                             v[val_idx].len);
                    idx += v[val_idx].len;
                }
            }
            break;
        }
        if (DEBUG_ON) dump_mapi_attr (attrs[i]);

    }
    attrs[i] = NULL;


    return attrs;
}

static void
mapi_attr_free (MAPI_Attr* attr)
{
    if (attr)
    {
        int i;
        for (i = 0; i < attr->num_values; i++)
        {
            if ((attr->type == szMAPI_STRING)
                || (attr->type == szMAPI_UNICODE_STRING)
                || (attr->type == szMAPI_BINARY))
            {
                FREE (attr->values[i].data.buf);
            }
        }
        FREE (attr->values);
        memset (attr, '\0', sizeof (MAPI_Attr));
    }
}

static void
mapi_attr_free_list (MAPI_Attr** attrs)
{
    int i;
    for (i = 0; attrs && attrs[i]; i++)
    {
        mapi_attr_free (attrs[i]);
        FREE (attrs[i]);
    }
}


static void
file_add_mapi_attrs (File* file, MAPI_Attr** attrs)
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
                if (file->name) FREE(file->name);
                file->name = munge_fname (a->values[0].len,
                                          a->values[0].data.buf);
                break;
                
              case MAPI_ATTACH_DATA_OBJ:
                file->len = a->values[0].len;
                if (file->data) FREE (file->data);
                file->data = MALLOC (file->len * sizeof(char));
                memmove (file->data, a->values[0].data.buf, file->len);
                break;
              }
          }
    }
}

static void
file_add_attr (File* file, Attr* attr)
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
            MAPI_Attr **mapi_attrs = decode_mapi (attr->len, attr->buf);
            if (mapi_attrs)
            {
                file_add_mapi_attrs (file, mapi_attrs);
                mapi_attr_free_list (mapi_attrs);
                FREE (mapi_attrs);
            }
        }
        break;

    case attATTACHTITLE:
        file->name = munge_fname (attr->len, attr->buf);
        break;
        
    case attATTACHDATA:
        file->len = attr->len;
        file->data = MALLOC(attr->len * sizeof (char));
        memmove (file->data, attr->buf, attr->len);
        break;
    }
}


/* The entry point into this module.  This parses an entire TNEF file. */
int
parse_file (FILE* input_file, char* directory, int flags)
{
    uint32 d;
    uint16 key;
    File *file = NULL;
    Attr *attr = NULL;

    /* store the program options in our file global variables */
    g_file = input_file;
    g_directory = directory;
    g_flags = flags;

    /* check that this is in fact a TNEF file */
    d = geti32();
    if (d != TNEF_SIGNATURE) 
    {
        fprintf (stdout, "Seems not to be a TNEF file\n");
        return 1;
    }

    /* Get the key */
    key = geti16();
    debug_print ("TNEF Key: %hx\n", key);

    /* The rest of the file is a series of 'messages' and 'attachments' */
    for (attr = decode_object(); 
         attr && !feof (g_file); 
         attr = decode_object()) 
    {
        /* This signals the beginning of a file */
        if (attr->name == attATTACHRENDDATA)
        {
            if (file)
            {
                file_write (file);
                file_free (file);
            }
            else
            {
                file = (File*)CALLOC (1, sizeof (File));
            }
        }
        /* Add the data to our lists. */
        switch (attr->lvl_type)
        {
        case LVL_MESSAGE:
            /* We currently have no use for these attributes */
            break;
        case LVL_ATTACHMENT:
            file_add_attr (file, attr);
            break;
        default:
            fprintf (stderr, "Invalid lvl type on attribute: %d\n",
                     attr->lvl_type);
            return 1;
            break;
        }
        attr_free (attr);
        FREE (attr);
    }
    if (file)
    {
        file_write (file);
        file_free (file);
        FREE (file);
    }
    return 0;
}
