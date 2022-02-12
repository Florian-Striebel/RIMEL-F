
































#if !defined(LAFE_PLATFORM_H_INCLUDED)
#define LAFE_PLATFORM_H_INCLUDED

#if defined(PLATFORM_CONFIG_H)

#include PLATFORM_CONFIG_H
#else

#include "config.h"
#endif


#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif


#if !defined(__FBSDID)
#define __FBSDID(a) struct _undefined_hack
#endif

#endif
