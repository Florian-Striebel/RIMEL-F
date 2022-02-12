
























#include "archive_platform.h"

__FBSDID("$FreeBSD: head/lib/libarchive/archive_write_set_compression_gzip.c 201081 2009-12-28 02:04:42Z kientzle $");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#include <time.h>
#if defined(HAVE_ZLIB_H)
#include <zlib.h>
#endif

#include "archive.h"
#include "archive_private.h"
#include "archive_string.h"
#include "archive_write_private.h"

#if ARCHIVE_VERSION_NUMBER < 4000000
int
archive_write_set_compression_gzip(struct archive *a)
{
__archive_write_filters_free(a);
return (archive_write_add_filter_gzip(a));
}
#endif



struct private_data {
int compression_level;
int timestamp;
#if defined(HAVE_ZLIB_H)
z_stream stream;
int64_t total_in;
unsigned char *compressed;
size_t compressed_buffer_size;
unsigned long crc;
#else
struct archive_write_program_data *pdata;
#endif
};





#define SET_NEXT_IN(st,src) (st)->stream.next_in = (Bytef *)(uintptr_t)(const void *)(src)


static int archive_compressor_gzip_options(struct archive_write_filter *,
const char *, const char *);
static int archive_compressor_gzip_open(struct archive_write_filter *);
static int archive_compressor_gzip_write(struct archive_write_filter *,
const void *, size_t);
static int archive_compressor_gzip_close(struct archive_write_filter *);
static int archive_compressor_gzip_free(struct archive_write_filter *);
#if defined(HAVE_ZLIB_H)
static int drive_compressor(struct archive_write_filter *,
struct private_data *, int finishing);
#endif





int
archive_write_add_filter_gzip(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct archive_write_filter *f = __archive_write_allocate_filter(_a);
struct private_data *data;
archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_add_filter_gzip");

data = calloc(1, sizeof(*data));
if (data == NULL) {
archive_set_error(&a->archive, ENOMEM, "Out of memory");
return (ARCHIVE_FATAL);
}
f->data = data;
f->open = &archive_compressor_gzip_open;
f->options = &archive_compressor_gzip_options;
f->close = &archive_compressor_gzip_close;
f->free = &archive_compressor_gzip_free;
f->code = ARCHIVE_FILTER_GZIP;
f->name = "gzip";
#if defined(HAVE_ZLIB_H)
data->compression_level = Z_DEFAULT_COMPRESSION;
return (ARCHIVE_OK);
#else
data->pdata = __archive_write_program_allocate("gzip");
if (data->pdata == NULL) {
free(data);
archive_set_error(&a->archive, ENOMEM, "Out of memory");
return (ARCHIVE_FATAL);
}
data->compression_level = 0;
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Using external gzip program");
return (ARCHIVE_WARN);
#endif
}

static int
archive_compressor_gzip_free(struct archive_write_filter *f)
{
struct private_data *data = (struct private_data *)f->data;

#if defined(HAVE_ZLIB_H)
free(data->compressed);
#else
__archive_write_program_free(data->pdata);
#endif
free(data);
f->data = NULL;
return (ARCHIVE_OK);
}




static int
archive_compressor_gzip_options(struct archive_write_filter *f, const char *key,
const char *value)
{
struct private_data *data = (struct private_data *)f->data;

if (strcmp(key, "compression-level") == 0) {
if (value == NULL || !(value[0] >= '0' && value[0] <= '9') ||
value[1] != '\0')
return (ARCHIVE_WARN);
data->compression_level = value[0] - '0';
return (ARCHIVE_OK);
}
if (strcmp(key, "timestamp") == 0) {
data->timestamp = (value == NULL)?-1:1;
return (ARCHIVE_OK);
}




return (ARCHIVE_WARN);
}

#if defined(HAVE_ZLIB_H)



static int
archive_compressor_gzip_open(struct archive_write_filter *f)
{
struct private_data *data = (struct private_data *)f->data;
int ret;

if (data->compressed == NULL) {
size_t bs = 65536, bpb;
if (f->archive->magic == ARCHIVE_WRITE_MAGIC) {


bpb = archive_write_get_bytes_per_block(f->archive);
if (bpb > bs)
bs = bpb;
else if (bpb != 0)
bs -= bs % bpb;
}
data->compressed_buffer_size = bs;
data->compressed
= (unsigned char *)malloc(data->compressed_buffer_size);
if (data->compressed == NULL) {
archive_set_error(f->archive, ENOMEM,
"Can't allocate data for compression buffer");
return (ARCHIVE_FATAL);
}
}

data->crc = crc32(0L, NULL, 0);
data->stream.next_out = data->compressed;
data->stream.avail_out = (uInt)data->compressed_buffer_size;


data->compressed[0] = 0x1f;
data->compressed[1] = 0x8b;
data->compressed[2] = 0x08;
data->compressed[3] = 0;
if (data->timestamp >= 0) {
time_t t = time(NULL);
data->compressed[4] = (uint8_t)(t)&0xff;
data->compressed[5] = (uint8_t)(t>>8)&0xff;
data->compressed[6] = (uint8_t)(t>>16)&0xff;
data->compressed[7] = (uint8_t)(t>>24)&0xff;
} else
memset(&data->compressed[4], 0, 4);
if (data->compression_level == 9)
data->compressed[8] = 2;
else if(data->compression_level == 1)
data->compressed[8] = 4;
else
data->compressed[8] = 0;
data->compressed[9] = 3;
data->stream.next_out += 10;
data->stream.avail_out -= 10;

f->write = archive_compressor_gzip_write;


ret = deflateInit2(&(data->stream),
data->compression_level,
Z_DEFLATED,
-15 ,
8,
Z_DEFAULT_STRATEGY);

if (ret == Z_OK) {
f->data = data;
return (ARCHIVE_OK);
}


archive_set_error(f->archive, ARCHIVE_ERRNO_MISC, "Internal error "
"initializing compression library");


switch (ret) {
case Z_STREAM_ERROR:
archive_set_error(f->archive, ARCHIVE_ERRNO_MISC,
"Internal error initializing "
"compression library: invalid setup parameter");
break;
case Z_MEM_ERROR:
archive_set_error(f->archive, ENOMEM,
"Internal error initializing compression library");
break;
case Z_VERSION_ERROR:
archive_set_error(f->archive, ARCHIVE_ERRNO_MISC,
"Internal error initializing "
"compression library: invalid library version");
break;
}

return (ARCHIVE_FATAL);
}




static int
archive_compressor_gzip_write(struct archive_write_filter *f, const void *buff,
size_t length)
{
struct private_data *data = (struct private_data *)f->data;
int ret;


data->crc = crc32(data->crc, (const Bytef *)buff, (uInt)length);
data->total_in += length;


SET_NEXT_IN(data, buff);
data->stream.avail_in = (uInt)length;
if ((ret = drive_compressor(f, data, 0)) != ARCHIVE_OK)
return (ret);

return (ARCHIVE_OK);
}




static int
archive_compressor_gzip_close(struct archive_write_filter *f)
{
unsigned char trailer[8];
struct private_data *data = (struct private_data *)f->data;
int ret;


ret = drive_compressor(f, data, 1);
if (ret == ARCHIVE_OK) {

ret = __archive_write_filter(f->next_filter,
data->compressed,
data->compressed_buffer_size - data->stream.avail_out);
}
if (ret == ARCHIVE_OK) {

trailer[0] = (uint8_t)(data->crc)&0xff;
trailer[1] = (uint8_t)(data->crc >> 8)&0xff;
trailer[2] = (uint8_t)(data->crc >> 16)&0xff;
trailer[3] = (uint8_t)(data->crc >> 24)&0xff;
trailer[4] = (uint8_t)(data->total_in)&0xff;
trailer[5] = (uint8_t)(data->total_in >> 8)&0xff;
trailer[6] = (uint8_t)(data->total_in >> 16)&0xff;
trailer[7] = (uint8_t)(data->total_in >> 24)&0xff;
ret = __archive_write_filter(f->next_filter, trailer, 8);
}

switch (deflateEnd(&(data->stream))) {
case Z_OK:
break;
default:
archive_set_error(f->archive, ARCHIVE_ERRNO_MISC,
"Failed to clean up compressor");
ret = ARCHIVE_FATAL;
}
return ret;
}








static int
drive_compressor(struct archive_write_filter *f,
struct private_data *data, int finishing)
{
int ret;

for (;;) {
if (data->stream.avail_out == 0) {
ret = __archive_write_filter(f->next_filter,
data->compressed,
data->compressed_buffer_size);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
data->stream.next_out = data->compressed;
data->stream.avail_out =
(uInt)data->compressed_buffer_size;
}


if (!finishing && data->stream.avail_in == 0)
return (ARCHIVE_OK);

ret = deflate(&(data->stream),
finishing ? Z_FINISH : Z_NO_FLUSH );

switch (ret) {
case Z_OK:


if (!finishing && data->stream.avail_in == 0)
return (ARCHIVE_OK);


break;
case Z_STREAM_END:

return (ARCHIVE_OK);
default:

archive_set_error(f->archive, ARCHIVE_ERRNO_MISC,
"GZip compression failed:"
" deflate() call returned status %d",
ret);
return (ARCHIVE_FATAL);
}
}
}

#else

static int
archive_compressor_gzip_open(struct archive_write_filter *f)
{
struct private_data *data = (struct private_data *)f->data;
struct archive_string as;
int r;

archive_string_init(&as);
archive_strcpy(&as, "gzip");


if (data->compression_level > 0) {
archive_strcat(&as, " -");
archive_strappend_char(&as, '0' + data->compression_level);
}
if (data->timestamp < 0)

archive_strcat(&as, " -n");
else if (data->timestamp > 0)

archive_strcat(&as, " -N");

f->write = archive_compressor_gzip_write;
r = __archive_write_program_open(f, data->pdata, as.s);
archive_string_free(&as);
return (r);
}

static int
archive_compressor_gzip_write(struct archive_write_filter *f, const void *buff,
size_t length)
{
struct private_data *data = (struct private_data *)f->data;

return __archive_write_program_write(f, data->pdata, buff, length);
}

static int
archive_compressor_gzip_close(struct archive_write_filter *f)
{
struct private_data *data = (struct private_data *)f->data;

return __archive_write_program_close(f, data->pdata);
}

#endif
