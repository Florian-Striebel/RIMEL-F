























#include "test.h"
__FBSDID("$FreeBSD$");

#include <locale.h>

DEFINE_TEST(test_zip_filename_encoding_UTF8)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "en_US.UTF-8")) {
skipping("en_US.UTF-8 locale not available on this system.");
return;
}





a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" for UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xD0\xBF\xD1\x80\xD0\xB8");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0x08, buff[7]);
assertEqualMem(buff + 30, "\xD0\xBF\xD1\x80\xD0\xB8", 6);





a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xD0\xBF\xD1\x80\xD0\xB8");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0x08, buff[7]);
assertEqualMem(buff + 30, "\xD0\xBF\xD1\x80\xD0\xB8", 6);





a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "abcABC");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);
assertEqualMem(buff + 30, "abcABC", 6);
}

DEFINE_TEST(test_zip_filename_encoding_KOI8R)
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
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from KOI8-R to UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xD0\xD2\xC9");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0x08, buff[7]);


assertEqualMem(buff + 30, "\xD0\xBF\xD1\x80\xD0\xB8", 6);




a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xD0\xD2\xC9");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);


assertEqualMem(buff + 30, "\xD0\xD2\xC9", 3);






a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from KOI8-R to UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "abcABC");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);
assertEqualMem(buff + 30, "abcABC", 6);
}




DEFINE_TEST(test_zip_filename_encoding_ru_RU_CP1251)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "ru_RU.CP1251")) {
skipping("Russian_Russia locale not available on this system.");
return;
}





a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xEF\xF0\xE8");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);


assertEqualMem(buff + 30, "\xEF\xF0\xE8", 3);
}






DEFINE_TEST(test_zip_filename_encoding_Russian_Russia)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "Russian_Russia")) {
skipping("Russian_Russia locale not available on this system.");
return;
}





a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from Russian_Russia.CP1251 to UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xEF\xF0\xE8");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0x08, buff[7]);


assertEqualMem(buff + 30, "\xD0\xBF\xD1\x80\xD0\xB8", 6);





a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xEF\xF0\xE8");
archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);


assertEqualMem(buff + 30, "\xAF\xE0\xA8", 3);
}

DEFINE_TEST(test_zip_filename_encoding_EUCJP)
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
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from eucJP to UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xC9\xBD.txt");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0x08, buff[7]);

assertEqualMem(buff + 30, "\xE8\xA1\xA8.txt", 7);




a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\xC9\xBD.txt");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);


assertEqualMem(buff + 30, "\xC9\xBD.txt", 6);






a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from eucJP to UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "abcABC");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);
assertEqualMem(buff + 30, "abcABC", 6);
}

DEFINE_TEST(test_zip_filename_encoding_CP932)
{
struct archive *a;
struct archive_entry *entry;
char buff[4096];
size_t used;

if (NULL == setlocale(LC_ALL, "Japanese_Japan") &&
NULL == setlocale(LC_ALL, "ja_JP.SJIS")) {
skipping("CP932/SJIS locale not available on this system.");
return;
}




a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from CP932/SJIS to UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\x95\x5C.txt");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0x08, buff[7]);

assertEqualMem(buff + 30, "\xE8\xA1\xA8.txt", 7);




a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "\x95\x5C.txt");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);


assertEqualMem(buff + 30, "\x95\x5C.txt", 6);






a = archive_write_new();
assertEqualInt(ARCHIVE_OK, archive_write_set_format_zip(a));
if (archive_write_set_options(a, "hdrcharset=UTF-8") != ARCHIVE_OK) {
skipping("This system cannot convert character-set"
" from CP932/SJIS to UTF-8.");
archive_write_free(a);
return;
}
assertEqualInt(ARCHIVE_OK,
archive_write_open_memory(a, buff, sizeof(buff), &used));

entry = archive_entry_new2(a);

archive_entry_set_pathname(entry, "abcABC");

archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_size(entry, 0);
assertEqualInt(ARCHIVE_OK, archive_write_header(a, entry));
archive_entry_free(entry);
assertEqualInt(ARCHIVE_OK, archive_write_free(a));



assertEqualInt(0, buff[7]);
assertEqualMem(buff + 30, "abcABC", 6);
}
