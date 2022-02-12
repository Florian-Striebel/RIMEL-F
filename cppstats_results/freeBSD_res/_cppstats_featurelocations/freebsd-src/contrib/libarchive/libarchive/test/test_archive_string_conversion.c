























#include "test.h"
__FBSDID("$FreeBSD$");

#include <locale.h>

#define __LIBARCHIVE_TEST
#include "archive_string.h"


















static int
unicode_to_utf8(char *p, uint32_t uc)
{
char *_p = p;


if (uc <= 0x7f) {
*p++ = (char)uc;
} else if (uc <= 0x7ff) {
*p++ = 0xc0 | ((uc >> 6) & 0x1f);
*p++ = 0x80 | (uc & 0x3f);
} else if (uc <= 0xffff) {
*p++ = 0xe0 | ((uc >> 12) & 0x0f);
*p++ = 0x80 | ((uc >> 6) & 0x3f);
*p++ = 0x80 | (uc & 0x3f);
} else {
*p++ = 0xf0 | ((uc >> 18) & 0x07);
*p++ = 0x80 | ((uc >> 12) & 0x3f);
*p++ = 0x80 | ((uc >> 6) & 0x3f);
*p++ = 0x80 | (uc & 0x3f);
}
return ((int)(p - _p));
}

static void
archive_be16enc(void *pp, uint16_t u)
{
unsigned char *p = (unsigned char *)pp;

p[0] = (u >> 8) & 0xff;
p[1] = u & 0xff;
}

static int
unicode_to_utf16be(char *p, uint32_t uc)
{
char *utf16 = p;

if (uc > 0xffff) {


uc -= 0x10000;
archive_be16enc(utf16, ((uc >> 10) & 0x3ff) + 0xD800);
archive_be16enc(utf16+2, (uc & 0x3ff) + 0xDC00);
return (4);
} else {
archive_be16enc(utf16, uc);
return (2);
}
}

static void
archive_le16enc(void *pp, uint16_t u)
{
unsigned char *p = (unsigned char *)pp;

p[0] = u & 0xff;
p[1] = (u >> 8) & 0xff;
}

static size_t
unicode_to_utf16le(char *p, uint32_t uc)
{
char *utf16 = p;

if (uc > 0xffff) {


uc -= 0x10000;
archive_le16enc(utf16, ((uc >> 10) & 0x3ff) + 0xD800);
archive_le16enc(utf16+2, (uc & 0x3ff) + 0xDC00);
return (4);
} else {
archive_le16enc(utf16, uc);
return (2);
}
}

static int
wc_size(void)
{
return (sizeof(wchar_t));
}

static int
unicode_to_wc(wchar_t *wp, uint32_t uc)
{
if (wc_size() == 4) {
*wp = (wchar_t)uc;
return (1);
}
if (uc > 0xffff) {


uc -= 0x10000;
*wp++ = (wchar_t)(((uc >> 10) & 0x3ff) + 0xD800);
*wp = (wchar_t)((uc & 0x3ff) + 0xDC00);
return (2);
} else {
*wp = (wchar_t)uc;
return (1);
}
}






static int
scan_unicode_pattern(char *out, wchar_t *wout, char *u16be, char *u16le,
const char *pattern, int mac_nfd)
{
unsigned uc = 0;
const char *p = pattern;
char *op = out;
wchar_t *owp = wout;
char *op16be = u16be;
char *op16le = u16le;
int ret = 0;

for (;;) {
if (*p >= '0' && *p <= '9')
uc = (uc << 4) + (*p - '0');
else if (*p >= 'A' && *p <= 'F')
uc = (uc << 4) + (*p - 'A' + 0x0a);
else {
if (mac_nfd && op == out) {






switch (uc) {
case 0x2194: case 0x219A: case 0x219B:
case 0x21AE: case 0x21CD: case 0x21CE:
case 0x21CF: case 0x2204: case 0x2209:
case 0x220C: case 0x2224: case 0x2226:
case 0x2241: case 0x2244: case 0x2247:
case 0x2249: case 0x2260: case 0x2262:
case 0x226D: case 0x226E: case 0x226F:
case 0x2270: case 0x2271: case 0x2274:
case 0x2275: case 0x2276: case 0x2278:
case 0x2279: case 0x227A: case 0x227B:
case 0x2280: case 0x2281: case 0x2284:
case 0x2285: case 0x2288: case 0x2289:
case 0x22AC: case 0x22AD: case 0x22AE:
case 0x22AF: case 0x22E0: case 0x22E1:
case 0x22E2: case 0x22E3: case 0x22EA:
case 0x22EB: case 0x22EC: case 0x22ED:










case 0x1109A: case 0x1109C: case 0x110AB:
ret = 1;
break;
}
}
op16be += unicode_to_utf16be(op16be, uc);
op16le += unicode_to_utf16le(op16le, uc);
owp += unicode_to_wc(owp, uc);
op += unicode_to_utf8(op, uc);
if (!*p) {
*op16be++ = 0;
*op16be = 0;
*op16le++ = 0;
*op16le = 0;
*owp = L'\0';
*op = '\0';
break;
}
uc = 0;
}
p++;
}
return (ret);
}

static int
is_wc_unicode(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
return (1);
#else
return (0);
#endif
}






static void
test_archive_string_normalization_nfc(const char *testdata)
{
struct archive *a, *a2;
struct archive_string utf8;
struct archive_mstring mstr;
struct archive_string_conv *f_sconv8, *t_sconv8;
struct archive_string_conv *f_sconv16be, *f_sconv16le;
FILE *fp;
char buff[512];
int line = 0;
int locale_is_utf8, wc_is_unicode;
int sconv_opt = SCONV_SET_OPT_NORMALIZATION_C;

locale_is_utf8 = (NULL != setlocale(LC_ALL, "en_US.UTF-8"));
wc_is_unicode = is_wc_unicode();

if (!locale_is_utf8 && !wc_is_unicode) {
skipping("A test of string normalization for NFC requires "
"a suitable locale; en_US.UTF-8 not available on this "
"system");
return;
}

archive_string_init(&utf8);
memset(&mstr, 0, sizeof(mstr));




assert((a = archive_read_new()) != NULL);
assertA(NULL != (f_sconv8 =
archive_string_conversion_from_charset(a, "UTF-8", 0)));
assertA(NULL != (f_sconv16be =
archive_string_conversion_from_charset(a, "UTF-16BE", 0)));
assertA(NULL != (f_sconv16le =
archive_string_conversion_from_charset(a, "UTF-16LE", 0)));
assert((a2 = archive_write_new()) != NULL);
assertA(NULL != (t_sconv8 =
archive_string_conversion_to_charset(a2, "UTF-8", 0)));
if (f_sconv8 == NULL || f_sconv16be == NULL || f_sconv16le == NULL ||
t_sconv8 == NULL) {

assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
archive_string_conversion_set_opt(f_sconv8, sconv_opt);
archive_string_conversion_set_opt(f_sconv16be, sconv_opt);
archive_string_conversion_set_opt(f_sconv16le, sconv_opt);
archive_string_conversion_set_opt(t_sconv8, sconv_opt);


assert((fp = fopen(testdata, "r")) != NULL);








while (fgets(buff, sizeof(buff), fp) != NULL) {
char nfc[80], nfd[80];
char utf8_nfc[80], utf8_nfd[80];
char utf16be_nfc[80], utf16be_nfd[80];
char utf16le_nfc[80], utf16le_nfd[80];
wchar_t wc_nfc[40], wc_nfd[40];
char *e, *p;
const wchar_t *wp;
const char *mp;
size_t mplen;

line++;
if (buff[0] == '#')
continue;
p = strchr(buff, ';');
if (p == NULL)
continue;
*p++ = '\0';

strncpy(nfc, buff, sizeof(nfc)-1);
nfc[sizeof(nfc)-1] = '\0';
e = p;
p = strchr(p, '\n');
if (p == NULL)
continue;
*p = '\0';

strncpy(nfd, e, sizeof(nfd)-1);
nfd[sizeof(nfd)-1] = '\0';




scan_unicode_pattern(utf8_nfc, wc_nfc, utf16be_nfc, utf16le_nfc,
nfc, 0);




scan_unicode_pattern(utf8_nfd, wc_nfd, utf16be_nfd, utf16le_nfd,
nfd, 0);

if (locale_is_utf8) {



assertEqualInt(0, archive_strcpy_l(
&utf8, utf8_nfd, f_sconv8));
failure("NFD(%s) should be converted to NFC(%s):%d",
nfd, nfc, line);
assertEqualUTF8String(utf8_nfc, utf8.s);




assertEqualInt(0, archive_strcpy_l(
&utf8, utf8_nfc, f_sconv8));
failure("NFC(%s) should not be any changed:%d",
nfc, line);
assertEqualUTF8String(utf8_nfc, utf8.s);




assertEqualInt(0, archive_strcpy_l(
&utf8, utf8_nfc, t_sconv8));
failure("NFC(%s) should not be any changed:%d",
nfc, line);
assertEqualUTF8String(utf8_nfc, utf8.s);




assertEqualInt(0, archive_strncpy_l(
&utf8, utf16be_nfd, 100000, f_sconv16be));
failure("NFD(%s) should be converted to NFC(%s):%d",
nfd, nfc, line);
assertEqualUTF8String(utf8_nfc, utf8.s);




assertEqualInt(0, archive_strncpy_l(
&utf8, utf16le_nfd, 100000, f_sconv16le));
failure("NFD(%s) should be converted to NFC(%s):%d",
nfd, nfc, line);
assertEqualUTF8String(utf8_nfc, utf8.s);
}








if (locale_is_utf8 || wc_is_unicode) {



assertEqualInt(0, archive_mstring_copy_mbs_len_l(
&mstr, utf8_nfd, 100000, f_sconv8));
assertEqualInt(0,
archive_mstring_get_wcs(a, &mstr, &wp));
failure("UTF-8 NFD(%s) should be converted "
"to WCS NFC(%s):%d", nfd, nfc, line);
assertEqualWString(wc_nfc, wp);




assertEqualInt(0, archive_mstring_copy_mbs_len_l(
&mstr, utf16be_nfd, 100000, f_sconv16be));
assertEqualInt(0,
archive_mstring_get_wcs(a, &mstr, &wp));
failure("UTF-8 NFD(%s) should be converted "
"to WCS NFC(%s):%d", nfd, nfc, line);
assertEqualWString(wc_nfc, wp);




assertEqualInt(0, archive_mstring_copy_mbs_len_l(
&mstr, utf16le_nfd, 100000, f_sconv16le));
assertEqualInt(0,
archive_mstring_get_wcs(a, &mstr, &wp));
failure("UTF-8 NFD(%s) should be converted "
"to WCS NFC(%s):%d", nfd, nfc, line);
assertEqualWString(wc_nfc, wp);




assertEqualInt(0,
archive_mstring_copy_wcs(&mstr, wc_nfc));
assertEqualInt(0, archive_mstring_get_mbs_l(
a, &mstr, &mp, &mplen, t_sconv8));
failure("WCS NFC(%s) should be UTF-8 NFC:%d"
,nfc, line);
assertEqualUTF8String(utf8_nfc, mp);
}
}

archive_string_free(&utf8);
archive_mstring_clean(&mstr);
fclose(fp);
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a2));
}

static void
test_archive_string_normalization_mac_nfd(const char *testdata)
{
struct archive *a, *a2;
struct archive_string utf8;
struct archive_mstring mstr;
struct archive_string_conv *f_sconv8, *t_sconv8;
struct archive_string_conv *f_sconv16be, *f_sconv16le;
FILE *fp;
char buff[512];
int line = 0;
int locale_is_utf8, wc_is_unicode;
int sconv_opt = SCONV_SET_OPT_NORMALIZATION_D;

locale_is_utf8 = (NULL != setlocale(LC_ALL, "en_US.UTF-8"));
wc_is_unicode = is_wc_unicode();

if (!locale_is_utf8 && !wc_is_unicode) {
skipping("A test of string normalization for NFD requires "
"a suitable locale; en_US.UTF-8 not available on this "
"system");
return;
}

archive_string_init(&utf8);
memset(&mstr, 0, sizeof(mstr));




assert((a = archive_read_new()) != NULL);
assertA(NULL != (f_sconv8 =
archive_string_conversion_from_charset(a, "UTF-8", 0)));
assertA(NULL != (f_sconv16be =
archive_string_conversion_from_charset(a, "UTF-16BE", 0)));
assertA(NULL != (f_sconv16le =
archive_string_conversion_from_charset(a, "UTF-16LE", 0)));
assert((a2 = archive_write_new()) != NULL);
assertA(NULL != (t_sconv8 =
archive_string_conversion_to_charset(a2, "UTF-8", 0)));
if (f_sconv8 == NULL || f_sconv16be == NULL || f_sconv16le == NULL ||
t_sconv8 == NULL) {

assertEqualInt(ARCHIVE_OK, archive_read_free(a));
return;
}
archive_string_conversion_set_opt(f_sconv8, sconv_opt);
archive_string_conversion_set_opt(f_sconv16be, sconv_opt);
archive_string_conversion_set_opt(f_sconv16le, sconv_opt);
archive_string_conversion_set_opt(t_sconv8, sconv_opt);


assert((fp = fopen(testdata, "r")) != NULL);








while (fgets(buff, sizeof(buff), fp) != NULL) {
char nfc[80], nfd[80];
char utf8_nfc[80], utf8_nfd[80];
char utf16be_nfc[80], utf16be_nfd[80];
char utf16le_nfc[80], utf16le_nfd[80];
wchar_t wc_nfc[40], wc_nfd[40];
char *e, *p;
const wchar_t *wp;
const char *mp;
size_t mplen;
int should_be_nfc;

line++;
if (buff[0] == '#')
continue;
p = strchr(buff, ';');
if (p == NULL)
continue;
*p++ = '\0';

strncpy(nfc, buff, sizeof(nfc)-1);
nfc[sizeof(nfc)-1] = '\0';
e = p;
p = strchr(p, '\n');
if (p == NULL)
continue;
*p = '\0';

strncpy(nfd, e, sizeof(nfd)-1);
nfd[sizeof(nfd)-1] = '\0';




should_be_nfc = scan_unicode_pattern(utf8_nfc, wc_nfc,
utf16be_nfc, utf16le_nfc, nfc, 1);




scan_unicode_pattern(utf8_nfd, wc_nfd, utf16be_nfd, utf16le_nfd,
nfd, 0);

if (locale_is_utf8) {



assertEqualInt(0, archive_strcpy_l(
&utf8, utf8_nfc, f_sconv8));
if (should_be_nfc) {
failure("NFC(%s) should not be converted to"
" NFD(%s):%d", nfc, nfd, line);
assertEqualUTF8String(utf8_nfc, utf8.s);
} else {
failure("NFC(%s) should be converted to"
" NFD(%s):%d", nfc, nfd, line);
assertEqualUTF8String(utf8_nfd, utf8.s);
}




assertEqualInt(0, archive_strcpy_l(
&utf8, utf8_nfd, f_sconv8));
failure("NFD(%s) should not be any changed:%d",
nfd, line);
assertEqualUTF8String(utf8_nfd, utf8.s);




assertEqualInt(0, archive_strcpy_l(
&utf8, utf8_nfd, t_sconv8));
failure("NFD(%s) should not be any changed:%d",
nfd, line);
assertEqualUTF8String(utf8_nfd, utf8.s);




assertEqualInt(0, archive_strncpy_l(
&utf8, utf16be_nfc, 100000, f_sconv16be));
if (should_be_nfc) {
failure("NFC(%s) should not be converted to"
" NFD(%s):%d", nfc, nfd, line);
assertEqualUTF8String(utf8_nfc, utf8.s);
} else {
failure("NFC(%s) should be converted to"
" NFD(%s):%d", nfc, nfd, line);
assertEqualUTF8String(utf8_nfd, utf8.s);
}




assertEqualInt(0, archive_strncpy_l(
&utf8, utf16le_nfc, 100000, f_sconv16le));
if (should_be_nfc) {
failure("NFC(%s) should not be converted to"
" NFD(%s):%d", nfc, nfd, line);
assertEqualUTF8String(utf8_nfc, utf8.s);
} else {
failure("NFC(%s) should be converted to"
" NFD(%s):%d", nfc, nfd, line);
assertEqualUTF8String(utf8_nfd, utf8.s);
}
}








if (locale_is_utf8 || wc_is_unicode) {



assertEqualInt(0, archive_mstring_copy_mbs_len_l(
&mstr, utf8_nfc, 100000, f_sconv8));
assertEqualInt(0,
archive_mstring_get_wcs(a, &mstr, &wp));
if (should_be_nfc) {
failure("UTF-8 NFC(%s) should not be converted "
"to WCS NFD(%s):%d", nfc, nfd, line);
assertEqualWString(wc_nfc, wp);
} else {
failure("UTF-8 NFC(%s) should be converted "
"to WCS NFD(%s):%d", nfc, nfd, line);
assertEqualWString(wc_nfd, wp);
}




assertEqualInt(0, archive_mstring_copy_mbs_len_l(
&mstr, utf16be_nfc, 100000, f_sconv16be));
assertEqualInt(0,
archive_mstring_get_wcs(a, &mstr, &wp));
if (should_be_nfc) {
failure("UTF-16BE NFC(%s) should not be "
"converted to WCS NFD(%s):%d",
nfc, nfd, line);
assertEqualWString(wc_nfc, wp);
} else {
failure("UTF-16BE NFC(%s) should be converted "
"to WCS NFD(%s):%d", nfc, nfd, line);
assertEqualWString(wc_nfd, wp);
}




assertEqualInt(0, archive_mstring_copy_mbs_len_l(
&mstr, utf16le_nfc, 100000, f_sconv16le));
assertEqualInt(0,
archive_mstring_get_wcs(a, &mstr, &wp));
if (should_be_nfc) {
failure("UTF-16LE NFC(%s) should not be "
"converted to WCS NFD(%s):%d",
nfc, nfd, line);
assertEqualWString(wc_nfc, wp);
} else {
failure("UTF-16LE NFC(%s) should be converted "
"to WCS NFD(%s):%d", nfc, nfd, line);
assertEqualWString(wc_nfd, wp);
}




assertEqualInt(0, archive_mstring_copy_wcs(
&mstr, wc_nfd));
assertEqualInt(0, archive_mstring_get_mbs_l(
a, &mstr, &mp, &mplen, t_sconv8));
failure("WCS NFD(%s) should be UTF-8 NFD:%d"
,nfd, line);
assertEqualUTF8String(utf8_nfd, mp);
}
}

archive_string_free(&utf8);
archive_mstring_clean(&mstr);
fclose(fp);
assertEqualInt(ARCHIVE_OK, archive_read_free(a));
assertEqualInt(ARCHIVE_OK, archive_write_free(a2));
}

static void
test_archive_string_canonicalization(void)
{
struct archive *a;
struct archive_string_conv *sconv;

setlocale(LC_ALL, "en_US.UTF-8");

assert((a = archive_read_new()) != NULL);

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "UTF-8", 1)));
failure("Charset name should be UTF-8");
assertEqualString("UTF-8",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "UTF8", 1)));
failure("Charset name should be UTF-8");
assertEqualString("UTF-8",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "utf8", 1)));
failure("Charset name should be UTF-8");
assertEqualString("UTF-8",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "UTF-16BE", 1)));
failure("Charset name should be UTF-16BE");
assertEqualString("UTF-16BE",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "UTF16BE", 1)));
failure("Charset name should be UTF-16BE");
assertEqualString("UTF-16BE",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "utf16be", 1)));
failure("Charset name should be UTF-16BE");
assertEqualString("UTF-16BE",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "UTF-16LE", 1)));
failure("Charset name should be UTF-16LE");
assertEqualString("UTF-16LE",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "UTF16LE", 1)));
failure("Charset name should be UTF-16LE");
assertEqualString("UTF-16LE",
archive_string_conversion_charset_name(sconv));

assertA(NULL != (sconv =
archive_string_conversion_to_charset(a, "utf16le", 1)));
failure("Charset name should be UTF-16LE");
assertEqualString("UTF-16LE",
archive_string_conversion_charset_name(sconv));

assertEqualInt(ARCHIVE_OK, archive_read_free(a));

}

static void
check_string(struct archive *a, struct archive_mstring *mstr, struct archive_string_conv *sc,
const char *exp, const wchar_t *wexp)
{

struct archive_mstring mstr2;
const char *p = NULL;
const wchar_t *wp = NULL;
size_t len = 0;

memset(&mstr2, 0, sizeof(mstr2));

archive_mstring_copy(&mstr2, mstr);
assertEqualInt(0, archive_mstring_get_mbs(a, &mstr2, &p));
assertEqualString(exp, p);
p = NULL;

archive_mstring_copy(&mstr2, mstr);
assertEqualInt(0, archive_mstring_get_utf8(a, &mstr2, &p));
assertEqualString(exp, p);
p = NULL;

archive_mstring_copy(&mstr2, mstr);
assertEqualInt(0, archive_mstring_get_wcs(a, &mstr2, &wp));
assertEqualWString(wexp, wp);
wp = NULL;

archive_mstring_copy(&mstr2, mstr);
assertEqualInt(0, archive_mstring_get_mbs_l(a, &mstr2, &p, &len, sc));
assertEqualString(exp, p);
assertEqualInt(len, strlen(exp));
p = NULL;
len = 0;

archive_mstring_clean(&mstr2);
}





static void
test_archive_string_set_get(void)
{
struct archive *a;
struct archive_mstring mstr;
struct archive_string_conv *sc;

setlocale(LC_ALL, "en_US.UTF-8");

assert((a = archive_read_new()) != NULL);
memset(&mstr, 0, sizeof(mstr));

assertA(NULL != (sc =
archive_string_conversion_to_charset(a, "UTF-8", 1)));
failure("Charset name should be UTF-8");
assertEqualString("UTF-8",
archive_string_conversion_charset_name(sc));

assertEqualInt(0, archive_mstring_copy_mbs(&mstr, "AAA"));
check_string(a, &mstr, sc, "AAA", L"AAA");
assertEqualInt(4, archive_mstring_copy_utf8(&mstr, "BBBB"));
check_string(a, &mstr, sc, "BBBB", L"BBBB");
assertEqualInt(0, archive_mstring_copy_wcs(&mstr, L"CCC12"));
check_string(a, &mstr, sc, "CCC12", L"CCC12");
assertEqualInt(0, archive_mstring_copy_mbs_len_l(&mstr, "DDDD-l", 6, sc));
check_string(a, &mstr, sc, "DDDD-l", L"DDDD-l");
assertEqualInt(0, archive_mstring_update_utf8(a, &mstr, "EEEEE---H"));
check_string(a, &mstr, sc, "EEEEE---H", L"EEEEE---H");

assertEqualInt(ARCHIVE_OK, archive_read_free(a));

}

DEFINE_TEST(test_archive_string_conversion)
{
static const char reffile[] = "test_archive_string_conversion.txt.Z";
static const char testdata[] = "testdata.txt";
struct archive *a;
struct archive_entry *ae;
char buff[512];
ssize_t size;
FILE *fp;




extract_reference_file(reffile);
assert((a = archive_read_new()) != NULL);
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_raw(a));
assertEqualIntA(a, ARCHIVE_OK,
archive_read_open_filename(a, reffile, 512));

assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
assert((fp = fopen(testdata, "w")) != NULL);
while ((size = archive_read_data(a, buff, 512)) > 0)
assertEqualInt(size, fwrite(buff, 1, size, fp));
assertEqualInt(0, fclose(fp));
assertEqualInt(ARCHIVE_OK, archive_read_free(a));

test_archive_string_normalization_nfc(testdata);
test_archive_string_normalization_mac_nfd(testdata);
test_archive_string_canonicalization();
test_archive_string_set_get();
}
