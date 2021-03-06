























#include "test.h"
__FBSDID("$FreeBSD");

#include <locale.h>












DEFINE_TEST(test_read_format_zip_high_compression)
{
const char *refname = "test_read_format_zip_high_compression.zip";
char *p;
size_t archive_size;
struct archive *a;
struct archive_entry *entry;

const void *pv;
size_t s;
int64_t o;

if (archive_zlib_version() == NULL) {
skipping("Zip compression test requires zlib");
return;
}

extract_reference_file(refname);
p = slurpfile(&archive_size, "%s", refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, archive_size, 16 * 1024));
assertEqualInt(ARCHIVE_OK, archive_read_next_header(a, &entry));

assertEqualInt(ARCHIVE_OK, archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(262144, s);
assertEqualInt(0, o);

assertEqualInt(ARCHIVE_OK, archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(160, s);
assertEqualInt(262144, o);

assertEqualInt(ARCHIVE_EOF, archive_read_data_block(a, &pv, &s, &o));

assertEqualInt(ARCHIVE_OK, archive_free(a));
free(p);
}




DEFINE_TEST(test_read_format_zip_high_compression2)
{
const size_t body_size = 1024 * 1024;
const size_t buff_size = 2 * 1024 * 1024;
char *body, *body_read, *buff;
int n;

if (archive_zlib_version() == NULL) {
skipping("Zip compression test requires zlib");
return;
}

assert((body = malloc(body_size)) != NULL);
assert((body_read = malloc(body_size)) != NULL);
assert((buff = malloc(buff_size)) != NULL);







for(n = 1024; n < (int)body_size; n += 1024) {
struct archive *a;
struct archive_entry *entry;
size_t used = 0;
const void *pv;
size_t s;
int64_t o;

memset(body, 255, body_size);
body[n] = 1;


assert((a = archive_write_new()) != NULL);
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK, archive_write_open_memory(a, buff, buff_size, &used));

entry = archive_entry_new2(a);
archive_entry_set_pathname(entry, "test");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 262150);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(262150, archive_write_data(a, body, 262150));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, buff, used, 17));
assertEqualInt(ARCHIVE_OK, archive_read_next_header(a, &entry));

assertEqualInt(ARCHIVE_OK, archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(262144, s);
assertEqualInt(0, o);

assertEqualInt(ARCHIVE_OK, archive_read_data_block(a, &pv, &s, &o));
assertEqualInt(6, s);
assertEqualInt(262144, o);

assertEqualInt(ARCHIVE_EOF, archive_read_data_block(a, &pv, &s, &o));

assertEqualInt(ARCHIVE_OK, archive_free(a));
}

free(body);
free(body_read);
free(buff);
}
