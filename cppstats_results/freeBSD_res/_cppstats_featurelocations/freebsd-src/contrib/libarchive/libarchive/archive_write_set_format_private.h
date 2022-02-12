


























#if !defined(ARCHIVE_WRITE_SET_FORMAT_PRIVATE_H_INCLUDED)
#define ARCHIVE_WRITE_SET_FORMAT_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#if !defined(__LIBARCHIVE_TEST)
#error This header is only to be used internally to libarchive.
#endif
#endif

#include "archive.h"
#include "archive_entry.h"

void __archive_write_entry_filetype_unsupported(struct archive *a,
struct archive_entry *entry, const char *format);
#endif
