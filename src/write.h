#ifndef WRITE_H
#define WRITE_H 1

#include "common.h"
#include "attr.h"
#include "mapi_attr.h"

extern void write_uint8 (FILE* fp, uint8 b);
extern void write_uint16 (FILE* fp, uint16 s);
extern void write_uint32 (FILE* fp, uint32 l);
extern void write_float (FILE *fp, float f);
extern void write_string(FILE* fp, const char *s);
extern void write_byte (FILE* fp, uint8 b);
extern void write_word (FILE *fp, uint16 w);
extern void write_dword (FILE *fp, uint32 dw);
extern void write_date (FILE *fp, struct date* dt);
extern void write_triple (FILE* fp, TRIPLE *triple);
extern void write_boolean (FILE *fp, uint16 b);
extern void write_uint64 (FILE *fp, uint32 bytes[2]);
extern void write_guid (FILE *fp, GUID *guid);

#endif /* WRITE_H */
