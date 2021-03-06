

























































#include "archive_platform.h"

__FBSDID("$FreeBSD: head/lib/libarchive/archive_write_set_compression_compress.c 201111 2009-12-28 03:33:05Z kientzle $");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "archive.h"
#include "archive_private.h"
#include "archive_write_private.h"

#define HSIZE 69001
#define HSHIFT 8
#define CHECK_GAP 10000

#define MAXCODE(bits) ((1 << (bits)) - 1)





#define FIRST 257
#define CLEAR 256

struct private_data {
int64_t in_count, out_count, checkpoint;

int code_len;
int cur_maxcode;
int max_maxcode;
int hashtab [HSIZE];
unsigned short codetab [HSIZE];
int first_free;
int compress_ratio;

int cur_code, cur_fcode;

int bit_offset;
unsigned char bit_buf;

unsigned char *compressed;
size_t compressed_buffer_size;
size_t compressed_offset;
};

static int archive_compressor_compress_open(struct archive_write_filter *);
static int archive_compressor_compress_write(struct archive_write_filter *,
const void *, size_t);
static int archive_compressor_compress_close(struct archive_write_filter *);
static int archive_compressor_compress_free(struct archive_write_filter *);

#if ARCHIVE_VERSION_NUMBER < 4000000
int
archive_write_set_compression_compress(struct archive *a)
{
__archive_write_filters_free(a);
return (archive_write_add_filter_compress(a));
}
#endif




int
archive_write_add_filter_compress(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct archive_write_filter *f = __archive_write_allocate_filter(_a);

archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_add_filter_compress");
f->open = &archive_compressor_compress_open;
f->code = ARCHIVE_FILTER_COMPRESS;
f->name = "compress";
return (ARCHIVE_OK);
}




static int
archive_compressor_compress_open(struct archive_write_filter *f)
{
struct private_data *state;
size_t bs = 65536, bpb;

f->code = ARCHIVE_FILTER_COMPRESS;
f->name = "compress";

state = (struct private_data *)calloc(1, sizeof(*state));
if (state == NULL) {
archive_set_error(f->archive, ENOMEM,
"Can't allocate data for compression");
return (ARCHIVE_FATAL);
}

if (f->archive->magic == ARCHIVE_WRITE_MAGIC) {


bpb = archive_write_get_bytes_per_block(f->archive);
if (bpb > bs)
bs = bpb;
else if (bpb != 0)
bs -= bs % bpb;
}
state->compressed_buffer_size = bs;
state->compressed = malloc(state->compressed_buffer_size);

if (state->compressed == NULL) {
archive_set_error(f->archive, ENOMEM,
"Can't allocate data for compression buffer");
free(state);
return (ARCHIVE_FATAL);
}

f->write = archive_compressor_compress_write;
f->close = archive_compressor_compress_close;
f->free = archive_compressor_compress_free;

state->max_maxcode = 0x10000;
state->in_count = 0;
state->bit_buf = 0;
state->bit_offset = 0;
state->out_count = 3;
state->compress_ratio = 0;
state->checkpoint = CHECK_GAP;
state->code_len = 9;
state->cur_maxcode = MAXCODE(state->code_len);
state->first_free = FIRST;

memset(state->hashtab, 0xff, sizeof(state->hashtab));


state->compressed[0] = 0x1f;
state->compressed[1] = 0x9d;
state->compressed[2] = 0x90;
state->compressed_offset = 3;

f->data = state;
return (0);
}
















static const unsigned char rmask[9] =
{0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

static int
output_byte(struct archive_write_filter *f, unsigned char c)
{
struct private_data *state = f->data;

state->compressed[state->compressed_offset++] = c;
++state->out_count;

if (state->compressed_buffer_size == state->compressed_offset) {
int ret = __archive_write_filter(f->next_filter,
state->compressed, state->compressed_buffer_size);
if (ret != ARCHIVE_OK)
return ARCHIVE_FATAL;
state->compressed_offset = 0;
}

return ARCHIVE_OK;
}

static int
output_code(struct archive_write_filter *f, int ocode)
{
struct private_data *state = f->data;
int bits, ret, clear_flg, bit_offset;

clear_flg = ocode == CLEAR;





bit_offset = state->bit_offset % 8;
state->bit_buf |= (ocode << bit_offset) & 0xff;
output_byte(f, state->bit_buf);

bits = state->code_len - (8 - bit_offset);
ocode >>= 8 - bit_offset;

if (bits >= 8) {
output_byte(f, ocode & 0xff);
ocode >>= 8;
bits -= 8;
}

state->bit_offset += state->code_len;
state->bit_buf = ocode & rmask[bits];
if (state->bit_offset == state->code_len * 8)
state->bit_offset = 0;





if (clear_flg || state->first_free > state->cur_maxcode) {




if (state->bit_offset > 0) {
while (state->bit_offset < state->code_len * 8) {
ret = output_byte(f, state->bit_buf);
if (ret != ARCHIVE_OK)
return ret;
state->bit_offset += 8;
state->bit_buf = 0;
}
}
state->bit_buf = 0;
state->bit_offset = 0;

if (clear_flg) {
state->code_len = 9;
state->cur_maxcode = MAXCODE(state->code_len);
} else {
state->code_len++;
if (state->code_len == 16)
state->cur_maxcode = state->max_maxcode;
else
state->cur_maxcode = MAXCODE(state->code_len);
}
}

return (ARCHIVE_OK);
}

static int
output_flush(struct archive_write_filter *f)
{
struct private_data *state = f->data;
int ret;


if (state->bit_offset % 8) {
state->code_len = (state->bit_offset % 8 + 7) / 8;
ret = output_byte(f, state->bit_buf);
if (ret != ARCHIVE_OK)
return ret;
}

return (ARCHIVE_OK);
}




static int
archive_compressor_compress_write(struct archive_write_filter *f,
const void *buff, size_t length)
{
struct private_data *state = (struct private_data *)f->data;
int i;
int ratio;
int c, disp, ret;
const unsigned char *bp;

if (length == 0)
return ARCHIVE_OK;

bp = buff;

if (state->in_count == 0) {
state->cur_code = *bp++;
++state->in_count;
--length;
}

while (length--) {
c = *bp++;
state->in_count++;
state->cur_fcode = (c << 16) + state->cur_code;
i = ((c << HSHIFT) ^ state->cur_code);

if (state->hashtab[i] == state->cur_fcode) {
state->cur_code = state->codetab[i];
continue;
}
if (state->hashtab[i] < 0)
goto nomatch;

if (i == 0)
disp = 1;
else
disp = HSIZE - i;
probe:
if ((i -= disp) < 0)
i += HSIZE;

if (state->hashtab[i] == state->cur_fcode) {
state->cur_code = state->codetab[i];
continue;
}
if (state->hashtab[i] >= 0)
goto probe;
nomatch:
ret = output_code(f, state->cur_code);
if (ret != ARCHIVE_OK)
return ret;
state->cur_code = c;
if (state->first_free < state->max_maxcode) {
state->codetab[i] = state->first_free++;
state->hashtab[i] = state->cur_fcode;
continue;
}
if (state->in_count < state->checkpoint)
continue;

state->checkpoint = state->in_count + CHECK_GAP;

if (state->in_count <= 0x007fffff && state->out_count != 0)
ratio = (int)(state->in_count * 256 / state->out_count);
else if ((ratio = (int)(state->out_count / 256)) == 0)
ratio = 0x7fffffff;
else
ratio = (int)(state->in_count / ratio);

if (ratio > state->compress_ratio)
state->compress_ratio = ratio;
else {
state->compress_ratio = 0;
memset(state->hashtab, 0xff, sizeof(state->hashtab));
state->first_free = FIRST;
ret = output_code(f, CLEAR);
if (ret != ARCHIVE_OK)
return ret;
}
}

return (ARCHIVE_OK);
}





static int
archive_compressor_compress_close(struct archive_write_filter *f)
{
struct private_data *state = (struct private_data *)f->data;
int ret;

ret = output_code(f, state->cur_code);
if (ret != ARCHIVE_OK)
return ret;
ret = output_flush(f);
if (ret != ARCHIVE_OK)
return ret;


ret = __archive_write_filter(f->next_filter,
state->compressed, state->compressed_offset);
return (ret);
}

static int
archive_compressor_compress_free(struct archive_write_filter *f)
{
struct private_data *state = (struct private_data *)f->data;

free(state->compressed);
free(state);
return (ARCHIVE_OK);
}
