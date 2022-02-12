

























#include "test.h"
__FBSDID("$FreeBSD$");

#include <locale.h>

static void
verify(struct archive *a) {
struct archive_entry *ae;
const char *p;

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assert((p = archive_entry_pathname_utf8(ae)) != NULL);
assertEqualUTF8String(p, "File 1.txt");

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assert((p = archive_entry_pathname_utf8(ae)) != NULL);
#if defined(__APPLE__)

assertEqualUTF8String(p, "File 2 - o\xCC\x88.txt");
#else

assertEqualUTF8String(p, "File 2 - \xC3\xB6.txt");
#endif

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assert((p = archive_entry_pathname_utf8(ae)) != NULL);
#if defined(__APPLE__)

assertEqualUTF8String(p, "File 3 - a\xCC\x88.txt");
#else

assertEqualUTF8String(p, "File 3 - \xC3\xA4.txt");
#endif


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assert((p = archive_entry_pathname_utf8(ae)) != NULL);
assertEqualUTF8String(p, "File 4 - xx.txt");

assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
}

DEFINE_TEST(test_read_format_zip_utf8_paths)
{
const char *refname = "test_read_format_zip_7075_utf8_paths.zip";
struct archive *a;
char *p;
size_t s;

extract_reference_file(refname);

if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 10240));
verify(a);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));


p = slurpfile(&s, "%s", refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, p, s, 31));
verify(a);
assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualIntA(a, ARCHIVE_OK, archive_free(a));
free(p);
}
