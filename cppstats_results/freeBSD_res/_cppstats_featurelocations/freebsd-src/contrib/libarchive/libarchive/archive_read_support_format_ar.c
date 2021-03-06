


























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_read_private.h"

struct ar {
int64_t entry_bytes_remaining;




size_t entry_bytes_unconsumed;
int64_t entry_offset;
int64_t entry_padding;
char *strtab;
size_t strtab_size;
char read_global_header;
};




#define AR_name_offset 0
#define AR_name_size 16
#define AR_date_offset 16
#define AR_date_size 12
#define AR_uid_offset 28
#define AR_uid_size 6
#define AR_gid_offset 34
#define AR_gid_size 6
#define AR_mode_offset 40
#define AR_mode_size 8
#define AR_size_offset 48
#define AR_size_size 10
#define AR_fmag_offset 58
#define AR_fmag_size 2

static int archive_read_format_ar_bid(struct archive_read *a, int);
static int archive_read_format_ar_cleanup(struct archive_read *a);
static int archive_read_format_ar_read_data(struct archive_read *a,
const void **buff, size_t *size, int64_t *offset);
static int archive_read_format_ar_skip(struct archive_read *a);
static int archive_read_format_ar_read_header(struct archive_read *a,
struct archive_entry *e);
static uint64_t ar_atol8(const char *p, unsigned char_cnt);
static uint64_t ar_atol10(const char *p, unsigned char_cnt);
static int ar_parse_gnu_filename_table(struct archive_read *a);
static int ar_parse_common_header(struct ar *ar, struct archive_entry *,
const char *h);

int
archive_read_support_format_ar(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct ar *ar;
int r;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_format_ar");

ar = (struct ar *)calloc(1, sizeof(*ar));
if (ar == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate ar data");
return (ARCHIVE_FATAL);
}
ar->strtab = NULL;

r = __archive_read_register_format(a,
ar,
"ar",
archive_read_format_ar_bid,
NULL,
archive_read_format_ar_read_header,
archive_read_format_ar_read_data,
archive_read_format_ar_skip,
NULL,
archive_read_format_ar_cleanup,
NULL,
NULL);

if (r != ARCHIVE_OK) {
free(ar);
return (r);
}
return (ARCHIVE_OK);
}

static int
archive_read_format_ar_cleanup(struct archive_read *a)
{
struct ar *ar;

ar = (struct ar *)(a->format->data);
free(ar->strtab);
free(ar);
(a->format->data) = NULL;
return (ARCHIVE_OK);
}

static int
archive_read_format_ar_bid(struct archive_read *a, int best_bid)
{
const void *h;

(void)best_bid;





if ((h = __archive_read_ahead(a, 8, NULL)) == NULL)
return (-1);
if (memcmp(h, "!<arch>\n", 8) == 0) {
return (64);
}
return (-1);
}

static int
_ar_read_header(struct archive_read *a, struct archive_entry *entry,
struct ar *ar, const char *h, size_t *unconsumed)
{
char filename[AR_name_size + 1];
uint64_t number;
size_t bsd_name_length, entry_size;
char *p, *st;
const void *b;
int r;


if (strncmp(h + AR_fmag_offset, "`\n", 2) != 0) {
archive_set_error(&a->archive, EINVAL,
"Incorrect file header signature");
return (ARCHIVE_FATAL);
}


strncpy(filename, h + AR_name_offset, AR_name_size);
filename[AR_name_size] = '\0';




if (a->archive.archive_format == ARCHIVE_FORMAT_AR) {







if (strncmp(filename, "#1/", 3) == 0)
a->archive.archive_format = ARCHIVE_FORMAT_AR_BSD;
else if (strchr(filename, '/') != NULL)
a->archive.archive_format = ARCHIVE_FORMAT_AR_GNU;
else if (strncmp(filename, "__.SYMDEF", 9) == 0)
a->archive.archive_format = ARCHIVE_FORMAT_AR_BSD;





}


if (a->archive.archive_format == ARCHIVE_FORMAT_AR_GNU)
a->archive.archive_format_name = "ar (GNU/SVR4)";
else if (a->archive.archive_format == ARCHIVE_FORMAT_AR_BSD)
a->archive.archive_format_name = "ar (BSD)";
else
a->archive.archive_format_name = "ar";








p = filename + AR_name_size - 1;
while (p >= filename && *p == ' ') {
*p = '\0';
p--;
}








if (filename[0] != '/' && p > filename && *p == '/') {
*p = '\0';
}

if (p < filename) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Found entry with empty filename");
return (ARCHIVE_FATAL);
}





if (strcmp(filename, "//") == 0) {

ar_parse_common_header(ar, entry, h);
archive_entry_copy_pathname(entry, filename);
archive_entry_set_filetype(entry, AE_IFREG);

number = ar_atol10(h + AR_size_offset, AR_size_size);
if (number > SIZE_MAX || number > 1024 * 1024 * 1024) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Filename table too large");
return (ARCHIVE_FATAL);
}
entry_size = (size_t)number;
if (entry_size == 0) {
archive_set_error(&a->archive, EINVAL,
"Invalid string table");
return (ARCHIVE_FATAL);
}
if (ar->strtab != NULL) {
archive_set_error(&a->archive, EINVAL,
"More than one string tables exist");
return (ARCHIVE_FATAL);
}


st = malloc(entry_size);
if (st == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate filename table buffer");
return (ARCHIVE_FATAL);
}
ar->strtab = st;
ar->strtab_size = entry_size;

if (*unconsumed) {
__archive_read_consume(a, *unconsumed);
*unconsumed = 0;
}

if ((b = __archive_read_ahead(a, entry_size, NULL)) == NULL)
return (ARCHIVE_FATAL);
memcpy(st, b, entry_size);
__archive_read_consume(a, entry_size);

ar->entry_bytes_remaining = 0;
archive_entry_set_size(entry, ar->entry_bytes_remaining);


return (ar_parse_gnu_filename_table(a));
}







if (filename[0] == '/' && filename[1] >= '0' && filename[1] <= '9') {
number = ar_atol10(h + AR_name_offset + 1, AR_name_size - 1);




if (ar->strtab == NULL || number >= ar->strtab_size) {
archive_set_error(&a->archive, EINVAL,
"Can't find long filename for GNU/SVR4 archive entry");
archive_entry_copy_pathname(entry, filename);

ar_parse_common_header(ar, entry, h);
return (ARCHIVE_FATAL);
}

archive_entry_copy_pathname(entry, &ar->strtab[(size_t)number]);

return (ar_parse_common_header(ar, entry, h));
}






if (strncmp(filename, "#1/", 3) == 0) {


ar_parse_common_header(ar, entry, h);


number = ar_atol10(h + AR_name_offset + 3, AR_name_size - 3);





if (number > SIZE_MAX - 1
|| number > 1024 * 1024
|| (int64_t)number > ar->entry_bytes_remaining) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Bad input file size");
return (ARCHIVE_FATAL);
}
bsd_name_length = (size_t)number;
ar->entry_bytes_remaining -= bsd_name_length;

archive_entry_set_size(entry, ar->entry_bytes_remaining);

if (*unconsumed) {
__archive_read_consume(a, *unconsumed);
*unconsumed = 0;
}


if ((b = __archive_read_ahead(a, bsd_name_length, NULL)) == NULL) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Truncated input file");
return (ARCHIVE_FATAL);
}

p = (char *)malloc(bsd_name_length + 1);
if (p == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate fname buffer");
return (ARCHIVE_FATAL);
}
strncpy(p, b, bsd_name_length);
p[bsd_name_length] = '\0';

__archive_read_consume(a, bsd_name_length);

archive_entry_copy_pathname(entry, p);
free(p);
return (ARCHIVE_OK);
}





if (strcmp(filename, "/") == 0 || strcmp(filename, "/SYM64/") == 0) {
archive_entry_copy_pathname(entry, filename);

r = ar_parse_common_header(ar, entry, h);

archive_entry_set_filetype(entry, AE_IFREG);
return (r);
}




if (strcmp(filename, "__.SYMDEF") == 0) {
archive_entry_copy_pathname(entry, filename);

return (ar_parse_common_header(ar, entry, h));
}






archive_entry_copy_pathname(entry, filename);
return (ar_parse_common_header(ar, entry, h));
}

static int
archive_read_format_ar_read_header(struct archive_read *a,
struct archive_entry *entry)
{
struct ar *ar = (struct ar*)(a->format->data);
size_t unconsumed;
const void *header_data;
int ret;

if (!ar->read_global_header) {




__archive_read_consume(a, 8);
ar->read_global_header = 1;

a->archive.archive_format = ARCHIVE_FORMAT_AR;
}


if ((header_data = __archive_read_ahead(a, 60, NULL)) == NULL)

return (ARCHIVE_EOF);

unconsumed = 60;

ret = _ar_read_header(a, entry, ar, (const char *)header_data, &unconsumed);

if (unconsumed)
__archive_read_consume(a, unconsumed);

return ret;
}


static int
ar_parse_common_header(struct ar *ar, struct archive_entry *entry,
const char *h)
{
uint64_t n;


archive_entry_set_filetype(entry, AE_IFREG);
archive_entry_set_mtime(entry,
(time_t)ar_atol10(h + AR_date_offset, AR_date_size), 0L);
archive_entry_set_uid(entry,
(uid_t)ar_atol10(h + AR_uid_offset, AR_uid_size));
archive_entry_set_gid(entry,
(gid_t)ar_atol10(h + AR_gid_offset, AR_gid_size));
archive_entry_set_mode(entry,
(mode_t)ar_atol8(h + AR_mode_offset, AR_mode_size));
n = ar_atol10(h + AR_size_offset, AR_size_size);

ar->entry_offset = 0;
ar->entry_padding = n % 2;
archive_entry_set_size(entry, n);
ar->entry_bytes_remaining = n;
return (ARCHIVE_OK);
}

static int
archive_read_format_ar_read_data(struct archive_read *a,
const void **buff, size_t *size, int64_t *offset)
{
ssize_t bytes_read;
struct ar *ar;

ar = (struct ar *)(a->format->data);

if (ar->entry_bytes_unconsumed) {
__archive_read_consume(a, ar->entry_bytes_unconsumed);
ar->entry_bytes_unconsumed = 0;
}

if (ar->entry_bytes_remaining > 0) {
*buff = __archive_read_ahead(a, 1, &bytes_read);
if (bytes_read == 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Truncated ar archive");
return (ARCHIVE_FATAL);
}
if (bytes_read < 0)
return (ARCHIVE_FATAL);
if (bytes_read > ar->entry_bytes_remaining)
bytes_read = (ssize_t)ar->entry_bytes_remaining;
*size = bytes_read;
ar->entry_bytes_unconsumed = bytes_read;
*offset = ar->entry_offset;
ar->entry_offset += bytes_read;
ar->entry_bytes_remaining -= bytes_read;
return (ARCHIVE_OK);
} else {
int64_t skipped = __archive_read_consume(a, ar->entry_padding);
if (skipped >= 0) {
ar->entry_padding -= skipped;
}
if (ar->entry_padding) {
if (skipped >= 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Truncated ar archive- failed consuming padding");
}
return (ARCHIVE_FATAL);
}
*buff = NULL;
*size = 0;
*offset = ar->entry_offset;
return (ARCHIVE_EOF);
}
}

static int
archive_read_format_ar_skip(struct archive_read *a)
{
int64_t bytes_skipped;
struct ar* ar;

ar = (struct ar *)(a->format->data);

bytes_skipped = __archive_read_consume(a,
ar->entry_bytes_remaining + ar->entry_padding
+ ar->entry_bytes_unconsumed);
if (bytes_skipped < 0)
return (ARCHIVE_FATAL);

ar->entry_bytes_remaining = 0;
ar->entry_bytes_unconsumed = 0;
ar->entry_padding = 0;

return (ARCHIVE_OK);
}

static int
ar_parse_gnu_filename_table(struct archive_read *a)
{
struct ar *ar;
char *p;
size_t size;

ar = (struct ar*)(a->format->data);
size = ar->strtab_size;

for (p = ar->strtab; p < ar->strtab + size - 1; ++p) {
if (*p == '/') {
*p++ = '\0';
if (*p != '\n')
goto bad_string_table;
*p = '\0';
}
}




if (p != ar->strtab + size && *p != '\n' && *p != '`')
goto bad_string_table;


ar->strtab[size - 1] = '\0';

return (ARCHIVE_OK);

bad_string_table:
archive_set_error(&a->archive, EINVAL,
"Invalid string table");
free(ar->strtab);
ar->strtab = NULL;
return (ARCHIVE_FATAL);
}

static uint64_t
ar_atol8(const char *p, unsigned char_cnt)
{
uint64_t l, limit, last_digit_limit;
unsigned int digit, base;

base = 8;
limit = UINT64_MAX / base;
last_digit_limit = UINT64_MAX % base;

while ((*p == ' ' || *p == '\t') && char_cnt-- > 0)
p++;

l = 0;
digit = *p - '0';
while (*p >= '0' && digit < base && char_cnt-- > 0) {
if (l>limit || (l == limit && digit > last_digit_limit)) {
l = UINT64_MAX;
break;
}
l = (l * base) + digit;
digit = *++p - '0';
}
return (l);
}

static uint64_t
ar_atol10(const char *p, unsigned char_cnt)
{
uint64_t l, limit, last_digit_limit;
unsigned int base, digit;

base = 10;
limit = UINT64_MAX / base;
last_digit_limit = UINT64_MAX % base;

while ((*p == ' ' || *p == '\t') && char_cnt-- > 0)
p++;
l = 0;
digit = *p - '0';
while (*p >= '0' && digit < base && char_cnt-- > 0) {
if (l > limit || (l == limit && digit > last_digit_limit)) {
l = UINT64_MAX;
break;
}
l = (l * base) + digit;
digit = *++p - '0';
}
return (l);
}
