


























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_write_private.h"
#include "archive_write_set_format_private.h"

struct ar_w {
uint64_t entry_bytes_remaining;
uint64_t entry_padding;
int is_strtab;
int has_strtab;
char wrote_global_header;
char *strtab;
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

static int archive_write_set_format_ar(struct archive_write *);
static int archive_write_ar_header(struct archive_write *,
struct archive_entry *);
static ssize_t archive_write_ar_data(struct archive_write *,
const void *buff, size_t s);
static int archive_write_ar_free(struct archive_write *);
static int archive_write_ar_close(struct archive_write *);
static int archive_write_ar_finish_entry(struct archive_write *);
static const char *ar_basename(const char *path);
static int format_octal(int64_t v, char *p, int s);
static int format_decimal(int64_t v, char *p, int s);

int
archive_write_set_format_ar_bsd(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
int r;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_set_format_ar_bsd");
r = archive_write_set_format_ar(a);
if (r == ARCHIVE_OK) {
a->archive.archive_format = ARCHIVE_FORMAT_AR_BSD;
a->archive.archive_format_name = "ar (BSD)";
}
return (r);
}

int
archive_write_set_format_ar_svr4(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
int r;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_set_format_ar_svr4");
r = archive_write_set_format_ar(a);
if (r == ARCHIVE_OK) {
a->archive.archive_format = ARCHIVE_FORMAT_AR_GNU;
a->archive.archive_format_name = "ar (GNU/SVR4)";
}
return (r);
}




static int
archive_write_set_format_ar(struct archive_write *a)
{
struct ar_w *ar;


if (a->format_free != NULL)
(a->format_free)(a);

ar = (struct ar_w *)calloc(1, sizeof(*ar));
if (ar == NULL) {
archive_set_error(&a->archive, ENOMEM, "Can't allocate ar data");
return (ARCHIVE_FATAL);
}
a->format_data = ar;

a->format_name = "ar";
a->format_write_header = archive_write_ar_header;
a->format_write_data = archive_write_ar_data;
a->format_close = archive_write_ar_close;
a->format_free = archive_write_ar_free;
a->format_finish_entry = archive_write_ar_finish_entry;
return (ARCHIVE_OK);
}

static int
archive_write_ar_header(struct archive_write *a, struct archive_entry *entry)
{
int ret, append_fn;
char buff[60];
char *ss, *se;
struct ar_w *ar;
const char *pathname;
const char *filename;
int64_t size;

append_fn = 0;
ar = (struct ar_w *)a->format_data;
ar->is_strtab = 0;
filename = NULL;
size = archive_entry_size(entry);





pathname = archive_entry_pathname(entry);
if (pathname == NULL || *pathname == '\0') {
archive_set_error(&a->archive, EINVAL,
"Invalid filename");
return (ARCHIVE_WARN);
}





if (!ar->wrote_global_header) {
__archive_write_output(a, "!<arch>\n", 8);
ar->wrote_global_header = 1;
}

memset(buff, ' ', 60);
memcpy(&buff[AR_fmag_offset], "`\n", 2);

if (strcmp(pathname, "/") == 0 ) {

buff[AR_name_offset] = '/';
goto stat;
}
if (strcmp(pathname, "/SYM64/") == 0) {

memcpy(buff + AR_name_offset, "/SYM64/", 7);
goto stat;
}
if (strcmp(pathname, "__.SYMDEF") == 0) {

memcpy(buff + AR_name_offset, "__.SYMDEF", 9);
goto stat;
}
if (strcmp(pathname, "//") == 0) {




ar->is_strtab = 1;
buff[AR_name_offset] = buff[AR_name_offset + 1] = '/';




goto size;
}





if ((filename = ar_basename(pathname)) == NULL) {

archive_set_error(&a->archive, EINVAL,
"Invalid filename");
return (ARCHIVE_WARN);
}

if (a->archive.archive_format == ARCHIVE_FORMAT_AR_GNU) {






if (strlen(filename) <= 15) {
memcpy(&buff[AR_name_offset],
filename, strlen(filename));
buff[AR_name_offset + strlen(filename)] = '/';
} else {






if (ar->has_strtab <= 0) {
archive_set_error(&a->archive, EINVAL,
"Can't find string table");
return (ARCHIVE_WARN);
}

se = (char *)malloc(strlen(filename) + 3);
if (se == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate filename buffer");
return (ARCHIVE_FATAL);
}

memcpy(se, filename, strlen(filename));
strcpy(se + strlen(filename), "/\n");

ss = strstr(ar->strtab, se);
free(se);

if (ss == NULL) {
archive_set_error(&a->archive, EINVAL,
"Invalid string table");
return (ARCHIVE_WARN);
}






buff[AR_name_offset] = '/';
if (format_decimal(ss - ar->strtab,
buff + AR_name_offset + 1,
AR_name_size - 1)) {
archive_set_error(&a->archive, ERANGE,
"string table offset too large");
return (ARCHIVE_WARN);
}
}
} else if (a->archive.archive_format == ARCHIVE_FORMAT_AR_BSD) {









if (strlen(filename) <= 16 && strchr(filename, ' ') == NULL) {
memcpy(&buff[AR_name_offset], filename, strlen(filename));
buff[AR_name_offset + strlen(filename)] = ' ';
}
else {
memcpy(buff + AR_name_offset, "#1/", 3);
if (format_decimal(strlen(filename),
buff + AR_name_offset + 3,
AR_name_size - 3)) {
archive_set_error(&a->archive, ERANGE,
"File name too long");
return (ARCHIVE_WARN);
}
append_fn = 1;
size += strlen(filename);
}
}

stat:
if (format_decimal(archive_entry_mtime(entry), buff + AR_date_offset, AR_date_size)) {
archive_set_error(&a->archive, ERANGE,
"File modification time too large");
return (ARCHIVE_WARN);
}
if (format_decimal(archive_entry_uid(entry), buff + AR_uid_offset, AR_uid_size)) {
archive_set_error(&a->archive, ERANGE,
"Numeric user ID too large");
return (ARCHIVE_WARN);
}
if (format_decimal(archive_entry_gid(entry), buff + AR_gid_offset, AR_gid_size)) {
archive_set_error(&a->archive, ERANGE,
"Numeric group ID too large");
return (ARCHIVE_WARN);
}
if (format_octal(archive_entry_mode(entry), buff + AR_mode_offset, AR_mode_size)) {
archive_set_error(&a->archive, ERANGE,
"Numeric mode too large");
return (ARCHIVE_WARN);
}




if (filename != NULL && archive_entry_filetype(entry) != AE_IFREG) {
archive_set_error(&a->archive, EINVAL,
"Regular file required for non-pseudo member");
return (ARCHIVE_WARN);
}

size:
if (format_decimal(size, buff + AR_size_offset, AR_size_size)) {
archive_set_error(&a->archive, ERANGE,
"File size out of range");
return (ARCHIVE_WARN);
}

ret = __archive_write_output(a, buff, 60);
if (ret != ARCHIVE_OK)
return (ret);

ar->entry_bytes_remaining = size;
ar->entry_padding = ar->entry_bytes_remaining % 2;

if (append_fn > 0) {
ret = __archive_write_output(a, filename, strlen(filename));
if (ret != ARCHIVE_OK)
return (ret);
ar->entry_bytes_remaining -= strlen(filename);
}

return (ARCHIVE_OK);
}

static ssize_t
archive_write_ar_data(struct archive_write *a, const void *buff, size_t s)
{
struct ar_w *ar;
int ret;

ar = (struct ar_w *)a->format_data;
if (s > ar->entry_bytes_remaining)
s = (size_t)ar->entry_bytes_remaining;

if (ar->is_strtab > 0) {
if (ar->has_strtab > 0) {
archive_set_error(&a->archive, EINVAL,
"More than one string tables exist");
return (ARCHIVE_WARN);
}

ar->strtab = (char *)malloc(s + 1);
if (ar->strtab == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate strtab buffer");
return (ARCHIVE_FATAL);
}
memcpy(ar->strtab, buff, s);
ar->strtab[s] = '\0';
ar->has_strtab = 1;
}

ret = __archive_write_output(a, buff, s);
if (ret != ARCHIVE_OK)
return (ret);

ar->entry_bytes_remaining -= s;
return (s);
}

static int
archive_write_ar_free(struct archive_write *a)
{
struct ar_w *ar;

ar = (struct ar_w *)a->format_data;

if (ar == NULL)
return (ARCHIVE_OK);

if (ar->has_strtab > 0) {
free(ar->strtab);
ar->strtab = NULL;
}

free(ar);
a->format_data = NULL;
return (ARCHIVE_OK);
}

static int
archive_write_ar_close(struct archive_write *a)
{
struct ar_w *ar;
int ret;





ar = (struct ar_w *)a->format_data;
if (!ar->wrote_global_header) {
ar->wrote_global_header = 1;
ret = __archive_write_output(a, "!<arch>\n", 8);
return (ret);
}

return (ARCHIVE_OK);
}

static int
archive_write_ar_finish_entry(struct archive_write *a)
{
struct ar_w *ar;
int ret;

ar = (struct ar_w *)a->format_data;

if (ar->entry_bytes_remaining != 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Entry remaining bytes larger than 0");
return (ARCHIVE_WARN);
}

if (ar->entry_padding == 0) {
return (ARCHIVE_OK);
}

if (ar->entry_padding != 1) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Padding wrong size: %ju should be 1 or 0",
(uintmax_t)ar->entry_padding);
return (ARCHIVE_WARN);
}

ret = __archive_write_output(a, "\n", 1);
return (ret);
}






static int
format_octal(int64_t v, char *p, int s)
{
int len;
char *h;

len = s;
h = p;


if (v < 0) {
while (len-- > 0)
*p++ = '0';
return (-1);
}

p += s;
do {
*--p = (char)('0' + (v & 7));
v >>= 3;
} while (--s > 0 && v > 0);

if (v == 0) {
memmove(h, p, len - s);
p = h + len - s;
while (s-- > 0)
*p++ = ' ';
return (0);
}

while (len-- > 0)
*p++ = '7';

return (-1);
}




static int
format_decimal(int64_t v, char *p, int s)
{
int len;
char *h;

len = s;
h = p;


if (v < 0) {
while (len-- > 0)
*p++ = '0';
return (-1);
}

p += s;
do {
*--p = (char)('0' + (v % 10));
v /= 10;
} while (--s > 0 && v > 0);

if (v == 0) {
memmove(h, p, len - s);
p = h + len - s;
while (s-- > 0)
*p++ = ' ';
return (0);
}

while (len-- > 0)
*p++ = '9';

return (-1);
}

static const char *
ar_basename(const char *path)
{
const char *endp, *startp;

endp = path + strlen(path) - 1;




if (*endp == '/')
return (NULL);


startp = endp;
while (startp > path && *(startp - 1) != '/')
startp--;

return (startp);
}
