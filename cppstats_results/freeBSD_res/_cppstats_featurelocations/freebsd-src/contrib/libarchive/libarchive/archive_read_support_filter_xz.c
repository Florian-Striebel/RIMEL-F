

























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
#if HAVE_LZMA_H
#include <lzma.h>
#endif

#include "archive.h"
#include "archive_endian.h"
#include "archive_private.h"
#include "archive_read_private.h"

#if HAVE_LZMA_H && HAVE_LIBLZMA

struct private_data {
lzma_stream stream;
unsigned char *out_block;
size_t out_block_size;
int64_t total_out;
char eof;
char in_stream;


char lzip_ver;
uint32_t crc32;
int64_t member_in;
int64_t member_out;
};

#if LZMA_VERSION_MAJOR >= 5

#define LZMA_MEMLIMIT UINT64_MAX
#else

#define LZMA_MEMLIMIT (1U << 30)
#endif


static ssize_t xz_filter_read(struct archive_read_filter *, const void **);
static int xz_filter_close(struct archive_read_filter *);
static int xz_lzma_bidder_init(struct archive_read_filter *);

#endif







static int xz_bidder_bid(struct archive_read_filter_bidder *,
struct archive_read_filter *);
static int xz_bidder_init(struct archive_read_filter *);
static int lzma_bidder_bid(struct archive_read_filter_bidder *,
struct archive_read_filter *);
static int lzma_bidder_init(struct archive_read_filter *);
static int lzip_has_member(struct archive_read_filter *);
static int lzip_bidder_bid(struct archive_read_filter_bidder *,
struct archive_read_filter *);
static int lzip_bidder_init(struct archive_read_filter *);

#if ARCHIVE_VERSION_NUMBER < 4000000

int
archive_read_support_compression_xz(struct archive *a)
{
return archive_read_support_filter_xz(a);
}
#endif

int
archive_read_support_filter_xz(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter_bidder *bidder;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_xz");

if (__archive_read_get_bidder(a, &bidder) != ARCHIVE_OK)
return (ARCHIVE_FATAL);

bidder->data = NULL;
bidder->name = "xz";
bidder->bid = xz_bidder_bid;
bidder->init = xz_bidder_init;
bidder->options = NULL;
bidder->free = NULL;
#if HAVE_LZMA_H && HAVE_LIBLZMA
return (ARCHIVE_OK);
#else
archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external xz program for xz decompression");
return (ARCHIVE_WARN);
#endif
}

#if ARCHIVE_VERSION_NUMBER < 4000000
int
archive_read_support_compression_lzma(struct archive *a)
{
return archive_read_support_filter_lzma(a);
}
#endif

int
archive_read_support_filter_lzma(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter_bidder *bidder;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_lzma");

if (__archive_read_get_bidder(a, &bidder) != ARCHIVE_OK)
return (ARCHIVE_FATAL);

bidder->data = NULL;
bidder->name = "lzma";
bidder->bid = lzma_bidder_bid;
bidder->init = lzma_bidder_init;
bidder->options = NULL;
bidder->free = NULL;
#if HAVE_LZMA_H && HAVE_LIBLZMA
return (ARCHIVE_OK);
#else
archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external lzma program for lzma decompression");
return (ARCHIVE_WARN);
#endif
}


#if ARCHIVE_VERSION_NUMBER < 4000000
int
archive_read_support_compression_lzip(struct archive *a)
{
return archive_read_support_filter_lzip(a);
}
#endif

int
archive_read_support_filter_lzip(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter_bidder *bidder;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_lzip");

if (__archive_read_get_bidder(a, &bidder) != ARCHIVE_OK)
return (ARCHIVE_FATAL);

bidder->data = NULL;
bidder->name = "lzip";
bidder->bid = lzip_bidder_bid;
bidder->init = lzip_bidder_init;
bidder->options = NULL;
bidder->free = NULL;
#if HAVE_LZMA_H && HAVE_LIBLZMA
return (ARCHIVE_OK);
#else
archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external lzip program for lzip decompression");
return (ARCHIVE_WARN);
#endif
}




static int
xz_bidder_bid(struct archive_read_filter_bidder *self,
struct archive_read_filter *filter)
{
const unsigned char *buffer;
ssize_t avail;

(void)self;

buffer = __archive_read_filter_ahead(filter, 6, &avail);
if (buffer == NULL)
return (0);




if (memcmp(buffer, "\xFD\x37\x7A\x58\x5A\x00", 6) != 0)
return (0);

return (48);
}













static int
lzma_bidder_bid(struct archive_read_filter_bidder *self,
struct archive_read_filter *filter)
{
const unsigned char *buffer;
ssize_t avail;
uint32_t dicsize;
uint64_t uncompressed_size;
int bits_checked;

(void)self;

buffer = __archive_read_filter_ahead(filter, 14, &avail);
if (buffer == NULL)
return (0);

















bits_checked = 0;
if (buffer[0] > (4 * 5 + 4) * 9 + 8)
return (0);

if (buffer[0] == 0x5d || buffer[0] == 0x5e)
bits_checked += 8;





uncompressed_size = archive_le64dec(buffer+5);
if (uncompressed_size == (uint64_t)ARCHIVE_LITERAL_LL(-1))
bits_checked += 64;








dicsize = archive_le32dec(buffer+1);
switch (dicsize) {
case 0x00001000:
case 0x00002000:
case 0x00004000:
case 0x00008000:
case 0x00010000:

case 0x00020000:
case 0x00040000:
case 0x00080000:

case 0x00100000:

case 0x00200000:

case 0x00400000:

case 0x00800000:

case 0x01000000:

case 0x02000000:

case 0x04000000:

case 0x08000000:
bits_checked += 32;
break;
default:





if (dicsize <= 0x03F00000 && dicsize >= 0x00300000 &&
(dicsize & ((1 << 20)-1)) == 0 &&
bits_checked == 8 + 64) {
bits_checked += 32;
break;
}



return (0);
}




return (bits_checked);
}

static int
lzip_has_member(struct archive_read_filter *filter)
{
const unsigned char *buffer;
ssize_t avail;
int bits_checked;
int log2dic;

buffer = __archive_read_filter_ahead(filter, 6, &avail);
if (buffer == NULL)
return (0);




bits_checked = 0;
if (memcmp(buffer, "LZIP", 4) != 0)
return (0);
bits_checked += 32;


if (buffer[4] != 0 && buffer[4] != 1)
return (0);
bits_checked += 8;


log2dic = buffer[5] & 0x1f;
if (log2dic < 12 || log2dic > 27)
return (0);
bits_checked += 8;

return (bits_checked);
}

static int
lzip_bidder_bid(struct archive_read_filter_bidder *self,
struct archive_read_filter *filter)
{

(void)self;
return (lzip_has_member(filter));
}

#if HAVE_LZMA_H && HAVE_LIBLZMA




static int
xz_bidder_init(struct archive_read_filter *self)
{
self->code = ARCHIVE_FILTER_XZ;
self->name = "xz";
return (xz_lzma_bidder_init(self));
}

static int
lzma_bidder_init(struct archive_read_filter *self)
{
self->code = ARCHIVE_FILTER_LZMA;
self->name = "lzma";
return (xz_lzma_bidder_init(self));
}

static int
lzip_bidder_init(struct archive_read_filter *self)
{
self->code = ARCHIVE_FILTER_LZIP;
self->name = "lzip";
return (xz_lzma_bidder_init(self));
}




static void
set_error(struct archive_read_filter *self, int ret)
{

switch (ret) {
case LZMA_STREAM_END:
case LZMA_OK:
break;
case LZMA_MEM_ERROR:
archive_set_error(&self->archive->archive, ENOMEM,
"Lzma library error: Cannot allocate memory");
break;
case LZMA_MEMLIMIT_ERROR:
archive_set_error(&self->archive->archive, ENOMEM,
"Lzma library error: Out of memory");
break;
case LZMA_FORMAT_ERROR:
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"Lzma library error: format not recognized");
break;
case LZMA_OPTIONS_ERROR:
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"Lzma library error: Invalid options");
break;
case LZMA_DATA_ERROR:
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"Lzma library error: Corrupted input data");
break;
case LZMA_BUF_ERROR:
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"Lzma library error: No progress is possible");
break;
default:

archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"Lzma decompression failed: Unknown error");
break;
}
}




static int
xz_lzma_bidder_init(struct archive_read_filter *self)
{
static const size_t out_block_size = 64 * 1024;
void *out_block;
struct private_data *state;
int ret;

state = (struct private_data *)calloc(sizeof(*state), 1);
out_block = (unsigned char *)malloc(out_block_size);
if (state == NULL || out_block == NULL) {
archive_set_error(&self->archive->archive, ENOMEM,
"Can't allocate data for xz decompression");
free(out_block);
free(state);
return (ARCHIVE_FATAL);
}

self->data = state;
state->out_block_size = out_block_size;
state->out_block = out_block;
self->read = xz_filter_read;
self->skip = NULL;
self->close = xz_filter_close;

state->stream.avail_in = 0;

state->stream.next_out = state->out_block;
state->stream.avail_out = state->out_block_size;

state->crc32 = 0;
if (self->code == ARCHIVE_FILTER_LZIP) {





state->in_stream = 0;
return (ARCHIVE_OK);
} else
state->in_stream = 1;


if (self->code == ARCHIVE_FILTER_XZ)
ret = lzma_stream_decoder(&(state->stream),
LZMA_MEMLIMIT,
LZMA_CONCATENATED);
else
ret = lzma_alone_decoder(&(state->stream),
LZMA_MEMLIMIT);

if (ret == LZMA_OK)
return (ARCHIVE_OK);


set_error(self, ret);

free(state->out_block);
free(state);
self->data = NULL;
return (ARCHIVE_FATAL);
}

static int
lzip_init(struct archive_read_filter *self)
{
struct private_data *state;
const unsigned char *h;
lzma_filter filters[2];
unsigned char props[5];
ssize_t avail_in;
uint32_t dicsize;
int log2dic, ret;

state = (struct private_data *)self->data;
h = __archive_read_filter_ahead(self->upstream, 6, &avail_in);
if (h == NULL)
return (ARCHIVE_FATAL);


state->lzip_ver = h[4];




props[0] = 0x5d;


log2dic = h[5] & 0x1f;
if (log2dic < 12 || log2dic > 27)
return (ARCHIVE_FATAL);
dicsize = 1U << log2dic;
if (log2dic > 12)
dicsize -= (dicsize / 16) * (h[5] >> 5);
archive_le32enc(props+1, dicsize);


__archive_read_filter_consume(self->upstream, 6);
state->member_in = 6;

filters[0].id = LZMA_FILTER_LZMA1;
filters[0].options = NULL;
filters[1].id = LZMA_VLI_UNKNOWN;
filters[1].options = NULL;

ret = lzma_properties_decode(&filters[0], NULL, props, sizeof(props));
if (ret != LZMA_OK) {
set_error(self, ret);
return (ARCHIVE_FATAL);
}
ret = lzma_raw_decoder(&(state->stream), filters);
free(filters[0].options);
if (ret != LZMA_OK) {
set_error(self, ret);
return (ARCHIVE_FATAL);
}
return (ARCHIVE_OK);
}

static int
lzip_tail(struct archive_read_filter *self)
{
struct private_data *state;
const unsigned char *f;
ssize_t avail_in;
int tail;

state = (struct private_data *)self->data;
if (state->lzip_ver == 0)
tail = 12;
else
tail = 20;
f = __archive_read_filter_ahead(self->upstream, tail, &avail_in);
if (f == NULL && avail_in < 0)
return (ARCHIVE_FATAL);
if (f == NULL || avail_in < tail) {
archive_set_error(&self->archive->archive, ARCHIVE_ERRNO_MISC,
"Lzip: Remaining data is less bytes");
return (ARCHIVE_FAILED);
}



if (state->crc32 != archive_le32dec(f)) {
archive_set_error(&self->archive->archive, ARCHIVE_ERRNO_MISC,
"Lzip: CRC32 error");
return (ARCHIVE_FAILED);
}


if ((uint64_t)state->member_out != archive_le64dec(f + 4)) {
archive_set_error(&self->archive->archive, ARCHIVE_ERRNO_MISC,
"Lzip: Uncompressed size error");
return (ARCHIVE_FAILED);
}


if (state->lzip_ver == 1 &&
(uint64_t)state->member_in + tail != archive_le64dec(f + 12)) {
archive_set_error(&self->archive->archive, ARCHIVE_ERRNO_MISC,
"Lzip: Member size error");
return (ARCHIVE_FAILED);
}
__archive_read_filter_consume(self->upstream, tail);



if (lzip_has_member(self->upstream) != 0) {
state->in_stream = 0;
state->crc32 = 0;
state->member_out = 0;
state->member_in = 0;
state->eof = 0;
}
return (ARCHIVE_OK);
}




static ssize_t
xz_filter_read(struct archive_read_filter *self, const void **p)
{
struct private_data *state;
size_t decompressed;
ssize_t avail_in;
int ret;

state = (struct private_data *)self->data;


state->stream.next_out = state->out_block;
state->stream.avail_out = state->out_block_size;


while (state->stream.avail_out > 0 && !state->eof) {
if (!state->in_stream) {



ret = lzip_init(self);
if (ret != ARCHIVE_OK)
return (ret);
state->in_stream = 1;
}
state->stream.next_in =
__archive_read_filter_ahead(self->upstream, 1, &avail_in);
if (state->stream.next_in == NULL && avail_in < 0) {
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"truncated input");
return (ARCHIVE_FATAL);
}
state->stream.avail_in = avail_in;


ret = lzma_code(&(state->stream),
(state->stream.avail_in == 0)? LZMA_FINISH: LZMA_RUN);
switch (ret) {
case LZMA_STREAM_END:
state->eof = 1;

case LZMA_OK:
__archive_read_filter_consume(self->upstream,
avail_in - state->stream.avail_in);
state->member_in +=
avail_in - state->stream.avail_in;
break;
default:
set_error(self, ret);
return (ARCHIVE_FATAL);
}
}

decompressed = state->stream.next_out - state->out_block;
state->total_out += decompressed;
state->member_out += decompressed;
if (decompressed == 0)
*p = NULL;
else {
*p = state->out_block;
if (self->code == ARCHIVE_FILTER_LZIP) {
state->crc32 = lzma_crc32(state->out_block,
decompressed, state->crc32);
if (state->eof) {
ret = lzip_tail(self);
if (ret != ARCHIVE_OK)
return (ret);
}
}
}
return (decompressed);
}




static int
xz_filter_close(struct archive_read_filter *self)
{
struct private_data *state;

state = (struct private_data *)self->data;
lzma_end(&(state->stream));
free(state->out_block);
free(state);
return (ARCHIVE_OK);
}

#else








static int
lzma_bidder_init(struct archive_read_filter *self)
{
int r;

r = __archive_read_program(self, "lzma -d -qq");



self->code = ARCHIVE_FILTER_LZMA;
self->name = "lzma";
return (r);
}

static int
xz_bidder_init(struct archive_read_filter *self)
{
int r;

r = __archive_read_program(self, "xz -d -qq");



self->code = ARCHIVE_FILTER_XZ;
self->name = "xz";
return (r);
}

static int
lzip_bidder_init(struct archive_read_filter *self)
{
int r;

r = __archive_read_program(self, "lzip -d -q");



self->code = ARCHIVE_FILTER_LZIP;
self->name = "lzip";
return (r);
}

#endif
