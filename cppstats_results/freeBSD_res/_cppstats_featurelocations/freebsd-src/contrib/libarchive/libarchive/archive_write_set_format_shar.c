

























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include <stdio.h>
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

struct shar {
int dump;
int end_of_line;
struct archive_entry *entry;
int has_data;
char *last_dir;


char outbuff[45];
size_t outpos;

int wrote_header;
struct archive_string work;
struct archive_string quoted_name;
};

static int archive_write_shar_close(struct archive_write *);
static int archive_write_shar_free(struct archive_write *);
static int archive_write_shar_header(struct archive_write *,
struct archive_entry *);
static ssize_t archive_write_shar_data_sed(struct archive_write *,
const void * buff, size_t);
static ssize_t archive_write_shar_data_uuencode(struct archive_write *,
const void * buff, size_t);
static int archive_write_shar_finish_entry(struct archive_write *);





static void
shar_quote(struct archive_string *buf, const char *str, int in_shell)
{
static const char meta[] = "\n \t'`\";&<>()|*?{}[]\\$!#^~";
size_t len;

while (*str != '\0') {
if ((len = strcspn(str, meta)) != 0) {
archive_strncat(buf, str, len);
str += len;
} else if (*str == '\n') {
if (in_shell)
archive_strcat(buf, "\"\n\"");
else
archive_strcat(buf, "\\n");
++str;
} else {
archive_strappend_char(buf, '\\');
archive_strappend_char(buf, *str);
++str;
}
}
}




int
archive_write_set_format_shar(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct shar *shar;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_set_format_shar");


if (a->format_free != NULL)
(a->format_free)(a);

shar = (struct shar *)calloc(1, sizeof(*shar));
if (shar == NULL) {
archive_set_error(&a->archive, ENOMEM, "Can't allocate shar data");
return (ARCHIVE_FATAL);
}
archive_string_init(&shar->work);
archive_string_init(&shar->quoted_name);
a->format_data = shar;
a->format_name = "shar";
a->format_write_header = archive_write_shar_header;
a->format_close = archive_write_shar_close;
a->format_free = archive_write_shar_free;
a->format_write_data = archive_write_shar_data_sed;
a->format_finish_entry = archive_write_shar_finish_entry;
a->archive.archive_format = ARCHIVE_FORMAT_SHAR_BASE;
a->archive.archive_format_name = "shar";
return (ARCHIVE_OK);
}







int
archive_write_set_format_shar_dump(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct shar *shar;

archive_write_set_format_shar(&a->archive);
shar = (struct shar *)a->format_data;
shar->dump = 1;
a->format_write_data = archive_write_shar_data_uuencode;
a->archive.archive_format = ARCHIVE_FORMAT_SHAR_DUMP;
a->archive.archive_format_name = "shar dump";
return (ARCHIVE_OK);
}

static int
archive_write_shar_header(struct archive_write *a, struct archive_entry *entry)
{
const char *linkname;
const char *name;
char *p, *pp;
struct shar *shar;

shar = (struct shar *)a->format_data;
if (!shar->wrote_header) {
archive_strcat(&shar->work, "#!/bin/sh\n");
archive_strcat(&shar->work, "#This is a shell archive\n");
shar->wrote_header = 1;
}


archive_entry_free(shar->entry);
shar->entry = archive_entry_clone(entry);
name = archive_entry_pathname(entry);


switch(archive_entry_filetype(entry)) {
case AE_IFREG:

break;
case AE_IFDIR:
archive_entry_set_size(entry, 0);

if (strcmp(name, ".") == 0 || strcmp(name, "./") == 0)
return (ARCHIVE_OK);
break;
case AE_IFIFO:
case AE_IFCHR:
case AE_IFBLK:

archive_entry_set_size(entry, 0);
break;
default:
archive_entry_set_size(entry, 0);
if (archive_entry_hardlink(entry) == NULL &&
archive_entry_symlink(entry) == NULL) {
__archive_write_entry_filetype_unsupported(
&a->archive, entry, "shar");
return (ARCHIVE_WARN);
}
}

archive_string_empty(&shar->quoted_name);
shar_quote(&shar->quoted_name, name, 1);


archive_string_sprintf(&shar->work, "echo x %s\n", shar->quoted_name.s);

if (archive_entry_filetype(entry) != AE_IFDIR) {

p = strdup(name);
pp = strrchr(p, '/');

if (pp != NULL) {
*pp = '\0';


if (strcmp(p, ".") == 0) {

free(p);
} else if (shar->last_dir == NULL) {
archive_strcat(&shar->work, "mkdir -p ");
shar_quote(&shar->work, p, 1);
archive_strcat(&shar->work,
" > /dev/null 2>&1\n");
shar->last_dir = p;
} else if (strcmp(p, shar->last_dir) == 0) {

free(p);
} else if (strlen(p) < strlen(shar->last_dir) &&
strncmp(p, shar->last_dir, strlen(p)) == 0) {

free(p);
} else {
archive_strcat(&shar->work, "mkdir -p ");
shar_quote(&shar->work, p, 1);
archive_strcat(&shar->work,
" > /dev/null 2>&1\n");
shar->last_dir = p;
}
} else {
free(p);
}
}


shar->has_data = 0;
if ((linkname = archive_entry_hardlink(entry)) != NULL) {
archive_strcat(&shar->work, "ln -f ");
shar_quote(&shar->work, linkname, 1);
archive_string_sprintf(&shar->work, " %s\n",
shar->quoted_name.s);
} else if ((linkname = archive_entry_symlink(entry)) != NULL) {
archive_strcat(&shar->work, "ln -fs ");
shar_quote(&shar->work, linkname, 1);
archive_string_sprintf(&shar->work, " %s\n",
shar->quoted_name.s);
} else {
switch(archive_entry_filetype(entry)) {
case AE_IFREG:
if (archive_entry_size(entry) == 0) {

archive_string_sprintf(&shar->work,
"test -e \"%s\" || :> \"%s\"\n",
shar->quoted_name.s, shar->quoted_name.s);
} else {
if (shar->dump) {
unsigned int mode = archive_entry_mode(entry) & 0777;
archive_string_sprintf(&shar->work,
"uudecode -p > %s << 'SHAR_END'\n",
shar->quoted_name.s);
archive_string_sprintf(&shar->work,
"begin %o ", mode);
shar_quote(&shar->work, name, 0);
archive_strcat(&shar->work, "\n");
} else {
archive_string_sprintf(&shar->work,
"sed 's/^X//' > %s << 'SHAR_END'\n",
shar->quoted_name.s);
}
shar->has_data = 1;
shar->end_of_line = 1;
shar->outpos = 0;
}
break;
case AE_IFDIR:
archive_string_sprintf(&shar->work,
"mkdir -p %s > /dev/null 2>&1\n",
shar->quoted_name.s);

free(shar->last_dir);

shar->last_dir = strdup(name);

pp = strrchr(shar->last_dir, '/');
if (pp != NULL && pp[1] == '\0')
*pp = '\0';




break;
case AE_IFIFO:
archive_string_sprintf(&shar->work,
"mkfifo %s\n", shar->quoted_name.s);
break;
case AE_IFCHR:
archive_string_sprintf(&shar->work,
"mknod %s c %ju %ju\n", shar->quoted_name.s,
(uintmax_t)archive_entry_rdevmajor(entry),
(uintmax_t)archive_entry_rdevminor(entry));
break;
case AE_IFBLK:
archive_string_sprintf(&shar->work,
"mknod %s b %ju %ju\n", shar->quoted_name.s,
(uintmax_t)archive_entry_rdevmajor(entry),
(uintmax_t)archive_entry_rdevminor(entry));
break;
default:
return (ARCHIVE_WARN);
}
}

return (ARCHIVE_OK);
}

static ssize_t
archive_write_shar_data_sed(struct archive_write *a, const void *buff, size_t n)
{
static const size_t ensured = 65533;
struct shar *shar;
const char *src;
char *buf, *buf_end;
int ret;
size_t written = n;

shar = (struct shar *)a->format_data;
if (!shar->has_data || n == 0)
return (0);

src = (const char *)buff;








if (archive_string_ensure(&shar->work, ensured + 3) == NULL) {
archive_set_error(&a->archive, ENOMEM, "Out of memory");
return (ARCHIVE_FATAL);
}

if (shar->work.length > ensured) {
ret = __archive_write_output(a, shar->work.s,
shar->work.length);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
archive_string_empty(&shar->work);
}
buf = shar->work.s + shar->work.length;
buf_end = shar->work.s + ensured;

if (shar->end_of_line) {
*buf++ = 'X';
shar->end_of_line = 0;
}

while (n-- != 0) {
if ((*buf++ = *src++) == '\n') {
if (n == 0)
shar->end_of_line = 1;
else
*buf++ = 'X';
}

if (buf >= buf_end) {
shar->work.length = buf - shar->work.s;
ret = __archive_write_output(a, shar->work.s,
shar->work.length);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
archive_string_empty(&shar->work);
buf = shar->work.s;
}
}

shar->work.length = buf - shar->work.s;

return (written);
}

#define UUENC(c) (((c)!=0) ? ((c) & 077) + ' ': '`')

static void
uuencode_group(const char _in[3], char out[4])
{
const unsigned char *in = (const unsigned char *)_in;
int t;

t = (in[0] << 16) | (in[1] << 8) | in[2];
out[0] = UUENC( 0x3f & (t >> 18) );
out[1] = UUENC( 0x3f & (t >> 12) );
out[2] = UUENC( 0x3f & (t >> 6) );
out[3] = UUENC( 0x3f & t );
}

static int
_uuencode_line(struct archive_write *a, struct shar *shar, const char *inbuf, size_t len)
{
char *buf;
size_t alloc_len;


alloc_len = shar->work.length + 62;
if (archive_string_ensure(&shar->work, alloc_len) == NULL) {
archive_set_error(&a->archive, ENOMEM, "Out of memory");
return (ARCHIVE_FATAL);
}

buf = shar->work.s + shar->work.length;
*buf++ = UUENC(len);
while (len >= 3) {
uuencode_group(inbuf, buf);
len -= 3;
inbuf += 3;
buf += 4;
}
if (len != 0) {
char tmp_buf[3];
tmp_buf[0] = inbuf[0];
if (len == 1)
tmp_buf[1] = '\0';
else
tmp_buf[1] = inbuf[1];
tmp_buf[2] = '\0';
uuencode_group(tmp_buf, buf);
buf += 4;
}
*buf++ = '\n';
if ((buf - shar->work.s) > (ptrdiff_t)(shar->work.length + 62)) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC, "Buffer overflow");
return (ARCHIVE_FATAL);
}
shar->work.length = buf - shar->work.s;
return (ARCHIVE_OK);
}

#define uuencode_line(__a, __shar, __inbuf, __len) do { int r = _uuencode_line(__a, __shar, __inbuf, __len); if (r != ARCHIVE_OK) return (ARCHIVE_FATAL); } while (0)






static ssize_t
archive_write_shar_data_uuencode(struct archive_write *a, const void *buff,
size_t length)
{
struct shar *shar;
const char *src;
size_t n;
int ret;

shar = (struct shar *)a->format_data;
if (!shar->has_data)
return (ARCHIVE_OK);
src = (const char *)buff;

if (shar->outpos != 0) {
n = 45 - shar->outpos;
if (n > length)
n = length;
memcpy(shar->outbuff + shar->outpos, src, n);
if (shar->outpos + n < 45) {
shar->outpos += n;
return length;
}
uuencode_line(a, shar, shar->outbuff, 45);
src += n;
n = length - n;
} else {
n = length;
}

while (n >= 45) {
uuencode_line(a, shar, src, 45);
src += 45;
n -= 45;

if (shar->work.length < 65536)
continue;
ret = __archive_write_output(a, shar->work.s,
shar->work.length);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
archive_string_empty(&shar->work);
}
if (n != 0) {
memcpy(shar->outbuff, src, n);
shar->outpos = n;
}
return (length);
}

static int
archive_write_shar_finish_entry(struct archive_write *a)
{
const char *g, *p, *u;
struct shar *shar;
int ret;

shar = (struct shar *)a->format_data;
if (shar->entry == NULL)
return (0);

if (shar->dump) {

if (shar->has_data) {
if (shar->outpos > 0)
uuencode_line(a, shar, shar->outbuff,
shar->outpos);
archive_strcat(&shar->work, "`\nend\n");
archive_strcat(&shar->work, "SHAR_END\n");
}





archive_string_sprintf(&shar->work, "chmod %o ",
(unsigned int)(archive_entry_mode(shar->entry) & 07777));
shar_quote(&shar->work, archive_entry_pathname(shar->entry), 1);
archive_strcat(&shar->work, "\n");

u = archive_entry_uname(shar->entry);
g = archive_entry_gname(shar->entry);
if (u != NULL || g != NULL) {
archive_strcat(&shar->work, "chown ");
if (u != NULL)
shar_quote(&shar->work, u, 1);
if (g != NULL) {
archive_strcat(&shar->work, ":");
shar_quote(&shar->work, g, 1);
}
archive_strcat(&shar->work, " ");
shar_quote(&shar->work,
archive_entry_pathname(shar->entry), 1);
archive_strcat(&shar->work, "\n");
}

if ((p = archive_entry_fflags_text(shar->entry)) != NULL) {
archive_string_sprintf(&shar->work, "chflags %s ", p);
shar_quote(&shar->work,
archive_entry_pathname(shar->entry), 1);
archive_strcat(&shar->work, "\n");
}



} else {
if (shar->has_data) {

if (!shar->end_of_line)
archive_strappend_char(&shar->work, '\n');
archive_strcat(&shar->work, "SHAR_END\n");
}
}

archive_entry_free(shar->entry);
shar->entry = NULL;

if (shar->work.length < 65536)
return (ARCHIVE_OK);

ret = __archive_write_output(a, shar->work.s, shar->work.length);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
archive_string_empty(&shar->work);

return (ARCHIVE_OK);
}

static int
archive_write_shar_close(struct archive_write *a)
{
struct shar *shar;
int ret;






shar = (struct shar *)a->format_data;







if (shar->wrote_header == 0)
return (ARCHIVE_OK);

archive_strcat(&shar->work, "exit\n");

ret = __archive_write_output(a, shar->work.s, shar->work.length);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);


archive_write_set_bytes_in_last_block(&a->archive, 1);





return (ARCHIVE_OK);
}

static int
archive_write_shar_free(struct archive_write *a)
{
struct shar *shar;

shar = (struct shar *)a->format_data;
if (shar == NULL)
return (ARCHIVE_OK);

archive_entry_free(shar->entry);
free(shar->last_dir);
archive_string_free(&(shar->work));
archive_string_free(&(shar->quoted_name));
free(shar);
a->format_data = NULL;
return (ARCHIVE_OK);
}
