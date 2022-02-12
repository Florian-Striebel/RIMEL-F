


























#if !defined(ARCHIVE_GETDATE_H_INCLUDED)
#define ARCHIVE_GETDATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif

#include <time.h>

time_t __archive_get_date(time_t now, const char *);

#endif
