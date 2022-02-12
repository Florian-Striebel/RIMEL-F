

























#include <sys/types.h>

#include "libelftc.h"

#include "_libelftc.h"

ELFTC_VCSID("$Id$");






#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#define ATIME st_atimespec
#define MTIME st_mtimespec
#define LIBELFTC_HAVE_UTIMES 1
#endif

#if defined(__DragonFly__) || defined(__linux__) || defined(__OpenBSD__)
#define ATIME st_atim
#define MTIME st_mtim
#define LIBELFTC_HAVE_UTIMES 1
#endif

#if LIBELFTC_HAVE_UTIMES
#include <sys/time.h>
#else
#include <utime.h>
#endif

int
elftc_set_timestamps(const char *fn, struct stat *sb)
{
#if LIBELFTC_HAVE_UTIMES




struct timeval tv[2];

tv[0].tv_sec = sb->ATIME.tv_sec;
tv[0].tv_usec = sb->ATIME.tv_nsec / 1000;
tv[1].tv_sec = sb->MTIME.tv_sec;
tv[1].tv_usec = sb->MTIME.tv_nsec / 1000;

return (utimes(fn, tv));
#else




struct utimbuf utb;

utb.actime = sb->st_atime;
utb.modtime = sb->st_mtime;
return (utime(fn, &utb));
#endif
}
