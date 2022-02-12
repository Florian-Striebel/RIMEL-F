

























#include "archive_platform.h"
__FBSDID("$FreeBSD: head/lib/libarchive/archive_util.c 201098 2009-12-28 02:58:14Z kientzle $");

#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_ZLIB_H)
#include <zlib.h>
#endif
#if defined(HAVE_LZMA_H)
#include <lzma.h>
#endif
#if defined(HAVE_BZLIB_H)
#include <bzlib.h>
#endif
#if defined(HAVE_LZ4_H)
#include <lz4.h>
#endif
#if defined(HAVE_ZSTD_H)
#include <zstd.h>
#endif

#include "archive.h"
#include "archive_private.h"
#include "archive_string.h"

const char *
archive_version_details(void)
{
static struct archive_string str;
static int init = 0;
const char *zlib = archive_zlib_version();
const char *liblzma = archive_liblzma_version();
const char *bzlib = archive_bzlib_version();
const char *liblz4 = archive_liblz4_version();
const char *libzstd = archive_libzstd_version();

if (!init) {
archive_string_init(&str);

archive_strcat(&str, ARCHIVE_VERSION_STRING);
if (zlib != NULL) {
archive_strcat(&str, " zlib/");
archive_strcat(&str, zlib);
}
if (liblzma) {
archive_strcat(&str, " liblzma/");
archive_strcat(&str, liblzma);
}
if (bzlib) {
const char *p = bzlib;
const char *sep = strchr(p, ',');
if (sep == NULL)
sep = p + strlen(p);
archive_strcat(&str, " bz2lib/");
archive_strncat(&str, p, sep - p);
}
if (liblz4) {
archive_strcat(&str, " liblz4/");
archive_strcat(&str, liblz4);
}
if (libzstd) {
archive_strcat(&str, " libzstd/");
archive_strcat(&str, libzstd);
}
}
return str.s;
}

const char *
archive_zlib_version(void)
{
#if defined(HAVE_ZLIB_H)
return ZLIB_VERSION;
#else
return NULL;
#endif
}

const char *
archive_liblzma_version(void)
{
#if defined(HAVE_LZMA_H)
return LZMA_VERSION_STRING;
#else
return NULL;
#endif
}

const char *
archive_bzlib_version(void)
{
#if defined(HAVE_BZLIB_H)
return BZ2_bzlibVersion();
#else
return NULL;
#endif
}

const char *
archive_liblz4_version(void)
{
#if defined(HAVE_LZ4_H) && defined(HAVE_LIBLZ4)
#define str(s) #s
#define NUMBER(x) str(x)
return NUMBER(LZ4_VERSION_MAJOR) "." NUMBER(LZ4_VERSION_MINOR) "." NUMBER(LZ4_VERSION_RELEASE);
#undef NUMBER
#undef str
#else
return NULL;
#endif
}

const char *
archive_libzstd_version(void)
{
#if HAVE_ZSTD_H && HAVE_LIBZSTD
return ZSTD_VERSION_STRING;
#else
return NULL;
#endif
}
