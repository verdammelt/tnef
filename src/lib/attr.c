
#include "tnef.h"
#include "alloc.h"
#include "io.h"

/* Format Strings */
#define UINT8_FMT "%u"
#define INT8_FMT "%d"
#define UINT16_FMT "%hu"
#define INT16_FMT "%hd"
#if (SIZEOF_INT == 2)
#define UINT32_FMT  "%lu"
#define INT32_FMT  "%ld"
#else
#define UINT32_FMT  "%u"
#define INT32_FMT  "%d"
#endif /* (SIZEOF_INT == 2) */


static void
copy_date_from_buf (unsigned char* buf, tnef_attr_date* dt)
{
    size_t i = 0;
    if (sizeof(buf)/sizeof(unsigned char) == 14)
    {
	dt->year = GETINT16 (buf + i); i += sizeof (uint16_t);
	dt->month = GETINT16 (buf + i); i += sizeof (uint16_t);
	dt->day = GETINT16 (buf + i); i += sizeof (uint16_t);
	dt->hour = GETINT16 (buf + i); i += sizeof (uint16_t);
	dt->min = GETINT16 (buf + i); i += sizeof (uint16_t);
	dt->sec = GETINT16 (buf + i); i += sizeof (uint16_t);
	dt->dow = GETINT16 (buf + i); i += sizeof (uint16_t);
    }
    else tnef_error = E_BADSIZE;
}

static void
copy_triple_from_buf (unsigned char* buf, tnef_attr_triple *t)
{
    t->trp.id = GETINT16 (buf);
    t->trp.chbgtrp = GETINT16 (buf+2);
    t->trp.cch = GETINT16 (buf+4);
    t->trp.cb = GETINT16 (buf+6);
    t->sender_display_name = (char*)(buf+8);
    t->sender_address = (char*)(buf+8+t->trp.cch);
}

static int tnef_checksum_attr (tnef_attr* attr)
{
    size_t i;
    uint16_t sum = 0;

    for (i = 0; i < attr->size; i++)
    {
	sum += (uint8_t)attr->raw_data[i];
    }
    sum %= 65536;
    return (tnef_error = ((sum == attr->checksum) ? E_OK : E_CHECKSUM));
}

tnef_attr* tnef_read_attr (TNEF* tnef)
{
    tnef_attr* attr = XCALLOC(tnef_attr, 1);
    
    attr->level = (tnef_attr_level)_read_uint8(tnef->fileptr);
    if (tnef_error == E_OK)
    {
	uint32_t type_and_name = _read_uint32(tnef->fileptr);
    
	attr->type = (type_and_name >> 16);
	attr->name = ((type_and_name << 16) >> 16);
	attr->size = _read_uint32(tnef->fileptr);
	if (tnef_error == E_OK)
	{
	    attr->raw_data = _read_buf(tnef->fileptr, attr->size);
	    if (tnef_error == E_OK)
	    {
		attr->checksum = _read_uint16(tnef->fileptr);
		if (tnef_error == E_OK)
		{
		    tnef_checksum_attr (attr);
		}
	    }
	}
    }

    if (tnef_error != E_OK)
    {
	tnef_free_attr(attr);
	attr = NULL;
    }

    return attr;
}

void tnef_free_attr (tnef_attr* attr)
{
    if (attr->raw_data == attr->data.buf) attr->data.buf = NULL;
    XFREE(attr->raw_data);

    switch (attr->type)
    {
    case szWORD:
	XFREE (attr->data.words);
	break;

    case szDWORD:
	XFREE (attr->data.dwords);
	break; 

    case szTEXT:
    case szSTRING:
	XFREE (attr->data.buf);
	break;
    }
    
    XFREE (attr);
}

void tnef_parse_attr (tnef_attr* attr)
{
    int i, j;

    switch (attr->type)
    {
    case szBYTE:
	attr->data.buf = attr->raw_data;
	break;

    case szSHORT:
	if (attr->size == 2)
	{
	    attr->data.int16 = GETINT16(attr->raw_data);
	}
	else tnef_error = E_BADSIZE;
	break;

    case szLONG:
	if (attr->size == 4)
	{
	    attr->data.int32 = GETINT32(attr->raw_data);
	}
	else tnef_error = E_BADSIZE;
	break;


    case szWORD:
	attr->data.words = XMALLOC(uint16_t, attr->size / 2);
	for (i=0, j=0; j < attr->size; i++, j+=2)
	{
	    attr->data.words[i] = GETINT16(attr->raw_data+j);
	}
	break;

    case szDWORD:
	attr->data.dwords = XMALLOC(uint32_t, attr->size / 4);
	for (i=0, j=0; j < attr->size; i++, j+=4)
	{
	    attr->data.dwords[i] = GETINT32(attr->raw_data+j);
	}
	break;

    case szDATE:
	copy_date_from_buf (attr->raw_data, &attr->data.date);
	break;

    case szTEXT:
    case szSTRING:
	attr->data.buf = XMALLOC (char, (attr->size + 1));
	strncpy (attr->data.buf, (char*)attr->raw_data, attr->size);
	attr->data.buf[attr->size] = '\0';
	break;

    case szTRIPLES:
	copy_triple_from_buf (attr->raw_data, &attr->data.triple);
	break;
    }
}

void tnef_dump_attr (tnef_attr* attr, FILE* out)
{
    int i;
    fprintf (out, "ATTR=[level:%#x type:%#04x<'%s'> name:%#04x<'%s'> size:%d checksum: %#x data:<",
	     attr->level, 
	     attr->type, get_attr_type_str (attr->type),
	     attr->name,get_attr_name_str (attr->name),
	     attr->size, attr->checksum);
    switch (attr->type)
    {
    case szTRIPLES:
	fprintf (out,
 	     "<id=" UINT16_FMT 
	     ",chgtrp=" UINT16_FMT 
	     ",cch=" UINT16_FMT 
	     ",cb=" UINT16_FMT "> "
	     "sender_display_name='%s', "
	     "sender_address='%s'",
	     attr->data.triple.trp.id,
	     attr->data.triple.trp.chbgtrp,
	     attr->data.triple.trp.cch,
	     attr->data.triple.trp.cb,
	     attr->data.triple.sender_display_name,
	     attr->data.triple.sender_address);
	break;

    case szSTRING:
    case szTEXT:
	fprintf (out, "'%s'", attr->data.buf);
	break;

    case szDATE:
	fprintf (out, "%04d-%02d-%02dT%02d:%02d:%02d",
		 attr->data.date.year,
		 attr->data.date.month,
		 attr->data.date.day,
		 attr->data.date.hour,
		 attr->data.date.min,
		 attr->data.date.sec);
	break;

    case szSHORT:
	fprintf (out, UINT16_FMT, attr->data.int16);
	break;

    case szLONG:
	fprintf (out, UINT32_FMT, attr->data.int32);
	break;

    case szBYTE:
	fprintf (out, "%02x", (uint8_t)attr->data.buf[0]);
	for (i = 1; i < attr->size; i++)
	{
	    fprintf (out, " %02x", (uint8_t)attr->data.buf[i]);
	}
	break;

    case szWORD:
	fprintf (out, "%04x", (uint16_t)attr->data.words[0]);
	for (i = 1; i < attr->size / sizeof(uint16_t); i++)
	{
	    fprintf (out, " %04x", (uint16_t)attr->data.words[i]);
	}
	break;

    case szDWORD:
	fprintf (out, "%08x", (uint32_t)attr->data.dwords[0]);
	for (i = 1; i < attr->size / sizeof(uint32_t); i++)
	{
	    fprintf (out, " %08x", (uint32_t)attr->data.dwords[i]);
	}
	break;

    default:
	fprintf (out, "UNKNOWN: %#x", attr->raw_data[0]);
	for (i = 1; i < attr->size; i++) 
	{
	    fprintf (out, " %#x", attr->raw_data[i]);
	}
    }
    fprintf (out, ">");
}
