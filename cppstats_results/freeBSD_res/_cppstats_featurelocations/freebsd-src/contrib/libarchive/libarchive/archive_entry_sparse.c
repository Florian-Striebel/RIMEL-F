

























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_entry_private.h"





void
archive_entry_sparse_clear(struct archive_entry *entry)
{
struct ae_sparse *sp;

while (entry->sparse_head != NULL) {
sp = entry->sparse_head->next;
free(entry->sparse_head);
entry->sparse_head = sp;
}
entry->sparse_tail = NULL;
}

void
archive_entry_sparse_add_entry(struct archive_entry *entry,
la_int64_t offset, la_int64_t length)
{
struct ae_sparse *sp;

if (offset < 0 || length < 0)

return;
if (offset > INT64_MAX - length ||
offset + length > archive_entry_size(entry))

return;
if ((sp = entry->sparse_tail) != NULL) {
if (sp->offset + sp->length > offset)

return;
if (sp->offset + sp->length == offset) {
if (sp->offset + sp->length + length < 0)


return;

sp->length += length;
return;
}
}

if ((sp = (struct ae_sparse *)malloc(sizeof(*sp))) == NULL)

return;

sp->offset = offset;
sp->length = length;
sp->next = NULL;

if (entry->sparse_head == NULL)
entry->sparse_head = entry->sparse_tail = sp;
else {

if (entry->sparse_tail != NULL)
entry->sparse_tail->next = sp;
entry->sparse_tail = sp;
}
}





int
archive_entry_sparse_count(struct archive_entry *entry)
{
struct ae_sparse *sp;
int count = 0;

for (sp = entry->sparse_head; sp != NULL; sp = sp->next)
count++;






if (count == 1) {
sp = entry->sparse_head;
if (sp->offset == 0 &&
sp->length >= archive_entry_size(entry)) {
count = 0;
archive_entry_sparse_clear(entry);
}
}

return (count);
}

int
archive_entry_sparse_reset(struct archive_entry * entry)
{
entry->sparse_p = entry->sparse_head;

return archive_entry_sparse_count(entry);
}

int
archive_entry_sparse_next(struct archive_entry * entry,
la_int64_t *offset, la_int64_t *length)
{
if (entry->sparse_p) {
*offset = entry->sparse_p->offset;
*length = entry->sparse_p->length;

entry->sparse_p = entry->sparse_p->next;

return (ARCHIVE_OK);
} else {
*offset = 0;
*length = 0;
return (ARCHIVE_WARN);
}
}




