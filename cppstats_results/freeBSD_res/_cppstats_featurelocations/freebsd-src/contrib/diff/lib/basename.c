


















#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "dirname.h"
#include <string.h>









char *
base_name (char const *name)
{
char const *base = name + FILESYSTEM_PREFIX_LEN (name);
char const *p;

for (p = base; *p; p++)
{
if (ISSLASH (*p))
{

do p++;
while (ISSLASH (*p));



if (! *p)
{
if (ISSLASH (*base))
base = p - 1;
break;
}


base = p;
}
}

return (char *) base;
}





size_t
base_len (char const *name)
{
size_t len;

for (len = strlen (name); 1 < len && ISSLASH (name[len - 1]); len--)
continue;

return len;
}
