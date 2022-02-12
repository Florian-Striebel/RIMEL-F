
























#include "archive_platform.h"

__FBSDID("$FreeBSD$");


#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "archive.h"
#include "archive_private.h"
#include "archive_read_private.h"

#define LRZIP_HEADER_MAGIC "LRZI"
#define LRZIP_HEADER_MAGIC_LEN 4

static int lrzip_bidder_bid(struct archive_read_filter_bidder *,
struct archive_read_filter *);
static int lrzip_bidder_init(struct archive_read_filter *);


static int
lrzip_reader_free(struct archive_read_filter_bidder *self)
{
(void)self;
return (ARCHIVE_OK);
}

int
archive_read_support_filter_lrzip(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter_bidder *reader;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_lrzip");

if (__archive_read_get_bidder(a, &reader) != ARCHIVE_OK)
return (ARCHIVE_FATAL);

reader->data = NULL;
reader->name = "lrzip";
reader->bid = lrzip_bidder_bid;
reader->init = lrzip_bidder_init;
reader->options = NULL;
reader->free = lrzip_reader_free;

archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external lrzip program for lrzip decompression");
return (ARCHIVE_WARN);
}




static int
lrzip_bidder_bid(struct archive_read_filter_bidder *self,
struct archive_read_filter *filter)
{
const unsigned char *p;
ssize_t avail, len;
int i;

(void)self;


len = 6;
p = __archive_read_filter_ahead(filter, len, &avail);
if (p == NULL || avail == 0)
return (0);

if (memcmp(p, LRZIP_HEADER_MAGIC, LRZIP_HEADER_MAGIC_LEN))
return (0);


if (p[LRZIP_HEADER_MAGIC_LEN])
return 0;

i = p[LRZIP_HEADER_MAGIC_LEN + 1];
if ((i < 6) || (i > 10))
return 0;

return (int)len;
}

static int
lrzip_bidder_init(struct archive_read_filter *self)
{
int r;

r = __archive_read_program(self, "lrzip -d -q");



self->code = ARCHIVE_FILTER_LRZIP;
self->name = "lrzip";
return (r);
}
