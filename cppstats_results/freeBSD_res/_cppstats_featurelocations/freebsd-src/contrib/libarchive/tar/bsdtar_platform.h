
































#if !defined(BSDTAR_PLATFORM_H_INCLUDED)
#define BSDTAR_PLATFORM_H_INCLUDED

#if defined(PLATFORM_CONFIG_H)

#include PLATFORM_CONFIG_H
#else

#include "config.h"
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include "bsdtar_windows.h"
#endif


#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif


#if !defined(__FBSDID)
#define __FBSDID(a) struct _undefined_hack
#endif

#if defined(HAVE_LIBARCHIVE)

#include <archive.h>
#include <archive_entry.h>
#else

#include "archive.h"
#include "archive_entry.h"
#endif

#if defined(HAVE_LIBACL)
#include <acl/libacl.h>
#endif









#if HAVE_DIRENT_H
#include <dirent.h>
#if HAVE_DIRENT_D_NAMLEN
#define DIRENT_NAMLEN(dirent) (dirent)->d_namlen
#else
#define DIRENT_NAMLEN(dirent) strlen((dirent)->d_name)
#endif
#else
#define dirent direct
#define DIRENT_NAMLEN(dirent) (dirent)->d_namlen
#if HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#if HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#if HAVE_NDIR_H
#include <ndir.h>
#endif
#endif

#if HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC
#define ARCHIVE_STAT_CTIME_NANOS(st) (st)->st_ctimespec.tv_nsec
#define ARCHIVE_STAT_MTIME_NANOS(st) (st)->st_mtimespec.tv_nsec
#elif HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC
#define ARCHIVE_STAT_CTIME_NANOS(st) (st)->st_ctim.tv_nsec
#define ARCHIVE_STAT_MTIME_NANOS(st) (st)->st_mtim.tv_nsec
#elif HAVE_STRUCT_STAT_ST_MTIME_N
#define ARCHIVE_STAT_CTIME_NANOS(st) (st)->st_ctime_n
#define ARCHIVE_STAT_MTIME_NANOS(st) (st)->st_mtime_n
#elif HAVE_STRUCT_STAT_ST_UMTIME
#define ARCHIVE_STAT_CTIME_NANOS(st) (st)->st_uctime * 1000
#define ARCHIVE_STAT_MTIME_NANOS(st) (st)->st_umtime * 1000
#elif HAVE_STRUCT_STAT_ST_MTIME_USEC
#define ARCHIVE_STAT_CTIME_NANOS(st) (st)->st_ctime_usec * 1000
#define ARCHIVE_STAT_MTIME_NANOS(st) (st)->st_mtime_usec * 1000
#else
#define ARCHIVE_STAT_CTIME_NANOS(st) (0)
#define ARCHIVE_STAT_MTIME_NANOS(st) (0)
#endif



#undef __LA_DEAD
#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5))

#define __LA_DEAD __attribute__((__noreturn__))
#else
#define __LA_DEAD
#endif

#endif
