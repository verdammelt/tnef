#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if STDC_HEADERS
#  if HAVE_STRING_H
#    include <string.h>
#  else
#    include <strings.h>
#  endif
#else
#  ifndef HAVE_STRRCHR
#    define strrchr rindex
#  endif /* !HAVE_STRRCHR */
extern char *strrchr();
#endif /* STDC_HEADERS */

#if !HAVE_BASENAME
/* works like basename(1) (NOTE: the returned pointer must not be freed! */
char*
basename (const char* path)
{
    char *ptr = strrchr (path, '/');
    return ptr ? ptr + 1 : (char*)path;
}
#endif /* !HAVE_BASENAME */

