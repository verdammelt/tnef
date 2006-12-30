
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define E_OK  (0)
#define E_EOF (-1)
#define E_SIGNATURE (-2)
#define E_CHECKSUM (-3)
#define E_BADSIZE (-4)
extern int tnef_error;

/* **********
   Base TNEF stuff 
   ********** */
typedef struct
{
    char* filename;
    FILE* fileptr;
    uint32_t signature;
    uint16_t key;
} TNEF;

extern TNEF* tnef_open (char* filename);
extern void tnef_close (TNEF* tnef);

/* ********** 
   MAPI Attr stuff
   ********** */
#include "mapi_types.h"
#include "mapi_names.h"

typedef struct
{
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
} tnef_mapi_guid;

typedef struct
{
    size_t size;
    union
    { 
	unsigned char *buf;
	uint16_t int16;
	uint32_t int32;
	uint32_t int64[2];
	tnef_mapi_guid guid;
    } data;
} tnef_mapi_attr_value;

typedef struct
{
    mapi_type type;
    mapi_name name;

    size_t num_names;
    unsigned char** names;

    size_t num_values;
    tnef_mapi_attr_value* values;
    tnef_mapi_guid guid;
} tnef_mapi_attr;

typedef struct
{
    size_t num_attrs;
    tnef_mapi_attr** array;
} tnef_mapi_attr_array;

/* **********
   Attr stuff
   ********** */
enum _tnef_attr_level { MESSAGE = 0x1, ATTACHMENT = 0x2 };
typedef enum _tnef_attr_level tnef_attr_level;

#include <time.h>
#include "attr_types.h"
#include "attr_names.h"

typedef struct
{
    struct TRP 
    {
	uint16_t id;
	uint16_t chbgtrp;
	uint16_t cch;
	uint16_t cb;
    } trp;
    char *sender_display_name;
    char *sender_address;
} tnef_attr_triple;

typedef struct
{
    int16_t year, month, day;
    int16_t hour, min, sec;
    int16_t dow;
} tnef_attr_date;

typedef struct
{
    tnef_attr_level level;
    attr_type type;
    attr_name name;
    
    size_t size;
    unsigned char* raw_data;
    uint32_t checksum;
    union
    {
	uint8_t int8;
	uint16_t int16;
	uint32_t int32;
	unsigned char *buf;
	uint16_t *words;
	uint32_t *dwords;
	tnef_attr_date date;
	tnef_attr_triple triple;
	tnef_mapi_attr_array mapi_attrs;
    } data;
} tnef_attr;

extern tnef_attr* tnef_read_attr (TNEF *tnef);
extern void tnef_free_attr (tnef_attr* attr);
extern void tnef_parse_attr (tnef_attr* attr);

extern int tnef_has_mapi_attr (tnef_attr *attr);
extern void tnef_parse_mapi_attr (tnef_attr *attr);
extern void tnef_dump_mapi_attr (tnef_mapi_attr *mapi_attr, FILE *out);

