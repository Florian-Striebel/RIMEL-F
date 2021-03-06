
























#include "bsdtar_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#include <ctype.h>
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_IO_H)
#include <io.h>
#endif
#if defined(HAVE_STDARG_H)
#include <stdarg.h>
#endif
#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_WCTYPE_H)
#include <wctype.h>
#else

#define iswprint isprint
#endif

#include "bsdtar.h"
#include "err.h"
#include "passphrase.h"

static size_t bsdtar_expand_char(char *, size_t, char);
static const char *strip_components(const char *path, int elements);

#if defined(_WIN32) && !defined(__CYGWIN__)
#define read _read
#endif





















void
safe_fprintf(FILE *f, const char *fmt, ...)
{
char fmtbuff_stack[256];
char outbuff[256];
char *fmtbuff_heap;
char *fmtbuff;
int fmtbuff_length;
int length, n;
va_list ap;
const char *p;
unsigned i;
wchar_t wc;
char try_wc;


fmtbuff_heap = NULL;
fmtbuff_length = sizeof(fmtbuff_stack);
fmtbuff = fmtbuff_stack;


va_start(ap, fmt);
length = vsnprintf(fmtbuff, fmtbuff_length, fmt, ap);
va_end(ap);


while (length < 0 || length >= fmtbuff_length) {
if (length >= fmtbuff_length)
fmtbuff_length = length+1;
else if (fmtbuff_length < 8192)
fmtbuff_length *= 2;
else if (fmtbuff_length < 1000000)
fmtbuff_length += fmtbuff_length / 4;
else {
length = fmtbuff_length;
fmtbuff_heap[length-1] = '\0';
break;
}
free(fmtbuff_heap);
fmtbuff_heap = malloc(fmtbuff_length);


if (fmtbuff_heap != NULL) {
fmtbuff = fmtbuff_heap;
va_start(ap, fmt);
length = vsnprintf(fmtbuff, fmtbuff_length, fmt, ap);
va_end(ap);
} else {


fmtbuff = fmtbuff_stack;
length = sizeof(fmtbuff_stack) - 1;
break;
}
}



if (mbtowc(NULL, NULL, 1) == -1) {


free(fmtbuff_heap);
return;
}


p = fmtbuff;
i = 0;
try_wc = 1;
while (*p != '\0') {



if (try_wc && (n = mbtowc(&wc, p, length)) != -1) {
length -= n;
if (iswprint(wc) && wc != L'\\') {

while (n-- > 0)
outbuff[i++] = *p++;
} else {

while (n-- > 0)
i += (unsigned)bsdtar_expand_char(
outbuff, i, *p++);
}
} else {


i += (unsigned)bsdtar_expand_char(outbuff, i, *p++);
try_wc = 0;
}


if (i > (sizeof(outbuff) - 128)) {
outbuff[i] = '\0';
fprintf(f, "%s", outbuff);
i = 0;
}
}
outbuff[i] = '\0';
fprintf(f, "%s", outbuff);


free(fmtbuff_heap);
}




static size_t
bsdtar_expand_char(char *buff, size_t offset, char c)
{
size_t i = offset;

if (isprint((unsigned char)c) && c != '\\')
buff[i++] = c;
else {
buff[i++] = '\\';
switch (c) {
case '\a': buff[i++] = 'a'; break;
case '\b': buff[i++] = 'b'; break;
case '\f': buff[i++] = 'f'; break;
case '\n': buff[i++] = 'n'; break;
#if '\r' != '\n'

case '\r': buff[i++] = 'r'; break;
#endif
case '\t': buff[i++] = 't'; break;
case '\v': buff[i++] = 'v'; break;
case '\\': buff[i++] = '\\'; break;
default:
sprintf(buff + i, "%03o", 0xFF & (int)c);
i += 3;
}
}

return (i - offset);
}

int
yes(const char *fmt, ...)
{
char buff[32];
char *p;
ssize_t l;

va_list ap;
va_start(ap, fmt);
vfprintf(stderr, fmt, ap);
va_end(ap);
fprintf(stderr, " (y/N)? ");
fflush(stderr);

l = read(2, buff, sizeof(buff) - 1);
if (l < 0) {
fprintf(stderr, "Keyboard read failed\n");
exit(1);
}
if (l == 0)
return (0);
buff[l] = 0;

for (p = buff; *p != '\0'; p++) {
if (isspace((unsigned char)*p))
continue;
switch(*p) {
case 'y': case 'Y':
return (1);
case 'n': case 'N':
return (0);
default:
return (0);
}
}

return (0);
}



















void
set_chdir(struct bsdtar *bsdtar, const char *newdir)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
if (newdir[0] == '/' || newdir[0] == '\\' ||

(((newdir[0] >= 'a' && newdir[0] <= 'z') ||
(newdir[0] >= 'A' && newdir[0] <= 'Z')) &&
newdir[1] == ':' && (newdir[2] == '/' || newdir[2] == '\\'))) {
#else
if (newdir[0] == '/') {
#endif

free(bsdtar->pending_chdir);
bsdtar->pending_chdir = NULL;
}
if (bsdtar->pending_chdir == NULL)

bsdtar->pending_chdir = strdup(newdir);
else {

char *old_pending = bsdtar->pending_chdir;
size_t old_len = strlen(old_pending);
bsdtar->pending_chdir = malloc(old_len + strlen(newdir) + 2);
if (old_pending[old_len - 1] == '/')
old_pending[old_len - 1] = '\0';
if (bsdtar->pending_chdir != NULL)
sprintf(bsdtar->pending_chdir, "%s/%s",
old_pending, newdir);
free(old_pending);
}
if (bsdtar->pending_chdir == NULL)
lafe_errc(1, errno, "No memory");
}

void
do_chdir(struct bsdtar *bsdtar)
{
if (bsdtar->pending_chdir == NULL)
return;

if (chdir(bsdtar->pending_chdir) != 0) {
lafe_errc(1, 0, "could not chdir to '%s'\n",
bsdtar->pending_chdir);
}
free(bsdtar->pending_chdir);
bsdtar->pending_chdir = NULL;
}

static const char *
strip_components(const char *p, int elements)
{

while (elements > 0) {
switch (*p++) {
case '/':
#if defined(_WIN32) && !defined(__CYGWIN__)
case '\\':
#endif
elements--;
break;
case '\0':

return (NULL);
}
}






for (;;) {
switch (*p) {
case '/':
#if defined(_WIN32) && !defined(__CYGWIN__)
case '\\':
#endif
++p;
break;
case '\0':
return (NULL);
default:
return (p);
}
}
}

static void
warn_strip_leading_char(struct bsdtar *bsdtar, const char *c)
{
if (!bsdtar->warned_lead_slash) {
lafe_warnc(0,
"Removing leading '%c' from member names",
c[0]);
bsdtar->warned_lead_slash = 1;
}
}

static void
warn_strip_drive_letter(struct bsdtar *bsdtar)
{
if (!bsdtar->warned_lead_slash) {
lafe_warnc(0,
"Removing leading drive letter from "
"member names");
bsdtar->warned_lead_slash = 1;
}
}





static const char*
strip_absolute_path(struct bsdtar *bsdtar, const char *p)
{
const char *rp;



if ((p[0] == '/' || p[0] == '\\') &&
(p[1] == '/' || p[1] == '\\') &&
(p[2] == '.' || p[2] == '?') &&
(p[3] == '/' || p[3] == '\\'))
{
if (p[2] == '?' &&
(p[4] == 'U' || p[4] == 'u') &&
(p[5] == 'N' || p[5] == 'n') &&
(p[6] == 'C' || p[6] == 'c') &&
(p[7] == '/' || p[7] == '\\'))
p += 8;
else
p += 4;
warn_strip_drive_letter(bsdtar);
}


do {
rp = p;
if (((p[0] >= 'a' && p[0] <= 'z') ||
(p[0] >= 'A' && p[0] <= 'Z')) &&
p[1] == ':') {
p += 2;
warn_strip_drive_letter(bsdtar);
}


while (p[0] == '/' || p[0] == '\\') {
if (p[1] == '.' &&
p[2] == '.' &&
(p[3] == '/' || p[3] == '\\')) {
p += 3;
} else if (p[1] == '.' &&
(p[2] == '/' || p[2] == '\\')) {
p += 2;
} else
p += 1;
warn_strip_leading_char(bsdtar, rp);
}
} while (rp != p);

return (p);
}













int
edit_pathname(struct bsdtar *bsdtar, struct archive_entry *entry)
{
const char *name = archive_entry_pathname(entry);
const char *original_name = name;
const char *hardlinkname = archive_entry_hardlink(entry);
const char *original_hardlinkname = hardlinkname;
#if defined(HAVE_REGEX_H) || defined(HAVE_PCREPOSIX_H)
char *subst_name;
int r;


r = apply_substitution(bsdtar, name, &subst_name, 0, 0);
if (r == -1) {
lafe_warnc(0, "Invalid substitution, skipping entry");
return 1;
}
if (r == 1) {
archive_entry_copy_pathname(entry, subst_name);
if (*subst_name == '\0') {
free(subst_name);
return -1;
} else
free(subst_name);
name = archive_entry_pathname(entry);
original_name = name;
}


if (hardlinkname != NULL) {
r = apply_substitution(bsdtar, hardlinkname, &subst_name, 0, 1);
if (r == -1) {
lafe_warnc(0, "Invalid substitution, skipping entry");
return 1;
}
if (r == 1) {
archive_entry_copy_hardlink(entry, subst_name);
free(subst_name);
}
hardlinkname = archive_entry_hardlink(entry);
original_hardlinkname = hardlinkname;
}


if (archive_entry_symlink(entry) != NULL) {
r = apply_substitution(bsdtar, archive_entry_symlink(entry), &subst_name, 1, 0);
if (r == -1) {
lafe_warnc(0, "Invalid substitution, skipping entry");
return 1;
}
if (r == 1) {
archive_entry_copy_symlink(entry, subst_name);
free(subst_name);
}
}
#endif


if (bsdtar->strip_components > 0) {
name = strip_components(name, bsdtar->strip_components);
if (name == NULL)
return (1);

if (hardlinkname != NULL) {
hardlinkname = strip_components(hardlinkname,
bsdtar->strip_components);
if (hardlinkname == NULL)
return (1);
}
}

if ((bsdtar->flags & OPTFLAG_ABSOLUTE_PATHS) == 0) {

name = strip_absolute_path(bsdtar, name);
if (*name == '\0')
name = ".";

if (hardlinkname != NULL) {
hardlinkname = strip_absolute_path(bsdtar, hardlinkname);
if (*hardlinkname == '\0')
return (1);
}
} else {

while (name[0] == '/' && name[1] == '/')
name++;
}


if (name != original_name) {
archive_entry_copy_pathname(entry, name);
}
if (hardlinkname != original_hardlinkname) {
archive_entry_copy_hardlink(entry, hardlinkname);
}
return (0);
}






const char *
tar_i64toa(int64_t n0)
{
static char buff[24];
uint64_t n = n0 < 0 ? -n0 : n0;
char *p = buff + sizeof(buff);

*--p = '\0';
do {
*--p = '0' + (int)(n % 10);
} while (n /= 10);
if (n0 < 0)
*--p = '-';
return p;
}
















int
pathcmp(const char *a, const char *b)
{

if (a[0] == '.' && a[1] == '/' && a[2] != '\0')
a += 2;
if (b[0] == '.' && b[1] == '/' && b[2] != '\0')
b += 2;

while (*a == *b) {
if (*a == '\0')
return (0);
a++;
b++;
}




if (a[0] == '/' && a[1] == '\0' && b[0] == '\0')
return (0);
if (a[0] == '\0' && b[0] == '/' && b[1] == '\0')
return (0);

return (*(const unsigned char *)a - *(const unsigned char *)b);
}

#define PPBUFF_SIZE 1024
const char *
passphrase_callback(struct archive *a, void *_client_data)
{
struct bsdtar *bsdtar = (struct bsdtar *)_client_data;
(void)a;

if (bsdtar->ppbuff == NULL) {
bsdtar->ppbuff = malloc(PPBUFF_SIZE);
if (bsdtar->ppbuff == NULL)
lafe_errc(1, errno, "Out of memory");
}
return lafe_readpassphrase("Enter passphrase:",
bsdtar->ppbuff, PPBUFF_SIZE);
}

void
passphrase_free(char *ppbuff)
{
if (ppbuff != NULL) {
memset(ppbuff, 0, PPBUFF_SIZE);
free(ppbuff);
}
}









void
list_item_verbose(struct bsdtar *bsdtar, FILE *out, struct archive_entry *entry)
{
char tmp[100];
size_t w;
const char *p;
const char *fmt;
time_t tim;
static time_t now;
struct tm *ltime;
#if defined(HAVE_LOCALTIME_R) || defined(HAVE__LOCALTIME64_S)
struct tm tmbuf;
#endif
#if defined(HAVE__LOCALTIME64_S)
errno_t terr;
__time64_t tmptime;
#endif








if (!bsdtar->u_width) {
bsdtar->u_width = 6;
bsdtar->gs_width = 13;
}
if (!now)
time(&now);
fprintf(out, "%s %d ",
archive_entry_strmode(entry),
archive_entry_nlink(entry));


p = archive_entry_uname(entry);
if ((p == NULL) || (*p == '\0')) {
sprintf(tmp, "%lu ",
(unsigned long)archive_entry_uid(entry));
p = tmp;
}
w = strlen(p);
if (w > bsdtar->u_width)
bsdtar->u_width = w;
fprintf(out, "%-*s ", (int)bsdtar->u_width, p);


p = archive_entry_gname(entry);
if (p != NULL && p[0] != '\0') {
fprintf(out, "%s", p);
w = strlen(p);
} else {
sprintf(tmp, "%lu",
(unsigned long)archive_entry_gid(entry));
w = strlen(tmp);
fprintf(out, "%s", tmp);
}






if (archive_entry_filetype(entry) == AE_IFCHR
|| archive_entry_filetype(entry) == AE_IFBLK) {
sprintf(tmp, "%lu,%lu",
(unsigned long)archive_entry_rdevmajor(entry),
(unsigned long)archive_entry_rdevminor(entry));
} else {
strcpy(tmp, tar_i64toa(archive_entry_size(entry)));
}
if (w + strlen(tmp) >= bsdtar->gs_width)
bsdtar->gs_width = w+strlen(tmp)+1;
fprintf(out, "%*s", (int)(bsdtar->gs_width - w), tmp);


tim = archive_entry_mtime(entry);
#define HALF_YEAR (time_t)365 * 86400 / 2
#if defined(_WIN32) && !defined(__CYGWIN__)
#define DAY_FMT "%d"
#else
#define DAY_FMT "%e"
#endif
if (tim < now - HALF_YEAR || tim > now + HALF_YEAR)
fmt = bsdtar->day_first ? DAY_FMT " %b %Y" : "%b " DAY_FMT " %Y";
else
fmt = bsdtar->day_first ? DAY_FMT " %b %H:%M" : "%b " DAY_FMT " %H:%M";
#if defined(HAVE_LOCALTIME_R)
ltime = localtime_r(&tim, &tmbuf);
#elif defined(HAVE__LOCALTIME64_S)
tmptime = tim;
terr = _localtime64_s(&tmbuf, &tmptime);
if (terr)
ltime = NULL;
else
ltime = &tmbuf;
#else
ltime = localtime(&tim);
#endif
strftime(tmp, sizeof(tmp), fmt, ltime);
fprintf(out, " %s ", tmp);
safe_fprintf(out, "%s", archive_entry_pathname(entry));


if (archive_entry_hardlink(entry))
safe_fprintf(out, " link to %s",
archive_entry_hardlink(entry));
else if (archive_entry_symlink(entry))
safe_fprintf(out, " -> %s", archive_entry_symlink(entry));
}
