























#include "test.h"
__FBSDID("$FreeBSD$");

#include <errno.h>
#include <stdlib.h>
#include <string.h>














static size_t filedatasize;
static void *filedata;





struct memblock {
struct memblock *next;
size_t size;
void *buff;
int64_t filebytes;
};





struct memdata {
int64_t filebytes;
void *buff;
struct memblock *first;
struct memblock *last;
};


#define KB ((int64_t)1024)
#define MB ((int64_t)1024 * KB)
#define GB ((int64_t)1024 * MB)
#define TB ((int64_t)1024 * GB)

static int64_t memory_read_skip(struct archive *, void *, int64_t request);
static ssize_t memory_read(struct archive *, void *, const void **buff);
static ssize_t memory_write(struct archive *, void *, const void *, size_t);


static ssize_t
memory_write(struct archive *a, void *_private, const void *buff, size_t size)
{
struct memdata *private = _private;
struct memblock *block;

(void)a;








if ((const char *)filedata <= (const char *)buff
&& (const char *)buff < (const char *)filedata + filedatasize) {

private->last->filebytes += (int64_t)size;
} else {


block = (struct memblock *)malloc(sizeof(*block));
memset(block, 0, sizeof(*block));
block->size = size;
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
return ((long)size);
}

static ssize_t
memory_read(struct archive *a, void *_private, const void **buff)
{
struct memdata *private = _private;
struct memblock *block;
ssize_t size;

(void)a;

free(private->buff);
private->buff = NULL;
if (private->first == NULL) {
private->last = NULL;
return (ARCHIVE_EOF);
}
if (private->filebytes > 0) {




if (private->filebytes > (int64_t)filedatasize)
size = (ssize_t)filedatasize;
else
size = (ssize_t)private->filebytes;
private->filebytes -= size;
*buff = filedata;
} else {



block = private->first;
private->first = block->next;
size = (ssize_t)block->size;
if (block->buff != NULL) {
private->buff = block->buff;
*buff = block->buff;
} else {
private->buff = NULL;
*buff = filedata;
}
private->filebytes = block->filebytes;
free(block);
}
return (size);
}


static int64_t
memory_read_skip(struct archive *a, void *_private, int64_t skip)
{
struct memdata *private = _private;

(void)a;

if (private->first == NULL) {
private->last = NULL;
return (0);
}
if (private->filebytes > 0) {
if (private->filebytes < skip)
skip = (off_t)private->filebytes;
private->filebytes -= skip;
} else {
skip = 0;
}
return (skip);
}

DEFINE_TEST(test_tar_large)
{

static int64_t tests[] = {

2 * GB - 1, 2 * GB, 2 * GB + 1,

4 * GB - 1, 4 * GB, 4 * GB + 1,

8 * GB - 1, 8 * GB, 8 * GB + 1,

64 * GB - 1, 64 * GB,

256 * GB, 1 * TB, 0 };
int i;
char namebuff[64];
struct memdata memdata;
struct archive_entry *ae;
struct archive *a;
int64_t filesize;
size_t writesize;

filedatasize = (size_t)(1 * MB);
filedata = malloc(filedatasize);
memset(filedata, 0xAA, filedatasize);
memset(&memdata, 0, sizeof(memdata));




a = archive_write_new();
archive_write_set_format_pax_restricted(a);
archive_write_set_bytes_per_block(a, 0);
archive_write_open(a, &memdata, NULL, memory_write, NULL);




for (i = 0; tests[i] != 0; i++) {
assert((ae = archive_entry_new()) != NULL);
sprintf(namebuff, "file_%d", i);
archive_entry_copy_pathname(ae, namebuff);
archive_entry_set_mode(ae, S_IFREG | 0755);
filesize = tests[i];

archive_entry_set_size(ae, filesize);

assertA(0 == archive_write_header(a, ae));
archive_entry_free(ae);




while (filesize > 0) {
writesize = filedatasize;
if ((int64_t)writesize > filesize)
writesize = (size_t)filesize;
assertA((int)writesize
== archive_write_data(a, filedata, writesize));
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
archive_read_support_format_tar(a);
archive_read_open2(a, &memdata, NULL,
memory_read, memory_read_skip, NULL);




for (i = 0; tests[i] > 0; i++) {
assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
sprintf(namebuff, "file_%d", i);
assertEqualString(namebuff, archive_entry_pathname(ae));
assert(tests[i] == archive_entry_size(ae));
}
assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
assertEqualString("lastfile", archive_entry_pathname(ae));

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

free(memdata.buff);
free(filedata);
}
