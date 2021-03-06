























#include "test.h"
__FBSDID("$FreeBSD");

#include <locale.h>

static void
test_read_format_lha_filename_UTF16_UTF8(const char *refname)
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
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=UTF-16")) {
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
skipping("This system cannot convert character-set"
" from UTF-16 to UTF-8.");
return;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


#if defined(__APPLE__)


#define UMLAUT_DIRNAME "\x55\xcc\x88\x4f\xcc\x88\x41\xcc\x88\x75\xcc\x88\x6f""\xcc\x88\x61\xcc\x88/"


#define UMLAUT_FNAME "\x61\xcc\x88\x6f\xcc\x88\x75\xcc\x88\x41\xcc\x88""\x4f\xcc\x88\x55\xcc\x88.txt"

#else


#define UMLAUT_DIRNAME "\xc3\x9c\xc3\x96\xc3\x84\xc3\xbc\xc3\xb6\xc3\xa4/"

#define UMLAUT_FNAME "\xc3\xa4\xc3\xb6\xc3\xbc\xc3\x84\xc3\x96\xc3\x9c.txt"
#endif


#define KATAKANA_FNAME "\xe3\x83\x86\xe3\x82\xb9\xe3\x83\x88.txt"
#define KATAKANA_DIRNAME "\xe3\x83\x86\xe3\x82\xb9\xe3\x83\x88/"


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(UMLAUT_DIRNAME UMLAUT_FNAME, archive_entry_pathname(ae));
assertEqualInt(12, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(UMLAUT_DIRNAME, archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(UMLAUT_DIRNAME KATAKANA_FNAME,
archive_entry_pathname(ae));
assertEqualInt(25, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(KATAKANA_DIRNAME UMLAUT_FNAME,
archive_entry_pathname(ae));
assertEqualInt(12, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(KATAKANA_DIRNAME, archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(UMLAUT_FNAME, archive_entry_pathname(ae));
assertEqualInt(12, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_LHA, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_lha_filename_UTF16)
{

const char *refname = "test_read_format_lha_filename_utf16.lzh";
extract_reference_file(refname);

test_read_format_lha_filename_UTF16_UTF8(refname);
}

