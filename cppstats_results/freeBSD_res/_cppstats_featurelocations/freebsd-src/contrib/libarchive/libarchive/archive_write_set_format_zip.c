






























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_LANGINFO_H)
#include <langinfo.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_ZLIB_H)
#include <zlib.h>
#endif

#include "archive.h"
#include "archive_cryptor_private.h"
#include "archive_endian.h"
#include "archive_entry.h"
#include "archive_entry_locale.h"
#include "archive_hmac_private.h"
#include "archive_private.h"
#include "archive_random_private.h"
#include "archive_write_private.h"
#include "archive_write_set_format_private.h"

#if !defined(HAVE_ZLIB_H)
#include "archive_crc32.h"
#endif

#define ZIP_ENTRY_FLAG_ENCRYPTED (1<<0)
#define ZIP_ENTRY_FLAG_LENGTH_AT_END (1<<3)
#define ZIP_ENTRY_FLAG_UTF8_NAME (1 << 11)

#define ZIP_4GB_MAX ARCHIVE_LITERAL_LL(0xffffffff)
#define ZIP_4GB_MAX_UNCOMPRESSED ARCHIVE_LITERAL_LL(0xff000000)

enum compression {
COMPRESSION_UNSPECIFIED = -1,
COMPRESSION_STORE = 0,
COMPRESSION_DEFLATE = 8
};

#if defined(HAVE_ZLIB_H)
#define COMPRESSION_DEFAULT COMPRESSION_DEFLATE
#else
#define COMPRESSION_DEFAULT COMPRESSION_STORE
#endif

enum encryption {
ENCRYPTION_NONE = 0,
ENCRYPTION_TRADITIONAL,
ENCRYPTION_WINZIP_AES128,
ENCRYPTION_WINZIP_AES256,
};

#define TRAD_HEADER_SIZE 12





#define WINZIP_AES_ENCRYPTION 99


#define WINZIP_AES128_HEADER_SIZE (8 + 2)
#define WINZIP_AES256_HEADER_SIZE (16 + 2)

#define AES_VENDOR_AE_1 0x0001
#define AES_VENDOR_AE_2 0x0002

#define AUTH_CODE_SIZE 10

#define MAX_DERIVED_KEY_BUF_SIZE (AES_MAX_KEY_SIZE * 2 + 2)

struct cd_segment {
struct cd_segment *next;
size_t buff_size;
unsigned char *buff;
unsigned char *p;
};

struct trad_enc_ctx {
uint32_t keys[3];
};

struct zip {

int64_t entry_offset;
int64_t entry_compressed_size;
int64_t entry_uncompressed_size;
int64_t entry_compressed_written;
int64_t entry_uncompressed_written;
int64_t entry_uncompressed_limit;
struct archive_entry *entry;
uint32_t entry_crc32;
enum compression entry_compression;
enum encryption entry_encryption;
int entry_flags;
int entry_uses_zip64;
int experiments;
struct trad_enc_ctx tctx;
char tctx_valid;
unsigned char trad_chkdat;
unsigned aes_vendor;
archive_crypto_ctx cctx;
char cctx_valid;
archive_hmac_sha1_ctx hctx;
char hctx_valid;

unsigned char *file_header;
size_t file_header_extra_offset;
unsigned long (*crc32func)(unsigned long crc, const void *buff, size_t len);

struct cd_segment *central_directory;
struct cd_segment *central_directory_last;
size_t central_directory_bytes;
size_t central_directory_entries;

int64_t written_bytes;

struct archive_string_conv *opt_sconv;
struct archive_string_conv *sconv_default;
enum compression requested_compression;
int deflate_compression_level;
int init_default_conversion;
enum encryption encryption_type;

#define ZIP_FLAG_AVOID_ZIP64 1
#define ZIP_FLAG_FORCE_ZIP64 2
#define ZIP_FLAG_EXPERIMENT_xl 4
int flags;

#if defined(HAVE_ZLIB_H)
z_stream stream;
#endif
size_t len_buf;
unsigned char *buf;
};



#define zipmin(a, b) ((a) > (b) ? (b) : (a))

static ssize_t archive_write_zip_data(struct archive_write *,
const void *buff, size_t s);
static int archive_write_zip_close(struct archive_write *);
static int archive_write_zip_free(struct archive_write *);
static int archive_write_zip_finish_entry(struct archive_write *);
static int archive_write_zip_header(struct archive_write *,
struct archive_entry *);
static int archive_write_zip_options(struct archive_write *,
const char *, const char *);
static unsigned int dos_time(const time_t);
static size_t path_length(struct archive_entry *);
static int write_path(struct archive_entry *, struct archive_write *);
static void copy_path(struct archive_entry *, unsigned char *);
static struct archive_string_conv *get_sconv(struct archive_write *, struct zip *);
static int trad_enc_init(struct trad_enc_ctx *, const char *, size_t);
static unsigned trad_enc_encrypt_update(struct trad_enc_ctx *, const uint8_t *,
size_t, uint8_t *, size_t);
static int init_traditional_pkware_encryption(struct archive_write *);
static int is_traditional_pkware_encryption_supported(void);
static int init_winzip_aes_encryption(struct archive_write *);
static int is_winzip_aes_encryption_supported(int encryption);

static unsigned char *
cd_alloc(struct zip *zip, size_t length)
{
unsigned char *p;

if (zip->central_directory == NULL
|| (zip->central_directory_last->p + length
> zip->central_directory_last->buff + zip->central_directory_last->buff_size)) {
struct cd_segment *segment = calloc(1, sizeof(*segment));
if (segment == NULL)
return NULL;
segment->buff_size = 64 * 1024;
segment->buff = malloc(segment->buff_size);
if (segment->buff == NULL) {
free(segment);
return NULL;
}
segment->p = segment->buff;

if (zip->central_directory == NULL) {
zip->central_directory
= zip->central_directory_last
= segment;
} else {
zip->central_directory_last->next = segment;
zip->central_directory_last = segment;
}
}

p = zip->central_directory_last->p;
zip->central_directory_last->p += length;
zip->central_directory_bytes += length;
return (p);
}

static unsigned long
real_crc32(unsigned long crc, const void *buff, size_t len)
{
return crc32(crc, buff, (unsigned int)len);
}

static unsigned long
fake_crc32(unsigned long crc, const void *buff, size_t len)
{
(void)crc;
(void)buff;
(void)len;
return 0;
}

static int
archive_write_zip_options(struct archive_write *a, const char *key,
const char *val)
{
struct zip *zip = a->format_data;
int ret = ARCHIVE_FAILED;

if (strcmp(key, "compression") == 0) {




if (val == NULL || val[0] == 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"%s: compression option needs a compression name",
a->format_name);
} else if (strcmp(val, "deflate") == 0) {
#if defined(HAVE_ZLIB_H)
zip->requested_compression = COMPRESSION_DEFLATE;
ret = ARCHIVE_OK;
#else
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"deflate compression not supported");
#endif
} else if (strcmp(val, "store") == 0) {
zip->requested_compression = COMPRESSION_STORE;
ret = ARCHIVE_OK;
}
return (ret);
} else if (strcmp(key, "compression-level") == 0) {
if (val == NULL || !(val[0] >= '0' && val[0] <= '9') || val[1] != '\0') {
return ARCHIVE_WARN;
}

if (val[0] == '0') {
zip->requested_compression = COMPRESSION_STORE;
return ARCHIVE_OK;
} else {
#if defined(HAVE_ZLIB_H)
zip->requested_compression = COMPRESSION_DEFLATE;
zip->deflate_compression_level = val[0] - '0';
return ARCHIVE_OK;
#else
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"deflate compression not supported");
#endif
}
} else if (strcmp(key, "encryption") == 0) {
if (val == NULL) {
zip->encryption_type = ENCRYPTION_NONE;
ret = ARCHIVE_OK;
} else if (val[0] == '1' || strcmp(val, "traditional") == 0
|| strcmp(val, "zipcrypt") == 0
|| strcmp(val, "ZipCrypt") == 0) {
if (is_traditional_pkware_encryption_supported()) {
zip->encryption_type = ENCRYPTION_TRADITIONAL;
ret = ARCHIVE_OK;
} else {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"encryption not supported");
}
} else if (strcmp(val, "aes128") == 0) {
if (is_winzip_aes_encryption_supported(
ENCRYPTION_WINZIP_AES128)) {
zip->encryption_type = ENCRYPTION_WINZIP_AES128;
ret = ARCHIVE_OK;
} else {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"encryption not supported");
}
} else if (strcmp(val, "aes256") == 0) {
if (is_winzip_aes_encryption_supported(
ENCRYPTION_WINZIP_AES256)) {
zip->encryption_type = ENCRYPTION_WINZIP_AES256;
ret = ARCHIVE_OK;
} else {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"encryption not supported");
}
} else {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"%s: unknown encryption '%s'",
a->format_name, val);
}
return (ret);
} else if (strcmp(key, "experimental") == 0) {
if (val == NULL || val[0] == 0) {
zip->flags &= ~ ZIP_FLAG_EXPERIMENT_xl;
} else {
zip->flags |= ZIP_FLAG_EXPERIMENT_xl;
}
return (ARCHIVE_OK);
} else if (strcmp(key, "fakecrc32") == 0) {




if (val == NULL || val[0] == 0) {
zip->crc32func = real_crc32;
} else {
zip->crc32func = fake_crc32;
}
return (ARCHIVE_OK);
} else if (strcmp(key, "hdrcharset") == 0) {



if (val == NULL || val[0] == 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"%s: hdrcharset option needs a character-set name",
a->format_name);
} else {
zip->opt_sconv = archive_string_conversion_to_charset(
&a->archive, val, 0);
if (zip->opt_sconv != NULL)
ret = ARCHIVE_OK;
else
ret = ARCHIVE_FATAL;
}
return (ret);
} else if (strcmp(key, "zip64") == 0) {






if (val != NULL && *val != '\0') {
zip->flags |= ZIP_FLAG_FORCE_ZIP64;
zip->flags &= ~ZIP_FLAG_AVOID_ZIP64;
} else {
zip->flags &= ~ZIP_FLAG_FORCE_ZIP64;
zip->flags |= ZIP_FLAG_AVOID_ZIP64;
}
return (ARCHIVE_OK);
}




return (ARCHIVE_WARN);
}

int
archive_write_zip_set_compression_deflate(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
int ret = ARCHIVE_FAILED;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW | ARCHIVE_STATE_HEADER | ARCHIVE_STATE_DATA,
"archive_write_zip_set_compression_deflate");
if (a->archive.archive_format != ARCHIVE_FORMAT_ZIP) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Can only use archive_write_zip_set_compression_deflate"
" with zip format");
ret = ARCHIVE_FATAL;
} else {
#if defined(HAVE_ZLIB_H)
struct zip *zip = a->format_data;
zip->requested_compression = COMPRESSION_DEFLATE;
ret = ARCHIVE_OK;
#else
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"deflate compression not supported");
ret = ARCHIVE_FAILED;
#endif
}
return (ret);
}

int
archive_write_zip_set_compression_store(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct zip *zip = a->format_data;
int ret = ARCHIVE_FAILED;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW | ARCHIVE_STATE_HEADER | ARCHIVE_STATE_DATA,
"archive_write_zip_set_compression_deflate");
if (a->archive.archive_format != ARCHIVE_FORMAT_ZIP) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Can only use archive_write_zip_set_compression_store"
" with zip format");
ret = ARCHIVE_FATAL;
} else {
zip->requested_compression = COMPRESSION_STORE;
ret = ARCHIVE_OK;
}
return (ret);
}

int
archive_write_set_format_zip(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct zip *zip;

archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_set_format_zip");


if (a->format_free != NULL)
(a->format_free)(a);

zip = (struct zip *) calloc(1, sizeof(*zip));
if (zip == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate zip data");
return (ARCHIVE_FATAL);
}


zip->requested_compression = COMPRESSION_UNSPECIFIED;
#if defined(HAVE_ZLIB_H)
zip->deflate_compression_level = Z_DEFAULT_COMPRESSION;
#endif
zip->crc32func = real_crc32;


zip->len_buf = 65536;
zip->buf = malloc(zip->len_buf);
if (zip->buf == NULL) {
free(zip);
archive_set_error(&a->archive, ENOMEM,
"Can't allocate compression buffer");
return (ARCHIVE_FATAL);
}

a->format_data = zip;
a->format_name = "zip";
a->format_options = archive_write_zip_options;
a->format_write_header = archive_write_zip_header;
a->format_write_data = archive_write_zip_data;
a->format_finish_entry = archive_write_zip_finish_entry;
a->format_close = archive_write_zip_close;
a->format_free = archive_write_zip_free;
a->archive.archive_format = ARCHIVE_FORMAT_ZIP;
a->archive.archive_format_name = "ZIP";

return (ARCHIVE_OK);
}

static int
is_all_ascii(const char *p)
{
const unsigned char *pp = (const unsigned char *)p;

while (*pp) {
if (*pp++ > 127)
return (0);
}
return (1);
}

static int
archive_write_zip_header(struct archive_write *a, struct archive_entry *entry)
{
unsigned char local_header[32];
unsigned char local_extra[144];
struct zip *zip = a->format_data;
unsigned char *e;
unsigned char *cd_extra;
size_t filename_length;
const char *slink = NULL;
size_t slink_size = 0;
struct archive_string_conv *sconv = get_sconv(a, zip);
int ret, ret2 = ARCHIVE_OK;
mode_t type;
int version_needed = 10;


type = archive_entry_filetype(entry);
if (type != AE_IFREG && type != AE_IFDIR && type != AE_IFLNK) {
__archive_write_entry_filetype_unsupported(
&a->archive, entry, "zip");
return ARCHIVE_FAILED;
};


if (zip->flags & ZIP_FLAG_AVOID_ZIP64) {

if (archive_entry_size_is_set(entry)
&& (archive_entry_size(entry) > ZIP_4GB_MAX)) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Files > 4GB require Zip64 extensions");
return ARCHIVE_FAILED;
}

if (zip->written_bytes > ZIP_4GB_MAX) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Archives > 4GB require Zip64 extensions");
return ARCHIVE_FAILED;
}
}


if (type != AE_IFREG)
archive_entry_set_size(entry, 0);



zip->entry_offset = zip->written_bytes;
zip->entry_uncompressed_limit = INT64_MAX;
zip->entry_compressed_size = 0;
zip->entry_uncompressed_size = 0;
zip->entry_compressed_written = 0;
zip->entry_uncompressed_written = 0;
zip->entry_flags = 0;
zip->entry_uses_zip64 = 0;
zip->entry_crc32 = zip->crc32func(0, NULL, 0);
zip->entry_encryption = 0;
archive_entry_free(zip->entry);
zip->entry = NULL;

if (zip->cctx_valid)
archive_encrypto_aes_ctr_release(&zip->cctx);
if (zip->hctx_valid)
archive_hmac_sha1_cleanup(&zip->hctx);
zip->tctx_valid = zip->cctx_valid = zip->hctx_valid = 0;

if (type == AE_IFREG
&&(!archive_entry_size_is_set(entry)
|| archive_entry_size(entry) > 0)) {
switch (zip->encryption_type) {
case ENCRYPTION_TRADITIONAL:
case ENCRYPTION_WINZIP_AES128:
case ENCRYPTION_WINZIP_AES256:
zip->entry_flags |= ZIP_ENTRY_FLAG_ENCRYPTED;
zip->entry_encryption = zip->encryption_type;
break;
case ENCRYPTION_NONE:
default:
break;
}
}


#if defined(_WIN32) && !defined(__CYGWIN__)


zip->entry = __la_win_entry_in_posix_pathseparator(entry);
if (zip->entry == entry)
zip->entry = archive_entry_clone(entry);
#else
zip->entry = archive_entry_clone(entry);
#endif
if (zip->entry == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate zip header data");
return (ARCHIVE_FATAL);
}

if (sconv != NULL) {
const char *p;
size_t len;

if (archive_entry_pathname_l(entry, &p, &len, sconv) != 0) {
if (errno == ENOMEM) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory for Pathname");
return (ARCHIVE_FATAL);
}
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Can't translate Pathname '%s' to %s",
archive_entry_pathname(entry),
archive_string_conversion_charset_name(sconv));
ret2 = ARCHIVE_WARN;
}
if (len > 0)
archive_entry_set_pathname(zip->entry, p);






if (type == AE_IFLNK) {
if (archive_entry_symlink_l(entry, &p, &len, sconv)) {
if (errno == ENOMEM) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory "
" for Symlink");
return (ARCHIVE_FATAL);
}

} else if (len > 0)
archive_entry_set_symlink(zip->entry, p);
}
}


if (!is_all_ascii(archive_entry_pathname(zip->entry))) {
if (zip->opt_sconv != NULL) {
if (strcmp(archive_string_conversion_charset_name(
zip->opt_sconv), "UTF-8") == 0)
zip->entry_flags |= ZIP_ENTRY_FLAG_UTF8_NAME;
#if HAVE_NL_LANGINFO
} else if (strcmp(nl_langinfo(CODESET), "UTF-8") == 0) {
zip->entry_flags |= ZIP_ENTRY_FLAG_UTF8_NAME;
#endif
}
}
filename_length = path_length(zip->entry);


if (type == AE_IFLNK) {
slink = archive_entry_symlink(zip->entry);
if (slink != NULL)
slink_size = strlen(slink);
else
slink_size = 0;
zip->entry_uncompressed_limit = slink_size;
zip->entry_compressed_size = slink_size;
zip->entry_uncompressed_size = slink_size;
zip->entry_crc32 = zip->crc32func(zip->entry_crc32,
(const unsigned char *)slink, slink_size);
zip->entry_compression = COMPRESSION_STORE;
version_needed = 20;
} else if (type != AE_IFREG) {
zip->entry_compression = COMPRESSION_STORE;
zip->entry_uncompressed_limit = 0;
version_needed = 20;
} else if (archive_entry_size_is_set(zip->entry)) {
int64_t size = archive_entry_size(zip->entry);
int64_t additional_size = 0;

zip->entry_uncompressed_limit = size;
zip->entry_compression = zip->requested_compression;
if (zip->entry_compression == COMPRESSION_UNSPECIFIED) {
zip->entry_compression = COMPRESSION_DEFAULT;
}
if (zip->entry_compression == COMPRESSION_STORE) {
zip->entry_compressed_size = size;
zip->entry_uncompressed_size = size;
version_needed = 10;
} else {
zip->entry_uncompressed_size = size;
version_needed = 20;
}

if (zip->entry_flags & ZIP_ENTRY_FLAG_ENCRYPTED) {
switch (zip->entry_encryption) {
case ENCRYPTION_TRADITIONAL:
additional_size = TRAD_HEADER_SIZE;
version_needed = 20;
break;
case ENCRYPTION_WINZIP_AES128:
additional_size = WINZIP_AES128_HEADER_SIZE
+ AUTH_CODE_SIZE;
version_needed = 20;
break;
case ENCRYPTION_WINZIP_AES256:
additional_size = WINZIP_AES256_HEADER_SIZE
+ AUTH_CODE_SIZE;
version_needed = 20;
break;
case ENCRYPTION_NONE:
default:
break;
}
if (zip->entry_compression == COMPRESSION_STORE)
zip->entry_compressed_size += additional_size;
}











if ((zip->flags & ZIP_FLAG_FORCE_ZIP64)
|| (zip->entry_uncompressed_size + additional_size > ZIP_4GB_MAX)
|| (zip->entry_uncompressed_size > ZIP_4GB_MAX_UNCOMPRESSED
&& zip->entry_compression != COMPRESSION_STORE)) {
zip->entry_uses_zip64 = 1;
version_needed = 45;
}


zip->entry_flags |= ZIP_ENTRY_FLAG_LENGTH_AT_END;
} else {





zip->entry_compression = COMPRESSION_DEFAULT;
zip->entry_flags |= ZIP_ENTRY_FLAG_LENGTH_AT_END;
if ((zip->flags & ZIP_FLAG_AVOID_ZIP64) == 0) {
zip->entry_uses_zip64 = 1;
version_needed = 45;
} else if (zip->entry_compression == COMPRESSION_STORE) {
version_needed = 10;
} else {
version_needed = 20;
}

if (zip->entry_flags & ZIP_ENTRY_FLAG_ENCRYPTED) {
switch (zip->entry_encryption) {
case ENCRYPTION_TRADITIONAL:
case ENCRYPTION_WINZIP_AES128:
case ENCRYPTION_WINZIP_AES256:
if (version_needed < 20)
version_needed = 20;
break;
case ENCRYPTION_NONE:
default:
break;
}
}
}


memset(local_header, 0, sizeof(local_header));
memcpy(local_header, "PK\003\004", 4);
archive_le16enc(local_header + 4, version_needed);
archive_le16enc(local_header + 6, zip->entry_flags);
if (zip->entry_encryption == ENCRYPTION_WINZIP_AES128
|| zip->entry_encryption == ENCRYPTION_WINZIP_AES256)
archive_le16enc(local_header + 8, WINZIP_AES_ENCRYPTION);
else
archive_le16enc(local_header + 8, zip->entry_compression);
archive_le32enc(local_header + 10,
dos_time(archive_entry_mtime(zip->entry)));
archive_le32enc(local_header + 14, zip->entry_crc32);
if (zip->entry_uses_zip64) {





archive_le32enc(local_header + 18, ZIP_4GB_MAX);
archive_le32enc(local_header + 22, ZIP_4GB_MAX);
} else {
archive_le32enc(local_header + 18, (uint32_t)zip->entry_compressed_size);
archive_le32enc(local_header + 22, (uint32_t)zip->entry_uncompressed_size);
}
archive_le16enc(local_header + 26, (uint16_t)filename_length);

if (zip->entry_encryption == ENCRYPTION_TRADITIONAL) {
if (zip->entry_flags & ZIP_ENTRY_FLAG_LENGTH_AT_END)
zip->trad_chkdat = local_header[11];
else
zip->trad_chkdat = local_header[17];
}


zip->file_header = cd_alloc(zip, 46);

++zip->central_directory_entries;
memset(zip->file_header, 0, 46);
memcpy(zip->file_header, "PK\001\002", 4);

archive_le16enc(zip->file_header + 4, 3 * 256 + version_needed);
archive_le16enc(zip->file_header + 6, version_needed);
archive_le16enc(zip->file_header + 8, zip->entry_flags);
if (zip->entry_encryption == ENCRYPTION_WINZIP_AES128
|| zip->entry_encryption == ENCRYPTION_WINZIP_AES256)
archive_le16enc(zip->file_header + 10, WINZIP_AES_ENCRYPTION);
else
archive_le16enc(zip->file_header + 10, zip->entry_compression);
archive_le32enc(zip->file_header + 12,
dos_time(archive_entry_mtime(zip->entry)));
archive_le16enc(zip->file_header + 28, (uint16_t)filename_length);

archive_le32enc(zip->file_header + 38,
((uint32_t)archive_entry_mode(zip->entry)) << 16);
e = cd_alloc(zip, filename_length);

copy_path(zip->entry, e);


memset(local_extra, 0, sizeof(local_extra));
e = local_extra;






memcpy(e, "UT", 2);
archive_le16enc(e + 2,
1
+ (archive_entry_mtime_is_set(entry) ? 4 : 0)
+ (archive_entry_atime_is_set(entry) ? 4 : 0)
+ (archive_entry_ctime_is_set(entry) ? 4 : 0));
e += 4;
*e++ =
(archive_entry_mtime_is_set(entry) ? 1 : 0)
| (archive_entry_atime_is_set(entry) ? 2 : 0)
| (archive_entry_ctime_is_set(entry) ? 4 : 0);
if (archive_entry_mtime_is_set(entry)) {
archive_le32enc(e, (uint32_t)archive_entry_mtime(entry));
e += 4;
}
if (archive_entry_atime_is_set(entry)) {
archive_le32enc(e, (uint32_t)archive_entry_atime(entry));
e += 4;
}
if (archive_entry_ctime_is_set(entry)) {
archive_le32enc(e, (uint32_t)archive_entry_ctime(entry));
e += 4;
}



memcpy(e, "ux\013\000\001", 5);
e += 5;
*e++ = 4;
archive_le32enc(e, (uint32_t)archive_entry_uid(entry));
e += 4;
*e++ = 4;
archive_le32enc(e, (uint32_t)archive_entry_gid(entry));
e += 4;


if ((zip->entry_flags & ZIP_ENTRY_FLAG_ENCRYPTED)
&& (zip->entry_encryption == ENCRYPTION_WINZIP_AES128
|| zip->entry_encryption == ENCRYPTION_WINZIP_AES256)) {

memcpy(e, "\001\231\007\000\001\000AE", 8);






if (archive_entry_size_is_set(zip->entry)
&& archive_entry_size(zip->entry) < 20) {
archive_le16enc(e+4, AES_VENDOR_AE_2);
zip->aes_vendor = AES_VENDOR_AE_2;
} else
zip->aes_vendor = AES_VENDOR_AE_1;
e += 8;

*e++ = (zip->entry_encryption == ENCRYPTION_WINZIP_AES128)?1:3;

archive_le16enc(e, zip->entry_compression);
e += 2;
}


zip->file_header_extra_offset = zip->central_directory_bytes;
cd_extra = cd_alloc(zip, e - local_extra);
memcpy(cd_extra, local_extra, e - local_extra);










if (zip->entry_uses_zip64) {
unsigned char *zip64_start = e;
memcpy(e, "\001\000\020\000", 4);
e += 4;
archive_le64enc(e, zip->entry_uncompressed_size);
e += 8;
archive_le64enc(e, zip->entry_compressed_size);
e += 8;
archive_le16enc(zip64_start + 2, (uint16_t)(e - (zip64_start + 4)));
}

if (zip->flags & ZIP_FLAG_EXPERIMENT_xl) {

unsigned char *external_info = e;
int included = 7;
memcpy(e, "xl\000\000", 4);
e += 4;
e[0] = included;
e += 1;
if (included & 1) {
archive_le16enc(e,
3 * 256 + version_needed);
e += 2;
}
if (included & 2) {
archive_le16enc(e, 0);
e += 2;
}
if (included & 4) {
archive_le32enc(e,
((uint32_t)archive_entry_mode(zip->entry)) << 16);
e += 4;
}
if (included & 8) {

}
archive_le16enc(external_info + 2, (uint16_t)(e - (external_info + 4)));
}


archive_le16enc(local_header + 28, (uint16_t)(e - local_extra));

ret = __archive_write_output(a, local_header, 30);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->written_bytes += 30;

ret = write_path(zip->entry, a);
if (ret <= ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->written_bytes += ret;

ret = __archive_write_output(a, local_extra, e - local_extra);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->written_bytes += e - local_extra;


if (slink != NULL) {
ret = __archive_write_output(a, slink, slink_size);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->entry_compressed_written += slink_size;
zip->entry_uncompressed_written += slink_size;
zip->written_bytes += slink_size;
}

#if defined(HAVE_ZLIB_H)
if (zip->entry_compression == COMPRESSION_DEFLATE) {
zip->stream.zalloc = Z_NULL;
zip->stream.zfree = Z_NULL;
zip->stream.opaque = Z_NULL;
zip->stream.next_out = zip->buf;
zip->stream.avail_out = (uInt)zip->len_buf;
if (deflateInit2(&zip->stream, zip->deflate_compression_level,
Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
archive_set_error(&a->archive, ENOMEM,
"Can't init deflate compressor");
return (ARCHIVE_FATAL);
}
}
#endif

return (ret2);
}

static ssize_t
archive_write_zip_data(struct archive_write *a, const void *buff, size_t s)
{
int ret;
struct zip *zip = a->format_data;

if ((int64_t)s > zip->entry_uncompressed_limit)
s = (size_t)zip->entry_uncompressed_limit;
zip->entry_uncompressed_written += s;

if (s == 0) return 0;

if (zip->entry_flags & ZIP_ENTRY_FLAG_ENCRYPTED) {
switch (zip->entry_encryption) {
case ENCRYPTION_TRADITIONAL:

if (!zip->tctx_valid) {
ret = init_traditional_pkware_encryption(a);
if (ret != ARCHIVE_OK)
return (ret);
zip->tctx_valid = 1;
}
break;
case ENCRYPTION_WINZIP_AES128:
case ENCRYPTION_WINZIP_AES256:
if (!zip->cctx_valid) {
ret = init_winzip_aes_encryption(a);
if (ret != ARCHIVE_OK)
return (ret);
zip->cctx_valid = zip->hctx_valid = 1;
}
break;
case ENCRYPTION_NONE:
default:
break;
}
}

switch (zip->entry_compression) {
case COMPRESSION_STORE:
if (zip->tctx_valid || zip->cctx_valid) {
const uint8_t *rb = (const uint8_t *)buff;
const uint8_t * const re = rb + s;

while (rb < re) {
size_t l;

if (zip->tctx_valid) {
l = trad_enc_encrypt_update(&zip->tctx,
rb, re - rb,
zip->buf, zip->len_buf);
} else {
l = zip->len_buf;
ret = archive_encrypto_aes_ctr_update(
&zip->cctx,
rb, re - rb, zip->buf, &l);
if (ret < 0) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"Failed to encrypt file");
return (ARCHIVE_FAILED);
}
archive_hmac_sha1_update(&zip->hctx,
zip->buf, l);
}
ret = __archive_write_output(a, zip->buf, l);
if (ret != ARCHIVE_OK)
return (ret);
zip->entry_compressed_written += l;
zip->written_bytes += l;
rb += l;
}
} else {
ret = __archive_write_output(a, buff, s);
if (ret != ARCHIVE_OK)
return (ret);
zip->written_bytes += s;
zip->entry_compressed_written += s;
}
break;
#if HAVE_ZLIB_H
case COMPRESSION_DEFLATE:
zip->stream.next_in = (unsigned char*)(uintptr_t)buff;
zip->stream.avail_in = (uInt)s;
do {
ret = deflate(&zip->stream, Z_NO_FLUSH);
if (ret == Z_STREAM_ERROR)
return (ARCHIVE_FATAL);
if (zip->stream.avail_out == 0) {
if (zip->tctx_valid) {
trad_enc_encrypt_update(&zip->tctx,
zip->buf, zip->len_buf,
zip->buf, zip->len_buf);
} else if (zip->cctx_valid) {
size_t outl = zip->len_buf;
ret = archive_encrypto_aes_ctr_update(
&zip->cctx,
zip->buf, zip->len_buf,
zip->buf, &outl);
if (ret < 0) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"Failed to encrypt file");
return (ARCHIVE_FAILED);
}
archive_hmac_sha1_update(&zip->hctx,
zip->buf, zip->len_buf);
}
ret = __archive_write_output(a, zip->buf,
zip->len_buf);
if (ret != ARCHIVE_OK)
return (ret);
zip->entry_compressed_written += zip->len_buf;
zip->written_bytes += zip->len_buf;
zip->stream.next_out = zip->buf;
zip->stream.avail_out = (uInt)zip->len_buf;
}
} while (zip->stream.avail_in != 0);
break;
#endif

case COMPRESSION_UNSPECIFIED:
default:
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Invalid ZIP compression type");
return ARCHIVE_FATAL;
}

zip->entry_uncompressed_limit -= s;
if (!zip->cctx_valid || zip->aes_vendor != AES_VENDOR_AE_2)
zip->entry_crc32 =
zip->crc32func(zip->entry_crc32, buff, (unsigned)s);
return (s);

}

static int
archive_write_zip_finish_entry(struct archive_write *a)
{
struct zip *zip = a->format_data;
int ret;

#if HAVE_ZLIB_H
if (zip->entry_compression == COMPRESSION_DEFLATE) {
for (;;) {
size_t remainder;

ret = deflate(&zip->stream, Z_FINISH);
if (ret == Z_STREAM_ERROR)
return (ARCHIVE_FATAL);
remainder = zip->len_buf - zip->stream.avail_out;
if (zip->tctx_valid) {
trad_enc_encrypt_update(&zip->tctx,
zip->buf, remainder, zip->buf, remainder);
} else if (zip->cctx_valid) {
size_t outl = remainder;
ret = archive_encrypto_aes_ctr_update(
&zip->cctx, zip->buf, remainder,
zip->buf, &outl);
if (ret < 0) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_MISC,
"Failed to encrypt file");
return (ARCHIVE_FAILED);
}
archive_hmac_sha1_update(&zip->hctx,
zip->buf, remainder);
}
ret = __archive_write_output(a, zip->buf, remainder);
if (ret != ARCHIVE_OK)
return (ret);
zip->entry_compressed_written += remainder;
zip->written_bytes += remainder;
zip->stream.next_out = zip->buf;
if (zip->stream.avail_out != 0)
break;
zip->stream.avail_out = (uInt)zip->len_buf;
}
deflateEnd(&zip->stream);
}
#endif
if (zip->hctx_valid) {
uint8_t hmac[20];
size_t hmac_len = 20;

archive_hmac_sha1_final(&zip->hctx, hmac, &hmac_len);
ret = __archive_write_output(a, hmac, AUTH_CODE_SIZE);
if (ret != ARCHIVE_OK)
return (ret);
zip->entry_compressed_written += AUTH_CODE_SIZE;
zip->written_bytes += AUTH_CODE_SIZE;
}


if ((zip->entry_flags & ZIP_ENTRY_FLAG_LENGTH_AT_END) != 0) {
char d[24];
memcpy(d, "PK\007\010", 4);
if (zip->cctx_valid && zip->aes_vendor == AES_VENDOR_AE_2)
archive_le32enc(d + 4, 0);
else
archive_le32enc(d + 4, zip->entry_crc32);
if (zip->entry_uses_zip64) {
archive_le64enc(d + 8,
(uint64_t)zip->entry_compressed_written);
archive_le64enc(d + 16,
(uint64_t)zip->entry_uncompressed_written);
ret = __archive_write_output(a, d, 24);
zip->written_bytes += 24;
} else {
archive_le32enc(d + 8,
(uint32_t)zip->entry_compressed_written);
archive_le32enc(d + 12,
(uint32_t)zip->entry_uncompressed_written);
ret = __archive_write_output(a, d, 16);
zip->written_bytes += 16;
}
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
}


if (zip->entry_compressed_written > ZIP_4GB_MAX
|| zip->entry_uncompressed_written > ZIP_4GB_MAX
|| zip->entry_offset > ZIP_4GB_MAX) {
unsigned char zip64[32];
unsigned char *z = zip64, *zd;
memcpy(z, "\001\000\000\000", 4);
z += 4;
if (zip->entry_uncompressed_written >= ZIP_4GB_MAX) {
archive_le64enc(z, zip->entry_uncompressed_written);
z += 8;
}
if (zip->entry_compressed_written >= ZIP_4GB_MAX) {
archive_le64enc(z, zip->entry_compressed_written);
z += 8;
}
if (zip->entry_offset >= ZIP_4GB_MAX) {
archive_le64enc(z, zip->entry_offset);
z += 8;
}
archive_le16enc(zip64 + 2, (uint16_t)(z - (zip64 + 4)));
zd = cd_alloc(zip, z - zip64);
if (zd == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate zip data");
return (ARCHIVE_FATAL);
}
memcpy(zd, zip64, z - zip64);

if (archive_le16dec(zip->file_header + 6) < 45)
archive_le16enc(zip->file_header + 6, 45);
}


if (zip->cctx_valid && zip->aes_vendor == AES_VENDOR_AE_2)
archive_le32enc(zip->file_header + 16, 0);
else
archive_le32enc(zip->file_header + 16, zip->entry_crc32);
archive_le32enc(zip->file_header + 20,
(uint32_t)zipmin(zip->entry_compressed_written,
ZIP_4GB_MAX));
archive_le32enc(zip->file_header + 24,
(uint32_t)zipmin(zip->entry_uncompressed_written,
ZIP_4GB_MAX));
archive_le16enc(zip->file_header + 30,
(uint16_t)(zip->central_directory_bytes - zip->file_header_extra_offset));
archive_le32enc(zip->file_header + 42,
(uint32_t)zipmin(zip->entry_offset,
ZIP_4GB_MAX));

return (ARCHIVE_OK);
}

static int
archive_write_zip_close(struct archive_write *a)
{
uint8_t buff[64];
int64_t offset_start, offset_end;
struct zip *zip = a->format_data;
struct cd_segment *segment;
int ret;

offset_start = zip->written_bytes;
segment = zip->central_directory;
while (segment != NULL) {
ret = __archive_write_output(a,
segment->buff, segment->p - segment->buff);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->written_bytes += segment->p - segment->buff;
segment = segment->next;
}
offset_end = zip->written_bytes;


if (offset_end - offset_start > ZIP_4GB_MAX
|| offset_start > ZIP_4GB_MAX
|| zip->central_directory_entries > 0xffffUL
|| (zip->flags & ZIP_FLAG_FORCE_ZIP64)) {

memset(buff, 0, 56);
memcpy(buff, "PK\006\006", 4);
archive_le64enc(buff + 4, 44);
archive_le16enc(buff + 12, 45);
archive_le16enc(buff + 14, 45);

archive_le64enc(buff + 24, zip->central_directory_entries);
archive_le64enc(buff + 32, zip->central_directory_entries);
archive_le64enc(buff + 40, offset_end - offset_start);
archive_le64enc(buff + 48, offset_start);
ret = __archive_write_output(a, buff, 56);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->written_bytes += 56;


memset(buff, 0, 20);
memcpy(buff, "PK\006\007", 4);
archive_le32enc(buff + 4, 0);
archive_le64enc(buff + 8, offset_end);
archive_le32enc(buff + 16, 1);
ret = __archive_write_output(a, buff, 20);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->written_bytes += 20;

}


memset(buff, 0, sizeof(buff));
memcpy(buff, "PK\005\006", 4);
archive_le16enc(buff + 8, (uint16_t)zipmin(0xffffU,
zip->central_directory_entries));
archive_le16enc(buff + 10, (uint16_t)zipmin(0xffffU,
zip->central_directory_entries));
archive_le32enc(buff + 12,
(uint32_t)zipmin(ZIP_4GB_MAX, (offset_end - offset_start)));
archive_le32enc(buff + 16,
(uint32_t)zipmin(ZIP_4GB_MAX, offset_start));
ret = __archive_write_output(a, buff, 22);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
zip->written_bytes += 22;
return (ARCHIVE_OK);
}

static int
archive_write_zip_free(struct archive_write *a)
{
struct zip *zip;
struct cd_segment *segment;

zip = a->format_data;
while (zip->central_directory != NULL) {
segment = zip->central_directory;
zip->central_directory = segment->next;
free(segment->buff);
free(segment);
}
free(zip->buf);
archive_entry_free(zip->entry);
if (zip->cctx_valid)
archive_encrypto_aes_ctr_release(&zip->cctx);
if (zip->hctx_valid)
archive_hmac_sha1_cleanup(&zip->hctx);


free(zip);
a->format_data = NULL;
return (ARCHIVE_OK);
}


static unsigned int
dos_time(const time_t unix_time)
{
struct tm *t;
unsigned int dt;
#if defined(HAVE_LOCALTIME_R) || defined(HAVE__LOCALTIME64_S)
struct tm tmbuf;
#endif
#if defined(HAVE__LOCALTIME64_S)
errno_t terr;
__time64_t tmptime;
#endif



#if defined(HAVE_LOCALTIME_R)
t = localtime_r(&unix_time, &tmbuf);
#elif defined(HAVE__LOCALTIME64_S)
tmptime = unix_time;
terr = _localtime64_s(&tmbuf, &tmptime);
if (terr)
t = NULL;
else
t = &tmbuf;
#else
t = localtime(&unix_time);
#endif


if (t->tm_year < 1980 - 1900)

dt = 0x00210000U;
else if (t->tm_year > 2107 - 1900)

dt = 0xff9fbf7dU;
else {
dt = 0;
dt += ((t->tm_year - 80) & 0x7f) << 9;
dt += ((t->tm_mon + 1) & 0x0f) << 5;
dt += (t->tm_mday & 0x1f);
dt <<= 16;
dt += (t->tm_hour & 0x1f) << 11;
dt += (t->tm_min & 0x3f) << 5;
dt += (t->tm_sec & 0x3e) >> 1;
}
return dt;
}

static size_t
path_length(struct archive_entry *entry)
{
mode_t type;
const char *path;
size_t len;

type = archive_entry_filetype(entry);
path = archive_entry_pathname(entry);

if (path == NULL)
return (0);
len = strlen(path);
if (type == AE_IFDIR && (path[0] == '\0' || path[len - 1] != '/'))
++len;
return len;
}

static int
write_path(struct archive_entry *entry, struct archive_write *archive)
{
int ret;
const char *path;
mode_t type;
size_t written_bytes;

path = archive_entry_pathname(entry);
type = archive_entry_filetype(entry);
written_bytes = 0;

if (path == NULL)
return (ARCHIVE_FATAL);

ret = __archive_write_output(archive, path, strlen(path));
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
written_bytes += strlen(path);


if ((type == AE_IFDIR) & (path[strlen(path) - 1] != '/')) {
ret = __archive_write_output(archive, "/", 1);
if (ret != ARCHIVE_OK)
return (ARCHIVE_FATAL);
written_bytes += 1;
}

return ((int)written_bytes);
}

static void
copy_path(struct archive_entry *entry, unsigned char *p)
{
const char *path;
size_t pathlen;
mode_t type;

path = archive_entry_pathname(entry);
pathlen = strlen(path);
type = archive_entry_filetype(entry);

memcpy(p, path, pathlen);


if ((type == AE_IFDIR) && (path[pathlen - 1] != '/'))
p[pathlen] = '/';
}


static struct archive_string_conv *
get_sconv(struct archive_write *a, struct zip *zip)
{
if (zip->opt_sconv != NULL)
return (zip->opt_sconv);

if (!zip->init_default_conversion) {
zip->sconv_default =
archive_string_default_conversion_for_write(&(a->archive));
zip->init_default_conversion = 1;
}
return (zip->sconv_default);
}





static void
trad_enc_update_keys(struct trad_enc_ctx *ctx, uint8_t c)
{
uint8_t t;
#define CRC32(c, b) (crc32(c ^ 0xffffffffUL, &b, 1) ^ 0xffffffffUL)

ctx->keys[0] = CRC32(ctx->keys[0], c);
ctx->keys[1] = (ctx->keys[1] + (ctx->keys[0] & 0xff)) * 134775813L + 1;
t = (ctx->keys[1] >> 24) & 0xff;
ctx->keys[2] = CRC32(ctx->keys[2], t);
#undef CRC32
}

static uint8_t
trad_enc_decrypt_byte(struct trad_enc_ctx *ctx)
{
unsigned temp = ctx->keys[2] | 2;
return (uint8_t)((temp * (temp ^ 1)) >> 8) & 0xff;
}

static unsigned
trad_enc_encrypt_update(struct trad_enc_ctx *ctx, const uint8_t *in,
size_t in_len, uint8_t *out, size_t out_len)
{
unsigned i, max;

max = (unsigned)((in_len < out_len)? in_len: out_len);

for (i = 0; i < max; i++) {
uint8_t t = in[i];
out[i] = t ^ trad_enc_decrypt_byte(ctx);
trad_enc_update_keys(ctx, t);
}
return i;
}

static int
trad_enc_init(struct trad_enc_ctx *ctx, const char *pw, size_t pw_len)
{

ctx->keys[0] = 305419896L;
ctx->keys[1] = 591751049L;
ctx->keys[2] = 878082192L;

for (;pw_len; --pw_len)
trad_enc_update_keys(ctx, *pw++);
return 0;
}

static int
is_traditional_pkware_encryption_supported(void)
{
uint8_t key[TRAD_HEADER_SIZE];

if (archive_random(key, sizeof(key)-1) != ARCHIVE_OK)
return (0);
return (1);
}

static int
init_traditional_pkware_encryption(struct archive_write *a)
{
struct zip *zip = a->format_data;
const char *passphrase;
uint8_t key[TRAD_HEADER_SIZE];
uint8_t key_encrypted[TRAD_HEADER_SIZE];
int ret;

passphrase = __archive_write_get_passphrase(a);
if (passphrase == NULL) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Encryption needs passphrase");
return ARCHIVE_FAILED;
}
if (archive_random(key, sizeof(key)-1) != ARCHIVE_OK) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Can't generate random number for encryption");
return ARCHIVE_FATAL;
}
trad_enc_init(&zip->tctx, passphrase, strlen(passphrase));


key[TRAD_HEADER_SIZE-1] = zip->trad_chkdat;
trad_enc_encrypt_update(&zip->tctx, key, TRAD_HEADER_SIZE,
key_encrypted, TRAD_HEADER_SIZE);

ret = __archive_write_output(a, key_encrypted, TRAD_HEADER_SIZE);
if (ret != ARCHIVE_OK)
return (ret);
zip->written_bytes += TRAD_HEADER_SIZE;
zip->entry_compressed_written += TRAD_HEADER_SIZE;
return (ret);
}

static int
init_winzip_aes_encryption(struct archive_write *a)
{
struct zip *zip = a->format_data;
const char *passphrase;
size_t key_len, salt_len;
uint8_t salt[16 + 2];
uint8_t derived_key[MAX_DERIVED_KEY_BUF_SIZE];
int ret;

passphrase = __archive_write_get_passphrase(a);
if (passphrase == NULL) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Encryption needs passphrase");
return (ARCHIVE_FAILED);
}
if (zip->entry_encryption == ENCRYPTION_WINZIP_AES128) {
salt_len = 8;
key_len = 16;
} else {

salt_len = 16;
key_len = 32;
}
if (archive_random(salt, salt_len) != ARCHIVE_OK) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Can't generate random number for encryption");
return (ARCHIVE_FATAL);
}
archive_pbkdf2_sha1(passphrase, strlen(passphrase),
salt, salt_len, 1000, derived_key, key_len * 2 + 2);

ret = archive_encrypto_aes_ctr_init(&zip->cctx, derived_key, key_len);
if (ret != 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Decryption is unsupported due to lack of crypto library");
return (ARCHIVE_FAILED);
}
ret = archive_hmac_sha1_init(&zip->hctx, derived_key + key_len,
key_len);
if (ret != 0) {
archive_encrypto_aes_ctr_release(&zip->cctx);
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Failed to initialize HMAC-SHA1");
return (ARCHIVE_FAILED);
}


salt[salt_len] = derived_key[key_len * 2];
salt[salt_len + 1] = derived_key[key_len * 2 + 1];


ret = __archive_write_output(a, salt, salt_len + 2);
if (ret != ARCHIVE_OK)
return (ret);
zip->written_bytes += salt_len + 2;
zip->entry_compressed_written += salt_len + 2;

return (ARCHIVE_OK);
}

static int
is_winzip_aes_encryption_supported(int encryption)
{
size_t key_len, salt_len;
uint8_t salt[16 + 2];
uint8_t derived_key[MAX_DERIVED_KEY_BUF_SIZE];
archive_crypto_ctx cctx;
archive_hmac_sha1_ctx hctx;
int ret;

if (encryption == ENCRYPTION_WINZIP_AES128) {
salt_len = 8;
key_len = 16;
} else {

salt_len = 16;
key_len = 32;
}
if (archive_random(salt, salt_len) != ARCHIVE_OK)
return (0);
ret = archive_pbkdf2_sha1("p", 1, salt, salt_len, 1000,
derived_key, key_len * 2 + 2);
if (ret != 0)
return (0);

ret = archive_encrypto_aes_ctr_init(&cctx, derived_key, key_len);
if (ret != 0)
return (0);
ret = archive_hmac_sha1_init(&hctx, derived_key + key_len,
key_len);
archive_encrypto_aes_ctr_release(&cctx);
if (ret != 0)
return (0);
archive_hmac_sha1_cleanup(&hctx);
return (1);
}
