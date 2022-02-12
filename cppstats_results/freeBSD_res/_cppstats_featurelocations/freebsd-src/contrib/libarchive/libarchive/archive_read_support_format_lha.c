
























#include "archive_platform.h"

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_entry_locale.h"
#include "archive_private.h"
#include "archive_read_private.h"
#include "archive_endian.h"


#define MAXMATCH 256
#define MINMATCH 3










#define LT_BITLEN_SIZE (UCHAR_MAX + 1 + MAXMATCH - MINMATCH + 1)


#define PT_BITLEN_SIZE (3 + 16)

struct lzh_dec {

int state;





int w_size;
int w_mask;

unsigned char *w_buff;

int w_pos;

int copy_pos;


int copy_len;




struct lzh_br {
#define CACHE_TYPE uint64_t
#define CACHE_BITS (8 * sizeof(CACHE_TYPE))

CACHE_TYPE cache_buffer;

int cache_avail;
} br;




struct huffman {
int len_size;
int len_avail;
int len_bits;
int freq[17];
unsigned char *bitlen;






#define HTBL_BITS 10
int max_bits;
int shift_bits;
int tbl_bits;
int tree_used;
int tree_avail;

uint16_t *tbl;

struct htree_t {
uint16_t left;
uint16_t right;
} *tree;
} lt, pt;

int blocks_avail;
int pos_pt_len_size;
int pos_pt_len_bits;
int literal_pt_len_size;
int literal_pt_len_bits;
int reading_position;
int loop;
int error;
};

struct lzh_stream {
const unsigned char *next_in;
int avail_in;
int64_t total_in;
const unsigned char *ref_ptr;
int avail_out;
int64_t total_out;
struct lzh_dec *ds;
};

struct lha {

int64_t entry_offset;
int64_t entry_bytes_remaining;
int64_t entry_unconsumed;
uint16_t entry_crc_calculated;

size_t header_size;
unsigned char level;
char method[3];
int64_t compsize;
int64_t origsize;
int setflag;
#define BIRTHTIME_IS_SET 1
#define ATIME_IS_SET 2
#define UNIX_MODE_IS_SET 4
#define CRC_IS_SET 8
time_t birthtime;
long birthtime_tv_nsec;
time_t mtime;
long mtime_tv_nsec;
time_t atime;
long atime_tv_nsec;
mode_t mode;
int64_t uid;
int64_t gid;
struct archive_string uname;
struct archive_string gname;
uint16_t header_crc;
uint16_t crc;

struct archive_string_conv *sconv_dir;
struct archive_string_conv *sconv_fname;
struct archive_string_conv *opt_sconv;

struct archive_string dirname;
struct archive_string filename;
struct archive_wstring ws;

unsigned char dos_attr;


char found_first_header;

char directory;


char decompress_init;
char end_of_entry;
char end_of_entry_cleanup;
char entry_is_compressed;

char format_name[64];

struct lzh_stream strm;
};




#define H_METHOD_OFFSET 2
#define H_ATTR_OFFSET 19
#define H_LEVEL_OFFSET 20
#define H_SIZE 22

static int archive_read_format_lha_bid(struct archive_read *, int);
static int archive_read_format_lha_options(struct archive_read *,
const char *, const char *);
static int archive_read_format_lha_read_header(struct archive_read *,
struct archive_entry *);
static int archive_read_format_lha_read_data(struct archive_read *,
const void **, size_t *, int64_t *);
static int archive_read_format_lha_read_data_skip(struct archive_read *);
static int archive_read_format_lha_cleanup(struct archive_read *);

static void lha_replace_path_separator(struct lha *,
struct archive_entry *);
static int lha_read_file_header_0(struct archive_read *, struct lha *);
static int lha_read_file_header_1(struct archive_read *, struct lha *);
static int lha_read_file_header_2(struct archive_read *, struct lha *);
static int lha_read_file_header_3(struct archive_read *, struct lha *);
static int lha_read_file_extended_header(struct archive_read *,
struct lha *, uint16_t *, int, size_t, size_t *);
static size_t lha_check_header_format(const void *);
static int lha_skip_sfx(struct archive_read *);
static time_t lha_dos_time(const unsigned char *);
static time_t lha_win_time(uint64_t, long *);
static unsigned char lha_calcsum(unsigned char, const void *,
int, size_t);
static int lha_parse_linkname(struct archive_wstring *,
struct archive_wstring *);
static int lha_read_data_none(struct archive_read *, const void **,
size_t *, int64_t *);
static int lha_read_data_lzh(struct archive_read *, const void **,
size_t *, int64_t *);
static void lha_crc16_init(void);
static uint16_t lha_crc16(uint16_t, const void *, size_t);
static int lzh_decode_init(struct lzh_stream *, const char *);
static void lzh_decode_free(struct lzh_stream *);
static int lzh_decode(struct lzh_stream *, int);
static int lzh_br_fillup(struct lzh_stream *, struct lzh_br *);
static int lzh_huffman_init(struct huffman *, size_t, int);
static void lzh_huffman_free(struct huffman *);
static int lzh_read_pt_bitlen(struct lzh_stream *, int start, int end);
static int lzh_make_fake_table(struct huffman *, uint16_t);
static int lzh_make_huffman_table(struct huffman *);
static inline int lzh_decode_huffman(struct huffman *, unsigned);
static int lzh_decode_huffman_tree(struct huffman *, unsigned, int);


int
archive_read_support_format_lha(struct archive *_a)
{
struct archive_read *a = (struct archive_read *)_a;
struct lha *lha;
int r;

archive_check_magic(_a, ARCHIVE_READ_MAGIC,
ARCHIVE_STATE_NEW, "archive_read_support_format_lha");

lha = (struct lha *)calloc(1, sizeof(*lha));
if (lha == NULL) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate lha data");
return (ARCHIVE_FATAL);
}
archive_string_init(&lha->ws);

r = __archive_read_register_format(a,
lha,
"lha",
archive_read_format_lha_bid,
archive_read_format_lha_options,
archive_read_format_lha_read_header,
archive_read_format_lha_read_data,
archive_read_format_lha_read_data_skip,
NULL,
archive_read_format_lha_cleanup,
NULL,
NULL);

if (r != ARCHIVE_OK)
free(lha);
return (ARCHIVE_OK);
}

static size_t
lha_check_header_format(const void *h)
{
const unsigned char *p = h;
size_t next_skip_bytes;

switch (p[H_METHOD_OFFSET+3]) {




case '0': case '1': case '2': case '3':
case '4': case '5': case '6': case '7':
case 'd':
case 's':
next_skip_bytes = 4;


if (p[0] == 0)
break;
if (p[H_METHOD_OFFSET] != '-' || p[H_METHOD_OFFSET+1] != 'l'
|| p[H_METHOD_OFFSET+4] != '-')
break;

if (p[H_METHOD_OFFSET+2] == 'h') {

if (p[H_METHOD_OFFSET+3] == 's')
break;
if (p[H_LEVEL_OFFSET] == 0)
return (0);
if (p[H_LEVEL_OFFSET] <= 3 && p[H_ATTR_OFFSET] == 0x20)
return (0);
}
if (p[H_METHOD_OFFSET+2] == 'z') {

if (p[H_LEVEL_OFFSET] != 0)
break;
if (p[H_METHOD_OFFSET+3] == 's'
|| p[H_METHOD_OFFSET+3] == '4'
|| p[H_METHOD_OFFSET+3] == '5')
return (0);
}
break;
case 'h': next_skip_bytes = 1; break;
case 'z': next_skip_bytes = 1; break;
case 'l': next_skip_bytes = 2; break;
case '-': next_skip_bytes = 3; break;
default : next_skip_bytes = 4; break;
}

return (next_skip_bytes);
}

static int
archive_read_format_lha_bid(struct archive_read *a, int best_bid)
{
const char *p;
const void *buff;
ssize_t bytes_avail, offset, window;
size_t next;



if (best_bid > 30)
return (-1);

if ((p = __archive_read_ahead(a, H_SIZE, NULL)) == NULL)
return (-1);

if (lha_check_header_format(p) == 0)
return (30);

if (p[0] == 'M' && p[1] == 'Z') {

offset = 0;
window = 4096;
while (offset < (1024 * 20)) {
buff = __archive_read_ahead(a, offset + window,
&bytes_avail);
if (buff == NULL) {

window >>= 1;
if (window < (H_SIZE + 3))
return (0);
continue;
}
p = (const char *)buff + offset;
while (p + H_SIZE < (const char *)buff + bytes_avail) {
if ((next = lha_check_header_format(p)) == 0)
return (30);
p += next;
}
offset = p - (const char *)buff;
}
}
return (0);
}

static int
archive_read_format_lha_options(struct archive_read *a,
const char *key, const char *val)
{
struct lha *lha;
int ret = ARCHIVE_FAILED;

lha = (struct lha *)(a->format->data);
if (strcmp(key, "hdrcharset") == 0) {
if (val == NULL || val[0] == 0)
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"lha: hdrcharset option needs a character-set name");
else {
lha->opt_sconv =
archive_string_conversion_from_charset(
&a->archive, val, 0);
if (lha->opt_sconv != NULL)
ret = ARCHIVE_OK;
else
ret = ARCHIVE_FATAL;
}
return (ret);
}




return (ARCHIVE_WARN);
}

static int
lha_skip_sfx(struct archive_read *a)
{
const void *h;
const char *p, *q;
size_t next, skip;
ssize_t bytes, window;

window = 4096;
for (;;) {
h = __archive_read_ahead(a, window, &bytes);
if (h == NULL) {

window >>= 1;
if (window < (H_SIZE + 3))
goto fatal;
continue;
}
if (bytes < H_SIZE)
goto fatal;
p = h;
q = p + bytes;





while (p + H_SIZE < q) {
if ((next = lha_check_header_format(p)) == 0) {
skip = p - (const char *)h;
__archive_read_consume(a, skip);
return (ARCHIVE_OK);
}
p += next;
}
skip = p - (const char *)h;
__archive_read_consume(a, skip);
}
fatal:
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Couldn't find out LHa header");
return (ARCHIVE_FATAL);
}

static int
truncated_error(struct archive_read *a)
{
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Truncated LHa header");
return (ARCHIVE_FATAL);
}

static int
archive_read_format_lha_read_header(struct archive_read *a,
struct archive_entry *entry)
{
struct archive_wstring linkname;
struct archive_wstring pathname;
struct lha *lha;
const unsigned char *p;
const char *signature;
int err;
struct archive_mstring conv_buffer;
const wchar_t *conv_buffer_p;

lha_crc16_init();

a->archive.archive_format = ARCHIVE_FORMAT_LHA;
if (a->archive.archive_format_name == NULL)
a->archive.archive_format_name = "lha";

lha = (struct lha *)(a->format->data);
lha->decompress_init = 0;
lha->end_of_entry = 0;
lha->end_of_entry_cleanup = 0;
lha->entry_unconsumed = 0;

if ((p = __archive_read_ahead(a, H_SIZE, NULL)) == NULL) {




signature = __archive_read_ahead(a, sizeof(signature[0]), NULL);
if (signature == NULL || signature[0] == 0)
return (ARCHIVE_EOF);
return (truncated_error(a));
}

signature = (const char *)p;
if (lha->found_first_header == 0 &&
signature[0] == 'M' && signature[1] == 'Z') {

err = lha_skip_sfx(a);
if (err < ARCHIVE_WARN)
return (err);

if ((p = __archive_read_ahead(a, sizeof(*p), NULL)) == NULL)
return (truncated_error(a));
signature = (const char *)p;
}

if (signature[0] == 0)
return (ARCHIVE_EOF);




if (lha_check_header_format(p) != 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Bad LHa file");
return (ARCHIVE_FATAL);
}


lha->found_first_header = 1;

lha->header_size = 0;
lha->level = p[H_LEVEL_OFFSET];
lha->method[0] = p[H_METHOD_OFFSET+1];
lha->method[1] = p[H_METHOD_OFFSET+2];
lha->method[2] = p[H_METHOD_OFFSET+3];
if (memcmp(lha->method, "lhd", 3) == 0)
lha->directory = 1;
else
lha->directory = 0;
if (memcmp(lha->method, "lh0", 3) == 0 ||
memcmp(lha->method, "lz4", 3) == 0)
lha->entry_is_compressed = 0;
else
lha->entry_is_compressed = 1;

lha->compsize = 0;
lha->origsize = 0;
lha->setflag = 0;
lha->birthtime = 0;
lha->birthtime_tv_nsec = 0;
lha->mtime = 0;
lha->mtime_tv_nsec = 0;
lha->atime = 0;
lha->atime_tv_nsec = 0;
lha->mode = (lha->directory)? 0777 : 0666;
lha->uid = 0;
lha->gid = 0;
archive_string_empty(&lha->dirname);
archive_string_empty(&lha->filename);
lha->dos_attr = 0;
if (lha->opt_sconv != NULL) {
lha->sconv_dir = lha->opt_sconv;
lha->sconv_fname = lha->opt_sconv;
} else {
lha->sconv_dir = NULL;
lha->sconv_fname = NULL;
}

switch (p[H_LEVEL_OFFSET]) {
case 0:
err = lha_read_file_header_0(a, lha);
break;
case 1:
err = lha_read_file_header_1(a, lha);
break;
case 2:
err = lha_read_file_header_2(a, lha);
break;
case 3:
err = lha_read_file_header_3(a, lha);
break;
default:
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Unsupported LHa header level %d", p[H_LEVEL_OFFSET]);
err = ARCHIVE_FATAL;
break;
}
if (err < ARCHIVE_WARN)
return (err);


if (!lha->directory && archive_strlen(&lha->filename) == 0)

return (truncated_error(a));





archive_string_init(&pathname);
archive_string_init(&linkname);
archive_string_init(&conv_buffer.aes_mbs);
archive_string_init(&conv_buffer.aes_mbs_in_locale);
archive_string_init(&conv_buffer.aes_utf8);
archive_string_init(&conv_buffer.aes_wcs);
if (0 != archive_mstring_copy_mbs_len_l(&conv_buffer, lha->dirname.s, lha->dirname.length, lha->sconv_dir)) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Pathname cannot be converted "
"from %s to Unicode.",
archive_string_conversion_charset_name(lha->sconv_dir));
err = ARCHIVE_FATAL;
} else if (0 != archive_mstring_get_wcs(&a->archive, &conv_buffer, &conv_buffer_p))
err = ARCHIVE_FATAL;
if (err == ARCHIVE_FATAL) {
archive_mstring_clean(&conv_buffer);
archive_wstring_free(&pathname);
archive_wstring_free(&linkname);
return (err);
}
archive_wstring_copy(&pathname, &conv_buffer.aes_wcs);

archive_string_empty(&conv_buffer.aes_mbs);
archive_string_empty(&conv_buffer.aes_mbs_in_locale);
archive_string_empty(&conv_buffer.aes_utf8);
archive_wstring_empty(&conv_buffer.aes_wcs);
if (0 != archive_mstring_copy_mbs_len_l(&conv_buffer, lha->filename.s, lha->filename.length, lha->sconv_fname)) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Pathname cannot be converted "
"from %s to Unicode.",
archive_string_conversion_charset_name(lha->sconv_fname));
err = ARCHIVE_FATAL;
}
else if (0 != archive_mstring_get_wcs(&a->archive, &conv_buffer, &conv_buffer_p))
err = ARCHIVE_FATAL;
if (err == ARCHIVE_FATAL) {
archive_mstring_clean(&conv_buffer);
archive_wstring_free(&pathname);
archive_wstring_free(&linkname);
return (err);
}
archive_wstring_concat(&pathname, &conv_buffer.aes_wcs);
archive_mstring_clean(&conv_buffer);

if ((lha->mode & AE_IFMT) == AE_IFLNK) {



if (!lha_parse_linkname(&linkname, &pathname)) {

archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Unknown symlink-name");
archive_wstring_free(&pathname);
archive_wstring_free(&linkname);
return (ARCHIVE_FAILED);
}
} else {




lha->mode = (lha->mode & ~AE_IFMT) |
((lha->directory)? AE_IFDIR: AE_IFREG);
}
if ((lha->setflag & UNIX_MODE_IS_SET) == 0 &&
(lha->dos_attr & 1) != 0)
lha->mode &= ~(0222);




archive_entry_copy_pathname_w(entry, pathname.s);
archive_wstring_free(&pathname);
if (archive_strlen(&linkname) > 0) {
archive_entry_copy_symlink_w(entry, linkname.s);
} else
archive_entry_set_symlink(entry, NULL);
archive_wstring_free(&linkname);





if (p[H_LEVEL_OFFSET] == 0)
lha_replace_path_separator(lha, entry);

archive_entry_set_mode(entry, lha->mode);
archive_entry_set_uid(entry, lha->uid);
archive_entry_set_gid(entry, lha->gid);
if (archive_strlen(&lha->uname) > 0)
archive_entry_set_uname(entry, lha->uname.s);
if (archive_strlen(&lha->gname) > 0)
archive_entry_set_gname(entry, lha->gname.s);
if (lha->setflag & BIRTHTIME_IS_SET) {
archive_entry_set_birthtime(entry, lha->birthtime,
lha->birthtime_tv_nsec);
archive_entry_set_ctime(entry, lha->birthtime,
lha->birthtime_tv_nsec);
} else {
archive_entry_unset_birthtime(entry);
archive_entry_unset_ctime(entry);
}
archive_entry_set_mtime(entry, lha->mtime, lha->mtime_tv_nsec);
if (lha->setflag & ATIME_IS_SET)
archive_entry_set_atime(entry, lha->atime,
lha->atime_tv_nsec);
else
archive_entry_unset_atime(entry);
if (lha->directory || archive_entry_symlink(entry) != NULL)
archive_entry_unset_size(entry);
else
archive_entry_set_size(entry, lha->origsize);




lha->entry_bytes_remaining = lha->compsize;
if (lha->entry_bytes_remaining < 0) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid LHa entry size");
return (ARCHIVE_FATAL);
}
lha->entry_offset = 0;
lha->entry_crc_calculated = 0;




if (lha->directory || lha->compsize == 0)
lha->end_of_entry = 1;

sprintf(lha->format_name, "lha -%c%c%c-",
lha->method[0], lha->method[1], lha->method[2]);
a->archive.archive_format_name = lha->format_name;

return (err);
}





static void
lha_replace_path_separator(struct lha *lha, struct archive_entry *entry)
{
const wchar_t *wp;
size_t i;

if ((wp = archive_entry_pathname_w(entry)) != NULL) {
archive_wstrcpy(&(lha->ws), wp);
for (i = 0; i < archive_strlen(&(lha->ws)); i++) {
if (lha->ws.s[i] == L'\\')
lha->ws.s[i] = L'/';
}
archive_entry_copy_pathname_w(entry, lha->ws.s);
}

if ((wp = archive_entry_symlink_w(entry)) != NULL) {
archive_wstrcpy(&(lha->ws), wp);
for (i = 0; i < archive_strlen(&(lha->ws)); i++) {
if (lha->ws.s[i] == L'\\')
lha->ws.s[i] = L'/';
}
archive_entry_copy_symlink_w(entry, lha->ws.s);
}
}
























#define H0_HEADER_SIZE_OFFSET 0
#define H0_HEADER_SUM_OFFSET 1
#define H0_COMP_SIZE_OFFSET 7
#define H0_ORIG_SIZE_OFFSET 11
#define H0_DOS_TIME_OFFSET 15
#define H0_NAME_LEN_OFFSET 21
#define H0_FILE_NAME_OFFSET 22
#define H0_FIXED_SIZE 24
static int
lha_read_file_header_0(struct archive_read *a, struct lha *lha)
{
const unsigned char *p;
int extdsize, namelen;
unsigned char headersum, sum_calculated;

if ((p = __archive_read_ahead(a, H0_FIXED_SIZE, NULL)) == NULL)
return (truncated_error(a));
lha->header_size = p[H0_HEADER_SIZE_OFFSET] + 2;
headersum = p[H0_HEADER_SUM_OFFSET];
lha->compsize = archive_le32dec(p + H0_COMP_SIZE_OFFSET);
lha->origsize = archive_le32dec(p + H0_ORIG_SIZE_OFFSET);
lha->mtime = lha_dos_time(p + H0_DOS_TIME_OFFSET);
namelen = p[H0_NAME_LEN_OFFSET];
extdsize = (int)lha->header_size - H0_FIXED_SIZE - namelen;
if ((namelen > 221 || extdsize < 0) && extdsize != -2) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid LHa header");
return (ARCHIVE_FATAL);
}
if ((p = __archive_read_ahead(a, lha->header_size, NULL)) == NULL)
return (truncated_error(a));

archive_strncpy(&lha->filename, p + H0_FILE_NAME_OFFSET, namelen);

if (extdsize >= 0) {
lha->crc = archive_le16dec(p + H0_FILE_NAME_OFFSET + namelen);
lha->setflag |= CRC_IS_SET;
}
sum_calculated = lha_calcsum(0, p, 2, lha->header_size - 2);


if (extdsize > 0) {



p += H0_FILE_NAME_OFFSET + namelen + 2;
if (p[0] == 'U' && extdsize == 12) {

lha->mtime = archive_le32dec(&p[2]);
lha->mode = archive_le16dec(&p[6]);
lha->uid = archive_le16dec(&p[8]);
lha->gid = archive_le16dec(&p[10]);
lha->setflag |= UNIX_MODE_IS_SET;
}
}
__archive_read_consume(a, lha->header_size);

if (sum_calculated != headersum) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"LHa header sum error");
return (ARCHIVE_FATAL);
}

return (ARCHIVE_OK);
}





























#define H1_HEADER_SIZE_OFFSET 0
#define H1_HEADER_SUM_OFFSET 1
#define H1_COMP_SIZE_OFFSET 7
#define H1_ORIG_SIZE_OFFSET 11
#define H1_DOS_TIME_OFFSET 15
#define H1_NAME_LEN_OFFSET 21
#define H1_FILE_NAME_OFFSET 22
#define H1_FIXED_SIZE 27
static int
lha_read_file_header_1(struct archive_read *a, struct lha *lha)
{
const unsigned char *p;
size_t extdsize;
int i, err, err2;
int namelen, padding;
unsigned char headersum, sum_calculated;

err = ARCHIVE_OK;

if ((p = __archive_read_ahead(a, H1_FIXED_SIZE, NULL)) == NULL)
return (truncated_error(a));

lha->header_size = p[H1_HEADER_SIZE_OFFSET] + 2;
headersum = p[H1_HEADER_SUM_OFFSET];

lha->compsize = archive_le32dec(p + H1_COMP_SIZE_OFFSET);
lha->origsize = archive_le32dec(p + H1_ORIG_SIZE_OFFSET);
lha->mtime = lha_dos_time(p + H1_DOS_TIME_OFFSET);
namelen = p[H1_NAME_LEN_OFFSET];

padding = ((int)lha->header_size) - H1_FIXED_SIZE - namelen;

if (namelen > 230 || padding < 0)
goto invalid;

if ((p = __archive_read_ahead(a, lha->header_size, NULL)) == NULL)
return (truncated_error(a));

for (i = 0; i < namelen; i++) {
if (p[i + H1_FILE_NAME_OFFSET] == 0xff)
goto invalid;
}
archive_strncpy(&lha->filename, p + H1_FILE_NAME_OFFSET, namelen);
lha->crc = archive_le16dec(p + H1_FILE_NAME_OFFSET + namelen);
lha->setflag |= CRC_IS_SET;

sum_calculated = lha_calcsum(0, p, 2, lha->header_size - 2);


__archive_read_consume(a, lha->header_size - 2);


err2 = lha_read_file_extended_header(a, lha, NULL, 2,
(size_t)(lha->compsize + 2), &extdsize);
if (err2 < ARCHIVE_WARN)
return (err2);
if (err2 < err)
err = err2;

lha->compsize -= extdsize - 2;

if (lha->compsize < 0)
goto invalid;

if (sum_calculated != headersum) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"LHa header sum error");
return (ARCHIVE_FATAL);
}
return (err);
invalid:
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid LHa header");
return (ARCHIVE_FATAL);
}























#define H2_HEADER_SIZE_OFFSET 0
#define H2_COMP_SIZE_OFFSET 7
#define H2_ORIG_SIZE_OFFSET 11
#define H2_TIME_OFFSET 15
#define H2_CRC_OFFSET 21
#define H2_FIXED_SIZE 24
static int
lha_read_file_header_2(struct archive_read *a, struct lha *lha)
{
const unsigned char *p;
size_t extdsize;
int err, padding;
uint16_t header_crc;

if ((p = __archive_read_ahead(a, H2_FIXED_SIZE, NULL)) == NULL)
return (truncated_error(a));

lha->header_size =archive_le16dec(p + H2_HEADER_SIZE_OFFSET);
lha->compsize = archive_le32dec(p + H2_COMP_SIZE_OFFSET);
lha->origsize = archive_le32dec(p + H2_ORIG_SIZE_OFFSET);
lha->mtime = archive_le32dec(p + H2_TIME_OFFSET);
lha->crc = archive_le16dec(p + H2_CRC_OFFSET);
lha->setflag |= CRC_IS_SET;

if (lha->header_size < H2_FIXED_SIZE) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid LHa header size");
return (ARCHIVE_FATAL);
}

header_crc = lha_crc16(0, p, H2_FIXED_SIZE);
__archive_read_consume(a, H2_FIXED_SIZE);


err = lha_read_file_extended_header(a, lha, &header_crc, 2,
lha->header_size - H2_FIXED_SIZE, &extdsize);
if (err < ARCHIVE_WARN)
return (err);


padding = (int)lha->header_size - (int)(H2_FIXED_SIZE + extdsize);
if (padding > 0) {
if ((p = __archive_read_ahead(a, padding, NULL)) == NULL)
return (truncated_error(a));
header_crc = lha_crc16(header_crc, p, padding);
__archive_read_consume(a, padding);
}

if (header_crc != lha->header_crc) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"LHa header CRC error");
return (ARCHIVE_FATAL);
}
return (err);
}























#define H3_FIELD_LEN_OFFSET 0
#define H3_COMP_SIZE_OFFSET 7
#define H3_ORIG_SIZE_OFFSET 11
#define H3_TIME_OFFSET 15
#define H3_CRC_OFFSET 21
#define H3_HEADER_SIZE_OFFSET 24
#define H3_FIXED_SIZE 28
static int
lha_read_file_header_3(struct archive_read *a, struct lha *lha)
{
const unsigned char *p;
size_t extdsize;
int err;
uint16_t header_crc;

if ((p = __archive_read_ahead(a, H3_FIXED_SIZE, NULL)) == NULL)
return (truncated_error(a));

if (archive_le16dec(p + H3_FIELD_LEN_OFFSET) != 4)
goto invalid;
lha->header_size =archive_le32dec(p + H3_HEADER_SIZE_OFFSET);
lha->compsize = archive_le32dec(p + H3_COMP_SIZE_OFFSET);
lha->origsize = archive_le32dec(p + H3_ORIG_SIZE_OFFSET);
lha->mtime = archive_le32dec(p + H3_TIME_OFFSET);
lha->crc = archive_le16dec(p + H3_CRC_OFFSET);
lha->setflag |= CRC_IS_SET;

if (lha->header_size < H3_FIXED_SIZE + 4)
goto invalid;
header_crc = lha_crc16(0, p, H3_FIXED_SIZE);
__archive_read_consume(a, H3_FIXED_SIZE);


err = lha_read_file_extended_header(a, lha, &header_crc, 4,
lha->header_size - H3_FIXED_SIZE, &extdsize);
if (err < ARCHIVE_WARN)
return (err);

if (header_crc != lha->header_crc) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"LHa header CRC error");
return (ARCHIVE_FATAL);
}
return (err);
invalid:
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid LHa header");
return (ARCHIVE_FATAL);
}















static int
lha_read_file_extended_header(struct archive_read *a, struct lha *lha,
uint16_t *crc, int sizefield_length, size_t limitsize, size_t *total_size)
{
const void *h;
const unsigned char *extdheader;
size_t extdsize;
size_t datasize;
unsigned int i;
unsigned char extdtype;

#define EXT_HEADER_CRC 0x00
#define EXT_FILENAME 0x01
#define EXT_DIRECTORY 0x02
#define EXT_DOS_ATTR 0x40
#define EXT_TIMESTAMP 0x41
#define EXT_FILESIZE 0x42
#define EXT_TIMEZONE 0x43
#define EXT_UTF16_FILENAME 0x44
#define EXT_UTF16_DIRECTORY 0x45
#define EXT_CODEPAGE 0x46
#define EXT_UNIX_MODE 0x50
#define EXT_UNIX_GID_UID 0x51
#define EXT_UNIX_GNAME 0x52
#define EXT_UNIX_UNAME 0x53
#define EXT_UNIX_MTIME 0x54
#define EXT_OS2_NEW_ATTR 0x7f
#define EXT_NEW_ATTR 0xff

*total_size = sizefield_length;

for (;;) {

if ((h =
__archive_read_ahead(a, sizefield_length, NULL)) == NULL)
return (truncated_error(a));


if (sizefield_length == sizeof(uint16_t))
extdsize = archive_le16dec(h);
else
extdsize = archive_le32dec(h);
if (extdsize == 0) {

if (crc != NULL)
*crc = lha_crc16(*crc, h, sizefield_length);
__archive_read_consume(a, sizefield_length);
return (ARCHIVE_OK);
}


if (((uint64_t)*total_size + extdsize) >
(uint64_t)limitsize ||
extdsize <= (size_t)sizefield_length)
goto invalid;


if ((h = __archive_read_ahead(a, extdsize, NULL)) == NULL)
return (truncated_error(a));
*total_size += extdsize;

extdheader = (const unsigned char *)h;

extdtype = extdheader[sizefield_length];

datasize = extdsize - (1 + sizefield_length);

extdheader += sizefield_length + 1;

if (crc != NULL && extdtype != EXT_HEADER_CRC)
*crc = lha_crc16(*crc, h, extdsize);
switch (extdtype) {
case EXT_HEADER_CRC:


if (datasize >= 2) {
lha->header_crc = archive_le16dec(extdheader);
if (crc != NULL) {
static const char zeros[2] = {0, 0};
*crc = lha_crc16(*crc, h,
extdsize - datasize);

*crc = lha_crc16(*crc, zeros, 2);
*crc = lha_crc16(*crc,
extdheader+2, datasize - 2);
}
}
break;
case EXT_FILENAME:
if (datasize == 0) {

archive_string_empty(&lha->filename);
break;
}
if (extdheader[0] == '\0')
goto invalid;
archive_strncpy(&lha->filename,
(const char *)extdheader, datasize);
break;
case EXT_UTF16_FILENAME:
if (datasize == 0) {

archive_string_empty(&lha->filename);
break;
} else if (datasize & 1) {

goto invalid;
}
if (extdheader[0] == '\0')
goto invalid;
archive_string_empty(&lha->filename);
archive_array_append(&lha->filename,
(const char *)extdheader, datasize);

lha->sconv_fname =
archive_string_conversion_from_charset(&a->archive,
"UTF-16LE", 1);
if (lha->sconv_fname == NULL)
return (ARCHIVE_FATAL);
break;
case EXT_DIRECTORY:
if (datasize == 0 || extdheader[0] == '\0')

goto invalid;

archive_strncpy(&lha->dirname,
(const char *)extdheader, datasize);




for (i = 0; i < lha->dirname.length; i++) {
if ((unsigned char)lha->dirname.s[i] == 0xFF)
lha->dirname.s[i] = '/';
}

if (lha->dirname.s[lha->dirname.length-1] != '/')

goto invalid;
break;
case EXT_UTF16_DIRECTORY:

if (datasize == 0 || (datasize & 1) ||
extdheader[0] == '\0') {

goto invalid;
}

archive_string_empty(&lha->dirname);
archive_array_append(&lha->dirname,
(const char *)extdheader, datasize);
lha->sconv_dir =
archive_string_conversion_from_charset(&a->archive,
"UTF-16LE", 1);
if (lha->sconv_dir == NULL)
return (ARCHIVE_FATAL);
else {




uint16_t dirSep;
uint16_t d = 1;
if (archive_be16dec(&d) == 1)
dirSep = 0x2F00;
else
dirSep = 0x002F;


uint16_t *utf16name =
(uint16_t *)lha->dirname.s;
for (i = 0; i < lha->dirname.length / 2; i++) {
if (utf16name[i] == 0xFFFF) {
utf16name[i] = dirSep;
}
}

if (utf16name[lha->dirname.length / 2 - 1] !=
dirSep) {

goto invalid;
}
}
break;
case EXT_DOS_ATTR:
if (datasize == 2)
lha->dos_attr = (unsigned char)
(archive_le16dec(extdheader) & 0xff);
break;
case EXT_TIMESTAMP:
if (datasize == (sizeof(uint64_t) * 3)) {
lha->birthtime = lha_win_time(
archive_le64dec(extdheader),
&lha->birthtime_tv_nsec);
extdheader += sizeof(uint64_t);
lha->mtime = lha_win_time(
archive_le64dec(extdheader),
&lha->mtime_tv_nsec);
extdheader += sizeof(uint64_t);
lha->atime = lha_win_time(
archive_le64dec(extdheader),
&lha->atime_tv_nsec);
lha->setflag |= BIRTHTIME_IS_SET |
ATIME_IS_SET;
}
break;
case EXT_FILESIZE:
if (datasize == sizeof(uint64_t) * 2) {
lha->compsize = archive_le64dec(extdheader);
extdheader += sizeof(uint64_t);
lha->origsize = archive_le64dec(extdheader);
}
break;
case EXT_CODEPAGE:



if (datasize == sizeof(uint32_t)) {
struct archive_string cp;
const char *charset;

archive_string_init(&cp);
switch (archive_le32dec(extdheader)) {
case 65001:
charset = "UTF-8";
break;
default:
archive_string_sprintf(&cp, "CP%d",
(int)archive_le32dec(extdheader));
charset = cp.s;
break;
}
lha->sconv_dir =
archive_string_conversion_from_charset(
&(a->archive), charset, 1);
lha->sconv_fname =
archive_string_conversion_from_charset(
&(a->archive), charset, 1);
archive_string_free(&cp);
if (lha->sconv_dir == NULL)
return (ARCHIVE_FATAL);
if (lha->sconv_fname == NULL)
return (ARCHIVE_FATAL);
}
break;
case EXT_UNIX_MODE:
if (datasize == sizeof(uint16_t)) {
lha->mode = archive_le16dec(extdheader);
lha->setflag |= UNIX_MODE_IS_SET;
}
break;
case EXT_UNIX_GID_UID:
if (datasize == (sizeof(uint16_t) * 2)) {
lha->gid = archive_le16dec(extdheader);
lha->uid = archive_le16dec(extdheader+2);
}
break;
case EXT_UNIX_GNAME:
if (datasize > 0)
archive_strncpy(&lha->gname,
(const char *)extdheader, datasize);
break;
case EXT_UNIX_UNAME:
if (datasize > 0)
archive_strncpy(&lha->uname,
(const char *)extdheader, datasize);
break;
case EXT_UNIX_MTIME:
if (datasize == sizeof(uint32_t))
lha->mtime = archive_le32dec(extdheader);
break;
case EXT_OS2_NEW_ATTR:

if (datasize == 16) {
lha->dos_attr = (unsigned char)
(archive_le16dec(extdheader) & 0xff);
lha->mode = archive_le16dec(extdheader+2);
lha->gid = archive_le16dec(extdheader+4);
lha->uid = archive_le16dec(extdheader+6);
lha->birthtime = archive_le32dec(extdheader+8);
lha->atime = archive_le32dec(extdheader+12);
lha->setflag |= UNIX_MODE_IS_SET
| BIRTHTIME_IS_SET | ATIME_IS_SET;
}
break;
case EXT_NEW_ATTR:
if (datasize == 20) {
lha->mode = (mode_t)archive_le32dec(extdheader);
lha->gid = archive_le32dec(extdheader+4);
lha->uid = archive_le32dec(extdheader+8);
lha->birthtime = archive_le32dec(extdheader+12);
lha->atime = archive_le32dec(extdheader+16);
lha->setflag |= UNIX_MODE_IS_SET
| BIRTHTIME_IS_SET | ATIME_IS_SET;
}
break;
case EXT_TIMEZONE:
break;
default:
break;
}

__archive_read_consume(a, extdsize);
}
invalid:
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid extended LHa header");
return (ARCHIVE_FATAL);
}

static int
lha_end_of_entry(struct archive_read *a)
{
struct lha *lha = (struct lha *)(a->format->data);
int r = ARCHIVE_EOF;

if (!lha->end_of_entry_cleanup) {
if ((lha->setflag & CRC_IS_SET) &&
lha->crc != lha->entry_crc_calculated) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"LHa data CRC error");
r = ARCHIVE_WARN;
}


lha->end_of_entry_cleanup = 1;
}
return (r);
}

static int
archive_read_format_lha_read_data(struct archive_read *a,
const void **buff, size_t *size, int64_t *offset)
{
struct lha *lha = (struct lha *)(a->format->data);
int r;

if (lha->entry_unconsumed) {

__archive_read_consume(a, lha->entry_unconsumed);
lha->entry_unconsumed = 0;
}
if (lha->end_of_entry) {
*offset = lha->entry_offset;
*size = 0;
*buff = NULL;
return (lha_end_of_entry(a));
}

if (lha->entry_is_compressed)
r = lha_read_data_lzh(a, buff, size, offset);
else

r = lha_read_data_none(a, buff, size, offset);
return (r);
}







static int
lha_read_data_none(struct archive_read *a, const void **buff,
size_t *size, int64_t *offset)
{
struct lha *lha = (struct lha *)(a->format->data);
ssize_t bytes_avail;

if (lha->entry_bytes_remaining == 0) {
*buff = NULL;
*size = 0;
*offset = lha->entry_offset;
lha->end_of_entry = 1;
return (ARCHIVE_OK);
}






*buff = __archive_read_ahead(a, 1, &bytes_avail);
if (bytes_avail <= 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Truncated LHa file data");
return (ARCHIVE_FATAL);
}
if (bytes_avail > lha->entry_bytes_remaining)
bytes_avail = (ssize_t)lha->entry_bytes_remaining;
lha->entry_crc_calculated =
lha_crc16(lha->entry_crc_calculated, *buff, bytes_avail);
*size = bytes_avail;
*offset = lha->entry_offset;
lha->entry_offset += bytes_avail;
lha->entry_bytes_remaining -= bytes_avail;
if (lha->entry_bytes_remaining == 0)
lha->end_of_entry = 1;
lha->entry_unconsumed = bytes_avail;
return (ARCHIVE_OK);
}








static int
lha_read_data_lzh(struct archive_read *a, const void **buff,
size_t *size, int64_t *offset)
{
struct lha *lha = (struct lha *)(a->format->data);
ssize_t bytes_avail;
int r;


if (!lha->decompress_init) {
r = lzh_decode_init(&(lha->strm), lha->method);
switch (r) {
case ARCHIVE_OK:
break;
case ARCHIVE_FAILED:

*buff = NULL;
*size = 0;
*offset = 0;
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Unsupported lzh compression method -%c%c%c-",
lha->method[0], lha->method[1], lha->method[2]);

archive_read_format_lha_read_data_skip(a);
return (ARCHIVE_WARN);
default:
archive_set_error(&a->archive, ENOMEM,
"Couldn't allocate memory "
"for lzh decompression");
return (ARCHIVE_FATAL);
}

lha->decompress_init = 1;
lha->strm.avail_out = 0;
lha->strm.total_out = 0;
}







lha->strm.next_in = __archive_read_ahead(a, 1, &bytes_avail);
if (bytes_avail <= 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Truncated LHa file body");
return (ARCHIVE_FATAL);
}
if (bytes_avail > lha->entry_bytes_remaining)
bytes_avail = (ssize_t)lha->entry_bytes_remaining;

lha->strm.avail_in = (int)bytes_avail;
lha->strm.total_in = 0;
lha->strm.avail_out = 0;

r = lzh_decode(&(lha->strm), bytes_avail == lha->entry_bytes_remaining);
switch (r) {
case ARCHIVE_OK:
break;
case ARCHIVE_EOF:
lha->end_of_entry = 1;
break;
default:
archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
"Bad lzh data");
return (ARCHIVE_FAILED);
}
lha->entry_unconsumed = lha->strm.total_in;
lha->entry_bytes_remaining -= lha->strm.total_in;

if (lha->strm.avail_out) {
*offset = lha->entry_offset;
*size = lha->strm.avail_out;
*buff = lha->strm.ref_ptr;
lha->entry_crc_calculated =
lha_crc16(lha->entry_crc_calculated, *buff, *size);
lha->entry_offset += *size;
} else {
*offset = lha->entry_offset;
*size = 0;
*buff = NULL;
if (lha->end_of_entry)
return (lha_end_of_entry(a));
}
return (ARCHIVE_OK);
}




static int
archive_read_format_lha_read_data_skip(struct archive_read *a)
{
struct lha *lha;
int64_t bytes_skipped;

lha = (struct lha *)(a->format->data);

if (lha->entry_unconsumed) {

__archive_read_consume(a, lha->entry_unconsumed);
lha->entry_unconsumed = 0;
}


if (lha->end_of_entry_cleanup)
return (ARCHIVE_OK);





bytes_skipped = __archive_read_consume(a, lha->entry_bytes_remaining);
if (bytes_skipped < 0)
return (ARCHIVE_FATAL);


lha->end_of_entry_cleanup = lha->end_of_entry = 1;
return (ARCHIVE_OK);
}

static int
archive_read_format_lha_cleanup(struct archive_read *a)
{
struct lha *lha = (struct lha *)(a->format->data);

lzh_decode_free(&(lha->strm));
archive_string_free(&(lha->dirname));
archive_string_free(&(lha->filename));
archive_string_free(&(lha->uname));
archive_string_free(&(lha->gname));
archive_wstring_free(&(lha->ws));
free(lha);
(a->format->data) = NULL;
return (ARCHIVE_OK);
}











static int
lha_parse_linkname(struct archive_wstring *linkname,
struct archive_wstring *pathname)
{
wchar_t * linkptr;
size_t symlen;

linkptr = wcschr(pathname->s, L'|');
if (linkptr != NULL) {
symlen = wcslen(linkptr + 1);
archive_wstrncpy(linkname, linkptr+1, symlen);

*linkptr = 0;
pathname->length = wcslen(pathname->s);

return (1);
}
return (0);
}


static time_t
lha_dos_time(const unsigned char *p)
{
int msTime, msDate;
struct tm ts;

msTime = archive_le16dec(p);
msDate = archive_le16dec(p+2);

memset(&ts, 0, sizeof(ts));
ts.tm_year = ((msDate >> 9) & 0x7f) + 80;
ts.tm_mon = ((msDate >> 5) & 0x0f) - 1;
ts.tm_mday = msDate & 0x1f;
ts.tm_hour = (msTime >> 11) & 0x1f;
ts.tm_min = (msTime >> 5) & 0x3f;
ts.tm_sec = (msTime << 1) & 0x3e;
ts.tm_isdst = -1;
return (mktime(&ts));
}


static time_t
lha_win_time(uint64_t wintime, long *ns)
{
#define EPOC_TIME ARCHIVE_LITERAL_ULL(116444736000000000)

if (wintime >= EPOC_TIME) {
wintime -= EPOC_TIME;
if (ns != NULL)
*ns = (long)(wintime % 10000000) * 100;
return (wintime / 10000000);
} else {
if (ns != NULL)
*ns = 0;
return (0);
}
}

static unsigned char
lha_calcsum(unsigned char sum, const void *pp, int offset, size_t size)
{
unsigned char const *p = (unsigned char const *)pp;

p += offset;
for (;size > 0; --size)
sum += *p++;
return (sum);
}

static uint16_t crc16tbl[2][256];
static void
lha_crc16_init(void)
{
unsigned int i;
static int crc16init = 0;

if (crc16init)
return;
crc16init = 1;

for (i = 0; i < 256; i++) {
unsigned int j;
uint16_t crc = (uint16_t)i;
for (j = 8; j; j--)
crc = (crc >> 1) ^ ((crc & 1) * 0xA001);
crc16tbl[0][i] = crc;
}

for (i = 0; i < 256; i++) {
crc16tbl[1][i] = (crc16tbl[0][i] >> 8)
^ crc16tbl[0][crc16tbl[0][i] & 0xff];
}
}

static uint16_t
lha_crc16(uint16_t crc, const void *pp, size_t len)
{
const unsigned char *p = (const unsigned char *)pp;
const uint16_t *buff;
const union {
uint32_t i;
char c[4];
} u = { 0x01020304 };

if (len == 0)
return crc;


if (((uintptr_t)p) & (uintptr_t)0x1) {
crc = (crc >> 8) ^ crc16tbl[0][(crc ^ *p++) & 0xff];
len--;
}
buff = (const uint16_t *)p;





for (;len >= 8; len -= 8) {


#undef bswap16
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define bswap16(x) _byteswap_ushort(x)
#elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 8) || __GNUC__ > 4)

#define bswap16(x) __builtin_bswap16(x)
#elif defined(__clang__)

#define bswap16(x) __builtin_bswap16(x)
#else
#define bswap16(x) ((((x) >> 8) & 0xff) | ((x) << 8))
#endif
#define CRC16W do { if(u.c[0] == 1) { crc ^= bswap16(*buff); buff++; } else crc ^= *buff++; crc = crc16tbl[1][crc & 0xff] ^ crc16tbl[0][crc >> 8];} while (0)






CRC16W;
CRC16W;
CRC16W;
CRC16W;
#undef CRC16W
#undef bswap16
}

p = (const unsigned char *)buff;
for (;len; len--) {
crc = (crc >> 8) ^ crc16tbl[0][(crc ^ *p++) & 0xff];
}
return crc;
}









static int
lzh_decode_init(struct lzh_stream *strm, const char *method)
{
struct lzh_dec *ds;
int w_bits, w_size;

if (strm->ds == NULL) {
strm->ds = calloc(1, sizeof(*strm->ds));
if (strm->ds == NULL)
return (ARCHIVE_FATAL);
}
ds = strm->ds;
ds->error = ARCHIVE_FAILED;
if (method == NULL || method[0] != 'l' || method[1] != 'h')
return (ARCHIVE_FAILED);
switch (method[2]) {
case '5':
w_bits = 13;
break;
case '6':
w_bits = 15;
break;
case '7':
w_bits = 16;
break;
default:
return (ARCHIVE_FAILED);
}
ds->error = ARCHIVE_FATAL;


ds->w_size = 1U << 17;
ds->w_mask = ds->w_size -1;
if (ds->w_buff == NULL) {
ds->w_buff = malloc(ds->w_size);
if (ds->w_buff == NULL)
return (ARCHIVE_FATAL);
}
w_size = 1U << w_bits;
memset(ds->w_buff + ds->w_size - w_size, 0x20, w_size);
ds->w_pos = 0;
ds->state = 0;
ds->pos_pt_len_size = w_bits + 1;
ds->pos_pt_len_bits = (w_bits == 15 || w_bits == 16)? 5: 4;
ds->literal_pt_len_size = PT_BITLEN_SIZE;
ds->literal_pt_len_bits = 5;
ds->br.cache_buffer = 0;
ds->br.cache_avail = 0;

if (lzh_huffman_init(&(ds->lt), LT_BITLEN_SIZE, 16)
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
ds->lt.len_bits = 9;
if (lzh_huffman_init(&(ds->pt), PT_BITLEN_SIZE, 16)
!= ARCHIVE_OK)
return (ARCHIVE_FATAL);
ds->error = 0;

return (ARCHIVE_OK);
}




static void
lzh_decode_free(struct lzh_stream *strm)
{

if (strm->ds == NULL)
return;
free(strm->ds->w_buff);
lzh_huffman_free(&(strm->ds->lt));
lzh_huffman_free(&(strm->ds->pt));
free(strm->ds);
strm->ds = NULL;
}





#define lzh_br_has(br, n) ((br)->cache_avail >= n)

#define lzh_br_bits(br, n) (((uint16_t)((br)->cache_buffer >> ((br)->cache_avail - (n)))) & cache_masks[n])


#define lzh_br_bits_forced(br, n) (((uint16_t)((br)->cache_buffer << ((n) - (br)->cache_avail))) & cache_masks[n])







#define lzh_br_read_ahead_0(strm, br, n) (lzh_br_has(br, (n)) || lzh_br_fillup(strm, br))




#define lzh_br_read_ahead(strm, br, n) (lzh_br_read_ahead_0((strm), (br), (n)) || lzh_br_has((br), (n)))



#define lzh_br_consume(br, n) ((br)->cache_avail -= (n))
#define lzh_br_unconsume(br, n) ((br)->cache_avail += (n))

static const uint16_t cache_masks[] = {
0x0000, 0x0001, 0x0003, 0x0007,
0x000F, 0x001F, 0x003F, 0x007F,
0x00FF, 0x01FF, 0x03FF, 0x07FF,
0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF,
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};








static int
lzh_br_fillup(struct lzh_stream *strm, struct lzh_br *br)
{
int n = CACHE_BITS - br->cache_avail;

for (;;) {
const int x = n >> 3;
if (strm->avail_in >= x) {
switch (x) {
case 8:
br->cache_buffer =
((uint64_t)strm->next_in[0]) << 56 |
((uint64_t)strm->next_in[1]) << 48 |
((uint64_t)strm->next_in[2]) << 40 |
((uint64_t)strm->next_in[3]) << 32 |
((uint32_t)strm->next_in[4]) << 24 |
((uint32_t)strm->next_in[5]) << 16 |
((uint32_t)strm->next_in[6]) << 8 |
(uint32_t)strm->next_in[7];
strm->next_in += 8;
strm->avail_in -= 8;
br->cache_avail += 8 * 8;
return (1);
case 7:
br->cache_buffer =
(br->cache_buffer << 56) |
((uint64_t)strm->next_in[0]) << 48 |
((uint64_t)strm->next_in[1]) << 40 |
((uint64_t)strm->next_in[2]) << 32 |
((uint32_t)strm->next_in[3]) << 24 |
((uint32_t)strm->next_in[4]) << 16 |
((uint32_t)strm->next_in[5]) << 8 |
(uint32_t)strm->next_in[6];
strm->next_in += 7;
strm->avail_in -= 7;
br->cache_avail += 7 * 8;
return (1);
case 6:
br->cache_buffer =
(br->cache_buffer << 48) |
((uint64_t)strm->next_in[0]) << 40 |
((uint64_t)strm->next_in[1]) << 32 |
((uint32_t)strm->next_in[2]) << 24 |
((uint32_t)strm->next_in[3]) << 16 |
((uint32_t)strm->next_in[4]) << 8 |
(uint32_t)strm->next_in[5];
strm->next_in += 6;
strm->avail_in -= 6;
br->cache_avail += 6 * 8;
return (1);
case 0:


return (1);
default:
break;
}
}
if (strm->avail_in == 0) {


return (0);
}
br->cache_buffer =
(br->cache_buffer << 8) | *strm->next_in++;
strm->avail_in--;
br->cache_avail += 8;
n -= 8;
}
}





















static int lzh_read_blocks(struct lzh_stream *, int);
static int lzh_decode_blocks(struct lzh_stream *, int);
#define ST_RD_BLOCK 0
#define ST_RD_PT_1 1
#define ST_RD_PT_2 2
#define ST_RD_PT_3 3
#define ST_RD_PT_4 4
#define ST_RD_LITERAL_1 5
#define ST_RD_LITERAL_2 6
#define ST_RD_LITERAL_3 7
#define ST_RD_POS_DATA_1 8
#define ST_GET_LITERAL 9
#define ST_GET_POS_1 10
#define ST_GET_POS_2 11
#define ST_COPY_DATA 12

static int
lzh_decode(struct lzh_stream *strm, int last)
{
struct lzh_dec *ds = strm->ds;
int avail_in;
int r;

if (ds->error)
return (ds->error);

avail_in = strm->avail_in;
do {
if (ds->state < ST_GET_LITERAL)
r = lzh_read_blocks(strm, last);
else
r = lzh_decode_blocks(strm, last);
} while (r == 100);
strm->total_in += avail_in - strm->avail_in;
return (r);
}

static void
lzh_emit_window(struct lzh_stream *strm, size_t s)
{
strm->ref_ptr = strm->ds->w_buff;
strm->avail_out = (int)s;
strm->total_out += s;
}

static int
lzh_read_blocks(struct lzh_stream *strm, int last)
{
struct lzh_dec *ds = strm->ds;
struct lzh_br *br = &(ds->br);
int c = 0, i;
unsigned rbits;

for (;;) {
switch (ds->state) {
case ST_RD_BLOCK:







if (!lzh_br_read_ahead_0(strm, br, 16)) {
if (!last)

return (ARCHIVE_OK);
if (lzh_br_has(br, 8)) {






goto failed;
}
if (ds->w_pos > 0) {
lzh_emit_window(strm, ds->w_pos);
ds->w_pos = 0;
return (ARCHIVE_OK);
}


return (ARCHIVE_EOF);
}
ds->blocks_avail = lzh_br_bits(br, 16);
if (ds->blocks_avail == 0)
goto failed;
lzh_br_consume(br, 16);




ds->pt.len_size = ds->literal_pt_len_size;
ds->pt.len_bits = ds->literal_pt_len_bits;
ds->reading_position = 0;

case ST_RD_PT_1:



if (!lzh_br_read_ahead(strm, br, ds->pt.len_bits)) {
if (last)
goto failed;
ds->state = ST_RD_PT_1;
return (ARCHIVE_OK);
}
ds->pt.len_avail = lzh_br_bits(br, ds->pt.len_bits);
lzh_br_consume(br, ds->pt.len_bits);

case ST_RD_PT_2:
if (ds->pt.len_avail == 0) {

if (!lzh_br_read_ahead(strm, br,
ds->pt.len_bits)) {
if (last)
goto failed;
ds->state = ST_RD_PT_2;
return (ARCHIVE_OK);
}
if (!lzh_make_fake_table(&(ds->pt),
lzh_br_bits(br, ds->pt.len_bits)))
goto failed;
lzh_br_consume(br, ds->pt.len_bits);
if (ds->reading_position)
ds->state = ST_GET_LITERAL;
else
ds->state = ST_RD_LITERAL_1;
break;
} else if (ds->pt.len_avail > ds->pt.len_size)
goto failed;
ds->loop = 0;
memset(ds->pt.freq, 0, sizeof(ds->pt.freq));
if (ds->pt.len_avail < 3 ||
ds->pt.len_size == ds->pos_pt_len_size) {
ds->state = ST_RD_PT_4;
break;
}

case ST_RD_PT_3:
ds->loop = lzh_read_pt_bitlen(strm, ds->loop, 3);
if (ds->loop < 3) {
if (ds->loop < 0 || last)
goto failed;

ds->state = ST_RD_PT_3;
return (ARCHIVE_OK);
}

if (!lzh_br_read_ahead(strm, br, 2)) {
if (last)
goto failed;
ds->state = ST_RD_PT_3;
return (ARCHIVE_OK);
}
c = lzh_br_bits(br, 2);
lzh_br_consume(br, 2);
if (c > ds->pt.len_avail - 3)
goto failed;
for (i = 3; c-- > 0 ;)
ds->pt.bitlen[i++] = 0;
ds->loop = i;

case ST_RD_PT_4:
ds->loop = lzh_read_pt_bitlen(strm, ds->loop,
ds->pt.len_avail);
if (ds->loop < ds->pt.len_avail) {
if (ds->loop < 0 || last)
goto failed;

ds->state = ST_RD_PT_4;
return (ARCHIVE_OK);
}
if (!lzh_make_huffman_table(&(ds->pt)))
goto failed;
if (ds->reading_position) {
ds->state = ST_GET_LITERAL;
break;
}

case ST_RD_LITERAL_1:
if (!lzh_br_read_ahead(strm, br, ds->lt.len_bits)) {
if (last)
goto failed;
ds->state = ST_RD_LITERAL_1;
return (ARCHIVE_OK);
}
ds->lt.len_avail = lzh_br_bits(br, ds->lt.len_bits);
lzh_br_consume(br, ds->lt.len_bits);

case ST_RD_LITERAL_2:
if (ds->lt.len_avail == 0) {

if (!lzh_br_read_ahead(strm, br,
ds->lt.len_bits)) {
if (last)
goto failed;
ds->state = ST_RD_LITERAL_2;
return (ARCHIVE_OK);
}
if (!lzh_make_fake_table(&(ds->lt),
lzh_br_bits(br, ds->lt.len_bits)))
goto failed;
lzh_br_consume(br, ds->lt.len_bits);
ds->state = ST_RD_POS_DATA_1;
break;
} else if (ds->lt.len_avail > ds->lt.len_size)
goto failed;
ds->loop = 0;
memset(ds->lt.freq, 0, sizeof(ds->lt.freq));

case ST_RD_LITERAL_3:
i = ds->loop;
while (i < ds->lt.len_avail) {
if (!lzh_br_read_ahead(strm, br,
ds->pt.max_bits)) {
if (last)
goto failed;
ds->loop = i;
ds->state = ST_RD_LITERAL_3;
return (ARCHIVE_OK);
}
rbits = lzh_br_bits(br, ds->pt.max_bits);
c = lzh_decode_huffman(&(ds->pt), rbits);
if (c > 2) {





lzh_br_consume(br, ds->pt.bitlen[c]);
c -= 2;
ds->lt.freq[c]++;
ds->lt.bitlen[i++] = c;
} else if (c == 0) {
lzh_br_consume(br, ds->pt.bitlen[c]);
ds->lt.bitlen[i++] = 0;
} else {

int n = (c == 1)?4:9;
if (!lzh_br_read_ahead(strm, br,
ds->pt.bitlen[c] + n)) {
if (last)
goto failed;
ds->loop = i;
ds->state = ST_RD_LITERAL_3;
return (ARCHIVE_OK);
}
lzh_br_consume(br, ds->pt.bitlen[c]);
c = lzh_br_bits(br, n);
lzh_br_consume(br, n);
c += (n == 4)?3:20;
if (i + c > ds->lt.len_avail)
goto failed;
memset(&(ds->lt.bitlen[i]), 0, c);
i += c;
}
}
if (i > ds->lt.len_avail ||
!lzh_make_huffman_table(&(ds->lt)))
goto failed;

case ST_RD_POS_DATA_1:




ds->pt.len_size = ds->pos_pt_len_size;
ds->pt.len_bits = ds->pos_pt_len_bits;
ds->reading_position = 1;
ds->state = ST_RD_PT_1;
break;
case ST_GET_LITERAL:
return (100);
}
}
failed:
return (ds->error = ARCHIVE_FAILED);
}

static int
lzh_decode_blocks(struct lzh_stream *strm, int last)
{
struct lzh_dec *ds = strm->ds;
struct lzh_br bre = ds->br;
struct huffman *lt = &(ds->lt);
struct huffman *pt = &(ds->pt);
unsigned char *w_buff = ds->w_buff;
unsigned char *lt_bitlen = lt->bitlen;
unsigned char *pt_bitlen = pt->bitlen;
int blocks_avail = ds->blocks_avail, c = 0;
int copy_len = ds->copy_len, copy_pos = ds->copy_pos;
int w_pos = ds->w_pos, w_mask = ds->w_mask, w_size = ds->w_size;
int lt_max_bits = lt->max_bits, pt_max_bits = pt->max_bits;
int state = ds->state;

for (;;) {
switch (state) {
case ST_GET_LITERAL:
for (;;) {
if (blocks_avail == 0) {


ds->state = ST_RD_BLOCK;
ds->br = bre;
ds->blocks_avail = 0;
ds->w_pos = w_pos;
ds->copy_pos = 0;
return (100);
}








if (!lzh_br_read_ahead(strm, &bre,
lt_max_bits)) {
if (!last)
goto next_data;





c = lzh_decode_huffman(lt,
lzh_br_bits_forced(&bre,
lt_max_bits));
lzh_br_consume(&bre, lt_bitlen[c]);
if (!lzh_br_has(&bre, 0))
goto failed;
} else {
c = lzh_decode_huffman(lt,
lzh_br_bits(&bre, lt_max_bits));
lzh_br_consume(&bre, lt_bitlen[c]);
}
blocks_avail--;
if (c > UCHAR_MAX)

break;





w_buff[w_pos] = c;
if (++w_pos >= w_size) {
w_pos = 0;
lzh_emit_window(strm, w_size);
goto next_data;
}
}



copy_len = c - (UCHAR_MAX + 1) + MINMATCH;

case ST_GET_POS_1:



if (!lzh_br_read_ahead(strm, &bre, pt_max_bits)) {
if (!last) {
state = ST_GET_POS_1;
ds->copy_len = copy_len;
goto next_data;
}
copy_pos = lzh_decode_huffman(pt,
lzh_br_bits_forced(&bre, pt_max_bits));
lzh_br_consume(&bre, pt_bitlen[copy_pos]);
if (!lzh_br_has(&bre, 0))
goto failed;
} else {
copy_pos = lzh_decode_huffman(pt,
lzh_br_bits(&bre, pt_max_bits));
lzh_br_consume(&bre, pt_bitlen[copy_pos]);
}

case ST_GET_POS_2:
if (copy_pos > 1) {


int p = copy_pos - 1;
if (!lzh_br_read_ahead(strm, &bre, p)) {
if (last)
goto failed;
state = ST_GET_POS_2;
ds->copy_len = copy_len;
ds->copy_pos = copy_pos;
goto next_data;
}
copy_pos = (1 << p) + lzh_br_bits(&bre, p);
lzh_br_consume(&bre, p);
}



copy_pos = (w_pos - copy_pos - 1) & w_mask;

case ST_COPY_DATA:




for (;;) {
int l;

l = copy_len;
if (copy_pos > w_pos) {
if (l > w_size - copy_pos)
l = w_size - copy_pos;
} else {
if (l > w_size - w_pos)
l = w_size - w_pos;
}
if ((copy_pos + l < w_pos)
|| (w_pos + l < copy_pos)) {

memcpy(w_buff + w_pos,
w_buff + copy_pos, l);
} else {
const unsigned char *s;
unsigned char *d;
int li;

d = w_buff + w_pos;
s = w_buff + copy_pos;
for (li = 0; li < l-1;) {
d[li] = s[li];li++;
d[li] = s[li];li++;
}
if (li < l)
d[li] = s[li];
}
w_pos += l;
if (w_pos == w_size) {
w_pos = 0;
lzh_emit_window(strm, w_size);
if (copy_len <= l)
state = ST_GET_LITERAL;
else {
state = ST_COPY_DATA;
ds->copy_len = copy_len - l;
ds->copy_pos =
(copy_pos + l) & w_mask;
}
goto next_data;
}
if (copy_len <= l)

break;
copy_len -= l;
copy_pos = (copy_pos + l) & w_mask;
}
state = ST_GET_LITERAL;
break;
}
}
failed:
return (ds->error = ARCHIVE_FAILED);
next_data:
ds->br = bre;
ds->blocks_avail = blocks_avail;
ds->state = state;
ds->w_pos = w_pos;
return (ARCHIVE_OK);
}

static int
lzh_huffman_init(struct huffman *hf, size_t len_size, int tbl_bits)
{
int bits;

if (hf->bitlen == NULL) {
hf->bitlen = malloc(len_size * sizeof(hf->bitlen[0]));
if (hf->bitlen == NULL)
return (ARCHIVE_FATAL);
}
if (hf->tbl == NULL) {
if (tbl_bits < HTBL_BITS)
bits = tbl_bits;
else
bits = HTBL_BITS;
hf->tbl = malloc(((size_t)1 << bits) * sizeof(hf->tbl[0]));
if (hf->tbl == NULL)
return (ARCHIVE_FATAL);
}
if (hf->tree == NULL && tbl_bits > HTBL_BITS) {
hf->tree_avail = 1 << (tbl_bits - HTBL_BITS + 4);
hf->tree = malloc(hf->tree_avail * sizeof(hf->tree[0]));
if (hf->tree == NULL)
return (ARCHIVE_FATAL);
}
hf->len_size = (int)len_size;
hf->tbl_bits = tbl_bits;
return (ARCHIVE_OK);
}

static void
lzh_huffman_free(struct huffman *hf)
{
free(hf->bitlen);
free(hf->tbl);
free(hf->tree);
}

static const char bitlen_tbl[0x400] = {
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 16, 0
};
static int
lzh_read_pt_bitlen(struct lzh_stream *strm, int start, int end)
{
struct lzh_dec *ds = strm->ds;
struct lzh_br *br = &(ds->br);
int c, i;

for (i = start; i < end; ) {












if (!lzh_br_read_ahead(strm, br, 3))
return (i);
if ((c = lzh_br_bits(br, 3)) == 7) {
if (!lzh_br_read_ahead(strm, br, 13))
return (i);
c = bitlen_tbl[lzh_br_bits(br, 13) & 0x3FF];
if (c)
lzh_br_consume(br, c - 3);
else
return (-1);
} else
lzh_br_consume(br, 3);
ds->pt.bitlen[i++] = c;
ds->pt.freq[c]++;
}
return (i);
}

static int
lzh_make_fake_table(struct huffman *hf, uint16_t c)
{
if (c >= hf->len_size)
return (0);
hf->tbl[0] = c;
hf->max_bits = 0;
hf->shift_bits = 0;
hf->bitlen[hf->tbl[0]] = 0;
return (1);
}




static int
lzh_make_huffman_table(struct huffman *hf)
{
uint16_t *tbl;
const unsigned char *bitlen;
int bitptn[17], weight[17];
int i, maxbits = 0, ptn, tbl_size, w;
int diffbits, len_avail;




ptn = 0;
for (i = 1, w = 1 << 15; i <= 16; i++, w >>= 1) {
bitptn[i] = ptn;
weight[i] = w;
if (hf->freq[i]) {
ptn += hf->freq[i] * w;
maxbits = i;
}
}
if (ptn != 0x10000 || maxbits > hf->tbl_bits)
return (0);

hf->max_bits = maxbits;






if (maxbits < 16) {
int ebits = 16 - maxbits;
for (i = 1; i <= maxbits; i++) {
bitptn[i] >>= ebits;
weight[i] >>= ebits;
}
}
if (maxbits > HTBL_BITS) {
unsigned htbl_max;
uint16_t *p;

diffbits = maxbits - HTBL_BITS;
for (i = 1; i <= HTBL_BITS; i++) {
bitptn[i] >>= diffbits;
weight[i] >>= diffbits;
}
htbl_max = bitptn[HTBL_BITS] +
weight[HTBL_BITS] * hf->freq[HTBL_BITS];
p = &(hf->tbl[htbl_max]);
while (p < &hf->tbl[1U<<HTBL_BITS])
*p++ = 0;
} else
diffbits = 0;
hf->shift_bits = diffbits;




tbl_size = 1 << HTBL_BITS;
tbl = hf->tbl;
bitlen = hf->bitlen;
len_avail = hf->len_avail;
hf->tree_used = 0;
for (i = 0; i < len_avail; i++) {
uint16_t *p;
int len, cnt;
uint16_t bit;
int extlen;
struct htree_t *ht;

if (bitlen[i] == 0)
continue;

len = bitlen[i];
ptn = bitptn[len];
cnt = weight[len];
if (len <= HTBL_BITS) {

if ((bitptn[len] = ptn + cnt) > tbl_size)
return (0);

p = &(tbl[ptn]);
if (cnt > 7) {
uint16_t *pc;

cnt -= 8;
pc = &p[cnt];
pc[0] = (uint16_t)i;
pc[1] = (uint16_t)i;
pc[2] = (uint16_t)i;
pc[3] = (uint16_t)i;
pc[4] = (uint16_t)i;
pc[5] = (uint16_t)i;
pc[6] = (uint16_t)i;
pc[7] = (uint16_t)i;
if (cnt > 7) {
cnt -= 8;
memcpy(&p[cnt], pc,
8 * sizeof(uint16_t));
pc = &p[cnt];
while (cnt > 15) {
cnt -= 16;
memcpy(&p[cnt], pc,
16 * sizeof(uint16_t));
}
}
if (cnt)
memcpy(p, pc, cnt * sizeof(uint16_t));
} else {
while (cnt > 1) {
p[--cnt] = (uint16_t)i;
p[--cnt] = (uint16_t)i;
}
if (cnt)
p[--cnt] = (uint16_t)i;
}
continue;
}





bitptn[len] = ptn + cnt;
bit = 1U << (diffbits -1);
extlen = len - HTBL_BITS;

p = &(tbl[ptn >> diffbits]);
if (*p == 0) {
*p = len_avail + hf->tree_used;
ht = &(hf->tree[hf->tree_used++]);
if (hf->tree_used > hf->tree_avail)
return (0);
ht->left = 0;
ht->right = 0;
} else {
if (*p < len_avail ||
*p >= (len_avail + hf->tree_used))
return (0);
ht = &(hf->tree[*p - len_avail]);
}
while (--extlen > 0) {
if (ptn & bit) {
if (ht->left < len_avail) {
ht->left = len_avail + hf->tree_used;
ht = &(hf->tree[hf->tree_used++]);
if (hf->tree_used > hf->tree_avail)
return (0);
ht->left = 0;
ht->right = 0;
} else {
ht = &(hf->tree[ht->left - len_avail]);
}
} else {
if (ht->right < len_avail) {
ht->right = len_avail + hf->tree_used;
ht = &(hf->tree[hf->tree_used++]);
if (hf->tree_used > hf->tree_avail)
return (0);
ht->left = 0;
ht->right = 0;
} else {
ht = &(hf->tree[ht->right - len_avail]);
}
}
bit >>= 1;
}
if (ptn & bit) {
if (ht->left != 0)
return (0);
ht->left = (uint16_t)i;
} else {
if (ht->right != 0)
return (0);
ht->right = (uint16_t)i;
}
}
return (1);
}

static int
lzh_decode_huffman_tree(struct huffman *hf, unsigned rbits, int c)
{
struct htree_t *ht;
int extlen;

ht = hf->tree;
extlen = hf->shift_bits;
while (c >= hf->len_avail) {
c -= hf->len_avail;
if (extlen-- <= 0 || c >= hf->tree_used)
return (0);
if (rbits & (1U << extlen))
c = ht[c].left;
else
c = ht[c].right;
}
return (c);
}

static inline int
lzh_decode_huffman(struct huffman *hf, unsigned rbits)
{
int c;




c = hf->tbl[rbits >> hf->shift_bits];
if (c < hf->len_avail || hf->len_avail == 0)
return (c);

return (lzh_decode_huffman_tree(hf, rbits, c));
}

