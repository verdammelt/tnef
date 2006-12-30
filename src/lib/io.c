
#include <stdio.h>
#include <errno.h>

#include "alloc.h"
#include "io.h"

extern int tnef_error;

/* Needed to transform char buffers into little endian numbers */
uint32_t GETINT32(unsigned char *p)
{
    return (uint32_t)((uint8_t)(p)[0]           \
		    +((uint8_t)(p)[1]<<8)     \
		    +((uint8_t)(p)[2]<<16)    \
		    +((uint8_t)(p)[3]<<24));
}

uint16_t GETINT16 (unsigned char* p)
{
    return (uint16_t)((uint8_t)(p)[0]+((uint8_t)(p)[1]<<8));
}

uint8_t GETINT8 (unsigned char *p)
{
    return (uint8_t)(p)[0];
}

unsigned char*
getbuf (FILE *fp, unsigned char buf[], size_t n)
{
    fread (buf, 1, n, fp);
    if (feof(fp)) tnef_error = -1;
    else if (ferror(fp)) tnef_error = errno;
    return buf;
}

uint8_t _read_uint8 (FILE* fp)
{
    unsigned char buf[1];
    return (uint8_t)GETINT8(getbuf(fp, buf, 1));
}

uint16_t _read_uint16 (FILE* fp)
{
    unsigned char buf[2];
    return (uint16_t)GETINT16(getbuf(fp, buf, 2));
}

uint32_t _read_uint32 (FILE* fp)
{
    unsigned char buf[4];
    return (uint32_t)GETINT32(getbuf(fp, buf, 4));
}

unsigned char* _read_buf (FILE* fp, size_t n)
{
    unsigned char* buf = XMALLOC(unsigned char, n);
    return getbuf (fp, buf, n);
}

