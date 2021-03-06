






























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD: strtoi.c,v 1.3 2015/01/22 03:10:50 christos Exp $");

#if defined(_KERNEL)
#include <sys/param.h>
#include <sys/types.h>
#include <lib/libkern/libkern.h>
#elif defined(_STANDALONE)
#include <sys/param.h>
#include <sys/types.h>
#include <lib/libkern/libkern.h>
#include <lib/libsa/stand.h>
#else
#include <stddef.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#endif

#define _FUNCNAME strtoi
#define __TYPE intmax_t
#define __WRAPPED strtoimax

#if !HAVE_STRTOI
#include "_strtoi.h"
#endif
