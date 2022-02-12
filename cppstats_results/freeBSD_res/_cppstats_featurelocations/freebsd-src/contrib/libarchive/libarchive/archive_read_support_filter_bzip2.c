
























#include "archive_platform.h"

__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_BZLIB_H)
#include <bzlib.h>
#endif

#include "archive.h"
#include "archive_private.h"
#include "archive_read_private.h"

#if defined(HAVE_BZLIB_H) && defined(BZ_CONFIG_ERROR)
struct private_data {
bz_stream stream;
char *out_block;
size_t out_block_size;
char valid;
char eof;
};


static ssize_t bzip2_filter_read(struct archive_read_filter *, const void **);
static int bzip2_filter_close(struct archive_read_filter *);
#endif







static int bzip2_reader_bid(struct archive_read_filter_bidder *, struct archive_read_filter *);
static int bzip2_reader_init(struct archive_read_filter *);
static int bzip2_reader_free(struct archive_read_filter_bidder *);

#if ARCHIVE_VERSION_NUMBER < 4000000

int
archive_read_support_compression_bzip2(struct archive *a)
{
return archive_read_support_filter_bzip2(a);
}
#endif

int
archive_read_support_filter_bzip2(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter_bidder *reader;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_bzip2");

if (__archive_read_get_bidder(a, &reader) != ARCHIVE_OK)
return (ARCHIVE_FATAL);

reader->data = NULL;
reader->name = "bzip2";
reader->bid = bzip2_reader_bid;
reader->init = bzip2_reader_init;
reader->options = NULL;
reader->free = bzip2_reader_free;
#if defined(HAVE_BZLIB_H) && defined(BZ_CONFIG_ERROR)
return (ARCHIVE_OK);
#else
archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external bzip2 program");
return (ARCHIVE_WARN);
#endif
}

static int
bzip2_reader_free(struct archive_read_filter_bidder *self){
(void)self;
return (ARCHIVE_OK);
}








static int
bzip2_reader_bid(struct archive_read_filter_bidder *self, struct archive_read_filter *filter)
{
const unsigned char *buffer;
ssize_t avail;
int bits_checked;

(void)self;


buffer = __archive_read_filter_ahead(filter, 14, &avail);
if (buffer == NULL)
return (0);


bits_checked = 0;
if (memcmp(buffer, "BZh", 3) != 0)
return (0);
bits_checked += 24;


if (buffer[3] < '1' || buffer[3] > '9')
return (0);
bits_checked += 5;




if (memcmp(buffer + 4, "\x31\x41\x59\x26\x53\x59", 6) == 0)
bits_checked += 48;
else if (memcmp(buffer + 4, "\x17\x72\x45\x38\x50\x90", 6) == 0)
bits_checked += 48;
else
return (0);

return (bits_checked);
}

#if !defined(HAVE_BZLIB_H) || !defined(BZ_CONFIG_ERROR)






static int
bzip2_reader_init(struct archive_read_filter *self)
{
int r;

r = __archive_read_program(self, "bzip2 -d");



self->code = ARCHIVE_FILTER_BZIP2;
self->name = "bzip2";
return (r);
}


#else




static int
bzip2_reader_init(struct archive_read_filter *self)
{
static const size_t out_block_size = 64 * 1024;
void *out_block;
struct private_data *state;

self->code = ARCHIVE_FILTER_BZIP2;
self->name = "bzip2";

state = (struct private_data *)calloc(sizeof(*state), 1);
out_block = (unsigned char *)malloc(out_block_size);
if (state == NULL || out_block == NULL) {
archive_set_error(&self->archive->archive, ENOMEM,
"Can't allocate data for bzip2 decompression");
free(out_block);
free(state);
return (ARCHIVE_FATAL);
}

self->data = state;
state->out_block_size = out_block_size;
state->out_block = out_block;
self->read = bzip2_filter_read;
self->skip = NULL;
self->close = bzip2_filter_close;

return (ARCHIVE_OK);
}




static ssize_t
bzip2_filter_read(struct archive_read_filter *self, const void **p)
{
struct private_data *state;
size_t decompressed;
const char *read_buf;
ssize_t ret;

state = (struct private_data *)self->data;

if (state->eof) {
*p = NULL;
return (0);
}


state->stream.next_out = state->out_block;
state->stream.avail_out = state->out_block_size;


for (;;) {
if (!state->valid) {
if (bzip2_reader_bid(self->bidder, self->upstream) == 0) {
state->eof = 1;
*p = state->out_block;
decompressed = state->stream.next_out
- state->out_block;
return (decompressed);
}

ret = BZ2_bzDecompressInit(&(state->stream),
0 ,
0 );


if (ret == BZ_MEM_ERROR)
ret = BZ2_bzDecompressInit(&(state->stream),
0 ,
1 );

if (ret != BZ_OK) {
const char *detail = NULL;
int err = ARCHIVE_ERRNO_MISC;
switch (ret) {
case BZ_PARAM_ERROR:
detail = "invalid setup parameter";
break;
case BZ_MEM_ERROR:
err = ENOMEM;
detail = "out of memory";
break;
case BZ_CONFIG_ERROR:
detail = "mis-compiled library";
break;
}
archive_set_error(&self->archive->archive, err,
"Internal error initializing decompressor%s%s",
detail == NULL ? "" : ": ",
detail);
return (ARCHIVE_FATAL);
}
state->valid = 1;
}



read_buf =
__archive_read_filter_ahead(self->upstream, 1, &ret);
if (read_buf == NULL) {
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"truncated bzip2 input");
return (ARCHIVE_FATAL);
}
state->stream.next_in = (char *)(uintptr_t)read_buf;
state->stream.avail_in = ret;

if (ret == 0) {
state->eof = 1;
*p = state->out_block;
decompressed = state->stream.next_out
- state->out_block;
return (decompressed);
}


ret = BZ2_bzDecompress(&(state->stream));
__archive_read_filter_consume(self->upstream,
state->stream.next_in - read_buf);

switch (ret) {
case BZ_STREAM_END:
switch (BZ2_bzDecompressEnd(&(state->stream))) {
case BZ_OK:
break;
default:
archive_set_error(&(self->archive->archive),
ARCHIVE_ERRNO_MISC,
"Failed to clean up decompressor");
return (ARCHIVE_FATAL);
}
state->valid = 0;

case BZ_OK:

if (state->stream.avail_out == 0) {
*p = state->out_block;
decompressed = state->stream.next_out
- state->out_block;
return (decompressed);
}
break;
default:
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC, "bzip decompression failed");
return (ARCHIVE_FATAL);
}
}
}




static int
bzip2_filter_close(struct archive_read_filter *self)
{
struct private_data *state;
int ret = ARCHIVE_OK;

state = (struct private_data *)self->data;

if (state->valid) {
switch (BZ2_bzDecompressEnd(&state->stream)) {
case BZ_OK:
break;
default:
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"Failed to clean up decompressor");
ret = ARCHIVE_FATAL;
}
state->valid = 0;
}

free(state->out_block);
free(state);
return (ret);
}

#endif
