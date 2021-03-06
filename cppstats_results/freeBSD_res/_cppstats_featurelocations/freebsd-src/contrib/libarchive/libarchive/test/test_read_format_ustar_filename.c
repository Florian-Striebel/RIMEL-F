























#include "test.h"
__FBSDID("$FreeBSD");

#include <locale.h>

static void
test_read_format_ustar_filename_eucJP_UTF8(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=eucJP")) {
skipping("This system cannot convert character-set"
" from eucJP to UTF-8.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xe6\xbc\xa2\xe5\xad\x97.txt",
archive_entry_pathname(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xe8\xa1\xa8.txt", archive_entry_pathname(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_ustar_filename_CP866_KOI8R(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia.20866") &&
NULL == setlocale(LC_ALL, "ru_RU.KOI8-R")) {
skipping("ru_RU.KOI8-R locale not available on this system.");
return;
}

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
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xd2\xc9\xd7\xc5\xd4",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_ustar_filename_CP866_UTF8(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}

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
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_ustar_filename_KOI8R_CP866(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia.866") &&
NULL == setlocale(LC_ALL, "ru_RU.CP866")) {
skipping("ru_RU.CP866 locale not available on this system.");
return;
}

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
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\x8f\x90\x88\x82\x85\x92",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_ustar_filename_KOI8R_UTF8(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}

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
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xd0\x9f\xd0\xa0\xd0\x98\xd0\x92\xd0\x95\xd0\xa2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_ustar_filename_eucJP_CP932(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Japanese_Japan") &&
NULL == setlocale(LC_ALL, "ja_JP.SJIS")) {
skipping("CP932 locale not available on this system.");
return;
}

assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
if (ARCHIVE_OK != archive_read_set_options(a, "hdrcharset=eucJP")) {
skipping("This system cannot convert character-set"
" from eucJP.");
goto cleanup;
}
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, refname, 10240));


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\x8a\xbf\x8e\x9a.txt", archive_entry_pathname(ae));
assertEqualInt(8, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\x95\x5c.txt", archive_entry_pathname(ae));
assertEqualInt(4, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_ustar_filename_CP866_CP1251(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia") &&
NULL == setlocale(LC_ALL, "ru_RU.CP1251")) {
skipping("CP1251 locale not available on this system.");
return;
}

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
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xef\xf0\xe8\xe2\xe5\xf2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}







static void
test_read_format_ustar_filename_CP866_CP1251_win(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia")) {
skipping("Russian_Russia locale not available on this system.");
return;
}

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
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xef\xf0\xe8\xe2\xe5\xf2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
test_read_format_ustar_filename_KOI8R_CP1251(const char *refname)
{
struct archive *a;
struct archive_entry *ae;




if (NULL == setlocale(LC_ALL, "Russian_Russia") &&
NULL == setlocale(LC_ALL, "ru_RU.CP1251")) {
skipping("CP1251 locale not available on this system.");
return;
}

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
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);


assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assertEqualString("\xcf\xd0\xc8\xc2\xc5\xd2",
archive_entry_pathname(ae));
assertEqualInt(6, archive_entry_size(ae));
assertEqualInt(archive_entry_is_encrypted(ae), 0);
assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);



assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));


assertEqualIntA(a, ARCHIVE_FILTER_COMPRESS, archive_filter_code(a, 0));
assertEqualIntA(a, ARCHIVE_FORMAT_TAR_USTAR, archive_format(a));


assertEqualInt(ARCHIVE_OK, archive_read_close(a));
cleanup:
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}


DEFINE_TEST(test_read_format_ustar_filename)
{
const char *refname1 = "test_read_format_ustar_filename_eucjp.tar.Z";
const char *refname2 = "test_read_format_ustar_filename_cp866.tar.Z";
const char *refname3 = "test_read_format_ustar_filename_koi8r.tar.Z";

extract_reference_file(refname1);
test_read_format_ustar_filename_eucJP_UTF8(refname1);
test_read_format_ustar_filename_eucJP_CP932(refname1);

extract_reference_file(refname2);
test_read_format_ustar_filename_CP866_KOI8R(refname2);
test_read_format_ustar_filename_CP866_UTF8(refname2);
test_read_format_ustar_filename_CP866_CP1251(refname2);
test_read_format_ustar_filename_CP866_CP1251_win(refname2);

extract_reference_file(refname3);
test_read_format_ustar_filename_KOI8R_CP866(refname3);
test_read_format_ustar_filename_KOI8R_UTF8(refname3);
test_read_format_ustar_filename_KOI8R_CP1251(refname3);
}
