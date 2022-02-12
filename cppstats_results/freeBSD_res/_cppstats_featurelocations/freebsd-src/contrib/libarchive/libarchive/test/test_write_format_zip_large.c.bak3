























#include "test.h"
__FBSDID("$FreeBSD$");

#include <errno.h>
#include <stdlib.h>
#include <string.h>









static size_t nullsize;
static void *nulldata;

struct fileblock {
struct fileblock *next;
int size;
void *buff;
int64_t gap_size;
};

struct fileblocks {
int64_t filesize;
int64_t fileposition;
int64_t gap_remaining;
void *buff;
struct fileblock *first;
struct fileblock *current;
struct fileblock *last;
};


#define KB ((int64_t)1024)
#define MB ((int64_t)1024 * KB)
#define GB ((int64_t)1024 * MB)
#define TB ((int64_t)1024 * GB)

static int64_t memory_read_skip(struct archive *, void *, int64_t request);
static ssize_t memory_read(struct archive *, void *, const void **buff);
static ssize_t memory_write(struct archive *, void *, const void *, size_t);

static uint16_t le16(const void *_p) {
const uint8_t *p = _p;
return p[0] | (p[1] << 8);
}

static uint32_t le32(const void *_p) {
const uint8_t *p = _p;
return le16(p) | ((uint32_t)le16(p + 2) << 16);
}

static uint64_t le64(const void *_p) {
const uint8_t *p = _p;
return le32(p) | ((uint64_t)le32(p + 4) << 32);
}

static ssize_t
memory_write(struct archive *a, void *_private, const void *buff, size_t size)
{
struct fileblocks *private = _private;
struct fileblock *block;

(void)a;

if ((const char *)nulldata <= (const char *)buff
&& (const char *)buff < (const char *)nulldata + nullsize) {

private->last->gap_size += (int64_t)size;
} else {


block = (struct fileblock *)malloc(sizeof(*block));
memset(block, 0, sizeof(*block));
block->size = (int)size;
block->buff = malloc(size);
memcpy(block->buff, buff, size);
if (private->last == NULL) {
private->first = private->last = block;
} else {
private->last->next = block;
private->last = block;
}
block->next = NULL;
}
private->filesize += size;
return ((long)size);
}

static ssize_t
memory_read(struct archive *a, void *_private, const void **buff)
{
struct fileblocks *private = _private;
ssize_t size;

(void)a;

while (private->current != NULL && private->buff == NULL && private->gap_remaining == 0) {
private->current = private->current->next;
if (private->current != NULL) {
private->buff = private->current->buff;
private->gap_remaining = private->current->gap_size;
}
}

if (private->current == NULL)
return (0);


if (private->buff != NULL) {
*buff = private->buff;
size = ((char *)private->current->buff + private->current->size)
- (char *)private->buff;
private->buff = NULL;
private->fileposition += size;
return (size);
}


if (private->gap_remaining > (int64_t)nullsize) {
private->gap_remaining -= nullsize;
*buff = nulldata;
private->fileposition += nullsize;
return (nullsize);
}


if (private->gap_remaining > 0) {
size = (ssize_t)private->gap_remaining;
*buff = nulldata;
private->gap_remaining = 0;
private->fileposition += size;

private->current = private->current->next;
if (private->current != NULL) {
private->buff = private->current->buff;
private->gap_remaining = private->current->gap_size;
}

return (size);
}
fprintf(stderr, "\n\n\nInternal failure\n\n\n");
exit(1);
}

static int
memory_read_open(struct archive *a, void *_private)
{
struct fileblocks *private = _private;

(void)a;

private->current = private->first;
private->fileposition = 0;
if (private->current != NULL) {
private->buff = private->current->buff;
private->gap_remaining = private->current->gap_size;
}
return (ARCHIVE_OK);
}

static int64_t
memory_read_seek(struct archive *a, void *_private, int64_t offset, int whence)
{
struct fileblocks *private = _private;

(void)a;
if (whence == SEEK_END) {
offset = private->filesize + offset;
} else if (whence == SEEK_CUR) {
offset = private->fileposition + offset;
}

if (offset < 0) {
fprintf(stderr, "\n\n\nInternal failure: negative seek\n\n\n");
exit(1);
}


private->fileposition = offset;


offset = 0;
private->current = private->first;
while (private->current != NULL) {
if (offset + private->current->size > private->fileposition) {

private->buff = (char *)private->current->buff
+ private->fileposition - offset;
private->gap_remaining = private->current->gap_size;
return private->fileposition;
}
offset += private->current->size;
if (offset + private->current->gap_size > private->fileposition) {

private->buff = NULL;
private->gap_remaining = private->current->gap_size
- (private->fileposition - offset);
return private->fileposition;
}
offset += private->current->gap_size;

private->current = private->current->next;
}
if (private->fileposition == private->filesize) {
return private->fileposition;
}
fprintf(stderr, "\n\n\nInternal failure: over-sized seek\n\n\n");
exit(1);
}

static int64_t
memory_read_skip(struct archive *a, void *_private, int64_t skip)
{
struct fileblocks *private = _private;
int64_t old_position = private->fileposition;
int64_t new_position = memory_read_seek(a, _private, skip, SEEK_CUR);
return (new_position - old_position);
}

static struct fileblocks *
fileblocks_new(void)
{
struct fileblocks *fileblocks;

fileblocks = calloc(1, sizeof(struct fileblocks));
return fileblocks;
}

static void
fileblocks_free(struct fileblocks *fileblocks)
{
while (fileblocks->first != NULL) {
struct fileblock *b = fileblocks->first;
fileblocks->first = fileblocks->first->next;
free(b->buff);
free(b);
}
free(fileblocks);
}



static int64_t test_sizes[] = {

2 * GB - 1, 2 * GB, 2 * GB + 1,

4 * GB - 1, 4 * GB, 4 * GB + 1,

16 * GB - 1, 16 * GB, 16 * GB + 1,
64 * GB - 1, 64 * GB, 64 * GB + 1,
256 * GB - 1, 256 * GB, 256 * GB + 1,
1 * TB,
0
};


static void
verify_large_zip(struct archive *a, struct fileblocks *fileblocks)
{
char namebuff[64];
struct archive_entry *ae;
int i;

assertEqualIntA(a, ARCHIVE_OK,
archive_read_set_options(a, "zip:ignorecrc32"));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_set_open_callback(a, memory_read_open));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_set_read_callback(a, memory_read));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_set_skip_callback(a, memory_read_skip));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_set_seek_callback(a, memory_read_seek));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_set_callback_data(a, fileblocks));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open1(a));




for (i = 0; test_sizes[i] > 0; i++) {
assertEqualIntA(a, ARCHIVE_OK,
archive_read_next_header(a, &ae));
sprintf(namebuff, "file_%d", i);
assertEqualString(namebuff, archive_entry_pathname(ae));
assertEqualInt(test_sizes[i], archive_entry_size(ae));
}
assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualString("lastfile", archive_entry_pathname(ae));

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
}

DEFINE_TEST(test_write_format_zip_large)
{
int i;
char namebuff[64];
struct fileblocks *fileblocks = fileblocks_new();
struct archive_entry *ae;
struct archive *a;
const char *p;
const char *cd_start, *zip64_eocd, *zip64_locator, *eocd;
int64_t cd_size;
char *buff;
int64_t filesize;
size_t writesize, buffsize, s;

nullsize = (size_t)(1 * MB);
nulldata = malloc(nullsize);
memset(nulldata, 0xAA, nullsize);




a = archive_write_new();
archive_write_set_format_zip(a);
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "zip:compression=store"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_options(a, "zip:fakecrc32"));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_set_bytes_per_block(a, 0));
assertEqualIntA(a, ARCHIVE_OK,
archive_write_open(a, fileblocks, NULL, memory_write, NULL));




for (i = 0; test_sizes[i] != 0; i++) {
assert((ae = archive_entry_new()) != NULL);
sprintf(namebuff, "file_%d", i);
archive_entry_copy_pathname(ae, namebuff);
archive_entry_set_mode(ae, S_IFREG | 0755);
filesize = test_sizes[i];
archive_entry_set_size(ae, filesize);

assertEqualIntA(a, ARCHIVE_OK,
archive_write_header(a, ae));
archive_entry_free(ae);




while (filesize > 0) {
writesize = nullsize;
if ((int64_t)writesize > filesize)
writesize = (size_t)filesize;
assertEqualIntA(a, (int)writesize,
(int)archive_write_data(a, nulldata, writesize));
filesize -= writesize;
}
}

assert((ae = archive_entry_new()) != NULL);
archive_entry_copy_pathname(ae, "lastfile");
archive_entry_set_mode(ae, S_IFREG | 0755);
assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);


assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));




a = archive_read_new();
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_format_zip_seekable(a));
verify_large_zip(a, fileblocks);
assertEqualInt(ARCHIVE_OK, archive_read_free(a));




a = archive_read_new();
assertEqualIntA(a, ARCHIVE_OK,
archive_read_support_format_zip_streamable(a));
verify_large_zip(a, fileblocks);
assertEqualInt(ARCHIVE_OK, archive_read_free(a));





#define FINAL_SIZE 8192
buff = malloc(FINAL_SIZE);
buffsize = 0;
memory_read_open(NULL, fileblocks);
memory_read_seek(NULL, fileblocks, -FINAL_SIZE, SEEK_END);
while ((s = memory_read(NULL, fileblocks, (const void **)&p)) > 0) {
memcpy(buff + buffsize, p, s);
buffsize += s;
}
assertEqualInt(buffsize, FINAL_SIZE);

p = buff + buffsize;


eocd = p - 22;
assertEqualMem(eocd, "PK\005\006\0\0\0\0", 8);
assertEqualMem(eocd + 8, "\021\0\021\0", 4);
cd_size = le32(eocd + 12);

assertEqualMem(eocd + 16, "\xff\xff\xff\xff", 4);
assertEqualMem(eocd + 20, "\0\0", 2);


zip64_locator = p - 42;
assertEqualMem(zip64_locator, "PK\006\007\0\0\0\0", 8);
zip64_eocd = p - (fileblocks->filesize - le64(zip64_locator + 8));
assertEqualMem(zip64_locator + 16, "\001\0\0\0", 4);


assert(zip64_eocd == p - 98);
assertEqualMem(zip64_eocd, "PK\006\006", 4);
assertEqualInt(44, le64(zip64_eocd + 4));
assertEqualMem(zip64_eocd + 12, "\055\0", 2);
assertEqualMem(zip64_eocd + 14, "\055\0", 2);
assertEqualMem(zip64_eocd + 16, "\0\0\0\0", 4);
assertEqualMem(zip64_eocd + 20, "\0\0\0\0", 4);
assertEqualInt(17, le64(zip64_eocd + 24));
assertEqualInt(17, le64(zip64_eocd + 32));
cd_size = le64(zip64_eocd + 40);
cd_start = p - (fileblocks->filesize - le64(zip64_eocd + 48));

assert(cd_start + cd_size == zip64_eocd);

assertEqualInt(le64(zip64_eocd + 48)
+ cd_size
+ 56
+ 20
+ 22,
fileblocks->filesize);


assertEqualMem(cd_start, "PK\001\002", 4);

fileblocks_free(fileblocks);
free(buff);
free(nulldata);
}
