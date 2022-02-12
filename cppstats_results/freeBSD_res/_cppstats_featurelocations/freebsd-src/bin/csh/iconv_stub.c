



























#include <dlfcn.h>
#include <stddef.h>

#include "iconv.h"

#undef iconv_open
#undef iconv
#undef iconv_close

#define ICONVLIB "libiconv.so"
#define ICONV_ENGINE "libiconv"
#define ICONV_OPEN "libiconv_open"
#define ICONV_CLOSE "libiconv_close"

typedef iconv_t iconv_open_t(const char *, const char *);

dl_iconv_t *dl_iconv;
dl_iconv_close_t *dl_iconv_close;

static int initialized;
static void *iconvlib;
static iconv_open_t *iconv_open;

iconv_t
dl_iconv_open(const char *tocode, const char *fromcode)
{
if (initialized) {
if (iconvlib == NULL)
return (iconv_t)-1;
} else {
initialized = 1;
iconvlib = dlopen(ICONVLIB, RTLD_LAZY | RTLD_GLOBAL);
if (iconvlib == NULL)
return (iconv_t)-1;
iconv_open = (iconv_open_t *)dlfunc(iconvlib, ICONV_OPEN);
if (iconv_open == NULL)
goto dlfunc_err;
dl_iconv = (dl_iconv_t *)dlfunc(iconvlib, ICONV_ENGINE);
if (dl_iconv == NULL)
goto dlfunc_err;
dl_iconv_close = (dl_iconv_close_t *)dlfunc(iconvlib,
ICONV_CLOSE);
if (dl_iconv_close == NULL)
goto dlfunc_err;
}
return iconv_open(tocode, fromcode);

dlfunc_err:
dlclose(iconvlib);
iconvlib = NULL;
return (iconv_t)-1;
}
