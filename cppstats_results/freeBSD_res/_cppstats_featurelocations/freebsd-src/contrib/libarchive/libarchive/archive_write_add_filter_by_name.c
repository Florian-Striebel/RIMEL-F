

























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "archive.h"
#include "archive_private.h"


static const
struct { const char *name; int (*setter)(struct archive *); } names[] =
{
{ "b64encode", archive_write_add_filter_b64encode },
{ "bzip2", archive_write_add_filter_bzip2 },
{ "compress", archive_write_add_filter_compress },
{ "grzip", archive_write_add_filter_grzip },
{ "gzip", archive_write_add_filter_gzip },
{ "lrzip", archive_write_add_filter_lrzip },
{ "lz4", archive_write_add_filter_lz4 },
{ "lzip", archive_write_add_filter_lzip },
{ "lzma", archive_write_add_filter_lzma },
{ "lzop", archive_write_add_filter_lzop },
{ "uuencode", archive_write_add_filter_uuencode },
{ "xz", archive_write_add_filter_xz },
{ "zstd", archive_write_add_filter_zstd },
{ NULL, NULL }
};

int
archive_write_add_filter_by_name(struct archive *a, const char *name)
{
int i;

for (i = 0; names[i].name != NULL; i++) {
if (strcmp(name, names[i].name) == 0)
return ((names[i].setter)(a));
}

archive_set_error(a, EINVAL, "No such filter '%s'", name);
a->state = ARCHIVE_STATE_FATAL;
return (ARCHIVE_FATAL);
}
