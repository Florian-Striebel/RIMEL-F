
































#if !defined(CPIO_PLATFORM_H_INCLUDED)
#define CPIO_PLATFORM_H_INCLUDED

#if defined(PLATFORM_CONFIG_H)

#include PLATFORM_CONFIG_H
#else

#include "config.h"
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include "cpio_windows.h"
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


#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5))

#define __LA_DEAD __attribute__((__noreturn__))
#else
#define __LA_DEAD
#endif

#endif
