
























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
{ "7zip", archive_write_set_format_7zip },
{ "ar", archive_write_set_format_ar_bsd },
{ "arbsd", archive_write_set_format_ar_bsd },
{ "argnu", archive_write_set_format_ar_svr4 },
{ "arsvr4", archive_write_set_format_ar_svr4 },
{ "bin", archive_write_set_format_cpio_bin },
{ "bsdtar", archive_write_set_format_pax_restricted },
{ "cd9660", archive_write_set_format_iso9660 },
{ "cpio", archive_write_set_format_cpio },
{ "gnutar", archive_write_set_format_gnutar },
{ "iso", archive_write_set_format_iso9660 },
{ "iso9660", archive_write_set_format_iso9660 },
{ "mtree", archive_write_set_format_mtree },
{ "mtree-classic", archive_write_set_format_mtree_classic },
{ "newc", archive_write_set_format_cpio_newc },
{ "odc", archive_write_set_format_cpio_odc },
{ "oldtar", archive_write_set_format_v7tar },
{ "pax", archive_write_set_format_pax },
{ "paxr", archive_write_set_format_pax_restricted },
{ "posix", archive_write_set_format_pax },
{ "pwb", archive_write_set_format_cpio_pwb },
{ "raw", archive_write_set_format_raw },
{ "rpax", archive_write_set_format_pax_restricted },
{ "shar", archive_write_set_format_shar },
{ "shardump", archive_write_set_format_shar_dump },
{ "ustar", archive_write_set_format_ustar },
{ "v7tar", archive_write_set_format_v7tar },
{ "v7", archive_write_set_format_v7tar },
{ "warc", archive_write_set_format_warc },
{ "xar", archive_write_set_format_xar },
{ "zip", archive_write_set_format_zip },
{ NULL, NULL }
};

int
archive_write_set_format_by_name(struct archive *a, const char *name)
{
int i;

for (i = 0; names[i].name != NULL; i++) {
if (strcmp(name, names[i].name) == 0)
return ((names[i].setter)(a));
}

archive_set_error(a, EINVAL, "No such format '%s'", name);
a->state = ARCHIVE_STATE_FATAL;
return (ARCHIVE_FATAL);
}
