#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "common.h"
#include "attr.h"
#include "mapi_attr.h"

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

void
write_uint8 (FILE* fp, uint8 b)
{
    fprintf (fp, UINT8_FMT, b);
}

void 
write_uint16 (FILE* fp, uint16 s)
{
    fprintf (fp, UINT16_FMT, s);
}

void
write_uint32 (FILE* fp, uint32 l)
{
    fprintf (fp, UINT32_FMT, l);
}

void
write_int8 (FILE* fp, int8 b)
{
    fprintf (fp, INT8_FMT, b);
}

void
write_int16 (FILE* fp, int16 s)
{
    fprintf (fp, INT16_FMT, s);
}

void
write_int32 (FILE* fp, int32 l)
{
    fprintf (fp, INT32_FMT, l);
}

void
write_float (FILE *fp, float f)
{
    fprintf (fp, "%f", f);
}

void
write_string (FILE *fp, const char *s)
{
    fprintf (fp, "'%s'", s);
}

void 
write_byte (FILE* fp, uint8 b)
{
    fprintf (fp, "0x%02x", b);
}

void
write_word (FILE *fp, uint16 w)
{
    fprintf (fp, "0x%04x", w);
}

void
write_dword (FILE *fp, uint32 dw)
{
    fprintf (fp, "0x%08x", dw);
}

void
write_date (FILE *fp, struct date* dt)
{
    fprintf (fp, "%s", date_to_str (dt));
}

void
write_triple (FILE* fp, TRIPLE* triple)
{
    fprintf (fp,
	     "{id=" UINT16_FMT 
	     ",chgtrp=" UINT16_FMT 
	     ",cch=" UINT16_FMT 
	     ",cb=" UINT16_FMT "} "
	     "sender_display_name='%s', "
	     "sender_address='%s'",
	     triple->trp.id,
	     triple->trp.chbgtrp,
	     triple->trp.cch,
	     triple->trp.cb,
	     triple->sender_display_name,
	     triple->sender_address);
}

void
write_boolean (FILE *fp, uint16 b)
{
    fprintf (fp, "%s", ((b == 0) ? "false" : "true"));
}

void
write_uint64 (FILE *fp, uint32 bytes[2])
{
    fprintf (fp, "0x%08x 0x%08x", bytes[0], bytes[1]);
}

void 
write_guid (FILE *fp, GUID *guid)
{
    int j;
    fprintf (fp, "{ 0x%04x 0x%02x 0x%02x { ",
	     guid->data1, guid->data2, guid->data3);
    for (j = 0; j < 8; j++)
    {
	write_byte (fp, guid->data4[j]);
	fprintf (fp, " ");
    }
    fprintf (fp, "}");
}
