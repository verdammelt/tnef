#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <assert.h>
#include <stdio.h>

#include "strdup.h"

#include "alloc.h"

#include "path.h"

/* concatenates fname1 with fname2 to make a pathname, adds '/' as needed
 */ 
char *
concat_fname (const char *fname1, const char* fname2)
{
    char *filename = NULL;

    assert (fname1 || fname2);

    if (!fname1)
    {
        filename = strdup (fname2);
    }
    else
    {
        int len = strlen (fname1);
        if (fname2) len += strlen (fname2);

        filename = (char *)MALLOC ((len + 2) * sizeof (char));
        strcpy (filename, fname1);

        if (fname2)
        {
            if ((filename[strlen(filename)-1] != '/')
                && (fname2[0] != '/'))
            {
                strcat (filename, "/");
            }
            strcat (filename, fname2);
        }
    }

    return filename;
}

