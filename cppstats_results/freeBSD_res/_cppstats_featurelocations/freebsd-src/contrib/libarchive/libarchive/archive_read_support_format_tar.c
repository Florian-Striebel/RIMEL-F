


























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include <stddef.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "archive.h"
#include "archive_acl_private.h"
#include "archive_entry.h"
#include "archive_entry_locale.h"
#include "archive_private.h"
#include "archive_read_private.h"

#define tar_min(a,b) ((a) < (b) ? (a) : (b))




struct archive_entry_header_ustar {
char name[100];
char mode[8];
char uid[8];
char gid[8];
char size[12];
char mtime[12];
char checksum[8];
char typeflag[1];
char linkname[100];
char magic[6];
char version[2];
char uname[32];
char gname[32];
char rdevmajor[8];
char rdevminor[8];
char prefix[155];
};




struct gnu_sparse {
char offset[12];
char numbytes[12];
};

struct archive_entry_header_gnutar {
char name[100];
char mode[8];
char uid[8];
char gid[8];
char size[12];
char mtime[12];
char checksum[8];
char typeflag[1];
char linkname[100];
char magic[8];
char uname[32];
char gname[32];
char rdevmajor[8];
char rdevminor[8];
char atime[12];
char ctime[12];
char offset[12];
char longnames[4];
char unused[1];
struct gnu_sparse sparse[4];
char isextended[1];
char realsize[12];




};




struct sparse_block {
struct sparse_block *next;
int64_t offset;
int64_t remaining;
int hole;
};

struct tar {
struct archive_string acl_text;
struct archive_string entry_pathname;

struct archive_string entry_pathname_override;
struct archive_string entry_linkpath;
struct archive_string entry_uname;
struct archive_string entry_gname;
struct archive_string longlink;
struct archive_string longname;
struct archive_string pax_header;
struct archive_string pax_global;
struct archive_string line;
int pax_hdrcharset_binary;
int header_recursion_depth;
int64_t entry_bytes_remaining;
int64_t entry_offset;
int64_t entry_padding;
int64_t entry_bytes_unconsumed;
int64_t realsize;
int sparse_allowed;
struct sparse_block *sparse_list;
struct sparse_block *sparse_last;
int64_t sparse_offset;
int64_t sparse_numbytes;
int sparse_gnu_major;
int sparse_gnu_minor;
char sparse_gnu_pending;

struct archive_string localname;
struct archive_string_conv *opt_sconv;
struct archive_string_conv *sconv;
struct archive_string_conv *sconv_acl;
struct archive_string_conv *sconv_default;
int init_default_conversion;
int compat_2x;
int process_mac_extensions;
int read_concatenated_archives;
int realsize_override;
};

static int archive_block_is_null(const char *p);
static char *base64_decode(const char *, size_t, size_t *);
static int gnu_add_sparse_entry(struct archive_read *, struct tar *,
int64_t offset, int64_t remaining);

static void gnu_clear_sparse_list(struct tar *);
static int gnu_sparse_old_read(struct archive_read *, struct tar *,
const struct archive_entry_header_gnutar *header, size_t *);
static int gnu_sparse_old_parse(struct archive_read *, struct tar *,
const struct gnu_sparse *sparse, int length);
static int gnu_sparse_01_parse(struct archive_read *, struct tar *,
const char *);
static ssize_t gnu_sparse_10_read(struct archive_read *, struct tar *,
size_t *);
static int header_Solaris_ACL(struct archive_read *, struct tar *,
struct archive_entry *, const void *, size_t *);
static int header_common(struct archive_read *, struct tar *,
struct archive_entry *, const void *);
static int header_old_tar(struct archive_read *, struct tar *,
struct archive_entry *, const void *);
static int header_pax_extensions(struct archive_read *, struct tar *,
struct archive_entry *, const void *, size_t *);
static int header_pax_global(struct archive_read *, struct tar *,
struct archive_entry *, const void *h, size_t *);
static int header_longlink(struct archive_read *, struct tar *,
struct archive_entry *, const void *h, size_t *);
static int header_longname(struct archive_read *, struct tar *,
struct archive_entry *, const void *h, size_t *);
static int read_mac_metadata_blob(struct archive_read *, struct tar *,
struct archive_entry *, const void *h, size_t *);
static int header_volume(struct archive_read *, struct tar *,
struct archive_entry *, const void *h, size_t *);
static int header_ustar(struct archive_read *, struct tar *,
struct archive_entry *, const void *h);
static int header_gnutar(struct archive_read *, struct tar *,
struct archive_entry *, const void *h, size_t *);
static int archive_read_format_tar_bid(struct archive_read *, int);
static int archive_read_format_tar_options(struct archive_read *,
const char *, const char *);
static int archive_read_format_tar_cleanup(struct archive_read *);
static int archive_read_format_tar_read_data(struct archive_read *a,
const void **buff, size_t *size, int64_t *offset);
static int archive_read_format_tar_skip(struct archive_read *a);
static int archive_read_format_tar_read_header(struct archive_read *,
struct archive_entry *);
static int checksum(struct archive_read *, const void *);
static int pax_attribute(struct archive_read *, struct tar *,
struct archive_entry *, const char *key, const char *value,
size_t value_length);
static int pax_attribute_acl(struct archive_read *, struct tar *,
struct archive_entry *, const char *, int);
static int pax_attribute_xattr(struct archive_entry *, const char *,
const char *);
static int pax_header(struct archive_read *, struct tar *,
struct archive_entry *, struct archive_string *);
static void pax_time(const char *, int64_t *sec, long *nanos);
static ssize_t readline(struct archive_read *, struct tar *, const char **,
ssize_t limit, size_t *);
static int read_body_to_string(struct archive_read *, struct tar *,
struct archive_string *, const void *h, size_t *);
static int solaris_sparse_parse(struct archive_read *, struct tar *,
struct archive_entry *, const char *);
static int64_t tar_atol(const char *, size_t);
static int64_t tar_atol10(const char *, size_t);
static int64_t tar_atol256(const char *, size_t);
static int64_t tar_atol8(const char *, size_t);
static int tar_read_header(struct archive_read *, struct tar *,
struct archive_entry *, size_t *);
static int tohex(int c);
static char *url_decode(const char *);
static void tar_flush_unconsumed(struct archive_read *, size_t *);


int
archive_read_support_format_gnutar(struct archive *a)
{
archive_check_magic(a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_format_gnutar");
return (archive_read_support_format_tar(a));
}


int
archive_read_support_format_tar(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct tar *tar;
int r;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_format_tar");

tar = (struct tar *)calloc(1, sizeof(*tar));
if (tar == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate tar data");
return (ARCHIVE_FATAL);
}
#if defined(HAVE_COPYFILE_H)

tar->process_mac_extensions = 1;
#endif

r = __archive_read_register_format(a, tar, "tar",
archive_read_format_tar_bid,
archive_read_format_tar_options,
archive_read_format_tar_read_header,
archive_read_format_tar_read_data,
archive_read_format_tar_skip,
NULL,
archive_read_format_tar_cleanup,
NULL,
NULL);

if (r != ARCHIVE_OK)
free(tar);
return (ARCHIVE_OK);
}

static int
archive_read_format_tar_cleanup(struct archive_read *a)
{
struct tar *tar;

tar = (struct tar *)(a->format->data);
gnu_clear_sparse_list(tar);
archive_string_free(&tar->acl_text);
archive_string_free(&tar->entry_pathname);
archive_string_free(&tar->entry_pathname_override);
archive_string_free(&tar->entry_linkpath);
archive_string_free(&tar->entry_uname);
archive_string_free(&tar->entry_gname);
archive_string_free(&tar->line);
archive_string_free(&tar->pax_global);
archive_string_free(&tar->pax_header);
archive_string_free(&tar->longname);
archive_string_free(&tar->longlink);
archive_string_free(&tar->localname);
free(tar);
(a->format->data) = NULL;
return (ARCHIVE_OK);
}
























static int
validate_number_field(const char* p_field, size_t i_size)
{
unsigned char marker = (unsigned char)p_field[0];
if (marker == 128 || marker == 255 || marker == 0) {

return 1;
} else {

size_t i = 0;

while (i < i_size && p_field[i] == ' ') {
++i;
}

while (i < i_size && p_field[i] >= '0' && p_field[i] <= '7') {
++i;
}

while (i < i_size) {
if (p_field[i] != ' ' && p_field[i] != 0) {
return 0;
}
++i;
}
return 1;
}
}

static int
archive_read_format_tar_bid(struct archive_read *a, int best_bid)
{
int bid;
const char *h;
const struct archive_entry_header_ustar *header;

(void)best_bid;

bid = 0;


h = __archive_read_ahead(a, 512, NULL);
if (h == NULL)
return (-1);


if (h[0] == 0 && archive_block_is_null(h)) {





return (10);
}


if (!checksum(a, h))
return (0);
bid += 48;

header = (const struct archive_entry_header_ustar *)h;


if ((memcmp(header->magic, "ustar\0", 6) == 0)
&& (memcmp(header->version, "00", 2) == 0))
bid += 56;


if ((memcmp(header->magic, "ustar ", 6) == 0)
&& (memcmp(header->version, " \0", 2) == 0))
bid += 56;


if (header->typeflag[0] != 0 &&
!( header->typeflag[0] >= '0' && header->typeflag[0] <= '9') &&
!( header->typeflag[0] >= 'A' && header->typeflag[0] <= 'Z') &&
!( header->typeflag[0] >= 'a' && header->typeflag[0] <= 'z') )
return (0);
bid += 2;




if (bid > 0 && (
validate_number_field(header->mode, sizeof(header->mode)) == 0
|| validate_number_field(header->uid, sizeof(header->uid)) == 0
|| validate_number_field(header->gid, sizeof(header->gid)) == 0
|| validate_number_field(header->mtime, sizeof(header->mtime)) == 0
|| validate_number_field(header->size, sizeof(header->size)) == 0
|| validate_number_field(header->rdevmajor, sizeof(header->rdevmajor)) == 0
|| validate_number_field(header->rdevminor, sizeof(header->rdevminor)) == 0)) {
bid = 0;
}

return (bid);
}

static int
archive_read_format_tar_options(struct archive_read *a,
const char *key, const char *val)
{
struct tar *tar;
int ret = ARCHIVE_FAILED;

tar = (struct tar *)(a->format->data);
if (strcmp(key, "compat-2x") == 0) {

tar->compat_2x = (val != NULL && val[0] != 0);
tar->init_default_conversion = tar->compat_2x;
return (ARCHIVE_OK);
} else if (strcmp(key, "hdrcharset") == 0) {
if (val == NULL || val[0] == 0)
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"tar: hdrcharset option needs a character-set name");
else {
tar->opt_sconv =
archive_string_conversion_from_charset(
&a->archive, val, 0);
if (tar->opt_sconv != NULL)
ret = ARCHIVE_OK;
else
ret = ARCHIVE_FATAL;
}
return (ret);
} else if (strcmp(key, "mac-ext") == 0) {
tar->process_mac_extensions = (val != NULL && val[0] != 0);
return (ARCHIVE_OK);
} else if (strcmp(key, "read_concatenated_archives") == 0) {
tar->read_concatenated_archives = (val != NULL && val[0] != 0);
return (ARCHIVE_OK);
}




return (ARCHIVE_WARN);
}





static void
tar_flush_unconsumed(struct archive_read *a, size_t *unconsumed)
{
if (*unconsumed) {









__archive_read_consume(a, *unconsumed);
*unconsumed = 0;
}
}






static int
archive_read_format_tar_read_header(struct archive_read *a,
struct archive_entry *entry)
{














static int default_inode;
static int default_dev;
struct tar *tar;
const char *p;
const wchar_t *wp;
int r;
size_t l, unconsumed = 0;


archive_entry_set_dev(entry, 1 + default_dev);
archive_entry_set_ino(entry, ++default_inode);

if (default_inode >= 0xffff) {
++default_dev;
default_inode = 0;
}

tar = (struct tar *)(a->format->data);
tar->entry_offset = 0;
gnu_clear_sparse_list(tar);
tar->realsize = -1;
tar->realsize_override = 0;


tar->sconv = tar->opt_sconv;
if (tar->sconv == NULL) {
if (!tar->init_default_conversion) {
tar->sconv_default =
archive_string_default_conversion_for_read(&(a->archive));
tar->init_default_conversion = 1;
}
tar->sconv = tar->sconv_default;
}

r = tar_read_header(a, tar, entry, &unconsumed);

tar_flush_unconsumed(a, &unconsumed);





if (tar->sparse_list == NULL) {
if (gnu_add_sparse_entry(a, tar, 0, tar->entry_bytes_remaining)
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
} else {
struct sparse_block *sb;

for (sb = tar->sparse_list; sb != NULL; sb = sb->next) {
if (!sb->hole)
archive_entry_sparse_add_entry(entry,
sb->offset, sb->remaining);
}
}

if (r == ARCHIVE_OK && archive_entry_filetype(entry) == AE_IFREG) {





if ((wp = archive_entry_pathname_w(entry)) != NULL) {
l = wcslen(wp);
if (l > 0 && wp[l - 1] == L'/') {
archive_entry_set_filetype(entry, AE_IFDIR);
}
} else if ((p = archive_entry_pathname(entry)) != NULL) {
l = strlen(p);
if (l > 0 && p[l - 1] == '/') {
archive_entry_set_filetype(entry, AE_IFDIR);
}
}
}
return (r);
}

static int
archive_read_format_tar_read_data(struct archive_read *a,
const void **buff, size_t *size, int64_t *offset)
{
ssize_t bytes_read;
struct tar *tar;
struct sparse_block *p;

tar = (struct tar *)(a->format->data);

for (;;) {

while (tar->sparse_list != NULL &&
tar->sparse_list->remaining == 0) {
p = tar->sparse_list;
tar->sparse_list = p->next;
free(p);
}

if (tar->entry_bytes_unconsumed) {
__archive_read_consume(a, tar->entry_bytes_unconsumed);
tar->entry_bytes_unconsumed = 0;
}


if (tar->sparse_list == NULL ||
tar->entry_bytes_remaining == 0) {
if (__archive_read_consume(a, tar->entry_padding) < 0)
return (ARCHIVE_FATAL);
tar->entry_padding = 0;
*buff = NULL;
*size = 0;
*offset = tar->realsize;
return (ARCHIVE_EOF);
}

*buff = __archive_read_ahead(a, 1, &bytes_read);
if (bytes_read < 0)
return (ARCHIVE_FATAL);
if (*buff == NULL) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Truncated tar archive");
return (ARCHIVE_FATAL);
}
if (bytes_read > tar->entry_bytes_remaining)
bytes_read = (ssize_t)tar->entry_bytes_remaining;


if (tar->sparse_list->remaining < bytes_read)
bytes_read = (ssize_t)tar->sparse_list->remaining;
*size = bytes_read;
*offset = tar->sparse_list->offset;
tar->sparse_list->remaining -= bytes_read;
tar->sparse_list->offset += bytes_read;
tar->entry_bytes_remaining -= bytes_read;
tar->entry_bytes_unconsumed = bytes_read;

if (!tar->sparse_list->hole)
return (ARCHIVE_OK);

}
}

static int
archive_read_format_tar_skip(struct archive_read *a)
{
int64_t bytes_skipped;
int64_t request;
struct sparse_block *p;
struct tar* tar;

tar = (struct tar *)(a->format->data);


request = 0;
for (p = tar->sparse_list; p != NULL; p = p->next) {
if (!p->hole) {
if (p->remaining >= INT64_MAX - request) {
return ARCHIVE_FATAL;
}
request += p->remaining;
}
}
if (request > tar->entry_bytes_remaining)
request = tar->entry_bytes_remaining;
request += tar->entry_padding + tar->entry_bytes_unconsumed;

bytes_skipped = __archive_read_consume(a, request);
if (bytes_skipped < 0)
return (ARCHIVE_FATAL);

tar->entry_bytes_remaining = 0;
tar->entry_bytes_unconsumed = 0;
tar->entry_padding = 0;


gnu_clear_sparse_list(tar);

return (ARCHIVE_OK);
}





static int
tar_read_header(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, size_t *unconsumed)
{
ssize_t bytes;
int err, eof_vol_header;
const char *h;
const struct archive_entry_header_ustar *header;
const struct archive_entry_header_gnutar *gnuheader;

eof_vol_header = 0;


for (;;) {
tar_flush_unconsumed(a, unconsumed);


h = __archive_read_ahead(a, 512, &bytes);
if (bytes < 0)
return ((int)bytes);
if (bytes == 0) {

return (ARCHIVE_EOF);
}
if (bytes < 512) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Truncated tar archive");
return (ARCHIVE_FATAL);
}
*unconsumed = 512;


if (h[0] != 0 || !archive_block_is_null(h))
break;


if (a->archive.archive_format_name == NULL) {
a->archive.archive_format = ARCHIVE_FORMAT_TAR;
a->archive.archive_format_name = "tar";
}

if (!tar->read_concatenated_archives) {

tar_flush_unconsumed(a, unconsumed);
h = __archive_read_ahead(a, 512, NULL);
if (h != NULL && h[0] == 0 && archive_block_is_null(h))
__archive_read_consume(a, 512);
archive_clear_error(&a->archive);
return (ARCHIVE_EOF);
}





}








if (!checksum(a, h)) {
tar_flush_unconsumed(a, unconsumed);
archive_set_error(&a->archive, EINVAL, "Damaged tar archive");
return (ARCHIVE_RETRY);
}

if (++tar->header_recursion_depth > 32) {
tar_flush_unconsumed(a, unconsumed);
archive_set_error(&a->archive, EINVAL, "Too many special headers");
return (ARCHIVE_WARN);
}


header = (const struct archive_entry_header_ustar *)h;

switch(header->typeflag[0]) {
case 'A':
a->archive.archive_format = ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE;
a->archive.archive_format_name = "Solaris tar";
err = header_Solaris_ACL(a, tar, entry, h, unconsumed);
break;
case 'g':
a->archive.archive_format = ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE;
a->archive.archive_format_name = "POSIX pax interchange format";
err = header_pax_global(a, tar, entry, h, unconsumed);
if (err == ARCHIVE_EOF)
return (err);
break;
case 'K':
err = header_longlink(a, tar, entry, h, unconsumed);
break;
case 'L':
err = header_longname(a, tar, entry, h, unconsumed);
break;
case 'V':
err = header_volume(a, tar, entry, h, unconsumed);
if (err == ARCHIVE_EOF)
eof_vol_header = 1;
break;
case 'X':
a->archive.archive_format = ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE;
a->archive.archive_format_name =
"POSIX pax interchange format (Sun variant)";
err = header_pax_extensions(a, tar, entry, h, unconsumed);
break;
case 'x':
a->archive.archive_format = ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE;
a->archive.archive_format_name = "POSIX pax interchange format";
err = header_pax_extensions(a, tar, entry, h, unconsumed);
break;
default:
gnuheader = (const struct archive_entry_header_gnutar *)h;
if (memcmp(gnuheader->magic, "ustar \0", 8) == 0) {
a->archive.archive_format = ARCHIVE_FORMAT_TAR_GNUTAR;
a->archive.archive_format_name = "GNU tar format";
err = header_gnutar(a, tar, entry, h, unconsumed);
} else if (memcmp(header->magic, "ustar", 5) == 0) {
if (a->archive.archive_format != ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE) {
a->archive.archive_format = ARCHIVE_FORMAT_TAR_USTAR;
a->archive.archive_format_name = "POSIX ustar format";
}
err = header_ustar(a, tar, entry, h);
} else {
a->archive.archive_format = ARCHIVE_FORMAT_TAR;
a->archive.archive_format_name = "tar (non-POSIX)";
err = header_old_tar(a, tar, entry, h);
}
}
if (err == ARCHIVE_FATAL)
return (err);

tar_flush_unconsumed(a, unconsumed);

h = NULL;
header = NULL;

--tar->header_recursion_depth;




if ((err == ARCHIVE_WARN || err == ARCHIVE_OK) &&
tar->header_recursion_depth == 0 &&
tar->process_mac_extensions) {
int err2 = read_mac_metadata_blob(a, tar, entry, h, unconsumed);
if (err2 < err)
err = err2;
}


if (err == ARCHIVE_WARN || err == ARCHIVE_OK) {
if (tar->sparse_gnu_pending) {
if (tar->sparse_gnu_major == 1 &&
tar->sparse_gnu_minor == 0) {
ssize_t bytes_read;

tar->sparse_gnu_pending = 0;

bytes_read = gnu_sparse_10_read(a, tar, unconsumed);
if (bytes_read < 0)
return ((int)bytes_read);
tar->entry_bytes_remaining -= bytes_read;
} else {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"Unrecognized GNU sparse file format");
return (ARCHIVE_WARN);
}
tar->sparse_gnu_pending = 0;
}
return (err);
}
if (err == ARCHIVE_EOF) {
if (!eof_vol_header) {

archive_set_error(&a->archive, EINVAL,
"Damaged tar archive");
} else {


return (ARCHIVE_EOF);
}
}
return (ARCHIVE_FATAL);
}




static int
checksum(struct archive_read *a, const void *h)
{
const unsigned char *bytes;
const struct archive_entry_header_ustar *header;
int check, sum;
size_t i;

(void)a;
bytes = (const unsigned char *)h;
header = (const struct archive_entry_header_ustar *)h;


for (i = 0; i < sizeof(header->checksum); ++i) {
char c = header->checksum[i];
if (c != ' ' && c != '\0' && (c < '0' || c > '7'))
return 0;
}





sum = (int)tar_atol(header->checksum, sizeof(header->checksum));
check = 0;
for (i = 0; i < 148; i++)
check += (unsigned char)bytes[i];
for (; i < 156; i++)
check += 32;
for (; i < 512; i++)
check += (unsigned char)bytes[i];
if (sum == check)
return (1);






check = 0;
for (i = 0; i < 148; i++)
check += (signed char)bytes[i];
for (; i < 156; i++)
check += 32;
for (; i < 512; i++)
check += (signed char)bytes[i];
if (sum == check)
return (1);

return (0);
}




static int
archive_block_is_null(const char *p)
{
unsigned i;

for (i = 0; i < 512; i++)
if (*p++)
return (0);
return (1);
}




static int
header_Solaris_ACL(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
const struct archive_entry_header_ustar *header;
size_t size;
int err, acl_type;
int64_t type;
char *acl, *p;





header = (const struct archive_entry_header_ustar *)h;
size = (size_t)tar_atol(header->size, sizeof(header->size));
err = read_body_to_string(a, tar, &(tar->acl_text), h, unconsumed);
if (err != ARCHIVE_OK)
return (err);


err = tar_read_header(a, tar, entry, unconsumed);
if ((err != ARCHIVE_OK) && (err != ARCHIVE_WARN))
return (err);







p = acl = tar->acl_text.s;
type = 0;
while (*p != '\0' && p < acl + size) {
if (*p < '0' || *p > '7') {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Malformed Solaris ACL attribute (invalid digit)");
return(ARCHIVE_WARN);
}
type <<= 3;
type += *p - '0';
if (type > 077777777) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Malformed Solaris ACL attribute (count too large)");
return (ARCHIVE_WARN);
}
p++;
}
switch ((int)type & ~0777777) {
case 01000000:

acl_type = ARCHIVE_ENTRY_ACL_TYPE_ACCESS;
break;
case 03000000:

acl_type = ARCHIVE_ENTRY_ACL_TYPE_NFS4;
break;
default:
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Malformed Solaris ACL attribute (unsupported type %o)",
(int)type);
return (ARCHIVE_WARN);
}
p++;

if (p >= acl + size) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Malformed Solaris ACL attribute (body overflow)");
return(ARCHIVE_WARN);
}


size -= (p - acl);
acl = p;

while (*p != '\0' && p < acl + size)
p++;

if (tar->sconv_acl == NULL) {
tar->sconv_acl = archive_string_conversion_from_charset(
&(a->archive), "UTF-8", 1);
if (tar->sconv_acl == NULL)
return (ARCHIVE_FATAL);
}
archive_strncpy(&(tar->localname), acl, p - acl);
err = archive_acl_from_text_l(archive_entry_acl(entry),
tar->localname.s, acl_type, tar->sconv_acl);
if (err != ARCHIVE_OK) {
if (errno == ENOMEM) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory for ACL");
} else
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Malformed Solaris ACL attribute (unparsable)");
}
return (err);
}




static int
header_longlink(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
int err;

err = read_body_to_string(a, tar, &(tar->longlink), h, unconsumed);
if (err != ARCHIVE_OK)
return (err);
err = tar_read_header(a, tar, entry, unconsumed);
if ((err != ARCHIVE_OK) && (err != ARCHIVE_WARN))
return (err);

archive_entry_copy_link(entry, tar->longlink.s);
return (ARCHIVE_OK);
}

static int
set_conversion_failed_error(struct archive_read *a,
struct archive_string_conv *sconv, const char *name)
{
if (errno == ENOMEM) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory for %s", name);
return (ARCHIVE_FATAL);
}
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"%s can't be converted from %s to current locale.",
name, archive_string_conversion_charset_name(sconv));
return (ARCHIVE_WARN);
}




static int
header_longname(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
int err;

err = read_body_to_string(a, tar, &(tar->longname), h, unconsumed);
if (err != ARCHIVE_OK)
return (err);

err = tar_read_header(a, tar, entry, unconsumed);
if ((err != ARCHIVE_OK) && (err != ARCHIVE_WARN))
return (err);
if (archive_entry_copy_pathname_l(entry, tar->longname.s,
archive_strlen(&(tar->longname)), tar->sconv) != 0)
err = set_conversion_failed_error(a, tar->sconv, "Pathname");
return (err);
}





static int
header_volume(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
(void)h;


return (tar_read_header(a, tar, entry, unconsumed));
}




static int
read_body_to_string(struct archive_read *a, struct tar *tar,
struct archive_string *as, const void *h, size_t *unconsumed)
{
int64_t size;
const struct archive_entry_header_ustar *header;
const void *src;

(void)tar;
header = (const struct archive_entry_header_ustar *)h;
size = tar_atol(header->size, sizeof(header->size));
if ((size > 1048576) || (size < 0)) {
archive_set_error(&a->archive, EINVAL,
"Special header too large");
return (ARCHIVE_FATAL);
}


if (archive_string_ensure(as, (size_t)size+1) == NULL) {
archive_set_error(&a->archive, ENOMEM,
"No memory");
return (ARCHIVE_FATAL);
}

tar_flush_unconsumed(a, unconsumed);


*unconsumed = (size_t)((size + 511) & ~ 511);
src = __archive_read_ahead(a, *unconsumed, NULL);
if (src == NULL) {
*unconsumed = 0;
return (ARCHIVE_FATAL);
}
memcpy(as->s, src, (size_t)size);
as->s[size] = '\0';
as->length = (size_t)size;
return (ARCHIVE_OK);
}











static int
header_common(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h)
{
const struct archive_entry_header_ustar *header;
char tartype;
int err = ARCHIVE_OK;

header = (const struct archive_entry_header_ustar *)h;
if (header->linkname[0])
archive_strncpy(&(tar->entry_linkpath),
header->linkname, sizeof(header->linkname));
else
archive_string_empty(&(tar->entry_linkpath));


archive_entry_set_mode(entry,
(mode_t)tar_atol(header->mode, sizeof(header->mode)));
archive_entry_set_uid(entry, tar_atol(header->uid, sizeof(header->uid)));
archive_entry_set_gid(entry, tar_atol(header->gid, sizeof(header->gid)));
tar->entry_bytes_remaining = tar_atol(header->size, sizeof(header->size));
if (tar->entry_bytes_remaining < 0) {
tar->entry_bytes_remaining = 0;
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Tar entry has negative size");
return (ARCHIVE_FATAL);
}
if (tar->entry_bytes_remaining == INT64_MAX) {

tar->entry_bytes_remaining = 0;
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Tar entry size overflow");
return (ARCHIVE_FATAL);
}
tar->realsize = tar->entry_bytes_remaining;
archive_entry_set_size(entry, tar->entry_bytes_remaining);
archive_entry_set_mtime(entry, tar_atol(header->mtime, sizeof(header->mtime)), 0);


tartype = header->typeflag[0];

switch (tartype) {
case '1':
if (archive_entry_copy_hardlink_l(entry, tar->entry_linkpath.s,
archive_strlen(&(tar->entry_linkpath)), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv,
"Linkname");
if (err == ARCHIVE_FATAL)
return (err);
}









if (archive_entry_size(entry) > 0)
archive_entry_set_filetype(entry, AE_IFREG);














if (archive_entry_size(entry) == 0) {

} else if (a->archive.archive_format
== ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE) {

} else if (a->archive.archive_format == ARCHIVE_FORMAT_TAR
|| a->archive.archive_format == ARCHIVE_FORMAT_TAR_GNUTAR)
{

archive_entry_set_size(entry, 0);
tar->entry_bytes_remaining = 0;
} else if (archive_read_format_tar_bid(a, 50) > 50) {






archive_entry_set_size(entry, 0);
tar->entry_bytes_remaining = 0;
}










break;
case '2':
archive_entry_set_filetype(entry, AE_IFLNK);
archive_entry_set_size(entry, 0);
tar->entry_bytes_remaining = 0;
if (archive_entry_copy_symlink_l(entry, tar->entry_linkpath.s,
archive_strlen(&(tar->entry_linkpath)), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv,
"Linkname");
if (err == ARCHIVE_FATAL)
return (err);
}
break;
case '3':
archive_entry_set_filetype(entry, AE_IFCHR);
archive_entry_set_size(entry, 0);
tar->entry_bytes_remaining = 0;
break;
case '4':
archive_entry_set_filetype(entry, AE_IFBLK);
archive_entry_set_size(entry, 0);
tar->entry_bytes_remaining = 0;
break;
case '5':
archive_entry_set_filetype(entry, AE_IFDIR);
archive_entry_set_size(entry, 0);
tar->entry_bytes_remaining = 0;
break;
case '6':
archive_entry_set_filetype(entry, AE_IFIFO);
archive_entry_set_size(entry, 0);
tar->entry_bytes_remaining = 0;
break;
case 'D':





archive_entry_set_filetype(entry, AE_IFDIR);
break;
case 'M':






break;
case 'N':



archive_entry_set_filetype(entry, AE_IFREG);
break;
case 'S':





case '0':





tar->sparse_allowed = 1;

default:




archive_entry_set_filetype(entry, AE_IFREG);
break;
}
return (err);
}




static int
header_old_tar(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h)
{
const struct archive_entry_header_ustar *header;
int err = ARCHIVE_OK, err2;


header = (const struct archive_entry_header_ustar *)h;
if (archive_entry_copy_pathname_l(entry,
header->name, sizeof(header->name), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv, "Pathname");
if (err == ARCHIVE_FATAL)
return (err);
}


err2 = header_common(a, tar, entry, h);
if (err > err2)
err = err2;

tar->entry_padding = 0x1ff & (-tar->entry_bytes_remaining);
return (err);
}





static int
read_mac_metadata_blob(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
int64_t size;
const void *data;
const char *p, *name;
const wchar_t *wp, *wname;

(void)h;

wname = wp = archive_entry_pathname_w(entry);
if (wp != NULL) {

for (; *wp != L'\0'; ++wp) {
if (wp[0] == '/' && wp[1] != L'\0')
wname = wp + 1;
}




if (wname[0] != L'.' || wname[1] != L'_' || wname[2] == L'\0')
return ARCHIVE_OK;
} else {

name = p = archive_entry_pathname(entry);
if (p == NULL)
return (ARCHIVE_FAILED);
for (; *p != '\0'; ++p) {
if (p[0] == '/' && p[1] != '\0')
name = p + 1;
}




if (name[0] != '.' || name[1] != '_' || name[2] == '\0')
return ARCHIVE_OK;
}


size = archive_entry_size(entry);













data = __archive_read_ahead(a, (size_t)size, NULL);
if (data == NULL) {
*unconsumed = 0;
return (ARCHIVE_FATAL);
}
archive_entry_copy_mac_metadata(entry, data, (size_t)size);
*unconsumed = (size_t)((size + 511) & ~ 511);
tar_flush_unconsumed(a, unconsumed);
return (tar_read_header(a, tar, entry, unconsumed));
}




static int
header_pax_global(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
int err;

err = read_body_to_string(a, tar, &(tar->pax_global), h, unconsumed);
if (err != ARCHIVE_OK)
return (err);
err = tar_read_header(a, tar, entry, unconsumed);
return (err);
}

static int
header_pax_extensions(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
int err, err2;

err = read_body_to_string(a, tar, &(tar->pax_header), h, unconsumed);
if (err != ARCHIVE_OK)
return (err);


err = tar_read_header(a, tar, entry, unconsumed);
if ((err != ARCHIVE_OK) && (err != ARCHIVE_WARN))
return (err);











err2 = pax_header(a, tar, entry, &tar->pax_header);
err = err_combine(err, err2);
tar->entry_padding = 0x1ff & (-tar->entry_bytes_remaining);
return (err);
}






static int
header_ustar(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h)
{
const struct archive_entry_header_ustar *header;
struct archive_string *as;
int err = ARCHIVE_OK, r;

header = (const struct archive_entry_header_ustar *)h;


as = &(tar->entry_pathname);
if (header->prefix[0]) {
archive_strncpy(as, header->prefix, sizeof(header->prefix));
if (as->s[archive_strlen(as) - 1] != '/')
archive_strappend_char(as, '/');
archive_strncat(as, header->name, sizeof(header->name));
} else {
archive_strncpy(as, header->name, sizeof(header->name));
}
if (archive_entry_copy_pathname_l(entry, as->s, archive_strlen(as),
tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv, "Pathname");
if (err == ARCHIVE_FATAL)
return (err);
}


r = header_common(a, tar, entry, h);
if (r == ARCHIVE_FATAL)
return (r);
if (r < err)
err = r;


if (archive_entry_copy_uname_l(entry,
header->uname, sizeof(header->uname), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv, "Uname");
if (err == ARCHIVE_FATAL)
return (err);
}

if (archive_entry_copy_gname_l(entry,
header->gname, sizeof(header->gname), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv, "Gname");
if (err == ARCHIVE_FATAL)
return (err);
}


if (header->typeflag[0] == '3' || header->typeflag[0] == '4') {
archive_entry_set_rdevmajor(entry, (dev_t)
tar_atol(header->rdevmajor, sizeof(header->rdevmajor)));
archive_entry_set_rdevminor(entry, (dev_t)
tar_atol(header->rdevminor, sizeof(header->rdevminor)));
}

tar->entry_padding = 0x1ff & (-tar->entry_bytes_remaining);

return (err);
}







static int
pax_header(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, struct archive_string *in_as)
{
size_t attr_length, l, line_length, value_length;
char *p;
char *key, *value;
struct archive_string *as;
struct archive_string_conv *sconv;
int err, err2;
char *attr = in_as->s;

attr_length = in_as->length;
tar->pax_hdrcharset_binary = 0;
archive_string_empty(&(tar->entry_gname));
archive_string_empty(&(tar->entry_linkpath));
archive_string_empty(&(tar->entry_pathname));
archive_string_empty(&(tar->entry_pathname_override));
archive_string_empty(&(tar->entry_uname));
err = ARCHIVE_OK;
while (attr_length > 0) {

line_length = 0;
l = attr_length;
p = attr;
while (l>0) {
if (*p == ' ') {
p++;
l--;
break;
}
if (*p < '0' || *p > '9') {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Ignoring malformed pax extended attributes");
return (ARCHIVE_WARN);
}
line_length *= 10;
line_length += *p - '0';
if (line_length > 999999) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Rejecting pax extended attribute > 1MB");
return (ARCHIVE_WARN);
}
p++;
l--;
}






if (line_length > attr_length
|| line_length < 1
|| attr[line_length - 1] != '\n')
{
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Ignoring malformed pax extended attribute");
return (ARCHIVE_WARN);
}


attr[line_length - 1] = '\0';


key = p;
if (key[0] == '=')
return (-1);
while (*p && *p != '=')
++p;
if (*p == '\0') {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Invalid pax extended attributes");
return (ARCHIVE_WARN);
}
*p = '\0';

value = p + 1;


value_length = attr + line_length - 1 - value;


err2 = pax_attribute(a, tar, entry, key, value, value_length);
if (err2 == ARCHIVE_FATAL)
return (err2);
err = err_combine(err, err2);


attr += line_length;
attr_length -= line_length;
}










if (tar->pax_hdrcharset_binary)
sconv = tar->opt_sconv;
else {
sconv = archive_string_conversion_from_charset(
&(a->archive), "UTF-8", 1);
if (sconv == NULL)
return (ARCHIVE_FATAL);
if (tar->compat_2x)
archive_string_conversion_set_opt(sconv,
SCONV_SET_OPT_UTF8_LIBARCHIVE2X);
}

if (archive_strlen(&(tar->entry_gname)) > 0) {
if (archive_entry_copy_gname_l(entry, tar->entry_gname.s,
archive_strlen(&(tar->entry_gname)), sconv) != 0) {
err = set_conversion_failed_error(a, sconv, "Gname");
if (err == ARCHIVE_FATAL)
return (err);

archive_entry_copy_gname(entry, tar->entry_gname.s);
}
}
if (archive_strlen(&(tar->entry_linkpath)) > 0) {
if (archive_entry_copy_link_l(entry, tar->entry_linkpath.s,
archive_strlen(&(tar->entry_linkpath)), sconv) != 0) {
err = set_conversion_failed_error(a, sconv, "Linkname");
if (err == ARCHIVE_FATAL)
return (err);

archive_entry_copy_link(entry, tar->entry_linkpath.s);
}
}









as = NULL;
if (archive_strlen(&(tar->entry_pathname_override)) > 0)
as = &(tar->entry_pathname_override);
else if (archive_strlen(&(tar->entry_pathname)) > 0)
as = &(tar->entry_pathname);
if (as != NULL) {
if (archive_entry_copy_pathname_l(entry, as->s,
archive_strlen(as), sconv) != 0) {
err = set_conversion_failed_error(a, sconv, "Pathname");
if (err == ARCHIVE_FATAL)
return (err);

archive_entry_copy_pathname(entry, as->s);
}
}
if (archive_strlen(&(tar->entry_uname)) > 0) {
if (archive_entry_copy_uname_l(entry, tar->entry_uname.s,
archive_strlen(&(tar->entry_uname)), sconv) != 0) {
err = set_conversion_failed_error(a, sconv, "Uname");
if (err == ARCHIVE_FATAL)
return (err);

archive_entry_copy_uname(entry, tar->entry_uname.s);
}
}
return (err);
}

static int
pax_attribute_xattr(struct archive_entry *entry,
const char *name, const char *value)
{
char *name_decoded;
void *value_decoded;
size_t value_len;

if (strlen(name) < 18 || (memcmp(name, "LIBARCHIVE.xattr.", 17)) != 0)
return 3;

name += 17;


name_decoded = url_decode(name);
if (name_decoded == NULL)
return 2;


value_decoded = base64_decode(value, strlen(value), &value_len);
if (value_decoded == NULL) {
free(name_decoded);
return 1;
}

archive_entry_xattr_add_entry(entry, name_decoded,
value_decoded, value_len);

free(name_decoded);
free(value_decoded);
return 0;
}

static int
pax_attribute_schily_xattr(struct archive_entry *entry,
const char *name, const char *value, size_t value_length)
{
if (strlen(name) < 14 || (memcmp(name, "SCHILY.xattr.", 13)) != 0)
return 1;

name += 13;

archive_entry_xattr_add_entry(entry, name, value, value_length);

return 0;
}

static int
pax_attribute_rht_security_selinux(struct archive_entry *entry,
const char *value, size_t value_length)
{
archive_entry_xattr_add_entry(entry, "security.selinux",
value, value_length);

return 0;
}

static int
pax_attribute_acl(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const char *value, int type)
{
int r;
const char* errstr;

switch (type) {
case ARCHIVE_ENTRY_ACL_TYPE_ACCESS:
errstr = "SCHILY.acl.access";
break;
case ARCHIVE_ENTRY_ACL_TYPE_DEFAULT:
errstr = "SCHILY.acl.default";
break;
case ARCHIVE_ENTRY_ACL_TYPE_NFS4:
errstr = "SCHILY.acl.ace";
break;
default:
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Unknown ACL type: %d", type);
return(ARCHIVE_FATAL);
}

if (tar->sconv_acl == NULL) {
tar->sconv_acl =
archive_string_conversion_from_charset(
&(a->archive), "UTF-8", 1);
if (tar->sconv_acl == NULL)
return (ARCHIVE_FATAL);
}

r = archive_acl_from_text_l(archive_entry_acl(entry), value, type,
tar->sconv_acl);
if (r != ARCHIVE_OK) {
if (r == ARCHIVE_FATAL) {
archive_set_error(&a->archive, ENOMEM,
"%s %s", "Can't allocate memory for ",
errstr);
return (r);
}
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC, "%s %s", "Parse error: ", errstr);
}
return (r);
}














static int
pax_attribute(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const char *key, const char *value, size_t value_length)
{
int64_t s;
long n;
int err = ARCHIVE_OK, r;

#if !defined(__FreeBSD__)
if (value == NULL)
value = "";

#endif
switch (key[0]) {
case 'G':

if (strncmp(key, "GNU.sparse", 10) == 0 &&
!tar->sparse_allowed) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Non-regular file cannot be sparse");
return (ARCHIVE_FATAL);
}


if (strcmp(key, "GNU.sparse.numblocks") == 0) {
tar->sparse_offset = -1;
tar->sparse_numbytes = -1;
tar->sparse_gnu_major = 0;
tar->sparse_gnu_minor = 0;
}
if (strcmp(key, "GNU.sparse.offset") == 0) {
tar->sparse_offset = tar_atol10(value, strlen(value));
if (tar->sparse_numbytes != -1) {
if (gnu_add_sparse_entry(a, tar,
tar->sparse_offset, tar->sparse_numbytes)
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
tar->sparse_offset = -1;
tar->sparse_numbytes = -1;
}
}
if (strcmp(key, "GNU.sparse.numbytes") == 0) {
tar->sparse_numbytes = tar_atol10(value, strlen(value));
if (tar->sparse_offset != -1) {
if (gnu_add_sparse_entry(a, tar,
tar->sparse_offset, tar->sparse_numbytes)
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
tar->sparse_offset = -1;
tar->sparse_numbytes = -1;
}
}
if (strcmp(key, "GNU.sparse.size") == 0) {
tar->realsize = tar_atol10(value, strlen(value));
archive_entry_set_size(entry, tar->realsize);
tar->realsize_override = 1;
}


if (strcmp(key, "GNU.sparse.map") == 0) {
tar->sparse_gnu_major = 0;
tar->sparse_gnu_minor = 1;
if (gnu_sparse_01_parse(a, tar, value) != ARCHIVE_OK)
return (ARCHIVE_WARN);
}


if (strcmp(key, "GNU.sparse.major") == 0) {
tar->sparse_gnu_major = (int)tar_atol10(value, strlen(value));
tar->sparse_gnu_pending = 1;
}
if (strcmp(key, "GNU.sparse.minor") == 0) {
tar->sparse_gnu_minor = (int)tar_atol10(value, strlen(value));
tar->sparse_gnu_pending = 1;
}
if (strcmp(key, "GNU.sparse.name") == 0) {






archive_strcpy(&(tar->entry_pathname_override), value);
}
if (strcmp(key, "GNU.sparse.realsize") == 0) {
tar->realsize = tar_atol10(value, strlen(value));
archive_entry_set_size(entry, tar->realsize);
tar->realsize_override = 1;
}
break;
case 'L':






if (strcmp(key, "LIBARCHIVE.creationtime") == 0) {
pax_time(value, &s, &n);
archive_entry_set_birthtime(entry, s, n);
}
if (strcmp(key, "LIBARCHIVE.symlinktype") == 0) {
if (strcmp(value, "file") == 0) {
archive_entry_set_symlink_type(entry,
AE_SYMLINK_TYPE_FILE);
} else if (strcmp(value, "dir") == 0) {
archive_entry_set_symlink_type(entry,
AE_SYMLINK_TYPE_DIRECTORY);
}
}
if (memcmp(key, "LIBARCHIVE.xattr.", 17) == 0)
pax_attribute_xattr(entry, key, value);
break;
case 'R':


if (strcmp(key, "RHT.security.selinux") == 0) {
pax_attribute_rht_security_selinux(entry, value,
value_length);
}
break;
case 'S':

if (strcmp(key, "SCHILY.acl.access") == 0) {
r = pax_attribute_acl(a, tar, entry, value,
ARCHIVE_ENTRY_ACL_TYPE_ACCESS);
if (r == ARCHIVE_FATAL)
return (r);
} else if (strcmp(key, "SCHILY.acl.default") == 0) {
r = pax_attribute_acl(a, tar, entry, value,
ARCHIVE_ENTRY_ACL_TYPE_DEFAULT);
if (r == ARCHIVE_FATAL)
return (r);
} else if (strcmp(key, "SCHILY.acl.ace") == 0) {
r = pax_attribute_acl(a, tar, entry, value,
ARCHIVE_ENTRY_ACL_TYPE_NFS4);
if (r == ARCHIVE_FATAL)
return (r);
} else if (strcmp(key, "SCHILY.devmajor") == 0) {
archive_entry_set_rdevmajor(entry,
(dev_t)tar_atol10(value, strlen(value)));
} else if (strcmp(key, "SCHILY.devminor") == 0) {
archive_entry_set_rdevminor(entry,
(dev_t)tar_atol10(value, strlen(value)));
} else if (strcmp(key, "SCHILY.fflags") == 0) {
archive_entry_copy_fflags_text(entry, value);
} else if (strcmp(key, "SCHILY.dev") == 0) {
archive_entry_set_dev(entry,
(dev_t)tar_atol10(value, strlen(value)));
} else if (strcmp(key, "SCHILY.ino") == 0) {
archive_entry_set_ino(entry,
tar_atol10(value, strlen(value)));
} else if (strcmp(key, "SCHILY.nlink") == 0) {
archive_entry_set_nlink(entry, (unsigned)
tar_atol10(value, strlen(value)));
} else if (strcmp(key, "SCHILY.realsize") == 0) {
tar->realsize = tar_atol10(value, strlen(value));
tar->realsize_override = 1;
archive_entry_set_size(entry, tar->realsize);
} else if (strncmp(key, "SCHILY.xattr.", 13) == 0) {
pax_attribute_schily_xattr(entry, key, value,
value_length);
} else if (strcmp(key, "SUN.holesdata") == 0) {

r = solaris_sparse_parse(a, tar, entry, value);
if (r < err) {
if (r == ARCHIVE_FATAL)
return (r);
err = r;
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"Parse error: SUN.holesdata");
}
}
break;
case 'a':
if (strcmp(key, "atime") == 0) {
pax_time(value, &s, &n);
archive_entry_set_atime(entry, s, n);
}
break;
case 'c':
if (strcmp(key, "ctime") == 0) {
pax_time(value, &s, &n);
archive_entry_set_ctime(entry, s, n);
} else if (strcmp(key, "charset") == 0) {

} else if (strcmp(key, "comment") == 0) {

}
break;
case 'g':
if (strcmp(key, "gid") == 0) {
archive_entry_set_gid(entry,
tar_atol10(value, strlen(value)));
} else if (strcmp(key, "gname") == 0) {
archive_strcpy(&(tar->entry_gname), value);
}
break;
case 'h':
if (strcmp(key, "hdrcharset") == 0) {
if (strcmp(value, "BINARY") == 0)

tar->pax_hdrcharset_binary = 1;
else if (strcmp(value, "ISO-IR 10646 2000 UTF-8") == 0)
tar->pax_hdrcharset_binary = 0;
}
break;
case 'l':

if (strcmp(key, "linkpath") == 0) {
archive_strcpy(&(tar->entry_linkpath), value);
}
break;
case 'm':
if (strcmp(key, "mtime") == 0) {
pax_time(value, &s, &n);
archive_entry_set_mtime(entry, s, n);
}
break;
case 'p':
if (strcmp(key, "path") == 0) {
archive_strcpy(&(tar->entry_pathname), value);
}
break;
case 'r':

break;
case 's':


if (strcmp(key, "size") == 0) {

tar->entry_bytes_remaining
= tar_atol10(value, strlen(value));






if (!tar->realsize_override) {
archive_entry_set_size(entry,
tar->entry_bytes_remaining);
tar->realsize
= tar->entry_bytes_remaining;
}
}
break;
case 'u':
if (strcmp(key, "uid") == 0) {
archive_entry_set_uid(entry,
tar_atol10(value, strlen(value)));
} else if (strcmp(key, "uname") == 0) {
archive_strcpy(&(tar->entry_uname), value);
}
break;
}
return (err);
}






static void
pax_time(const char *p, int64_t *ps, long *pn)
{
char digit;
int64_t s;
unsigned long l;
int sign;
int64_t limit, last_digit_limit;

limit = INT64_MAX / 10;
last_digit_limit = INT64_MAX % 10;

s = 0;
sign = 1;
if (*p == '-') {
sign = -1;
p++;
}
while (*p >= '0' && *p <= '9') {
digit = *p - '0';
if (s > limit ||
(s == limit && digit > last_digit_limit)) {
s = INT64_MAX;
break;
}
s = (s * 10) + digit;
++p;
}

*ps = s * sign;


*pn = 0;

if (*p != '.')
return;

l = 100000000UL;
do {
++p;
if (*p >= '0' && *p <= '9')
*pn += (*p - '0') * l;
else
break;
} while (l /= 10);
}




static int
header_gnutar(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const void *h, size_t *unconsumed)
{
const struct archive_entry_header_gnutar *header;
int64_t t;
int err = ARCHIVE_OK;








err = header_common(a, tar, entry, h);
if (err == ARCHIVE_FATAL)
return (err);


header = (const struct archive_entry_header_gnutar *)h;
if (archive_entry_copy_pathname_l(entry,
header->name, sizeof(header->name), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv, "Pathname");
if (err == ARCHIVE_FATAL)
return (err);
}





if (archive_entry_copy_uname_l(entry,
header->uname, sizeof(header->uname), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv, "Uname");
if (err == ARCHIVE_FATAL)
return (err);
}

if (archive_entry_copy_gname_l(entry,
header->gname, sizeof(header->gname), tar->sconv) != 0) {
err = set_conversion_failed_error(a, tar->sconv, "Gname");
if (err == ARCHIVE_FATAL)
return (err);
}


if (header->typeflag[0] == '3' || header->typeflag[0] == '4') {
archive_entry_set_rdevmajor(entry, (dev_t)
tar_atol(header->rdevmajor, sizeof(header->rdevmajor)));
archive_entry_set_rdevminor(entry, (dev_t)
tar_atol(header->rdevminor, sizeof(header->rdevminor)));
} else
archive_entry_set_rdev(entry, 0);

tar->entry_padding = 0x1ff & (-tar->entry_bytes_remaining);


t = tar_atol(header->atime, sizeof(header->atime));
if (t > 0)
archive_entry_set_atime(entry, t, 0);
t = tar_atol(header->ctime, sizeof(header->ctime));
if (t > 0)
archive_entry_set_ctime(entry, t, 0);

if (header->realsize[0] != 0) {
tar->realsize
= tar_atol(header->realsize, sizeof(header->realsize));
archive_entry_set_size(entry, tar->realsize);
tar->realsize_override = 1;
}

if (header->sparse[0].offset[0] != 0) {
if (gnu_sparse_old_read(a, tar, header, unconsumed)
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
} else {
if (header->isextended[0] != 0) {

}
}

return (err);
}

static int
gnu_add_sparse_entry(struct archive_read *a, struct tar *tar,
int64_t offset, int64_t remaining)
{
struct sparse_block *p;

p = (struct sparse_block *)calloc(1, sizeof(*p));
if (p == NULL) {
archive_set_error(&a->archive, ENOMEM, "Out of memory");
return (ARCHIVE_FATAL);
}
if (tar->sparse_last != NULL)
tar->sparse_last->next = p;
else
tar->sparse_list = p;
tar->sparse_last = p;
if (remaining < 0 || offset < 0 || offset > INT64_MAX - remaining) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC, "Malformed sparse map data");
return (ARCHIVE_FATAL);
}
p->offset = offset;
p->remaining = remaining;
return (ARCHIVE_OK);
}

static void
gnu_clear_sparse_list(struct tar *tar)
{
struct sparse_block *p;

while (tar->sparse_list != NULL) {
p = tar->sparse_list;
tar->sparse_list = p->next;
free(p);
}
tar->sparse_last = NULL;
}













static int
gnu_sparse_old_read(struct archive_read *a, struct tar *tar,
const struct archive_entry_header_gnutar *header, size_t *unconsumed)
{
ssize_t bytes_read;
const void *data;
struct extended {
struct gnu_sparse sparse[21];
char isextended[1];
char padding[7];
};
const struct extended *ext;

if (gnu_sparse_old_parse(a, tar, header->sparse, 4) != ARCHIVE_OK)
return (ARCHIVE_FATAL);
if (header->isextended[0] == 0)
return (ARCHIVE_OK);

do {
tar_flush_unconsumed(a, unconsumed);
data = __archive_read_ahead(a, 512, &bytes_read);
if (bytes_read < 0)
return (ARCHIVE_FATAL);
if (bytes_read < 512) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Truncated tar archive "
"detected while reading sparse file data");
return (ARCHIVE_FATAL);
}
*unconsumed = 512;
ext = (const struct extended *)data;
if (gnu_sparse_old_parse(a, tar, ext->sparse, 21) != ARCHIVE_OK)
return (ARCHIVE_FATAL);
} while (ext->isextended[0] != 0);
if (tar->sparse_list != NULL)
tar->entry_offset = tar->sparse_list->offset;
return (ARCHIVE_OK);
}

static int
gnu_sparse_old_parse(struct archive_read *a, struct tar *tar,
const struct gnu_sparse *sparse, int length)
{
while (length > 0 && sparse->offset[0] != 0) {
if (gnu_add_sparse_entry(a, tar,
tar_atol(sparse->offset, sizeof(sparse->offset)),
tar_atol(sparse->numbytes, sizeof(sparse->numbytes)))
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
sparse++;
length--;
}
return (ARCHIVE_OK);
}



























static int
gnu_sparse_01_parse(struct archive_read *a, struct tar *tar, const char *p)
{
const char *e;
int64_t offset = -1, size = -1;

for (;;) {
e = p;
while (*e != '\0' && *e != ',') {
if (*e < '0' || *e > '9')
return (ARCHIVE_WARN);
e++;
}
if (offset < 0) {
offset = tar_atol10(p, e - p);
if (offset < 0)
return (ARCHIVE_WARN);
} else {
size = tar_atol10(p, e - p);
if (size < 0)
return (ARCHIVE_WARN);
if (gnu_add_sparse_entry(a, tar, offset, size)
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
offset = -1;
}
if (*e == '\0')
return (ARCHIVE_OK);
p = e + 1;
}
}
























static int64_t
gnu_sparse_10_atol(struct archive_read *a, struct tar *tar,
int64_t *remaining, size_t *unconsumed)
{
int64_t l, limit, last_digit_limit;
const char *p;
ssize_t bytes_read;
int base, digit;

base = 10;
limit = INT64_MAX / base;
last_digit_limit = INT64_MAX % base;





do {
bytes_read = readline(a, tar, &p,
(ssize_t)tar_min(*remaining, 100), unconsumed);
if (bytes_read <= 0)
return (ARCHIVE_FATAL);
*remaining -= bytes_read;
} while (p[0] == '#');

l = 0;
while (bytes_read > 0) {
if (*p == '\n')
return (l);
if (*p < '0' || *p >= '0' + base)
return (ARCHIVE_WARN);
digit = *p - '0';
if (l > limit || (l == limit && digit > last_digit_limit))
l = INT64_MAX;
else
l = (l * base) + digit;
p++;
bytes_read--;
}

return (ARCHIVE_WARN);
}





static ssize_t
gnu_sparse_10_read(struct archive_read *a, struct tar *tar, size_t *unconsumed)
{
ssize_t bytes_read;
int entries;
int64_t offset, size, to_skip, remaining;


gnu_clear_sparse_list(tar);

remaining = tar->entry_bytes_remaining;


entries = (int)gnu_sparse_10_atol(a, tar, &remaining, unconsumed);
if (entries < 0)
return (ARCHIVE_FATAL);

while (entries-- > 0) {

offset = gnu_sparse_10_atol(a, tar, &remaining, unconsumed);
if (offset < 0)
return (ARCHIVE_FATAL);
size = gnu_sparse_10_atol(a, tar, &remaining, unconsumed);
if (size < 0)
return (ARCHIVE_FATAL);

if (gnu_add_sparse_entry(a, tar, offset, size) != ARCHIVE_OK)
return (ARCHIVE_FATAL);
}

tar_flush_unconsumed(a, unconsumed);
bytes_read = (ssize_t)(tar->entry_bytes_remaining - remaining);
to_skip = 0x1ff & -bytes_read;

if (to_skip > remaining)
return (ARCHIVE_FATAL);
if (to_skip != __archive_read_consume(a, to_skip))
return (ARCHIVE_FATAL);
return ((ssize_t)(bytes_read + to_skip));
}







static int
solaris_sparse_parse(struct archive_read *a, struct tar *tar,
struct archive_entry *entry, const char *p)
{
const char *e;
int64_t start, end;
int hole = 1;

(void)entry;

end = 0;
if (*p == ' ')
p++;
else
return (ARCHIVE_WARN);
for (;;) {
e = p;
while (*e != '\0' && *e != ' ') {
if (*e < '0' || *e > '9')
return (ARCHIVE_WARN);
e++;
}
start = end;
end = tar_atol10(p, e - p);
if (end < 0)
return (ARCHIVE_WARN);
if (start < end) {
if (gnu_add_sparse_entry(a, tar, start,
end - start) != ARCHIVE_OK)
return (ARCHIVE_FATAL);
tar->sparse_last->hole = hole;
}
if (*e == '\0')
return (ARCHIVE_OK);
p = e + 1;
hole = hole == 0;
}
}


















static int64_t
tar_atol(const char *p, size_t char_cnt)
{




if (*p & 0x80)
return (tar_atol256(p, char_cnt));
return (tar_atol8(p, char_cnt));
}






static int64_t
tar_atol_base_n(const char *p, size_t char_cnt, int base)
{
int64_t l, maxval, limit, last_digit_limit;
int digit, sign;

maxval = INT64_MAX;
limit = INT64_MAX / base;
last_digit_limit = INT64_MAX % base;




while (char_cnt != 0 && (*p == ' ' || *p == '\t')) {
p++;
char_cnt--;
}

sign = 1;
if (char_cnt != 0 && *p == '-') {
sign = -1;
p++;
char_cnt--;

maxval = INT64_MIN;
limit = -(INT64_MIN / base);
last_digit_limit = -(INT64_MIN % base);
}

l = 0;
if (char_cnt != 0) {
digit = *p - '0';
while (digit >= 0 && digit < base && char_cnt != 0) {
if (l>limit || (l == limit && digit >= last_digit_limit)) {
return maxval;
}
l = (l * base) + digit;
digit = *++p - '0';
char_cnt--;
}
}
return (sign < 0) ? -l : l;
}

static int64_t
tar_atol8(const char *p, size_t char_cnt)
{
return tar_atol_base_n(p, char_cnt, 8);
}

static int64_t
tar_atol10(const char *p, size_t char_cnt)
{
return tar_atol_base_n(p, char_cnt, 10);
}











static int64_t
tar_atol256(const char *_p, size_t char_cnt)
{
uint64_t l;
const unsigned char *p = (const unsigned char *)_p;
unsigned char c, neg;


c = *p;
if (c & 0x40) {
neg = 0xff;
c |= 0x80;
l = ~ARCHIVE_LITERAL_ULL(0);
} else {
neg = 0;
c &= 0x7f;
l = 0;
}



while (char_cnt > sizeof(int64_t)) {
--char_cnt;
if (c != neg)
return neg ? INT64_MIN : INT64_MAX;
c = *++p;
}


if ((c ^ neg) & 0x80) {
return neg ? INT64_MIN : INT64_MAX;
}


while (--char_cnt > 0) {
l = (l << 8) | c;
c = *++p;
}
l = (l << 8) | c;

return (int64_t)(l);
}







static ssize_t
readline(struct archive_read *a, struct tar *tar, const char **start,
ssize_t limit, size_t *unconsumed)
{
ssize_t bytes_read;
ssize_t total_size = 0;
const void *t;
const char *s;
void *p;

tar_flush_unconsumed(a, unconsumed);

t = __archive_read_ahead(a, 1, &bytes_read);
if (bytes_read <= 0)
return (ARCHIVE_FATAL);
s = t;
p = memchr(t, '\n', bytes_read);

if (p != NULL) {
bytes_read = 1 + ((const char *)p) - s;
if (bytes_read > limit) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Line too long");
return (ARCHIVE_FATAL);
}
*unconsumed = bytes_read;
*start = s;
return (bytes_read);
}
*unconsumed = bytes_read;

for (;;) {
if (total_size + bytes_read > limit) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Line too long");
return (ARCHIVE_FATAL);
}
if (archive_string_ensure(&tar->line, total_size + bytes_read) == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate working buffer");
return (ARCHIVE_FATAL);
}
memcpy(tar->line.s + total_size, t, bytes_read);
tar_flush_unconsumed(a, unconsumed);
total_size += bytes_read;

if (p != NULL) {
*start = tar->line.s;
return (total_size);
}

t = __archive_read_ahead(a, 1, &bytes_read);
if (bytes_read <= 0)
return (ARCHIVE_FATAL);
s = t;
p = memchr(t, '\n', bytes_read);

if (p != NULL) {
bytes_read = 1 + ((const char *)p) - s;
}
*unconsumed = bytes_read;
}
}










static char *
base64_decode(const char *s, size_t len, size_t *out_len)
{
static const unsigned char digits[64] = {
'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
'O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b',
'c','d','e','f','g','h','i','j','k','l','m','n','o','p',
'q','r','s','t','u','v','w','x','y','z','0','1','2','3',
'4','5','6','7','8','9','+','/' };
static unsigned char decode_table[128];
char *out, *d;
const unsigned char *src = (const unsigned char *)s;


if (decode_table[digits[1]] != 1) {
unsigned i;
memset(decode_table, 0xff, sizeof(decode_table));
for (i = 0; i < sizeof(digits); i++)
decode_table[digits[i]] = i;
}



out = (char *)malloc(len - len / 4 + 1);
if (out == NULL) {
*out_len = 0;
return (NULL);
}
d = out;

while (len > 0) {

int v = 0;
int group_size = 0;
while (group_size < 4 && len > 0) {

if (*src == '=' || *src == '_') {
len = 0;
break;
}

if (*src > 127 || *src < 32
|| decode_table[*src] == 0xff) {
len--;
src++;
continue;
}
v <<= 6;
v |= decode_table[*src++];
len --;
group_size++;
}

v <<= 6 * (4 - group_size);

switch (group_size) {
case 4: d[2] = v & 0xff;

case 3: d[1] = (v >> 8) & 0xff;

case 2: d[0] = (v >> 16) & 0xff;
break;
case 1:
break;
}
d += group_size * 3 / 4;
}

*out_len = d - out;
return (out);
}

static char *
url_decode(const char *in)
{
char *out, *d;
const char *s;

out = (char *)malloc(strlen(in) + 1);
if (out == NULL)
return (NULL);
for (s = in, d = out; *s != '\0'; ) {
if (s[0] == '%' && s[1] != '\0' && s[2] != '\0') {

int digit1 = tohex(s[1]);
int digit2 = tohex(s[2]);
if (digit1 >= 0 && digit2 >= 0) {

s += 3;

*d++ = ((digit1 << 4) | digit2);
continue;
}

}
*d++ = *s++;
}
*d = '\0';
return (out);
}

static int
tohex(int c)
{
if (c >= '0' && c <= '9')
return (c - '0');
else if (c >= 'A' && c <= 'F')
return (c - 'A' + 10);
else if (c >= 'a' && c <= 'f')
return (c - 'a' + 10);
else
return (-1);
}
