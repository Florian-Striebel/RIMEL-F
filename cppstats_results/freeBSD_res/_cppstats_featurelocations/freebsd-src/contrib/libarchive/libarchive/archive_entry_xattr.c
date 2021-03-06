
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if defined(HAVE_LINUX_FS_H)
#include <linux/fs.h>
#endif




#if defined(HAVE_LINUX_EXT2_FS_H)
#include <linux/ext2_fs.h>
#endif
#if defined(HAVE_EXT2FS_EXT2_FS_H) && !defined(__CYGWIN__)
#include <ext2fs/ext2_fs.h>
#endif
#include <stddef.h>
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_WCHAR_H)
#include <wchar.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_entry_private.h"





void
archive_entry_xattr_clear(struct archive_entry *entry)
{
struct ae_xattr *xp;

while (entry->xattr_head != NULL) {
xp = entry->xattr_head->next;
free(entry->xattr_head->name);
free(entry->xattr_head->value);
free(entry->xattr_head);
entry->xattr_head = xp;
}

entry->xattr_head = NULL;
}

void
archive_entry_xattr_add_entry(struct archive_entry *entry,
const char *name, const void *value, size_t size)
{
struct ae_xattr *xp;

if ((xp = (struct ae_xattr *)malloc(sizeof(struct ae_xattr))) == NULL)
__archive_errx(1, "Out of memory");

if ((xp->name = strdup(name)) == NULL)
__archive_errx(1, "Out of memory");

if ((xp->value = malloc(size)) != NULL) {
memcpy(xp->value, value, size);
xp->size = size;
} else
xp->size = 0;

xp->next = entry->xattr_head;
entry->xattr_head = xp;
}





int
archive_entry_xattr_count(struct archive_entry *entry)
{
struct ae_xattr *xp;
int count = 0;

for (xp = entry->xattr_head; xp != NULL; xp = xp->next)
count++;

return count;
}

int
archive_entry_xattr_reset(struct archive_entry * entry)
{
entry->xattr_p = entry->xattr_head;

return archive_entry_xattr_count(entry);
}

int
archive_entry_xattr_next(struct archive_entry * entry,
const char **name, const void **value, size_t *size)
{
if (entry->xattr_p) {
*name = entry->xattr_p->name;
*value = entry->xattr_p->value;
*size = entry->xattr_p->size;

entry->xattr_p = entry->xattr_p->next;

return (ARCHIVE_OK);
} else {
*name = NULL;
*value = NULL;
*size = (size_t)0;
return (ARCHIVE_WARN);
}
}




