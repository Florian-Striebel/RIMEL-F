
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif

#include "archive.h"
#include "archive_private.h"
#include "archive_write_set_format_private.h"


static const
struct { int code; int (*setter)(struct archive *); } codes[] =
{
{ ARCHIVE_FORMAT_7ZIP, archive_write_set_format_7zip },
{ ARCHIVE_FORMAT_CPIO, archive_write_set_format_cpio },
{ ARCHIVE_FORMAT_CPIO_BIN_LE, archive_write_set_format_cpio_bin },
{ ARCHIVE_FORMAT_CPIO_PWB, archive_write_set_format_cpio_pwb },
{ ARCHIVE_FORMAT_CPIO_POSIX, archive_write_set_format_cpio_odc },
{ ARCHIVE_FORMAT_CPIO_SVR4_NOCRC, archive_write_set_format_cpio_newc },
{ ARCHIVE_FORMAT_ISO9660, archive_write_set_format_iso9660 },
{ ARCHIVE_FORMAT_MTREE, archive_write_set_format_mtree },
{ ARCHIVE_FORMAT_RAW, archive_write_set_format_raw },
{ ARCHIVE_FORMAT_SHAR, archive_write_set_format_shar },
{ ARCHIVE_FORMAT_SHAR_BASE, archive_write_set_format_shar },
{ ARCHIVE_FORMAT_SHAR_DUMP, archive_write_set_format_shar_dump },
{ ARCHIVE_FORMAT_TAR, archive_write_set_format_pax_restricted },
{ ARCHIVE_FORMAT_TAR_GNUTAR, archive_write_set_format_gnutar },
{ ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE, archive_write_set_format_pax },
{ ARCHIVE_FORMAT_TAR_PAX_RESTRICTED,
archive_write_set_format_pax_restricted },
{ ARCHIVE_FORMAT_TAR_USTAR, archive_write_set_format_ustar },
{ ARCHIVE_FORMAT_WARC, archive_write_set_format_warc },
{ ARCHIVE_FORMAT_XAR, archive_write_set_format_xar },
{ ARCHIVE_FORMAT_ZIP, archive_write_set_format_zip },
{ 0, NULL }
};

int
archive_write_set_format(struct archive *a, int code)
{
int i;

for (i = 0; codes[i].code != 0; i++) {
if (code == codes[i].code)
return ((codes[i].setter)(a));
}

archive_set_error(a, EINVAL, "No such format");
return (ARCHIVE_FATAL);
}

void
__archive_write_entry_filetype_unsupported(struct archive *a,
struct archive_entry *entry, const char *format)
{
const char *name = NULL;

switch (archive_entry_filetype(entry)) {



case AE_IFDIR:
name = "directories";
break;
case AE_IFLNK:
name = "symbolic links";
break;
case AE_IFCHR:
name = "character devices";
break;
case AE_IFBLK:
name = "block devices";
break;
case AE_IFIFO:
name = "named pipes";
break;
case AE_IFSOCK:
name = "sockets";
break;
default:
break;
}

if (name != NULL) {
archive_set_error(a, ARCHIVE_ERRNO_FILE_FORMAT,
"%s: %s format cannot archive %s",
archive_entry_pathname(entry), format, name);
} else {
archive_set_error(a, ARCHIVE_ERRNO_FILE_FORMAT,
"%s: %s format cannot archive files with mode 0%lo",
archive_entry_pathname(entry), format,
(unsigned long)archive_entry_mode(entry));
}
}
