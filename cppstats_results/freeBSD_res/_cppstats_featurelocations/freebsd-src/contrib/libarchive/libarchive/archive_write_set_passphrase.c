
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include "archive_write_private.h"

int
archive_write_set_passphrase(struct archive *_a, const char *p)
{
struct archive_write *a = (struct archive_write *)_a;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC, ARCHIVE_STATE_NEW,
"archive_write_set_passphrase");

if (p == NULL || p[0] == '\0') {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Empty passphrase is unacceptable");
return (ARCHIVE_FAILED);
}
free(a->passphrase);
a->passphrase = strdup(p);
if (a->passphrase == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate data for passphrase");
return (ARCHIVE_FATAL);
}
return (ARCHIVE_OK);
}


int
archive_write_set_passphrase_callback(struct archive *_a, void *client_data,
archive_passphrase_callback *cb)
{
struct archive_write *a = (struct archive_write *)_a;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC, ARCHIVE_STATE_NEW,
"archive_write_set_passphrase_callback");

a->passphrase_callback = cb;
a->passphrase_client_data = client_data;
return (ARCHIVE_OK);
}


const char *
__archive_write_get_passphrase(struct archive_write *a)
{

if (a->passphrase != NULL)
return (a->passphrase);

if (a->passphrase_callback != NULL) {
const char *p;
p = a->passphrase_callback(&a->archive,
a->passphrase_client_data);
if (p != NULL) {
a->passphrase = strdup(p);
if (a->passphrase == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate data for passphrase");
return (NULL);
}
return (a->passphrase);
}
}
return (NULL);
}
