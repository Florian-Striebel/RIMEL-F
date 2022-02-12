























#include "test.h"
__FBSDID("$FreeBSD$");

#include <locale.h>














static void
test_pax_filename_encoding_1(void)
{
static const char testname[] = "test_pax_filename_encoding.tar";




char filename[] = "abc\314\214mno\374xyz";
struct archive *a;
struct archive_entry *entry;




extract_reference_file(testname);
a = archive_read_new();
assertEqualInt(ARCHIVE_OK, archive_read_support_format_tar(a));
assertEqualInt(ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualInt(ARCHIVE_OK,
archive_read_open_filename(a, testname, 10240));





failure("Invalid UTF8 in a pax archive pathname should cause a warning");
assertEqualInt(ARCHIVE_WARN, archive_read_next_header(a, &entry));
assertEqualString(filename, archive_entry_pathname(entry));




failure("A pathname with hdrcharset=BINARY can have invalid UTF8\n"
" characters in it without generating a warning");
assertEqualInt(ARCHIVE_OK, archive_read_next_header(a, &entry));
assertEqualString(filename, archive_entry_pathname(entry));
archive_read_free(a);
}






static void
test_pax_filename_encoding_2(void)
{
char filename[] = "abc\314\214mno\374xyz";
struct archive *a;
struct archive_entry *entry;
char buff[65536];
char longname[] = "abc\314\214mno\374xyz"
"/abc\314\214mno\374xyz/abcdefghijklmnopqrstuvwxyz"
"/abc\314\214mno\374xyz/abcdefghijklmnopqrstuvwxyz"
"/abc\314\214mno\374xyz/abcdefghijklmnopqrstuvwxyz"
"/abc\314\214mno\374xyz/abcdefghijklmnopqrstuvwxyz"
"/abc\314\214mno\374xyz/abcdefghijklmnopqrstuvwxyz"
"/abc\314\214mno\374xyz/abcdefghijklmnopqrstuvwxyz"
;
size_t used;






if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("invalid encoding tests require a suitable locale;"
" en_US.UTF-8 not available on this system");
return;
}

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, 0, archive_write_set_format_pax(a));
assertEqualIntA(a, 0, archive_write_add_filter_none(a));
assertEqualIntA(a, 0, archive_write_set_bytes_per_block(a, 0));
assertEqualInt(0,
archive_write_open_memory(a, buff, sizeof(buff), &used));

assert((entry = archive_entry_new()) != NULL);

archive_entry_copy_pathname(entry, filename);
archive_entry_copy_gname(entry, filename);
archive_entry_copy_uname(entry, filename);
archive_entry_copy_hardlink(entry, filename);
archive_entry_set_filetype(entry, AE_IFREG);
failure("This should generate a warning for nonconvertible names.");
assertEqualInt(ARCHIVE_WARN, archive_write_header(a, entry));
archive_entry_free(entry);

assert((entry = archive_entry_new()) != NULL);

archive_entry_copy_pathname(entry, filename);
archive_entry_copy_gname(entry, filename);
archive_entry_copy_uname(entry, filename);
archive_entry_copy_symlink(entry, filename);
archive_entry_set_filetype(entry, AE_IFLNK);
failure("This should generate a warning for nonconvertible names.");
assertEqualInt(ARCHIVE_WARN, archive_write_header(a, entry));
archive_entry_free(entry);

assert((entry = archive_entry_new()) != NULL);

archive_entry_copy_pathname(entry, longname);
archive_entry_set_filetype(entry, AE_IFREG);
failure("This should generate a warning for nonconvertible names.");
assertEqualInt(ARCHIVE_WARN, archive_write_header(a, entry));
archive_entry_free(entry);

assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));





assert((a = archive_read_new()) != NULL);
assertEqualInt(0, archive_read_support_format_tar(a));
assertEqualInt(0, archive_read_open_memory(a, buff, used));

assertEqualInt(0, archive_read_next_header(a, &entry));
assertEqualString(filename, archive_entry_pathname(entry));
assertEqualString(filename, archive_entry_gname(entry));
assertEqualString(filename, archive_entry_uname(entry));
assertEqualString(filename, archive_entry_hardlink(entry));

assertEqualInt(0, archive_read_next_header(a, &entry));
assertEqualString(filename, archive_entry_pathname(entry));
assertEqualString(filename, archive_entry_gname(entry));
assertEqualString(filename, archive_entry_uname(entry));
assertEqualString(filename, archive_entry_symlink(entry));

assertEqualInt(0, archive_read_next_header(a, &entry));
assertEqualString(longname, archive_entry_pathname(entry));

assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

#if 0






static void
test_pax_filename_encoding_3(void)
{
wchar_t badname[] = L"xxxAyyyBzzz";
const char badname_utf8[] = "xxx\xE1\x88\xB4yyy\xE5\x99\xB8zzz";
struct archive *a;
struct archive_entry *entry;
char buff[65536];
size_t used;

badname[3] = 0x1234;
badname[7] = 0x5678;


if (NULL == setlocale(LC_ALL, "C")) {
skipping("Can't set \"C\" locale, so can't exercise "
"certain character-conversion failures");
return;
}


if (wctomb(buff, 0x1234) > 0) {
skipping("Cannot test conversion failures because \"C\" "
"locale on this system has no invalid characters.");
return;
}


if (wctomb(buff, 0x1234) > 0) {
skipping("Cannot test conversion failures because \"C\" "
"locale on this system has no invalid characters.");
return;
}




entry = archive_entry_new();
if (archive_entry_update_pathname_utf8(entry, badname_utf8)) {
archive_entry_free(entry);
skipping("Cannot test conversion failures.");
return;
}
archive_entry_free(entry);

assert((a = archive_write_new()) != NULL);
assertEqualIntA(a, 0, archive_write_set_format_pax(a));
assertEqualIntA(a, 0, archive_write_add_filter_none(a));
assertEqualIntA(a, 0, archive_write_set_bytes_per_block(a, 0));
assertEqualInt(0,
archive_write_open_memory(a, buff, sizeof(buff), &used));

assert((entry = archive_entry_new()) != NULL);

archive_entry_copy_pathname_w(entry, badname);
archive_entry_set_filetype(entry, AE_IFREG);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);

assert((entry = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(entry, L"abc");

archive_entry_copy_gname_w(entry, badname);
archive_entry_set_filetype(entry, AE_IFREG);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);

assert((entry = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(entry, L"abc");

archive_entry_copy_uname_w(entry, badname);
archive_entry_set_filetype(entry, AE_IFREG);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);

assert((entry = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(entry, L"abc");

archive_entry_copy_hardlink_w(entry, badname);
archive_entry_set_filetype(entry, AE_IFREG);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);

assert((entry = archive_entry_new()) != NULL);
archive_entry_copy_pathname_w(entry, L"abc");

archive_entry_copy_symlink_w(entry, badname);
archive_entry_set_filetype(entry, AE_IFLNK);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);

assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));





assert((a = archive_read_new()) != NULL);
assertEqualInt(0, archive_read_support_format_tar(a));
assertEqualInt(0, archive_read_open_memory(a, buff, used));

failure("A non-convertible pathname should cause a warning.");
assertEqualInt(ARCHIVE_WARN, archive_read_next_header(a, &entry));
assertEqualWString(badname, archive_entry_pathname_w(entry));
failure("If native locale can't convert, we should get UTF-8 back.");
assertEqualString(badname_utf8, archive_entry_pathname(entry));

failure("A non-convertible gname should cause a warning.");
assertEqualInt(ARCHIVE_WARN, archive_read_next_header(a, &entry));
assertEqualWString(badname, archive_entry_gname_w(entry));
failure("If native locale can't convert, we should get UTF-8 back.");
assertEqualString(badname_utf8, archive_entry_gname(entry));

failure("A non-convertible uname should cause a warning.");
assertEqualInt(ARCHIVE_WARN, archive_read_next_header(a, &entry));
assertEqualWString(badname, archive_entry_uname_w(entry));
failure("If native locale can't convert, we should get UTF-8 back.");
assertEqualString(badname_utf8, archive_entry_uname(entry));

failure("A non-convertible hardlink should cause a warning.");
assertEqualInt(ARCHIVE_WARN, archive_read_next_header(a, &entry));
assertEqualWString(badname, archive_entry_hardlink_w(entry));
failure("If native locale can't convert, we should get UTF-8 back.");
assertEqualString(badname_utf8, archive_entry_hardlink(entry));

failure("A non-convertible symlink should cause a warning.");
assertEqualInt(ARCHIVE_WARN, archive_read_next_header(a, &entry));
assertEqualWString(badname, archive_entry_symlink_w(entry));
assertEqualWString(NULL, archive_entry_hardlink_w(entry));
failure("If native locale can't convert, we should get UTF-8 back.");
assertEqualString(badname_utf8, archive_entry_symlink(entry));

assertEqualInt(ARCHIVE_EOF, archive_read_next_header(a, &entry));

assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
#else
static void
test_pax_filename_encoding_3(void)
{
}
#endif




DEFINE_TEST(test_pax_filename_encoding_KOI8R)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "ru_RU.KOI8-R")) {
skipping("KOI8-R locale not available on this system.");
return;
}


a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from KOI8-R to UTF-8.");
archive_write_free(a);
return;
}
archive_write_free(a);



a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);
archive_entry_set_pathname(entry, "\xD0\xD2\xC9");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualMem(buff + 512, "15 path=\xD0\xBF\xD1\x80\xD0\xB8\x0A", 15);
}




DEFINE_TEST(test_pax_filename_encoding_CP1251)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "Russian_Russia") &&
NULL == setlocale(LC_ALL, "ru_RU.CP1251")) {
skipping("KOI8-R locale not available on this system.");
return;
}


a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from KOI8-R to UTF-8.");
archive_write_free(a);
return;
}
archive_write_free(a);



a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);
archive_entry_set_pathname(entry, "\xef\xf0\xe8");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualMem(buff + 512, "15 path=\xD0\xBF\xD1\x80\xD0\xB8\x0A", 15);
}




DEFINE_TEST(test_pax_filename_encoding_EUCJP)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "ja_JP.eucJP")) {
skipping("eucJP locale not available on this system.");
return;
}


a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from eucJP to UTF-8.");
archive_write_free(a);
return;
}
archive_write_free(a);



a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);
archive_entry_set_pathname(entry, "\xC9\xBD.txt");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualMem(buff + 512, "16 path=\xE8\xA1\xA8.txt\x0A", 16);

}




DEFINE_TEST(test_pax_filename_encoding_CP932)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "Japanese_Japan") &&
NULL == setlocale(LC_ALL, "ja_JP.SJIS")) {
skipping("eucJP locale not available on this system.");
return;
}


a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from CP932/SJIS to UTF-8.");
archive_write_free(a);
return;
}
archive_write_free(a);



a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);
archive_entry_set_pathname(entry, "\x95\x5C.txt");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualMem(buff + 512, "16 path=\xE8\xA1\xA8.txt\x0A", 16);

}





DEFINE_TEST(test_pax_filename_encoding_KOI8R_BINARY)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "ru_RU.KOI8-R")) {
skipping("KOI8-R locale not available on this system.");
return;
}

a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));

assertEqualInt(ARCHIVE_OK,
archive_write_set_options(a, "hdrcharset=BINARY"));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);
archive_entry_set_pathname(entry, "\xD0\xD2\xC9");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));


assertEqualMem(buff + 512, "21 hdrcharset=BINARY\x0A", 21);


assertEqualMem(buff + 512+21, "12 path=\xD0\xD2\xC9\x0A", 12);
}





DEFINE_TEST(test_pax_filename_encoding_KOI8R_CP1251)
{
struct archive *a;

if (NULL == setlocale(LC_ALL, "ru_RU.KOI8-R")) {
skipping("KOI8-R locale not available on this system.");
return;
}

a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_pax(a));

assertEqualInt(ARCHIVE_FAILED,
archive_write_set_options(a, "hdrcharset=CP1251"));
assertEqualInt(ARCHIVE_OK, archive_write_free(a));
}


DEFINE_TEST(test_pax_filename_encoding)
{
test_pax_filename_encoding_1();
test_pax_filename_encoding_2();
test_pax_filename_encoding_3();
}
