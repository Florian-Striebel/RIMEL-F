
































#include "archive_platform.h"

#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif
#if !defined(lint)
__RCSID("$NetBSD$");
#endif

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if MAJOR_IN_MKDEV
#include <sys/mkdev.h>
#define HAVE_MAJOR
#elif MAJOR_IN_SYSMACROS
#include <sys/sysmacros.h>
#define HAVE_MAJOR
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "archive_pack_dev.h"

static pack_t pack_netbsd;
static pack_t pack_freebsd;
static pack_t pack_8_8;
static pack_t pack_12_20;
static pack_t pack_14_18;
static pack_t pack_8_24;
static pack_t pack_bsdos;
static int compare_format(const void *, const void *);

static const char iMajorError[] = "invalid major number";
static const char iMinorError[] = "invalid minor number";
static const char tooManyFields[] = "too many fields for format";



#if !defined(HAVE_MAJOR) && !defined(major)

#define major(x) ((int)(0x00ff & ((x) >> 8)))
#define minor(x) ((int)(0xffff00ff & (x)))
#define makedev(maj,min) ((0xff00 & ((maj)<<8)) | (0xffff00ff & (min)))
#endif


#if defined(__QNXNTO__)

#include <sys/netmgr.h>
#define apd_makedev(maj, min) makedev(ND_LOCAL_NODE, (maj), (min))
#elif defined makedev

#define apd_makedev(maj, min) makedev((maj), (min))
#elif defined mkdev || ((defined _WIN32 || defined __WIN32__) && !defined(__CYGWIN__))

#define apd_makedev(maj, min) mkdev((maj), (min))
#else

#define apd_makedev(maj, min) makedev((maj), (min))
#endif


dev_t
pack_native(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = apd_makedev(numbers[0], numbers[1]);
if ((unsigned long)major(dev) != numbers[0])
*error = iMajorError;
else if ((unsigned long)minor(dev) != numbers[1])
*error = iMinorError;
} else
*error = tooManyFields;
return (dev);
}


static dev_t
pack_netbsd(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = makedev_netbsd(numbers[0], numbers[1]);
if ((unsigned long)major_netbsd(dev) != numbers[0])
*error = iMajorError;
else if ((unsigned long)minor_netbsd(dev) != numbers[1])
*error = iMinorError;
} else
*error = tooManyFields;
return (dev);
}


#define major_freebsd(x) ((int32_t)(((x) & 0x0000ff00) >> 8))
#define minor_freebsd(x) ((int32_t)(((x) & 0xffff00ff) >> 0))
#define makedev_freebsd(x,y) ((dev_t)((((x) << 8) & 0x0000ff00) | (((y) << 0) & 0xffff00ff)))


static dev_t
pack_freebsd(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = makedev_freebsd(numbers[0], numbers[1]);
if ((unsigned long)major_freebsd(dev) != numbers[0])
*error = iMajorError;
if ((unsigned long)minor_freebsd(dev) != numbers[1])
*error = iMinorError;
} else
*error = tooManyFields;
return (dev);
}


#define major_8_8(x) ((int32_t)(((x) & 0x0000ff00) >> 8))
#define minor_8_8(x) ((int32_t)(((x) & 0x000000ff) >> 0))
#define makedev_8_8(x,y) ((dev_t)((((x) << 8) & 0x0000ff00) | (((y) << 0) & 0x000000ff)))


static dev_t
pack_8_8(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = makedev_8_8(numbers[0], numbers[1]);
if ((unsigned long)major_8_8(dev) != numbers[0])
*error = iMajorError;
if ((unsigned long)minor_8_8(dev) != numbers[1])
*error = iMinorError;
} else
*error = tooManyFields;
return (dev);
}


#define major_12_20(x) ((int32_t)(((x) & 0xfff00000) >> 20))
#define minor_12_20(x) ((int32_t)(((x) & 0x000fffff) >> 0))
#define makedev_12_20(x,y) ((dev_t)((((x) << 20) & 0xfff00000) | (((y) << 0) & 0x000fffff)))


static dev_t
pack_12_20(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = makedev_12_20(numbers[0], numbers[1]);
if ((unsigned long)major_12_20(dev) != numbers[0])
*error = iMajorError;
if ((unsigned long)minor_12_20(dev) != numbers[1])
*error = iMinorError;
} else
*error = tooManyFields;
return (dev);
}


#define major_14_18(x) ((int32_t)(((x) & 0xfffc0000) >> 18))
#define minor_14_18(x) ((int32_t)(((x) & 0x0003ffff) >> 0))
#define makedev_14_18(x,y) ((dev_t)((((x) << 18) & 0xfffc0000) | (((y) << 0) & 0x0003ffff)))


static dev_t
pack_14_18(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = makedev_14_18(numbers[0], numbers[1]);
if ((unsigned long)major_14_18(dev) != numbers[0])
*error = iMajorError;
if ((unsigned long)minor_14_18(dev) != numbers[1])
*error = iMinorError;
} else
*error = tooManyFields;
return (dev);
}


#define major_8_24(x) ((int32_t)(((x) & 0xff000000) >> 24))
#define minor_8_24(x) ((int32_t)(((x) & 0x00ffffff) >> 0))
#define makedev_8_24(x,y) ((dev_t)((((x) << 24) & 0xff000000) | (((y) << 0) & 0x00ffffff)))


static dev_t
pack_8_24(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = makedev_8_24(numbers[0], numbers[1]);
if ((unsigned long)major_8_24(dev) != numbers[0])
*error = iMajorError;
if ((unsigned long)minor_8_24(dev) != numbers[1])
*error = iMinorError;
} else
*error = tooManyFields;
return (dev);
}


#define major_12_12_8(x) ((int32_t)(((x) & 0xfff00000) >> 20))
#define unit_12_12_8(x) ((int32_t)(((x) & 0x000fff00) >> 8))
#define subunit_12_12_8(x) ((int32_t)(((x) & 0x000000ff) >> 0))
#define makedev_12_12_8(x,y,z) ((dev_t)((((x) << 20) & 0xfff00000) | (((y) << 8) & 0x000fff00) | (((z) << 0) & 0x000000ff)))



static dev_t
pack_bsdos(int n, unsigned long numbers[], const char **error)
{
dev_t dev = 0;

if (n == 2) {
dev = makedev_12_20(numbers[0], numbers[1]);
if ((unsigned long)major_12_20(dev) != numbers[0])
*error = iMajorError;
if ((unsigned long)minor_12_20(dev) != numbers[1])
*error = iMinorError;
} else if (n == 3) {
dev = makedev_12_12_8(numbers[0], numbers[1], numbers[2]);
if ((unsigned long)major_12_12_8(dev) != numbers[0])
*error = iMajorError;
if ((unsigned long)unit_12_12_8(dev) != numbers[1])
*error = "invalid unit number";
if ((unsigned long)subunit_12_12_8(dev) != numbers[2])
*error = "invalid subunit number";
} else
*error = tooManyFields;
return (dev);
}




static const struct format {
const char *name;
pack_t *pack;
} formats[] = {
{"386bsd", pack_8_8},
{"4bsd", pack_8_8},
{"bsdos", pack_bsdos},
{"freebsd", pack_freebsd},
{"hpux", pack_8_24},
{"isc", pack_8_8},
{"linux", pack_8_8},
{"native", pack_native},
{"netbsd", pack_netbsd},
{"osf1", pack_12_20},
{"sco", pack_8_8},
{"solaris", pack_14_18},
{"sunos", pack_8_8},
{"svr3", pack_8_8},
{"svr4", pack_14_18},
{"ultrix", pack_8_8},
};

static int
compare_format(const void *key, const void *element)
{
const char *name;
const struct format *format;

name = key;
format = element;

return (strcmp(name, format->name));
}


pack_t *
pack_find(const char *name)
{
struct format *format;

format = bsearch(name, formats,
sizeof(formats)/sizeof(formats[0]),
sizeof(formats[0]), compare_format);
if (format == 0)
return (NULL);
return (format->pack);
}
