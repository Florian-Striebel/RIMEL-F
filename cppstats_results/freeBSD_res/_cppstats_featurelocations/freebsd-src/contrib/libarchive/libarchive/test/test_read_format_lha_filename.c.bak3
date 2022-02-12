























#include "test.h"
__FBSDID("$FreeBSD");

#include <locale.h>

static void
test_read_format_lha_filename_CP932_eucJP(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "ja_JP.eucJP")) {
skipping("ja_JP.eucJP locale not available on this system.");
return;
}







assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP932")) {
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
skipping("This system cannot convert character-set"
" from CP932 to eucJP.");
return;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xB4\xC1\xBB\xFA\x2E\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xC9\xBD\x2E\x74\x78\x74", archive_entry_pathname(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_LHA, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_lha_filename_CP932_UTF8(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}






assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP932")) {
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
skipping("This system cannot convert character-set"
" from CP932 to UTF-8.");
return;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xE6\xBC\xA2\xE5\xAD\x97\x2E\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(8, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xE8\xA1\xA8\x2E\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(4, archive_entry_size(ae));



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_LHA, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

#if defined(_WIN32) && !defined(__CYGWIN__)
static void
test_read_format_lha_filename_CP932_Windows(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "jpn")) {
skipping("jpn locale not available on this system.");
return;
}






assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\x8A\xBF\x8E\x9A\x2E\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(8, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\x95\x5C\x2E\x74\x78\x74", archive_entry_pathname(ae));
assertEqualInt(4, archive_entry_size(ae));



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_LHA, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
#else

static void
test_read_format_lha_filename_CP932_Windows(const char *refname)
{
(void)refname;
}
#endif

DEFINE_TEST(test_read_format_lha_filename)
{

const char *refname = "test_read_format_lha_filename_cp932.lzh";

extract_reference_file(refname);

test_read_format_lha_filename_CP932_eucJP(refname);
test_read_format_lha_filename_CP932_UTF8(refname);
test_read_format_lha_filename_CP932_Windows(refname);
}
