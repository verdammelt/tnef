#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

#if STDC_HEADERS
#  include <stdlib.h>
#else
extern void* malloc (size_t size);
#endif /* STDC_HEADERS */

#include "alloc.h"

/* attempts to malloc memory, if fails print error and call abort */
void*
MALLOC (size_t size)
{
    void *ptr = malloc (size);
    if (!ptr 
        && (size != 0))         /* some libc don't like size == 0 */
    {
        perror ("Memory allocation failure");
        abort();
    }
    return ptr;
}


