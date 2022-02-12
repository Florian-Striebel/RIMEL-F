

























#include <sys/param.h>
#include <assert.h>
#include <errno.h>
#include <libelftc.h>
#include <stdlib.h>
#include <string.h>

#include "_libelftc.h"

ELFTC_VCSID("$Id: elftc_demangle.c 3296 2016-01-09 14:17:28Z jkoshy $");

static unsigned int
is_mangled(const char *s, unsigned int style)
{

switch (style) {
case ELFTC_DEM_ARM: return (is_cpp_mangled_ARM(s) ? style : 0);
case ELFTC_DEM_GNU2: return (is_cpp_mangled_gnu2(s) ? style : 0);
case ELFTC_DEM_GNU3: return (is_cpp_mangled_gnu3(s) ? style : 0);
}


if (is_cpp_mangled_gnu3(s))
return (ELFTC_DEM_GNU3);
if (is_cpp_mangled_gnu2(s))
return (ELFTC_DEM_GNU2);
if (is_cpp_mangled_ARM(s))
return (ELFTC_DEM_ARM);


return (0);
}

static char *
demangle(const char *s, unsigned int style, unsigned int rc)
{

(void) rc;
switch (style) {
case ELFTC_DEM_ARM: return (cpp_demangle_ARM(s));
case ELFTC_DEM_GNU2: return (cpp_demangle_gnu2(s));
case ELFTC_DEM_GNU3: return (cpp_demangle_gnu3(s));
default:
assert(0);
return (NULL);
}
}

int
elftc_demangle(const char *mangledname, char *buffer, size_t bufsize,
unsigned int flags)
{
unsigned int style, rc;
char *rlt;

style = flags & 0xFFFF;
rc = flags >> 16;

if (mangledname == NULL ||
((style = is_mangled(mangledname, style)) == 0)) {
errno = EINVAL;
return (-1);
}

if ((rlt = demangle(mangledname, style, rc)) == NULL) {
errno = EINVAL;
return (-1);
}

if (buffer == NULL || bufsize < strlen(rlt) + 1) {
free(rlt);
errno = ENAMETOOLONG;
return (-1);
}

strncpy(buffer, rlt, bufsize);
buffer[bufsize - 1] = '\0';
free(rlt);

return (0);
}
