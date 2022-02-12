























#include "test.h"
__FBSDID("$FreeBSD");

#include <locale.h>

DEFINE_TEST(test_read_format_zip_filename_CP932_eucJP)
{
const char *refname = "test_read_format_zip_filename_cp932.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "ja_JP.eucJP")) {
skipping("ja_JP.eucJP locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP932")) {
skipping("This system cannot convert character-set"
" from CP932 to eucJP.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(
"\xc9\xbd\xa4\xc0\xa4\xe8\x2f\xb0\xec\xcd\xf7\xc9\xbd\x2e\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(
"\xc9\xbd\xa4\xc0\xa4\xe8\x2f\xb4\xc1\xbb\xfa\x2e\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_CP932_UTF8)
{
const char *refname = "test_read_format_zip_filename_cp932.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP932")) {
skipping("This system cannot convert character-set"
" from CP932 to UTF-8.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
#if defined(__APPLE__)

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\x9f\xe3\x82\x99\xe3\x82\x88\x2f"
"\xe4\xb8\x80\xe8\xa6\xa7\xe8\xa1\xa8\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#else

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\xa0\xe3\x82\x88\x2f"
"\xe4\xb8\x80\xe8\xa6\xa7\xe8\xa1\xa8\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#endif
assertEqualInt(5, archive_entry_size(ae));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
#if defined(__APPLE__)

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\x9f\xe3\x82\x99\xe3\x82\x88\x2f"
"\xe6\xbc\xa2\xe5\xad\x97\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#else

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\xa0\xe3\x82\x88\x2f"
"\xe6\xbc\xa2\xe5\xad\x97\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#endif
assertEqualInt(5, archive_entry_size(ae));



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_UTF8_eucJP)
{
const char *refname = "test_read_format_zip_filename_utf8_jp.zip";
struct archive *a;
struct archive_entry *ae;






if (NULL == setlocale(LC_ALL, "ja_JP.eucJP")) {
skipping("ja_JP.eucJP locale not availablefilename_ on "
"this system.");
return;
}
extract_reference_file(refname);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=UTF-8")) {
skipping("This system cannot convert character-set"
" from UTF-8 to eucJP.");
goto cleanup;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertEqualString("\xc9\xbd\xa4\xc0\xa4\xe8\x2f",
archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualString(
"\xc9\xbd\xa4\xc0\xa4\xe8\x2f\xb0\xec\xcd\xf7\xc9\xbd\x2e\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualString(
"\xc9\xbd\xa4\xc0\xa4\xe8\x2f\xb4\xc1\xbb\xfa\x2e\x74\x78\x74",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_UTF8_UTF8)
{
const char *refname = "test_read_format_zip_filename_utf8_jp.zip";
struct archive *a;
struct archive_entry *ae;






if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
#if defined(__APPLE__)

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\x9f\xe3\x82\x99\xe3\x82\x88\x2f",
archive_entry_pathname(ae));
#else

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\xa0\xe3\x82\x88\x2f",
archive_entry_pathname(ae));
#endif
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
#if defined(__APPLE__)

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\x9f\xe3\x82\x99\xe3\x82\x88\x2f"
"\xe4\xb8\x80\xe8\xa6\xa7\xe8\xa1\xa8\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#else

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\xa0\xe3\x82\x88\x2f"
"\xe4\xb8\x80\xe8\xa6\xa7\xe8\xa1\xa8\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#endif
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
#if defined(__APPLE__)

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\x9f\xe3\x82\x99\xe3\x82\x88\x2f"
"\xe6\xbc\xa2\xe5\xad\x97\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#else

assertEqualUTF8String(
"\xe8\xa1\xa8\xe3\x81\xa0\xe3\x82\x88\x2f"
"\xe6\xbc\xa2\xe5\xad\x97\x2e\x74\x78\x74",
archive_entry_pathname(ae));
#endif
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_CP866_KOI8R)
{
const char *refname = "test_read_format_zip_filename_cp866.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia.20866") &&
NULL == setlocale(LC_ALL, "ru_RU.KOI8-R")) {
skipping("ru_RU.KOI8-R locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP866")) {
skipping("This system cannot convert character-set"
" from CP866 to KOI8-R.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xf0\xf2\xe9\xf7\xe5\xf4",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xd2\xc9\xd7\xc5\xd4",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_CP866_UTF8)
{
const char *refname = "test_read_format_zip_filename_cp866.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP866")) {
skipping("This system cannot convert character-set"
" from CP866 to UTF-8.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\x9f\xd0\xa0\xd0\x98\xd0\x92\xd0\x95\xd0\xa2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_KOI8R_CP866)
{
const char *refname = "test_read_format_zip_filename_koi8r.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia.866") &&
NULL == setlocale(LC_ALL, "ru_RU.CP866")) {
skipping("ru_RU.CP866 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=KOI8-R")) {
skipping("This system cannot convert character-set"
" from KOI8-R to CP866.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xaf\xe0\xa8\xa2\xa5\xe2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\x8f\x90\x88\x82\x85\x92",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_KOI8R_UTF8)
{
const char *refname = "test_read_format_zip_filename_koi8r.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=KOI8-R")) {
skipping("This system cannot convert character-set"
" from KOI8-R to UTF-8.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\x9f\xd0\xa0\xd0\x98\xd0\x92\xd0\x95\xd0\xa2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_UTF8_KOI8R)
{
const char *refname = "test_read_format_zip_filename_utf8_ru.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia.20866") &&
NULL == setlocale(LC_ALL, "ru_RU.KOI8-R")) {
skipping("ru_RU.KOI8-R locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=UTF-8")) {
skipping("This system cannot convert character-set"
" from UTF-8 to KOI8-R.");
goto cleanup;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xf0\xf2\xe9\xf7\xe5\xf4",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xd2\xc9\xd7\xc5\xd4",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_UTF8_CP866)
{
const char *refname = "test_read_format_zip_filename_utf8_ru.zip";
struct archive *a;
struct archive_entry *ae;






if (NULL == setlocale(LC_ALL, "Russian_Russia.866") &&
NULL == setlocale(LC_ALL, "ru_RU.CP866")) {
skipping("ru_RU.CP866 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=UTF-8")) {
skipping("This system cannot convert character-set"
" from UTF-8 to CP866.");
goto cleanup;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\x8f\x90\x88\x82\x85\x92",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xaf\xe0\xa8\xa2\xa5\xe2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_UTF8_UTF8_ru)
{
const char *refname = "test_read_format_zip_filename_utf8_ru.zip";
struct archive *a;
struct archive_entry *ae;






if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\x9f\xd0\xa0\xd0\x98\xd0\x92\xd0\x95\xd0\xa2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_CP932_CP932)
{
const char *refname = "test_read_format_zip_filename_cp932.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Japanese_Japan") &&
NULL == setlocale(LC_ALL, "ja_JP.SJIS")) {
skipping("CP932 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP932")) {
skipping("This system cannot convert character-set"
" from CP932.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(
"\x95\x5c\x82\xbe\x82\xe6\x2f\x88\xea\x97\x97\x95\x5c.txt",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString(
"\x95\x5c\x82\xbe\x82\xe6\x2f\x8a\xbf\x8e\x9a.txt",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_UTF8_CP932)
{
const char *refname = "test_read_format_zip_filename_utf8_jp.zip";
struct archive *a;
struct archive_entry *ae;






if (NULL == setlocale(LC_ALL, "Japanese_Japan") &&
NULL == setlocale(LC_ALL, "ja_JP.SJIS")) {
skipping("CP932 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=UTF-8")) {
skipping("This system cannot convert character-set"
" from UTF-8 to CP932.");
goto cleanup;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
assertEqualString(
"\x95\x5c\x82\xbe\x82\xe6\x2f",
archive_entry_pathname(ae));
assertEqualInt(0, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualString(
"\x95\x5c\x82\xbe\x82\xe6\x2f\x88\xea\x97\x97\x95\x5c.txt",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
assertEqualString(
"\x95\x5c\x82\xbe\x82\xe6\x2f\x8a\xbf\x8e\x9a.txt",
archive_entry_pathname(ae));
assertEqualInt(5, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_CP866_CP1251)
{
const char *refname = "test_read_format_zip_filename_cp866.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia") &&
NULL == setlocale(LC_ALL, "ru_RU.CP1251")) {
skipping("CP1251 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=CP866")) {
skipping("This system cannot convert character-set"
" from CP866 to CP1251.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xcf\xd0\xc8\xc2\xc5\xd2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xef\xf0\xe8\xe2\xe5\xf2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}







DEFINE_TEST(test_read_format_zip_filename_CP866_CP1251_win)
{
const char *refname = "test_read_format_zip_filename_cp866.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia")) {
skipping("Russian_Russia locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xcf\xd0\xc8\xc2\xc5\xd2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xef\xf0\xe8\xe2\xe5\xf2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_KOI8R_CP1251)
{
const char *refname = "test_read_format_zip_filename_koi8r.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia") &&
NULL == setlocale(LC_ALL, "ru_RU.CP1251")) {
skipping("CP1251 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=KOI8-R")) {
skipping("This system cannot convert character-set"
" from KOI8-R to CP1251.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xef\xf0\xe8\xe2\xe5\xf2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xcf\xd0\xc8\xc2\xc5\xd2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_zip_filename_UTF8_CP1251)
{
const char *refname = "test_read_format_zip_filename_utf8_ru.zip";
struct archive *a;
struct archive_entry *ae;






if (NULL == setlocale(LC_ALL, "Russian_Russia") &&
NULL == setlocale(LC_ALL, "ru_RU.CP1251")) {
skipping("CP1251 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=UTF-8")) {
skipping("This system cannot convert character-set"
" from UTF-8 to CP1251.");
goto cleanup;
}
assertEqualInt(ARCHIVE_OK, archive_read_free(a));


assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xcf\xd0\xc8\xc2\xc5\xd2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xef\xf0\xe8\xe2\xe5\xf2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}











DEFINE_TEST(test_read_format_zip_filename_KOI8R_UTF8_2)
{
const char *refname = "test_read_format_zip_filename_utf8_ru2.zip";
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}
extract_reference_file(refname);

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=KOI8-R")) {
skipping("This system cannot convert character-set"
" from KOI8-R to UTF-8.");
goto next_test;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\x9f\xd0\xa0\xd0\x98\xd0\x92\xd0\x95\xd0\xa2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);






assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
next_test:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));





assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));







assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));

assertEqualString("\xf0\xf2\xe9\xf7\xe5\xf4",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
