
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include "archive_read_private.h"

static void
add_passphrase_to_tail(struct archive_read *a,
struct archive_read_passphrase *p)
{
*a->passphrases.last = p;
a->passphrases.last = &p->next;
p->next = NULL;
}

static struct archive_read_passphrase *
remove_passphrases_from_head(struct archive_read *a)
{
struct archive_read_passphrase *p;

p = a->passphrases.first;
if (p != NULL)
a->passphrases.first = p->next;
return (p);
}

static void
insert_passphrase_to_head(struct archive_read *a,
struct archive_read_passphrase *p)
{
p->next = a->passphrases.first;
a->passphrases.first = p;
if (&a->passphrases.first == a->passphrases.last) {
a->passphrases.last = &p->next;
p->next = NULL;
}
}

static struct archive_read_passphrase *
new_read_passphrase(struct archive_read *a, const char *passphrase)
{
struct archive_read_passphrase *p;

p = malloc(sizeof(*p));
if (p == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory");
return (NULL);
}
p->passphrase = strdup(passphrase);
if (p->passphrase == NULL) {
free(p);
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory");
return (NULL);
}
return (p);
}

int
archive_read_add_passphrase(struct archive *_a, const char *passphrase)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_passphrase *p;

archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
"archive_read_add_passphrase");

if (passphrase == NULL || passphrase[0] == '\0') {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Empty passphrase is unacceptable");
return (ARCHIVE_FAILED);
}

p = new_read_passphrase(a, passphrase);
if (p == NULL)
return (ARCHIVE_FATAL);
add_passphrase_to_tail(a, p);

return (ARCHIVE_OK);
}

int
archive_read_set_passphrase_callback(struct archive *_a, void *client_data,
archive_passphrase_callback *cb)
{
struct archive_read *a = (struct archive_read *)_a;

archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
"archive_read_set_passphrase_callback");

a->passphrases.callback = cb;
a->passphrases.client_data = client_data;
return (ARCHIVE_OK);
}





void
__archive_read_reset_passphrase(struct archive_read *a)
{

a->passphrases.candidate = -1;
}




const char *
__archive_read_next_passphrase(struct archive_read *a)
{
struct archive_read_passphrase *p;
const char *passphrase;

if (a->passphrases.candidate < 0) {

int cnt = 0;

for (p = a->passphrases.first; p != NULL; p = p->next)
cnt++;
a->passphrases.candidate = cnt;
p = a->passphrases.first;
} else if (a->passphrases.candidate > 1) {

a->passphrases.candidate--;
p = remove_passphrases_from_head(a);
add_passphrase_to_tail(a, p);

p = a->passphrases.first;
} else if (a->passphrases.candidate == 1) {

a->passphrases.candidate = 0;
if (a->passphrases.first->next != NULL) {

p = remove_passphrases_from_head(a);
add_passphrase_to_tail(a, p);
}
p = NULL;
} else
p = NULL;

if (p != NULL)
passphrase = p->passphrase;
else if (a->passphrases.callback != NULL) {



passphrase = a->passphrases.callback(&a->archive,
a->passphrases.client_data);
if (passphrase != NULL) {
p = new_read_passphrase(a, passphrase);
if (p == NULL)
return (NULL);
insert_passphrase_to_head(a, p);
a->passphrases.candidate = 1;
}
} else
passphrase = NULL;

return (passphrase);
}
