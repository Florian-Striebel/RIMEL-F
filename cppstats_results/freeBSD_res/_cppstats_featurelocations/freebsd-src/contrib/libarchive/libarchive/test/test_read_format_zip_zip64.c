























#include "test.h"
__FBSDID("$FreeBSD$");






static void
verify_file0_seek(struct archive *a)
{
struct archive_entry *ae;

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("-", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0660, archive_entry_mode(ae));
assertEqualInt(6, archive_entry_size(ae));
#if defined(HAVE_ZLIB_H)
{
char data[16];
assertEqualIntA(a, 6, archive_read_data(a, data, 16));
assertEqualMem(data, "file0\x0a", 6);
}
#endif
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}

static void
verify_file0_stream(struct archive *a, int size_known)
{
struct archive_entry *ae;

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("-", archive_entry_pathname(ae));
assertEqualInt(AE_IFREG | 0664, archive_entry_mode(ae));
if (size_known) {


assert(archive_entry_size_is_set(ae));
assertEqualInt(6, archive_entry_size(ae));
} else {

assert(!archive_entry_size_is_set(ae));
}
#if defined(HAVE_ZLIB_H)
{
char data[16];
assertEqualIntA(a, 6, archive_read_data(a, data, 16));
assertEqualMem(data, "file0\x0a", 6);
}
#endif
assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_zip64a)
{
const char *refname = "test_read_format_zip_zip64a.zip";
struct archive *a;
char *p;
size_t s;

extract_reference_file(refname);
p = slurpfile(&s, "%s", refname);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip_seekable(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));
verify_file0_seek(a);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip_streamable(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));
verify_file0_stream(a, 0);
free(p);
}

DEFINE_TEST(test_read_format_zip_zip64b)
{
const char *refname = "test_read_format_zip_zip64b.zip";
struct archive *a;
char *p;
size_t s;

extract_reference_file(refname);
p = slurpfile(&s, "%s", refname);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip_seekable(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));
verify_file0_seek(a);


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip_streamable(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));
verify_file0_stream(a, 1);
free(p);
}

