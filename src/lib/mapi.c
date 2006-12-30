
#include <stdio.h>

#include "tnef.h"
#include "alloc.h"

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

#define MULTI_VALUE_FLAG 0x1000
#define GUID_EXISTS_FLAG 0x8000

static const uint32_t rtf_uncompressed_magic = 0x414c454d;
static const uint32_t rtf_compressed_magic = 0x75465a4c;
static const char* rtf_prebuf = "{\\rtf1\\ansi\\mac\\deff0\\deftab720{\\fonttbl;}{\\f0\\fnil \\froman \\fswiss \\fmodern \\fscript \\fdecor MS Sans SerifSymbolArialTimes New RomanCourier{\\colortbl\\red0\\green0\\blue0\n\r\\par \\pard\\plain\\f0\\fs20\\b\\i\\u\\tab\\tx";

char*
unicode_to_utf8 (size_t len, unsigned char* buf)
{
    int i = 0;
    int j = 0;
    char *utf8 = malloc (3 * len/2 + 1); /* won't get any longer than this */

    for (i = 0; i < len - 1; i += 2)
    {
	uint32_t c = GETINT16(buf + i);
	if (c <= 0x007f)
	{
	    utf8[j++] = 0x00 | ((c & 0x007f) >> 0);
	}
	else if (c < 0x07ff)
	{
	    utf8[j++] = 0xc0 | ((c & 0x07c0) >> 6);
	    utf8[j++] = 0x80 | ((c & 0x003f) >> 0);
	}
	else
	{
	    utf8[j++] = 0xe0 | ((c & 0xf000) >> 12);
	    utf8[j++] = 0x80 | ((c & 0x0fc0) >> 6);
	    utf8[j++] = 0x80 | ((c & 0x003f) >> 0);
	}
    }
    
    utf8[j] = '\0';
    
    return utf8;
}

/* return the length padded to a 4 byte boundary */
static size_t
pad_to_4byte (size_t length)
{
    return (length+3) & ~3;
}

int tnef_has_mapi_attr (tnef_attr *attr)
{
    return (attr->name == attMAPIPROPS || attr->name == attATTACHMENT);
}

void copy_guid_from_buf (tnef_mapi_guid* guid, unsigned char* buf)
{
    int i;
    int idx = 0;

    guid->data1 = GETINT32(buf + idx); idx += sizeof (uint32_t);
    guid->data2 = GETINT16(buf + idx); idx += sizeof (uint16_t);
    guid->data3 = GETINT16(buf + idx); idx += sizeof (uint16_t);
    for (i = 0; i < 8; i++, idx += sizeof (uint8_t))
	guid->data4[i] = (uint8_t)(buf[idx]);
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
    unsigned char *dest = XCALLOC(unsigned char, rtf_prebuf_len + len);

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
	     * pointers appropriately */ 
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
    
    ret = XCALLOC(unsigned char, len);
    memmove (ret, dest+rtf_prebuf_len, len);
    XFREE(dest);
    
    return ret;
}

static void
parse_rtf_data (tnef_mapi_attr_value* value)
{
    unsigned char* data = value->data.buf;
    size_t compr_size = 0L;
    size_t uncompr_size = 0L;
    uint32_t magic;
    uint32_t checksum;
    size_t idx = 0;
    
    compr_size = GETINT32(data + idx); idx += 4;
    uncompr_size = GETINT32(data + idx); idx += 4;
    magic = GETINT32(data + idx); idx += 4;
    checksum = GETINT32 (data + idx); idx += 4;

    value->size = uncompr_size;

    if (magic == rtf_uncompressed_magic) /* uncompressed rtf stream */
    {
	value->data.buf = XCALLOC(unsigned char, value->size);
	memmove (value->data.buf, data+4, uncompr_size);
    }
    else if (magic == rtf_compressed_magic) /* compressed rtf stream */
    {
	value->data.buf = decompress_rtf_data (data+idx, uncompr_size);
    }
}

/* replaces data in value with parsed data */
void tnef_mapi_parse_rtf_data (tnef_mapi_attr *attr)
{
    if (attr->name == MAPI_RTF_COMPRESSED)
    {
	int i;
	for (i = 0; i < attr->num_values; i++)
	{
	    parse_rtf_data (&attr->values[i]);
	}
    }
}

void tnef_parse_mapi_attr (tnef_attr *attr)
{
    unsigned char *buf = attr->data.buf;
    size_t idx = 0;

    attr->data.mapi_attrs.num_attrs = GETINT32(buf+idx); idx += 4;
    attr->data.mapi_attrs.array = XMALLOC(tnef_mapi_attr*, (attr->data.mapi_attrs.num_attrs + 1));
    if (tnef_error == E_OK)
    {
	tnef_mapi_attr** attrs = attr->data.mapi_attrs.array;
	int i;

	for (i = 0; i < attr->data.mapi_attrs.num_attrs; i++)
	{
	    tnef_mapi_attr* a = attrs[i] = XCALLOC(tnef_mapi_attr, 1);

	    a->type = GETINT16(buf+idx); idx += 2;
	    if (tnef_error == E_OK)
	    {
		a->name = GETINT16(buf+idx); idx += 2;
		if (tnef_error == E_OK)
		{
		    /* Multi-valued attributes have their types modified by the MULTI_VALUE_FLAG value */
		    if (a->type & MULTI_VALUE_FLAG) a->type -= MULTI_VALUE_FLAG;
		    
		    /* handle the special case of the GUID prefixed properties */
		    if (a->name >= GUID_EXISTS_FLAG)
		    {
			copy_guid_from_buf (&a->guid, buf+idx);
			idx += sizeof (tnef_mapi_guid);

			a->num_names = GETINT32(buf+idx); idx+=4;
			if (a->num_names > 0)
			{
			    int n;

			    a->names = XMALLOC(unsigned char*, a->num_names);
			    for (n = 0; n < a->num_names; n++)
			    {
				int x;
				size_t len = GETINT32(buf+idx); idx += 4;

				/* read the data into a buffer */
				a->names[n] = XMALLOC(unsigned char, len);
				for (x = 0; x < (len >> 1); x++)
				    a->names[n][x] = (buf+idx)[x*2];

				/* But what are we going to do with it? */
		    
				idx += pad_to_4byte(len);
			    }
			}
			else
			{
			    /* get the 'real' name */
			    a->name = GETINT32(buf+idx); idx += 4;
			}
		    }

		    switch (a->type)
		    {
		    case szMAPI_NULL:
			a->num_values = 0;
			a->values = NULL;
			break;

		    case szMAPI_SHORT:        /* 2 bytes */
			a->num_values = 1;
			a->values = XMALLOC(tnef_mapi_attr_value, 1);
			a->values[0].size = 2;
			a->values[0].data.int16 = GETINT16(buf+idx);
			idx += 4; /* advance by 4! */
			break;

		    case szMAPI_INT:
		    case szMAPI_FLOAT:      /* 4 bytes */
		    case szMAPI_ERROR:
		    case szMAPI_BOOLEAN:	/* spec says 2 bytes but reality is 4! */
			a->num_values = 1;
			a->values = XMALLOC(tnef_mapi_attr_value, 1);
			a->values[0].size = 4;
			a->values[0].data.int32 = GETINT32(buf+idx);
			idx += a->values[0].size;
			break;

		    case szMAPI_DOUBLE:
		    case szMAPI_APPTIME:
		    case szMAPI_CURRENCY:
		    case szMAPI_INT8BYTE:
		    case szMAPI_SYSTIME:         /* 8 bytes */
			a->num_values = 1;
			a->values = XMALLOC(tnef_mapi_attr_value, 1);
			a->values[0].size = 8;
			a->values[0].data.int64[0] = GETINT32(buf+idx);
			a->values[0].data.int64[1] = GETINT32(buf+idx+4);
			idx += a->values[0].size;
			break;

		    case szMAPI_CLSID:
			a->num_values = 1;
			a->values = XMALLOC(tnef_mapi_attr_value, 1);
			a->values[0].size = sizeof (tnef_mapi_guid);
			copy_guid_from_buf (&a->values[0].data.guid, buf+idx);
			idx += a->values[0].size;
			break;

		    case szMAPI_STRING:
		    case szMAPI_UNICODE_STRING:
		    case szMAPI_OBJECT:
		    case szMAPI_BINARY:       /* variable length */
		    case szMAPI_UNSPECIFIED:
		    {
			size_t val_idx = 0;
			a->num_values = GETINT32(buf+idx); idx += 4;
			a->values = XMALLOC(tnef_mapi_attr_value, a->num_values);
			tnef_mapi_attr_value* v = a->values;
			for (val_idx = 0; val_idx < a->num_values; val_idx++)
			{
			    v[val_idx].size = GETINT32(buf+idx); idx += 4;

			    if (a->type == szMAPI_UNICODE_STRING)
			    {
				v[val_idx].data.buf = unicode_to_utf8(v[val_idx].size, buf+idx);
			    }
			    else
			    {
				v[val_idx].data.buf = XMALLOC(unsigned char, v[val_idx].size);
				memmove (v[val_idx].data.buf,
					 buf+idx,
					 v[val_idx].size);
			    }
			    idx += pad_to_4byte(v[val_idx].size);
			}
		    }
		    break;
		    }

		    tnef_mapi_parse_rtf_data (a);
		}
	    }
	}
    }
}

void dump_guid (tnef_mapi_guid guid, FILE* out)
{
    int i;
    fprintf (out, "guid:<%04x %02x %02x <",
	     guid.data1, guid.data2, guid.data3);
    fprintf (out, "%02x", guid.data4[0]);
    for (i = 1; i < 8; i++)
    {
	fprintf (out, " %02x", guid.data4[i]);
    }
    fprintf (out, ">>");
}

void dump_mapi_attr_value (mapi_name name, mapi_type type, tnef_mapi_attr_value value, FILE *out)
{
    int i;

    switch (type)
    {
    case szMAPI_UNSPECIFIED:
    case szMAPI_NULL:
	fprintf (out, "NULL");
	break;

    case szMAPI_SHORT:
	fprintf (out, UINT16_FMT, (uint16_t)value.data.int16);
	break;

    case szMAPI_ERROR: 
    case szMAPI_INT: 
	fprintf (out, UINT32_FMT, (uint32_t)value.data.int32);
	break;

    case szMAPI_FLOAT:
    case szMAPI_DOUBLE:
	fprintf (out, "%f", (float)value.data.int32);
	break;

    case szMAPI_BOOLEAN: 
	fprintf (out, "%s", ((((uint16_t)value.data.int32) == 0) ? "false" : "true"));
	break;

    case szMAPI_STRING: 
    case szMAPI_UNICODE_STRING: 
	fprintf (out, "'%s'", value.data.buf);
	break;

    case szMAPI_CURRENCY: 
    case szMAPI_APPTIME: 
    case szMAPI_INT8BYTE: 
    case szMAPI_SYSTIME:
	fprintf (out, "%08x %08x", value.data.int64[0], value.data.int64[1]);
	break;

    case szMAPI_CLSID: 
	dump_guid(value.data.guid, out);
	break;

    case szMAPI_OBJECT: 
    case szMAPI_BINARY:
	fprintf (out, "%02x", value.data.buf[0]);
	for (i = 1; i < value.size; i++)
	{
	    fprintf (out, " %02x", value.data.buf[i]);
	}
	break;

    default:
	fprintf (out, "UNKNOWN %02x", value.data.buf[0]);
	for (i = 1; i < value.size; i++)
	{
	    fprintf (out, " %02x", value.data.buf[i]);
	}
	break;
    }
}

void dump_mapi_attr_values (size_t num_values, mapi_name name, mapi_type type, tnef_mapi_attr_value* values, FILE *out)
{
    int i;

    fprintf (out, " values:<num:%d data:<", num_values);
    if (num_values > 0)
    {
	dump_mapi_attr_value (name, type, values[0], out);
	for (i = 1; i < num_values; i++) dump_mapi_attr_value (name, type, values[i], out);
    }
    fprintf (out, ">>");
}

void tnef_dump_mapi_attr (tnef_mapi_attr *mapi, FILE *out)
{
    int i;
    fprintf (out, "MAPIATTR=[type:%#04x<'%s'> name:%#04x<'%s'>",
	     mapi->type, get_mapi_type_str (mapi->type),
	     mapi->name, get_mapi_name_str (mapi->name));

    fprintf (out, " names:<");
    if (mapi->num_names > 0)
    {
	fprintf (out, "'%s'", mapi->names[0]);
	for (i = 1; i < mapi->num_names; i++)
	{
	    fprintf (out, ",'%s'", mapi->names[i]);
	}
    }
    fprintf (out, "> ");
    
    dump_mapi_attr_values (mapi->num_values, mapi->name, mapi->type, mapi->values, out);
    fprintf (out, " ");
    dump_guid(mapi->guid, out);
}

