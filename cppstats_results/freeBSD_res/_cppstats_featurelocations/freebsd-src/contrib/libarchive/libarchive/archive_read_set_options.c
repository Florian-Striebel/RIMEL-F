
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include "archive_read_private.h"
#include "archive_options_private.h"

static int archive_set_format_option(struct archive *a,
const char *m, const char *o, const char *v);
static int archive_set_filter_option(struct archive *a,
const char *m, const char *o, const char *v);
static int archive_set_option(struct archive *a,
const char *m, const char *o, const char *v);

int
archive_read_set_format_option(struct archive *a, const char *m, const char *o,
const char *v)
{
return _archive_set_option(a, m, o, v,
ARCHIVE_READ_MAGIC, "archive_read_set_format_option",
archive_set_format_option);
}

int
archive_read_set_filter_option(struct archive *a, const char *m, const char *o,
const char *v)
{
return _archive_set_option(a, m, o, v,
ARCHIVE_READ_MAGIC, "archive_read_set_filter_option",
archive_set_filter_option);
}

int
archive_read_set_option(struct archive *a, const char *m, const char *o,
const char *v)
{
return _archive_set_option(a, m, o, v,
ARCHIVE_READ_MAGIC, "archive_read_set_option",
archive_set_option);
}

int
archive_read_set_options(struct archive *a, const char *options)
{
return _archive_set_options(a, options,
ARCHIVE_READ_MAGIC, "archive_read_set_options",
archive_set_option);
}

static int
archive_set_format_option(struct archive *_a, const char *m, const char *o,
const char *v)
{
struct archive_read *a = (struct archive_read *)_a;
size_t i;
int r, rv = ARCHIVE_WARN, matched_modules = 0;

for (i = 0; i < sizeof(a->formats)/sizeof(a->formats[0]); i++) {
struct archive_format_descriptor *format = &a->formats[i];

if (format->options == NULL || format->name == NULL)

continue;
if (m != NULL) {
if (strcmp(format->name, m) != 0)
continue;
++matched_modules;
}

a->format = format;
r = format->options(a, o, v);
a->format = NULL;

if (r == ARCHIVE_FATAL)
return (ARCHIVE_FATAL);

if (r == ARCHIVE_OK)
rv = ARCHIVE_OK;
}


if (m != NULL && matched_modules == 0)
return ARCHIVE_WARN - 1;
return (rv);
}

static int
archive_set_filter_option(struct archive *_a, const char *m, const char *o,
const char *v)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter *filter;
struct archive_read_filter_bidder *bidder;
int r, rv = ARCHIVE_WARN, matched_modules = 0;

for (filter = a->filter; filter != NULL; filter = filter->upstream) {
bidder = filter->bidder;
if (bidder == NULL)
continue;
if (bidder->options == NULL)

continue;
if (m != NULL) {
if (strcmp(filter->name, m) != 0)
continue;
++matched_modules;
}

r = bidder->options(bidder, o, v);

if (r == ARCHIVE_FATAL)
return (ARCHIVE_FATAL);

if (r == ARCHIVE_OK)
rv = ARCHIVE_OK;
}


if (m != NULL && matched_modules == 0)
return ARCHIVE_WARN - 1;
return (rv);
}

static int
archive_set_option(struct archive *a, const char *m, const char *o,
const char *v)
{
return _archive_set_either_option(a, m, o, v,
archive_set_format_option,
archive_set_filter_option);
}
