
#include "tnef.h"
#include "alloc.h"
#include "io.h"

#define TNEF_SIGNATURE 0x223e9f78

int tnef_error = 0;

TNEF* tnef_open (char* filename)
{
    TNEF* tnef = XMALLOC(TNEF, 1);
    tnef->filename = XMALLOC(char, strlen(filename));
    strcpy (tnef->filename, filename);
    tnef->fileptr = fopen (tnef->filename, "r");

    if (tnef->fileptr && tnef_error == E_OK)
    {
	tnef->signature = _read_uint32(tnef->fileptr);

	if (tnef_error == E_OK)
	{
	    if (tnef->signature != TNEF_SIGNATURE)
	    {
		tnef_error = E_SIGNATURE;
	    }

	    tnef->key = _read_uint16(tnef->fileptr);
	}
    }

    if (tnef_error) 
    {
	tnef_close (tnef);
	tnef == NULL;
    }

    return tnef;
}

void tnef_close (TNEF* tnef)
{
    fclose (tnef->fileptr);
    XFREE (tnef->filename);
    XFREE (tnef);
}


