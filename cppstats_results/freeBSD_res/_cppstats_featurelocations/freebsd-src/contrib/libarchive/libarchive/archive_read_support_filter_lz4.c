
























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
#if defined(HAVE_LZ4_H)
#include <lz4.h>
#endif

#include "archive.h"
#include "archive_endian.h"
#include "archive_private.h"
#include "archive_read_private.h"
#include "archive_xxhash.h"

#define LZ4_MAGICNUMBER 0x184d2204
#define LZ4_SKIPPABLED 0x184d2a50
#define LZ4_LEGACY 0x184c2102

#if defined(HAVE_LIBLZ4)
struct private_data {
enum { SELECT_STREAM,
READ_DEFAULT_STREAM,
READ_DEFAULT_BLOCK,
READ_LEGACY_STREAM,
READ_LEGACY_BLOCK,
} stage;
struct {
unsigned block_independence:1;
unsigned block_checksum:3;
unsigned stream_size:1;
unsigned stream_checksum:1;
unsigned preset_dictionary:1;
int block_maximum_size;
} flags;
int64_t stream_size;
uint32_t dict_id;
char *out_block;
size_t out_block_size;


size_t unconsumed;
size_t decoded_size;
void *xxh32_state;

char valid;
char eof;
};

#define LEGACY_BLOCK_SIZE (8 * 1024 * 1024)


static ssize_t lz4_filter_read(struct archive_read_filter *, const void **);
static int lz4_filter_close(struct archive_read_filter *);
#endif







static int lz4_reader_bid(struct archive_read_filter_bidder *, struct archive_read_filter *);
static int lz4_reader_init(struct archive_read_filter *);
static int lz4_reader_free(struct archive_read_filter_bidder *);
#if defined(HAVE_LIBLZ4)
static ssize_t lz4_filter_read_default_stream(struct archive_read_filter *,
const void **);
static ssize_t lz4_filter_read_legacy_stream(struct archive_read_filter *,
const void **);
#endif

int
archive_read_support_filter_lz4(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct archive_read_filter_bidder *reader;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_filter_lz4");

if (__archive_read_get_bidder(a, &reader) != ARCHIVE_OK)
return (ARCHIVE_FATAL);

reader->data = NULL;
reader->name = "lz4";
reader->bid = lz4_reader_bid;
reader->init = lz4_reader_init;
reader->options = NULL;
reader->free = lz4_reader_free;
#if defined(HAVE_LIBLZ4)
return (ARCHIVE_OK);
#else
archive_set_error(_a, ARCHIVE_ERRNO_MISC,
"Using external lz4 program");
return (ARCHIVE_WARN);
#endif
}

static int
lz4_reader_free(struct archive_read_filter_bidder *self){
(void)self;
return (ARCHIVE_OK);
}








static int
lz4_reader_bid(struct archive_read_filter_bidder *self,
struct archive_read_filter *filter)
{
const unsigned char *buffer;
ssize_t avail;
int bits_checked;
uint32_t number;

(void)self;


buffer = __archive_read_filter_ahead(filter, 11, &avail);
if (buffer == NULL)
return (0);


bits_checked = 0;
if ((number = archive_le32dec(buffer)) == LZ4_MAGICNUMBER) {
unsigned char flag, BD;

bits_checked += 32;


flag = buffer[4];

if (((flag & 0xc0) >> 6) != 1)
return (0);

if (flag & 2)
return (0);
bits_checked += 8;
BD = buffer[5];

if (((BD & 0x70) >> 4) < 4)
return (0);

if (BD & ~0x70)
return (0);
bits_checked += 8;
} else if (number == LZ4_LEGACY) {
bits_checked += 32;
}

return (bits_checked);
}

#if !defined(HAVE_LIBLZ4)






static int
lz4_reader_init(struct archive_read_filter *self)
{
int r;

r = __archive_read_program(self, "lz4 -d -q");



self->code = ARCHIVE_FILTER_LZ4;
self->name = "lz4";
return (r);
}


#else




static int
lz4_reader_init(struct archive_read_filter *self)
{
struct private_data *state;

self->code = ARCHIVE_FILTER_LZ4;
self->name = "lz4";

state = (struct private_data *)calloc(sizeof(*state), 1);
if (state == NULL) {
archive_set_error(&self->archive->archive, ENOMEM,
"Can't allocate data for lz4 decompression");
return (ARCHIVE_FATAL);
}

self->data = state;
state->stage = SELECT_STREAM;
self->read = lz4_filter_read;
self->skip = NULL;
self->close = lz4_filter_close;

return (ARCHIVE_OK);
}

static int
lz4_allocate_out_block(struct archive_read_filter *self)
{
struct private_data *state = (struct private_data *)self->data;
size_t out_block_size = state->flags.block_maximum_size;
void *out_block;

if (!state->flags.block_independence)
out_block_size += 64 * 1024;
if (state->out_block_size < out_block_size) {
free(state->out_block);
out_block = (unsigned char *)malloc(out_block_size);
state->out_block_size = out_block_size;
if (out_block == NULL) {
archive_set_error(&self->archive->archive, ENOMEM,
"Can't allocate data for lz4 decompression");
return (ARCHIVE_FATAL);
}
state->out_block = out_block;
}
if (!state->flags.block_independence)
memset(state->out_block, 0, 64 * 1024);
return (ARCHIVE_OK);
}

static int
lz4_allocate_out_block_for_legacy(struct archive_read_filter *self)
{
struct private_data *state = (struct private_data *)self->data;
size_t out_block_size = LEGACY_BLOCK_SIZE;
void *out_block;

if (state->out_block_size < out_block_size) {
free(state->out_block);
out_block = (unsigned char *)malloc(out_block_size);
state->out_block_size = out_block_size;
if (out_block == NULL) {
archive_set_error(&self->archive->archive, ENOMEM,
"Can't allocate data for lz4 decompression");
return (ARCHIVE_FATAL);
}
state->out_block = out_block;
}
return (ARCHIVE_OK);
}




static ssize_t
lz4_filter_read(struct archive_read_filter *self, const void **p)
{
struct private_data *state = (struct private_data *)self->data;
ssize_t ret;

if (state->eof) {
*p = NULL;
return (0);
}

__archive_read_filter_consume(self->upstream, state->unconsumed);
state->unconsumed = 0;

switch (state->stage) {
case SELECT_STREAM:
break;
case READ_DEFAULT_STREAM:
case READ_LEGACY_STREAM:

archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC, "Invalid sequence.");
return (ARCHIVE_FATAL);
case READ_DEFAULT_BLOCK:
ret = lz4_filter_read_default_stream(self, p);
if (ret != 0 || state->stage != SELECT_STREAM)
return ret;
break;
case READ_LEGACY_BLOCK:
ret = lz4_filter_read_legacy_stream(self, p);
if (ret != 0 || state->stage != SELECT_STREAM)
return ret;
break;
default:
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC, "Program error.");
return (ARCHIVE_FATAL);
break;
}

while (state->stage == SELECT_STREAM) {
const char *read_buf;


read_buf = __archive_read_filter_ahead(self->upstream, 4,
NULL);
if (read_buf == NULL) {
state->eof = 1;
*p = NULL;
return (0);
}
uint32_t number = archive_le32dec(read_buf);
__archive_read_filter_consume(self->upstream, 4);
if (number == LZ4_MAGICNUMBER)
return lz4_filter_read_default_stream(self, p);
else if (number == LZ4_LEGACY)
return lz4_filter_read_legacy_stream(self, p);
else if ((number & ~0xF) == LZ4_SKIPPABLED) {
read_buf = __archive_read_filter_ahead(
self->upstream, 4, NULL);
if (read_buf == NULL) {
archive_set_error(
&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"Malformed lz4 data");
return (ARCHIVE_FATAL);
}
uint32_t skip_bytes = archive_le32dec(read_buf);
__archive_read_filter_consume(self->upstream,
4 + skip_bytes);
} else {

state->eof = 1;
*p = NULL;
return (0);
}
}
state->eof = 1;
*p = NULL;
return (0);
}

static int
lz4_filter_read_descriptor(struct archive_read_filter *self)
{
struct private_data *state = (struct private_data *)self->data;
const char *read_buf;
ssize_t bytes_remaining;
ssize_t descriptor_bytes;
unsigned char flag, bd;
unsigned int chsum, chsum_verifier;


read_buf = __archive_read_filter_ahead(self->upstream, 2,
&bytes_remaining);
if (read_buf == NULL) {
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"truncated lz4 input");
return (ARCHIVE_FATAL);
}




flag = (unsigned char)read_buf[0];

if ((flag & 0xc0) != 1<<6)
goto malformed_error;

if (flag & 0x02)
goto malformed_error;
state->flags.block_independence = (flag & 0x20) != 0;
state->flags.block_checksum = (flag & 0x10)?4:0;
state->flags.stream_size = (flag & 0x08) != 0;
state->flags.stream_checksum = (flag & 0x04) != 0;
state->flags.preset_dictionary = (flag & 0x01) != 0;


bd = (unsigned char)read_buf[1];

if (bd & 0x8f)
goto malformed_error;

switch (read_buf[1] >> 4) {
case 4:
state->flags.block_maximum_size = 64 * 1024;
break;
case 5:
state->flags.block_maximum_size = 256 * 1024;
break;
case 6:
state->flags.block_maximum_size = 1024 * 1024;
break;
case 7:
state->flags.block_maximum_size = 4 * 1024 * 1024;
break;
default:
goto malformed_error;
}


descriptor_bytes = 3;
if (state->flags.stream_size)
descriptor_bytes += 8;
if (state->flags.preset_dictionary)
descriptor_bytes += 4;
if (bytes_remaining < descriptor_bytes) {
read_buf = __archive_read_filter_ahead(self->upstream,
descriptor_bytes, &bytes_remaining);
if (read_buf == NULL) {
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"truncated lz4 input");
return (ARCHIVE_FATAL);
}
}

chsum = __archive_xxhash.XXH32(read_buf, (int)descriptor_bytes -1, 0);
chsum = (chsum >> 8) & 0xff;
chsum_verifier = read_buf[descriptor_bytes-1] & 0xff;
if (chsum != chsum_verifier)
goto malformed_error;

__archive_read_filter_consume(self->upstream, descriptor_bytes);


if (lz4_allocate_out_block(self) != ARCHIVE_OK)
return (ARCHIVE_FATAL);
if (state->flags.stream_checksum)
state->xxh32_state = __archive_xxhash.XXH32_init(0);

state->decoded_size = 0;

return (ARCHIVE_OK);
malformed_error:
archive_set_error(&self->archive->archive, ARCHIVE_ERRNO_MISC,
"malformed lz4 data");
return (ARCHIVE_FATAL);
}

static ssize_t
lz4_filter_read_data_block(struct archive_read_filter *self, const void **p)
{
struct private_data *state = (struct private_data *)self->data;
ssize_t compressed_size;
const char *read_buf;
ssize_t bytes_remaining;
int checksum_size;
ssize_t uncompressed_size;
size_t prefix64k;

*p = NULL;


read_buf = __archive_read_filter_ahead(self->upstream, 4,
&bytes_remaining);
if (read_buf == NULL)
goto truncated_error;
compressed_size = archive_le32dec(read_buf);
if ((compressed_size & 0x7fffffff) > state->flags.block_maximum_size)
goto malformed_error;

if (compressed_size == 0) {
__archive_read_filter_consume(self->upstream, 4);
return 0;
}

checksum_size = state->flags.block_checksum;

if (compressed_size & 0x80000000U) {
compressed_size &= 0x7fffffff;
uncompressed_size = compressed_size;
} else
uncompressed_size = 0;






read_buf = __archive_read_filter_ahead(self->upstream,
4 + compressed_size + checksum_size, &bytes_remaining);
if (read_buf == NULL)
goto truncated_error;


if (checksum_size) {
unsigned int chsum = __archive_xxhash.XXH32(
read_buf + 4, (int)compressed_size, 0);
unsigned int chsum_block =
archive_le32dec(read_buf + 4 + compressed_size);
if (chsum != chsum_block)
goto malformed_error;
}



if (uncompressed_size) {

if (!state->flags.block_independence) {
prefix64k = 64 * 1024;
if (uncompressed_size < (ssize_t)prefix64k) {
memcpy(state->out_block
+ prefix64k - uncompressed_size,
read_buf + 4,
uncompressed_size);
memset(state->out_block, 0,
prefix64k - uncompressed_size);
} else {
memcpy(state->out_block,
read_buf + 4
+ uncompressed_size - prefix64k,
prefix64k);
}
state->decoded_size = 0;
}
state->unconsumed = 4 + uncompressed_size + checksum_size;
*p = read_buf + 4;
return uncompressed_size;
}




if (state->flags.block_independence) {
prefix64k = 0;
uncompressed_size = LZ4_decompress_safe(read_buf + 4,
state->out_block, (int)compressed_size,
state->flags.block_maximum_size);
} else {
prefix64k = 64 * 1024;
if (state->decoded_size) {
if (state->decoded_size < prefix64k) {
memmove(state->out_block
+ prefix64k - state->decoded_size,
state->out_block + prefix64k,
state->decoded_size);
memset(state->out_block, 0,
prefix64k - state->decoded_size);
} else {
memmove(state->out_block,
state->out_block + state->decoded_size,
prefix64k);
}
}
#if LZ4_VERSION_MAJOR >= 1 && LZ4_VERSION_MINOR >= 7
uncompressed_size = LZ4_decompress_safe_usingDict(
read_buf + 4,
state->out_block + prefix64k, (int)compressed_size,
state->flags.block_maximum_size,
state->out_block,
prefix64k);
#else
uncompressed_size = LZ4_decompress_safe_withPrefix64k(
read_buf + 4,
state->out_block + prefix64k, (int)compressed_size,
state->flags.block_maximum_size);
#endif
}


if (uncompressed_size < 0) {
archive_set_error(&(self->archive->archive),
ARCHIVE_ERRNO_MISC, "lz4 decompression failed");
return (ARCHIVE_FATAL);
}

state->unconsumed = 4 + compressed_size + checksum_size;
*p = state->out_block + prefix64k;
state->decoded_size = uncompressed_size;
return uncompressed_size;

malformed_error:
archive_set_error(&self->archive->archive, ARCHIVE_ERRNO_MISC,
"malformed lz4 data");
return (ARCHIVE_FATAL);
truncated_error:
archive_set_error(&self->archive->archive, ARCHIVE_ERRNO_MISC,
"truncated lz4 input");
return (ARCHIVE_FATAL);
}

static ssize_t
lz4_filter_read_default_stream(struct archive_read_filter *self, const void **p)
{
struct private_data *state = (struct private_data *)self->data;
const char *read_buf;
ssize_t bytes_remaining;
ssize_t ret;

if (state->stage == SELECT_STREAM) {
state->stage = READ_DEFAULT_STREAM;

if((ret = lz4_filter_read_descriptor(self)) != ARCHIVE_OK)
return (ret);
state->stage = READ_DEFAULT_BLOCK;
}

ret = lz4_filter_read_data_block(self, p);



if (ret == 0 && *p == NULL)
state->stage = SELECT_STREAM;


if (state->flags.stream_checksum) {
if (state->stage == SELECT_STREAM) {
unsigned int checksum;
unsigned int checksum_stream;
read_buf = __archive_read_filter_ahead(self->upstream,
4, &bytes_remaining);
if (read_buf == NULL) {
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC, "truncated lz4 input");
return (ARCHIVE_FATAL);
}
checksum = archive_le32dec(read_buf);
__archive_read_filter_consume(self->upstream, 4);
checksum_stream = __archive_xxhash.XXH32_digest(
state->xxh32_state);
state->xxh32_state = NULL;
if (checksum != checksum_stream) {
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"lz4 stream checksum error");
return (ARCHIVE_FATAL);
}
} else if (ret > 0)
__archive_xxhash.XXH32_update(state->xxh32_state,
*p, (int)ret);
}
return (ret);
}

static ssize_t
lz4_filter_read_legacy_stream(struct archive_read_filter *self, const void **p)
{
struct private_data *state = (struct private_data *)self->data;
uint32_t compressed;
const char *read_buf;
ssize_t ret;

*p = NULL;
ret = lz4_allocate_out_block_for_legacy(self);
if (ret != ARCHIVE_OK)
return ret;


read_buf = __archive_read_filter_ahead(self->upstream, 4, NULL);
if (read_buf == NULL) {
if (state->stage == SELECT_STREAM) {
state->stage = READ_LEGACY_STREAM;
archive_set_error(&self->archive->archive,
ARCHIVE_ERRNO_MISC,
"truncated lz4 input");
return (ARCHIVE_FATAL);
}
state->stage = SELECT_STREAM;
return 0;
}
state->stage = READ_LEGACY_BLOCK;
compressed = archive_le32dec(read_buf);
if (compressed > LZ4_COMPRESSBOUND(LEGACY_BLOCK_SIZE)) {
state->stage = SELECT_STREAM;
return 0;
}


read_buf = __archive_read_filter_ahead(self->upstream,
4 + compressed, NULL);
if (read_buf == NULL) {
archive_set_error(&(self->archive->archive),
ARCHIVE_ERRNO_MISC, "truncated lz4 input");
return (ARCHIVE_FATAL);
}
ret = LZ4_decompress_safe(read_buf + 4, state->out_block,
compressed, (int)state->out_block_size);
if (ret < 0) {
archive_set_error(&(self->archive->archive),
ARCHIVE_ERRNO_MISC, "lz4 decompression failed");
return (ARCHIVE_FATAL);
}
*p = state->out_block;
state->unconsumed = 4 + compressed;
return ret;
}




static int
lz4_filter_close(struct archive_read_filter *self)
{
struct private_data *state;
int ret = ARCHIVE_OK;

state = (struct private_data *)self->data;
free(state->xxh32_state);
free(state->out_block);
free(state);
return (ret);
}

#endif
