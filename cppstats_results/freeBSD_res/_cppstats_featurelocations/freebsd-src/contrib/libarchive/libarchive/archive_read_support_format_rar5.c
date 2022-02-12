
























#include "archive_platform.h"
#include "archive_endian.h"

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include <time.h>
#if defined(HAVE_ZLIB_H)
#include <zlib.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

#include "archive.h"
#if !defined(HAVE_ZLIB_H)
#include "archive_crc32.h"
#endif

#include "archive_entry.h"
#include "archive_entry_locale.h"
#include "archive_ppmd7_private.h"
#include "archive_entry_private.h"

#if defined(HAVE_BLAKE2_H)
#include <blake2.h>
#else
#include "archive_blake2.h"
#endif





#define rar5_min(a, b) (((a) > (b)) ? (b) : (a))
#define rar5_max(a, b) (((a) > (b)) ? (a) : (b))
#define rar5_countof(X) ((const ssize_t) (sizeof(X) / sizeof(*X)))

#if defined DEBUG
#define DEBUG_CODE if(1)
#define LOG(...) do { printf("rar5: " __VA_ARGS__); puts(""); } while(0)
#else
#define DEBUG_CODE if(0)
#endif













static unsigned char rar5_signature_xor[] = { 243, 192, 211, 128, 187, 166, 160, 161 };
static const size_t g_unpack_window_size = 0x20000;



#define MAX_NAME_IN_CHARS 2048
#define MAX_NAME_IN_BYTES (4 * MAX_NAME_IN_CHARS)

struct file_header {
ssize_t bytes_remaining;
ssize_t unpacked_size;
int64_t last_offset;
int64_t last_size;

uint8_t solid : 1;
uint8_t service : 1;
uint8_t eof : 1;
uint8_t dir : 1;


uint64_t e_mtime;
uint64_t e_ctime;
uint64_t e_atime;
uint32_t e_unix_ns;


uint32_t stored_crc32;
uint32_t calculated_crc32;
uint8_t blake2sp[32];
blake2sp_state b2state;
char has_blake2;


uint64_t redir_type;
uint64_t redir_flags;

ssize_t solid_window_size;
};

enum EXTRA {
EX_CRYPT = 0x01,
EX_HASH = 0x02,
EX_HTIME = 0x03,
EX_VERSION = 0x04,
EX_REDIR = 0x05,
EX_UOWNER = 0x06,
EX_SUBDATA = 0x07
};

#define REDIR_SYMLINK_IS_DIR 1

enum REDIR_TYPE {
REDIR_TYPE_NONE = 0,
REDIR_TYPE_UNIXSYMLINK = 1,
REDIR_TYPE_WINSYMLINK = 2,
REDIR_TYPE_JUNCTION = 3,
REDIR_TYPE_HARDLINK = 4,
REDIR_TYPE_FILECOPY = 5,
};

#define OWNER_USER_NAME 0x01
#define OWNER_GROUP_NAME 0x02
#define OWNER_USER_UID 0x04
#define OWNER_GROUP_GID 0x08
#define OWNER_MAXNAMELEN 256

enum FILTER_TYPE {
FILTER_DELTA = 0,
FILTER_E8 = 1,
FILTER_E8E9 = 2,
FILTER_ARM = 3,
FILTER_AUDIO = 4,
FILTER_RGB = 5,
FILTER_ITANIUM = 6,
FILTER_PPM = 7,

FILTER_NONE = 8,
};

struct filter_info {
int type;
int channels;
int pos_r;

int64_t block_start;
ssize_t block_length;
uint16_t width;
};

struct data_ready {
char used;
const uint8_t* buf;
size_t size;
int64_t offset;
};

struct cdeque {
uint16_t beg_pos;
uint16_t end_pos;
uint16_t cap_mask;
uint16_t size;
size_t* arr;
};

struct decode_table {
uint32_t size;
int32_t decode_len[16];
uint32_t decode_pos[16];
uint32_t quick_bits;
uint8_t quick_len[1 << 10];
uint16_t quick_num[1 << 10];
uint16_t decode_num[306];
};

struct comp_state {


uint8_t initialized : 1;


uint8_t all_filters_applied : 1;



uint8_t switch_multivolume : 1;



uint8_t block_parsing_finished : 1;

signed int notused : 4;

int flags;
int method;
int version;
ssize_t window_size;
uint8_t* window_buf;

uint8_t* filtered_buf;
const uint8_t* block_buf;
size_t window_mask;
int64_t write_ptr;

int64_t last_write_ptr;

int64_t last_unstore_ptr;




int64_t solid_offset;


ssize_t cur_block_size;
int last_len;



#define HUFF_BC 20
struct decode_table bd;
#define HUFF_NC 306
struct decode_table ld;
#define HUFF_DC 64
struct decode_table dd;
#define HUFF_LDC 16
struct decode_table ldd;
#define HUFF_RC 44
struct decode_table rd;
#define HUFF_TABLE_SIZE (HUFF_NC + HUFF_DC + HUFF_RC + HUFF_LDC)


struct cdeque filters;
int64_t last_block_start;
ssize_t last_block_length;


int dist_cache[4];


struct data_ready dready[2];
};


struct bit_reader {
int8_t bit_addr;
int in_addr;
};



struct compressed_block_header {







uint8_t block_flags_u8;
uint8_t block_cksum;
};


struct main_header {

uint8_t solid : 1;


uint8_t volume : 1;
uint8_t endarc : 1;
uint8_t notused : 5;

unsigned int vol_no;
};

struct generic_header {
uint8_t split_after : 1;
uint8_t split_before : 1;
uint8_t padding : 6;
int size;
int last_header_id;
};

struct multivolume {
unsigned int expected_vol_no;
uint8_t* push_buf;
};


struct rar5 {
int header_initialized;



int skipped_magic;





int skip_mode;






int merge_mode;





uint64_t qlist_offset;





uint64_t rr_offset;


struct generic_header generic;
struct main_header main;
struct comp_state cstate;
struct file_header file;
struct bit_reader bits;
struct multivolume vol;



struct compressed_block_header last_block_hdr;
};



static void rar5_signature(char *buf);
static int verify_global_checksums(struct archive_read* a);
static int rar5_read_data_skip(struct archive_read *a);
static int push_data_ready(struct archive_read* a, struct rar5* rar,
const uint8_t* buf, size_t size, int64_t offset);


enum CDE_RETURN_VALUES {
CDE_OK, CDE_ALLOC, CDE_PARAM, CDE_OUT_OF_BOUNDS,
};


static void cdeque_clear(struct cdeque* d) {
d->size = 0;
d->beg_pos = 0;
d->end_pos = 0;
}




static int cdeque_init(struct cdeque* d, int max_capacity_power_of_2) {
if(d == NULL || max_capacity_power_of_2 == 0)
return CDE_PARAM;

d->cap_mask = max_capacity_power_of_2 - 1;
d->arr = NULL;

if((max_capacity_power_of_2 & d->cap_mask) != 0)
return CDE_PARAM;

cdeque_clear(d);
d->arr = malloc(sizeof(void*) * max_capacity_power_of_2);

return d->arr ? CDE_OK : CDE_ALLOC;
}


static size_t cdeque_size(struct cdeque* d) {
return d->size;
}




static void cdeque_front_fast(struct cdeque* d, void** value) {
*value = (void*) d->arr[d->beg_pos];
}



static int cdeque_front(struct cdeque* d, void** value) {
if(d->size > 0) {
cdeque_front_fast(d, value);
return CDE_OK;
} else
return CDE_OUT_OF_BOUNDS;
}



static int cdeque_push_back(struct cdeque* d, void* item) {
if(d == NULL)
return CDE_PARAM;

if(d->size == d->cap_mask + 1)
return CDE_OUT_OF_BOUNDS;

d->arr[d->end_pos] = (size_t) item;
d->end_pos = (d->end_pos + 1) & d->cap_mask;
d->size++;

return CDE_OK;
}



static void cdeque_pop_front_fast(struct cdeque* d, void** value) {
*value = (void*) d->arr[d->beg_pos];
d->beg_pos = (d->beg_pos + 1) & d->cap_mask;
d->size--;
}



static int cdeque_pop_front(struct cdeque* d, void** value) {
if(!d || !value)
return CDE_PARAM;

if(d->size == 0)
return CDE_OUT_OF_BOUNDS;

cdeque_pop_front_fast(d, value);
return CDE_OK;
}


static void** cdeque_filter_p(struct filter_info** f) {
return (void**) (size_t) f;
}


static void* cdeque_filter(struct filter_info* f) {
return (void**) (size_t) f;
}




static void cdeque_free(struct cdeque* d) {
if(!d)
return;

if(!d->arr)
return;

free(d->arr);

d->arr = NULL;
d->beg_pos = -1;
d->end_pos = -1;
d->cap_mask = 0;
}

static inline
uint8_t bf_bit_size(const struct compressed_block_header* hdr) {
return hdr->block_flags_u8 & 7;
}

static inline
uint8_t bf_byte_count(const struct compressed_block_header* hdr) {
return (hdr->block_flags_u8 >> 3) & 7;
}

static inline
uint8_t bf_is_table_present(const struct compressed_block_header* hdr) {
return (hdr->block_flags_u8 >> 7) & 1;
}

static inline struct rar5* get_context(struct archive_read* a) {
return (struct rar5*) a->format->data;
}


static void circular_memcpy(uint8_t* dst, uint8_t* window, const uint64_t mask,
int64_t start, int64_t end)
{
if((start & mask) > (end & mask)) {
ssize_t len1 = mask + 1 - (start & mask);
ssize_t len2 = end & mask;

memcpy(dst, &window[start & mask], len1);
memcpy(dst + len1, window, len2);
} else {
memcpy(dst, &window[start & mask], (size_t) (end - start));
}
}

static uint32_t read_filter_data(struct rar5* rar, uint32_t offset) {
uint8_t linear_buf[4];
circular_memcpy(linear_buf, rar->cstate.window_buf,
rar->cstate.window_mask, offset, offset + 4);
return archive_le32dec(linear_buf);
}

static void write_filter_data(struct rar5* rar, uint32_t offset,
uint32_t value)
{
archive_le32enc(&rar->cstate.filtered_buf[offset], value);
}


static struct filter_info* add_new_filter(struct rar5* rar) {
struct filter_info* f =
(struct filter_info*) calloc(1, sizeof(struct filter_info));

if(!f) {
return NULL;
}

cdeque_push_back(&rar->cstate.filters, cdeque_filter(f));
return f;
}

static int run_delta_filter(struct rar5* rar, struct filter_info* flt) {
int i;
ssize_t dest_pos, src_pos = 0;

for(i = 0; i < flt->channels; i++) {
uint8_t prev_byte = 0;
for(dest_pos = i;
dest_pos < flt->block_length;
dest_pos += flt->channels)
{
uint8_t byte;

byte = rar->cstate.window_buf[
(rar->cstate.solid_offset + flt->block_start +
src_pos) & rar->cstate.window_mask];

prev_byte -= byte;
rar->cstate.filtered_buf[dest_pos] = prev_byte;
src_pos++;
}
}

return ARCHIVE_OK;
}

static int run_e8e9_filter(struct rar5* rar, struct filter_info* flt,
int extended)
{
const uint32_t file_size = 0x1000000;
ssize_t i;

circular_memcpy(rar->cstate.filtered_buf,
rar->cstate.window_buf, rar->cstate.window_mask,
rar->cstate.solid_offset + flt->block_start,
rar->cstate.solid_offset + flt->block_start + flt->block_length);

for(i = 0; i < flt->block_length - 4;) {
uint8_t b = rar->cstate.window_buf[
(rar->cstate.solid_offset + flt->block_start +
i++) & rar->cstate.window_mask];





if(b == 0xE8 || (extended && b == 0xE9)) {

uint32_t addr;
uint32_t offset = (i + flt->block_start) % file_size;

addr = read_filter_data(rar,
(uint32_t)(rar->cstate.solid_offset +
flt->block_start + i) & rar->cstate.window_mask);

if(addr & 0x80000000) {
if(((addr + offset) & 0x80000000) == 0) {
write_filter_data(rar, (uint32_t)i,
addr + file_size);
}
} else {
if((addr - file_size) & 0x80000000) {
uint32_t naddr = addr - offset;
write_filter_data(rar, (uint32_t)i,
naddr);
}
}

i += 4;
}
}

return ARCHIVE_OK;
}

static int run_arm_filter(struct rar5* rar, struct filter_info* flt) {
ssize_t i = 0;
uint32_t offset;

circular_memcpy(rar->cstate.filtered_buf,
rar->cstate.window_buf, rar->cstate.window_mask,
rar->cstate.solid_offset + flt->block_start,
rar->cstate.solid_offset + flt->block_start + flt->block_length);

for(i = 0; i < flt->block_length - 3; i += 4) {
uint8_t* b = &rar->cstate.window_buf[
(rar->cstate.solid_offset +
flt->block_start + i + 3) & rar->cstate.window_mask];

if(*b == 0xEB) {

offset = read_filter_data(rar,
(rar->cstate.solid_offset + flt->block_start + i) &
rar->cstate.window_mask) & 0x00ffffff;

offset -= (uint32_t) ((i + flt->block_start) / 4);
offset = (offset & 0x00ffffff) | 0xeb000000;
write_filter_data(rar, (uint32_t)i, offset);
}
}

return ARCHIVE_OK;
}

static int run_filter(struct archive_read* a, struct filter_info* flt) {
int ret;
struct rar5* rar = get_context(a);

free(rar->cstate.filtered_buf);

rar->cstate.filtered_buf = malloc(flt->block_length);
if(!rar->cstate.filtered_buf) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory for filter data.");
return ARCHIVE_FATAL;
}

switch(flt->type) {
case FILTER_DELTA:
ret = run_delta_filter(rar, flt);
break;

case FILTER_E8:

case FILTER_E8E9:
ret = run_e8e9_filter(rar, flt,
flt->type == FILTER_E8E9);
break;

case FILTER_ARM:
ret = run_arm_filter(rar, flt);
break;

default:
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Unsupported filter type: 0x%x", flt->type);
return ARCHIVE_FATAL;
}

if(ret != ARCHIVE_OK) {

return ret;
}

if(ARCHIVE_OK != push_data_ready(a, rar, rar->cstate.filtered_buf,
flt->block_length, rar->cstate.last_write_ptr))
{
archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Stack overflow when submitting unpacked data");

return ARCHIVE_FATAL;
}

rar->cstate.last_write_ptr += flt->block_length;
return ARCHIVE_OK;
}





static void push_data(struct archive_read* a, struct rar5* rar,
const uint8_t* buf, int64_t idx_begin, int64_t idx_end)
{
const uint64_t wmask = rar->cstate.window_mask;
const ssize_t solid_write_ptr = (rar->cstate.solid_offset +
rar->cstate.last_write_ptr) & wmask;

idx_begin += rar->cstate.solid_offset;
idx_end += rar->cstate.solid_offset;






if((idx_begin & wmask) > (idx_end & wmask)) {


const ssize_t frag1_size = rar->cstate.window_size -
(idx_begin & wmask);
const ssize_t frag2_size = idx_end & wmask;


push_data_ready(a, rar, buf + solid_write_ptr, frag1_size,
rar->cstate.last_write_ptr);


push_data_ready(a, rar, buf, frag2_size,
rar->cstate.last_write_ptr + frag1_size);

rar->cstate.last_write_ptr += frag1_size + frag2_size;
} else {


push_data_ready(a, rar,
buf + solid_write_ptr, (idx_end - idx_begin) & wmask,
rar->cstate.last_write_ptr);

rar->cstate.last_write_ptr += idx_end - idx_begin;
}
}



static void push_window_data(struct archive_read* a, struct rar5* rar,
int64_t idx_begin, int64_t idx_end)
{
push_data(a, rar, rar->cstate.window_buf, idx_begin, idx_end);
}

static int apply_filters(struct archive_read* a) {
struct filter_info* flt;
struct rar5* rar = get_context(a);
int ret;

rar->cstate.all_filters_applied = 0;



if(CDE_OK == cdeque_front(&rar->cstate.filters,
cdeque_filter_p(&flt))) {


if(rar->cstate.write_ptr > flt->block_start &&
rar->cstate.write_ptr >= flt->block_start +
flt->block_length) {


if(rar->cstate.last_write_ptr == flt->block_start) {


ret = run_filter(a, flt);
if(ret != ARCHIVE_OK) {

return ret;
}




(void) cdeque_pop_front(&rar->cstate.filters,
cdeque_filter_p(&flt));

free(flt);
} else {


push_window_data(a, rar,
rar->cstate.last_write_ptr,
flt->block_start);
}



return ARCHIVE_RETRY;
}
}

rar->cstate.all_filters_applied = 1;
return ARCHIVE_OK;
}

static void dist_cache_push(struct rar5* rar, int value) {
int* q = rar->cstate.dist_cache;

q[3] = q[2];
q[2] = q[1];
q[1] = q[0];
q[0] = value;
}

static int dist_cache_touch(struct rar5* rar, int idx) {
int* q = rar->cstate.dist_cache;
int i, dist = q[idx];

for(i = idx; i > 0; i--)
q[i] = q[i - 1];

q[0] = dist;
return dist;
}

static void free_filters(struct rar5* rar) {
struct cdeque* d = &rar->cstate.filters;








while(cdeque_size(d) > 0) {
struct filter_info* f = NULL;


if (CDE_OK == cdeque_pop_front(d, cdeque_filter_p(&f)))
free(f);
}

cdeque_clear(d);


rar->cstate.last_block_start = 0;
rar->cstate.last_block_length = 0;
}

static void reset_file_context(struct rar5* rar) {
memset(&rar->file, 0, sizeof(rar->file));
blake2sp_init(&rar->file.b2state, 32);

if(rar->main.solid) {
rar->cstate.solid_offset += rar->cstate.write_ptr;
} else {
rar->cstate.solid_offset = 0;
}

rar->cstate.write_ptr = 0;
rar->cstate.last_write_ptr = 0;
rar->cstate.last_unstore_ptr = 0;

rar->file.redir_type = REDIR_TYPE_NONE;
rar->file.redir_flags = 0;

free_filters(rar);
}

static inline int get_archive_read(struct archive* a,
struct archive_read** ar)
{
*ar = (struct archive_read*) a;
archive_check_magic(a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
"archive_read_support_format_rar5");

return ARCHIVE_OK;
}

static int read_ahead(struct archive_read* a, size_t how_many,
const uint8_t** ptr)
{
ssize_t avail = -1;
if(!ptr)
return 0;

*ptr = __archive_read_ahead(a, how_many, &avail);
if(*ptr == NULL) {
return 0;
}

return 1;
}

static int consume(struct archive_read* a, int64_t how_many) {
int ret;

ret = how_many == __archive_read_consume(a, how_many)
? ARCHIVE_OK
: ARCHIVE_FATAL;

return ret;
}


















static int read_var(struct archive_read* a, uint64_t* pvalue,
uint64_t* pvalue_len)
{
uint64_t result = 0;
size_t shift, i;
const uint8_t* p;
uint8_t b;




if(!read_ahead(a, 8, &p))
return 0;

for(shift = 0, i = 0; i < 8; i++, shift += 7) {
b = p[i];



result += (b & (uint64_t)0x7F) << shift;





if((b & 0x80) == 0) {
if(pvalue) {
*pvalue = result;
}





if(pvalue_len) {
*pvalue_len = 1 + i;
} else {






if(ARCHIVE_OK != consume(a, 1 + i)) {
return 0;
}
}


return 1;
}
}




if(pvalue) {
*pvalue = result;
}

if(pvalue_len) {
*pvalue_len = 9;
} else {
if(ARCHIVE_OK != consume(a, 9)) {
return 0;
}
}

return 1;
}

static int read_var_sized(struct archive_read* a, size_t* pvalue,
size_t* pvalue_len)
{
uint64_t v;
uint64_t v_size = 0;

const int ret = pvalue_len ? read_var(a, &v, &v_size)
: read_var(a, &v, NULL);

if(ret == 1 && pvalue) {
*pvalue = (size_t) v;
}

if(pvalue_len) {

*pvalue_len = (size_t) v_size;
}

return ret;
}

static int read_bits_32(struct rar5* rar, const uint8_t* p, uint32_t* value) {
uint32_t bits = ((uint32_t) p[rar->bits.in_addr]) << 24;
bits |= p[rar->bits.in_addr + 1] << 16;
bits |= p[rar->bits.in_addr + 2] << 8;
bits |= p[rar->bits.in_addr + 3];
bits <<= rar->bits.bit_addr;
bits |= p[rar->bits.in_addr + 4] >> (8 - rar->bits.bit_addr);
*value = bits;
return ARCHIVE_OK;
}

static int read_bits_16(struct rar5* rar, const uint8_t* p, uint16_t* value) {
int bits = (int) ((uint32_t) p[rar->bits.in_addr]) << 16;
bits |= (int) p[rar->bits.in_addr + 1] << 8;
bits |= (int) p[rar->bits.in_addr + 2];
bits >>= (8 - rar->bits.bit_addr);
*value = bits & 0xffff;
return ARCHIVE_OK;
}

static void skip_bits(struct rar5* rar, int bits) {
const int new_bits = rar->bits.bit_addr + bits;
rar->bits.in_addr += new_bits >> 3;
rar->bits.bit_addr = new_bits & 7;
}


static int read_consume_bits(struct rar5* rar, const uint8_t* p, int n,
int* value)
{
uint16_t v;
int ret, num;

if(n == 0 || n > 16) {


return ARCHIVE_FATAL;
}

ret = read_bits_16(rar, p, &v);
if(ret != ARCHIVE_OK)
return ret;

num = (int) v;
num >>= 16 - n;

skip_bits(rar, n);

if(value)
*value = num;

return ARCHIVE_OK;
}

static int read_u32(struct archive_read* a, uint32_t* pvalue) {
const uint8_t* p;
if(!read_ahead(a, 4, &p))
return 0;

*pvalue = archive_le32dec(p);
return ARCHIVE_OK == consume(a, 4) ? 1 : 0;
}

static int read_u64(struct archive_read* a, uint64_t* pvalue) {
const uint8_t* p;
if(!read_ahead(a, 8, &p))
return 0;

*pvalue = archive_le64dec(p);
return ARCHIVE_OK == consume(a, 8) ? 1 : 0;
}

static int bid_standard(struct archive_read* a) {
const uint8_t* p;
char signature[sizeof(rar5_signature_xor)];

rar5_signature(signature);

if(!read_ahead(a, sizeof(rar5_signature_xor), &p))
return -1;

if(!memcmp(signature, p, sizeof(rar5_signature_xor)))
return 30;

return -1;
}

static int rar5_bid(struct archive_read* a, int best_bid) {
int my_bid;

if(best_bid > 30)
return -1;

my_bid = bid_standard(a);
if(my_bid > -1) {
return my_bid;
}

return -1;
}

static int rar5_options(struct archive_read *a, const char *key,
const char *val) {
(void) a;
(void) key;
(void) val;





return ARCHIVE_WARN;
}

static void init_header(struct archive_read* a) {
a->archive.archive_format = ARCHIVE_FORMAT_RAR_V5;
a->archive.archive_format_name = "RAR5";
}

static void init_window_mask(struct rar5* rar) {
if (rar->cstate.window_size)
rar->cstate.window_mask = rar->cstate.window_size - 1;
else
rar->cstate.window_mask = 0;
}

enum HEADER_FLAGS {
HFL_EXTRA_DATA = 0x0001,
HFL_DATA = 0x0002,
HFL_SKIP_IF_UNKNOWN = 0x0004,
HFL_SPLIT_BEFORE = 0x0008,
HFL_SPLIT_AFTER = 0x0010,
HFL_CHILD = 0x0020,
HFL_INHERITED = 0x0040
};

static int process_main_locator_extra_block(struct archive_read* a,
struct rar5* rar)
{
uint64_t locator_flags;

enum LOCATOR_FLAGS {
QLIST = 0x01, RECOVERY = 0x02,
};

if(!read_var(a, &locator_flags, NULL)) {
return ARCHIVE_EOF;
}

if(locator_flags & QLIST) {
if(!read_var(a, &rar->qlist_offset, NULL)) {
return ARCHIVE_EOF;
}


}

if(locator_flags & RECOVERY) {
if(!read_var(a, &rar->rr_offset, NULL)) {
return ARCHIVE_EOF;
}


}

return ARCHIVE_OK;
}

static int parse_file_extra_hash(struct archive_read* a, struct rar5* rar,
ssize_t* extra_data_size)
{
size_t hash_type = 0;
size_t value_len;

enum HASH_TYPE {
BLAKE2sp = 0x00
};

if(!read_var_sized(a, &hash_type, &value_len))
return ARCHIVE_EOF;

*extra_data_size -= value_len;
if(ARCHIVE_OK != consume(a, value_len)) {
return ARCHIVE_EOF;
}



if(hash_type == BLAKE2sp) {
const uint8_t* p;
const int hash_size = sizeof(rar->file.blake2sp);

if(!read_ahead(a, hash_size, &p))
return ARCHIVE_EOF;

rar->file.has_blake2 = 1;
memcpy(&rar->file.blake2sp, p, hash_size);

if(ARCHIVE_OK != consume(a, hash_size)) {
return ARCHIVE_EOF;
}

*extra_data_size -= hash_size;
} else {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Unsupported hash type (0x%x)", (int) hash_type);
return ARCHIVE_FATAL;
}

return ARCHIVE_OK;
}

static uint64_t time_win_to_unix(uint64_t win_time) {
const size_t ns_in_sec = 10000000;
const uint64_t sec_to_unix = 11644473600LL;
return win_time / ns_in_sec - sec_to_unix;
}

static int parse_htime_item(struct archive_read* a, char unix_time,
uint64_t* where, ssize_t* extra_data_size)
{
if(unix_time) {
uint32_t time_val;
if(!read_u32(a, &time_val))
return ARCHIVE_EOF;

*extra_data_size -= 4;
*where = (uint64_t) time_val;
} else {
uint64_t windows_time;
if(!read_u64(a, &windows_time))
return ARCHIVE_EOF;

*where = time_win_to_unix(windows_time);
*extra_data_size -= 8;
}

return ARCHIVE_OK;
}

static int parse_file_extra_version(struct archive_read* a,
struct archive_entry* e, ssize_t* extra_data_size)
{
size_t flags = 0;
size_t version = 0;
size_t value_len = 0;
struct archive_string version_string;
struct archive_string name_utf8_string;
const char* cur_filename;


if(!read_var_sized(a, &flags, &value_len))
return ARCHIVE_EOF;

*extra_data_size -= value_len;
if(ARCHIVE_OK != consume(a, value_len))
return ARCHIVE_EOF;

if(!read_var_sized(a, &version, &value_len))
return ARCHIVE_EOF;

*extra_data_size -= value_len;
if(ARCHIVE_OK != consume(a, value_len))
return ARCHIVE_EOF;



cur_filename = archive_entry_pathname_utf8(e);
if(cur_filename == NULL) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Version entry without file name");
return ARCHIVE_FATAL;
}

archive_string_init(&version_string);
archive_string_init(&name_utf8_string);



archive_string_sprintf(&version_string, ";%zu", version);


archive_strcat(&name_utf8_string, cur_filename);
archive_strcat(&name_utf8_string, version_string.s);


archive_entry_update_pathname_utf8(e, name_utf8_string.s);


archive_string_free(&version_string);
archive_string_free(&name_utf8_string);
return ARCHIVE_OK;
}

static int parse_file_extra_htime(struct archive_read* a,
struct archive_entry* e, struct rar5* rar, ssize_t* extra_data_size)
{
char unix_time = 0;
size_t flags = 0;
size_t value_len;

enum HTIME_FLAGS {
IS_UNIX = 0x01,
HAS_MTIME = 0x02,
HAS_CTIME = 0x04,
HAS_ATIME = 0x08,
HAS_UNIX_NS = 0x10,
};

if(!read_var_sized(a, &flags, &value_len))
return ARCHIVE_EOF;

*extra_data_size -= value_len;
if(ARCHIVE_OK != consume(a, value_len)) {
return ARCHIVE_EOF;
}

unix_time = flags & IS_UNIX;

if(flags & HAS_MTIME) {
parse_htime_item(a, unix_time, &rar->file.e_mtime,
extra_data_size);
archive_entry_set_mtime(e, rar->file.e_mtime, 0);
}

if(flags & HAS_CTIME) {
parse_htime_item(a, unix_time, &rar->file.e_ctime,
extra_data_size);
archive_entry_set_ctime(e, rar->file.e_ctime, 0);
}

if(flags & HAS_ATIME) {
parse_htime_item(a, unix_time, &rar->file.e_atime,
extra_data_size);
archive_entry_set_atime(e, rar->file.e_atime, 0);
}

if(flags & HAS_UNIX_NS) {
if(!read_u32(a, &rar->file.e_unix_ns))
return ARCHIVE_EOF;

*extra_data_size -= 4;
}

return ARCHIVE_OK;
}

static int parse_file_extra_redir(struct archive_read* a,
struct archive_entry* e, struct rar5* rar, ssize_t* extra_data_size)
{
uint64_t value_size = 0;
size_t target_size = 0;
char target_utf8_buf[MAX_NAME_IN_BYTES];
const uint8_t* p;

if(!read_var(a, &rar->file.redir_type, &value_size))
return ARCHIVE_EOF;
if(ARCHIVE_OK != consume(a, (int64_t)value_size))
return ARCHIVE_EOF;
*extra_data_size -= value_size;

if(!read_var(a, &rar->file.redir_flags, &value_size))
return ARCHIVE_EOF;
if(ARCHIVE_OK != consume(a, (int64_t)value_size))
return ARCHIVE_EOF;
*extra_data_size -= value_size;

if(!read_var_sized(a, &target_size, NULL))
return ARCHIVE_EOF;
*extra_data_size -= target_size + 1;

if(!read_ahead(a, target_size, &p))
return ARCHIVE_EOF;

if(target_size > (MAX_NAME_IN_CHARS - 1)) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Link target is too long");
return ARCHIVE_FATAL;
}

if(target_size == 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"No link target specified");
return ARCHIVE_FATAL;
}

memcpy(target_utf8_buf, p, target_size);
target_utf8_buf[target_size] = 0;

if(ARCHIVE_OK != consume(a, (int64_t)target_size))
return ARCHIVE_EOF;

switch(rar->file.redir_type) {
case REDIR_TYPE_UNIXSYMLINK:
case REDIR_TYPE_WINSYMLINK:
archive_entry_set_filetype(e, AE_IFLNK);
archive_entry_update_symlink_utf8(e, target_utf8_buf);
if (rar->file.redir_flags & REDIR_SYMLINK_IS_DIR) {
archive_entry_set_symlink_type(e,
AE_SYMLINK_TYPE_DIRECTORY);
} else {
archive_entry_set_symlink_type(e,
AE_SYMLINK_TYPE_FILE);
}
break;

case REDIR_TYPE_HARDLINK:
archive_entry_set_filetype(e, AE_IFREG);
archive_entry_update_hardlink_utf8(e, target_utf8_buf);
break;

default:

break;
}
return ARCHIVE_OK;
}

static int parse_file_extra_owner(struct archive_read* a,
struct archive_entry* e, ssize_t* extra_data_size)
{
uint64_t flags = 0;
uint64_t value_size = 0;
uint64_t id = 0;
size_t name_len = 0;
size_t name_size = 0;
char namebuf[OWNER_MAXNAMELEN];
const uint8_t* p;

if(!read_var(a, &flags, &value_size))
return ARCHIVE_EOF;
if(ARCHIVE_OK != consume(a, (int64_t)value_size))
return ARCHIVE_EOF;
*extra_data_size -= value_size;

if ((flags & OWNER_USER_NAME) != 0) {
if(!read_var_sized(a, &name_size, NULL))
return ARCHIVE_EOF;
*extra_data_size -= name_size + 1;

if(!read_ahead(a, name_size, &p))
return ARCHIVE_EOF;

if (name_size >= OWNER_MAXNAMELEN) {
name_len = OWNER_MAXNAMELEN - 1;
} else {
name_len = name_size;
}

memcpy(namebuf, p, name_len);
namebuf[name_len] = 0;
if(ARCHIVE_OK != consume(a, (int64_t)name_size))
return ARCHIVE_EOF;

archive_entry_set_uname(e, namebuf);
}
if ((flags & OWNER_GROUP_NAME) != 0) {
if(!read_var_sized(a, &name_size, NULL))
return ARCHIVE_EOF;
*extra_data_size -= name_size + 1;

if(!read_ahead(a, name_size, &p))
return ARCHIVE_EOF;

if (name_size >= OWNER_MAXNAMELEN) {
name_len = OWNER_MAXNAMELEN - 1;
} else {
name_len = name_size;
}

memcpy(namebuf, p, name_len);
namebuf[name_len] = 0;
if(ARCHIVE_OK != consume(a, (int64_t)name_size))
return ARCHIVE_EOF;

archive_entry_set_gname(e, namebuf);
}
if ((flags & OWNER_USER_UID) != 0) {
if(!read_var(a, &id, &value_size))
return ARCHIVE_EOF;
if(ARCHIVE_OK != consume(a, (int64_t)value_size))
return ARCHIVE_EOF;
*extra_data_size -= value_size;

archive_entry_set_uid(e, (la_int64_t)id);
}
if ((flags & OWNER_GROUP_GID) != 0) {
if(!read_var(a, &id, &value_size))
return ARCHIVE_EOF;
if(ARCHIVE_OK != consume(a, (int64_t)value_size))
return ARCHIVE_EOF;
*extra_data_size -= value_size;

archive_entry_set_gid(e, (la_int64_t)id);
}
return ARCHIVE_OK;
}

static int process_head_file_extra(struct archive_read* a,
struct archive_entry* e, struct rar5* rar, ssize_t extra_data_size)
{
size_t extra_field_size;
size_t extra_field_id = 0;
int ret = ARCHIVE_FATAL;
size_t var_size;

while(extra_data_size > 0) {
if(!read_var_sized(a, &extra_field_size, &var_size))
return ARCHIVE_EOF;

extra_data_size -= var_size;
if(ARCHIVE_OK != consume(a, var_size)) {
return ARCHIVE_EOF;
}

if(!read_var_sized(a, &extra_field_id, &var_size))
return ARCHIVE_EOF;

extra_data_size -= var_size;
if(ARCHIVE_OK != consume(a, var_size)) {
return ARCHIVE_EOF;
}

switch(extra_field_id) {
case EX_HASH:
ret = parse_file_extra_hash(a, rar,
&extra_data_size);
break;
case EX_HTIME:
ret = parse_file_extra_htime(a, e, rar,
&extra_data_size);
break;
case EX_REDIR:
ret = parse_file_extra_redir(a, e, rar,
&extra_data_size);
break;
case EX_UOWNER:
ret = parse_file_extra_owner(a, e,
&extra_data_size);
break;
case EX_VERSION:
ret = parse_file_extra_version(a, e,
&extra_data_size);
break;
case EX_CRYPT:

case EX_SUBDATA:

default:

return consume(a, extra_data_size);
}
}

if(ret != ARCHIVE_OK) {

return ret;
}

return ARCHIVE_OK;
}

static int process_head_file(struct archive_read* a, struct rar5* rar,
struct archive_entry* entry, size_t block_flags)
{
ssize_t extra_data_size = 0;
size_t data_size = 0;
size_t file_flags = 0;
size_t file_attr = 0;
size_t compression_info = 0;
size_t host_os = 0;
size_t name_size = 0;
uint64_t unpacked_size, window_size;
uint32_t mtime = 0, crc = 0;
int c_method = 0, c_version = 0;
char name_utf8_buf[MAX_NAME_IN_BYTES];
const uint8_t* p;

enum FILE_FLAGS {
DIRECTORY = 0x0001, UTIME = 0x0002, CRC32 = 0x0004,
UNKNOWN_UNPACKED_SIZE = 0x0008,
};

enum FILE_ATTRS {
ATTR_READONLY = 0x1, ATTR_HIDDEN = 0x2, ATTR_SYSTEM = 0x4,
ATTR_DIRECTORY = 0x10,
};

enum COMP_INFO_FLAGS {
SOLID = 0x0040,
};

enum HOST_OS {
HOST_WINDOWS = 0,
HOST_UNIX = 1,
};

archive_entry_clear(entry);


if(!rar->cstate.switch_multivolume) {
reset_file_context(rar);
}

if(block_flags & HFL_EXTRA_DATA) {
size_t edata_size = 0;
if(!read_var_sized(a, &edata_size, NULL))
return ARCHIVE_EOF;


extra_data_size = (ssize_t) edata_size;
}

if(block_flags & HFL_DATA) {
if(!read_var_sized(a, &data_size, NULL))
return ARCHIVE_EOF;

rar->file.bytes_remaining = data_size;
} else {
rar->file.bytes_remaining = 0;

archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"no data found in file/service block");
return ARCHIVE_FATAL;
}

if(!read_var_sized(a, &file_flags, NULL))
return ARCHIVE_EOF;

if(!read_var(a, &unpacked_size, NULL))
return ARCHIVE_EOF;

if(file_flags & UNKNOWN_UNPACKED_SIZE) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Files with unknown unpacked size are not supported");
return ARCHIVE_FATAL;
}

rar->file.dir = (uint8_t) ((file_flags & DIRECTORY) > 0);

if(!read_var_sized(a, &file_attr, NULL))
return ARCHIVE_EOF;

if(file_flags & UTIME) {
if(!read_u32(a, &mtime))
return ARCHIVE_EOF;
}

if(file_flags & CRC32) {
if(!read_u32(a, &crc))
return ARCHIVE_EOF;
}

if(!read_var_sized(a, &compression_info, NULL))
return ARCHIVE_EOF;

c_method = (int) (compression_info >> 7) & 0x7;
c_version = (int) (compression_info & 0x3f);


window_size = (rar->file.dir > 0) ?
0 :
g_unpack_window_size << ((compression_info >> 10) & 15);
rar->cstate.method = c_method;
rar->cstate.version = c_version + 50;
rar->file.solid = (compression_info & SOLID) > 0;




if(rar->file.solid > 0 && rar->cstate.window_buf == NULL) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Declared solid file, but no window buffer "
"initialized yet.");
return ARCHIVE_FATAL;
}



if(window_size > (64 * 1024 * 1024) ||
(rar->file.dir == 0 && window_size == 0))
{
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Declared dictionary size is not supported.");
return ARCHIVE_FATAL;
}

if(rar->file.solid > 0) {


if(rar->file.solid_window_size > 0 &&
rar->file.solid_window_size != (ssize_t) window_size)
{
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Window size for this solid file doesn't match "
"the window size used in previous solid file. ");
return ARCHIVE_FATAL;
}
}



if(rar->cstate.switch_multivolume == 0) {


rar->cstate.window_size = (ssize_t) window_size;
}

if(rar->file.solid > 0 && rar->file.solid_window_size == 0) {



rar->file.solid_window_size = rar->cstate.window_size;
}

init_window_mask(rar);

rar->file.service = 0;

if(!read_var_sized(a, &host_os, NULL))
return ARCHIVE_EOF;

if(host_os == HOST_WINDOWS) {


__LA_MODE_T mode;

if(file_attr & ATTR_DIRECTORY) {
if (file_attr & ATTR_READONLY) {
mode = 0555 | AE_IFDIR;
} else {
mode = 0755 | AE_IFDIR;
}
} else {
if (file_attr & ATTR_READONLY) {
mode = 0444 | AE_IFREG;
} else {
mode = 0644 | AE_IFREG;
}
}

archive_entry_set_mode(entry, mode);

if (file_attr & (ATTR_READONLY | ATTR_HIDDEN | ATTR_SYSTEM)) {
char *fflags_text, *ptr;

fflags_text = malloc(22 * sizeof(char));
if (fflags_text != NULL) {
ptr = fflags_text;
if (file_attr & ATTR_READONLY) {
strcpy(ptr, "rdonly,");
ptr = ptr + 7;
}
if (file_attr & ATTR_HIDDEN) {
strcpy(ptr, "hidden,");
ptr = ptr + 7;
}
if (file_attr & ATTR_SYSTEM) {
strcpy(ptr, "system,");
ptr = ptr + 7;
}
if (ptr > fflags_text) {

*(ptr - 1) = '\0';
archive_entry_copy_fflags_text(entry,
fflags_text);
}
free(fflags_text);
}
}
} else if(host_os == HOST_UNIX) {

archive_entry_set_mode(entry, (__LA_MODE_T) file_attr);
} else {

archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Unsupported Host OS: 0x%x", (int) host_os);

return ARCHIVE_FATAL;
}

if(!read_var_sized(a, &name_size, NULL))
return ARCHIVE_EOF;

if(!read_ahead(a, name_size, &p))
return ARCHIVE_EOF;

if(name_size > (MAX_NAME_IN_CHARS - 1)) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Filename is too long");

return ARCHIVE_FATAL;
}

if(name_size == 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"No filename specified");

return ARCHIVE_FATAL;
}

memcpy(name_utf8_buf, p, name_size);
name_utf8_buf[name_size] = 0;
if(ARCHIVE_OK != consume(a, name_size)) {
return ARCHIVE_EOF;
}

archive_entry_update_pathname_utf8(entry, name_utf8_buf);

if(extra_data_size > 0) {
int ret = process_head_file_extra(a, entry, rar,
extra_data_size);












if(ret != ARCHIVE_OK)
return ret;
}

if((file_flags & UNKNOWN_UNPACKED_SIZE) == 0) {
rar->file.unpacked_size = (ssize_t) unpacked_size;
if(rar->file.redir_type == REDIR_TYPE_NONE)
archive_entry_set_size(entry, unpacked_size);
}

if(file_flags & UTIME) {
archive_entry_set_mtime(entry, (time_t) mtime, 0);
}

if(file_flags & CRC32) {
rar->file.stored_crc32 = crc;
}

if(!rar->cstate.switch_multivolume) {


rar->cstate.block_parsing_finished = 1;
rar->cstate.all_filters_applied = 1;
rar->cstate.initialized = 0;
}

if(rar->generic.split_before > 0) {






return ARCHIVE_RETRY;
} else {
return ARCHIVE_OK;
}
}

static int process_head_service(struct archive_read* a, struct rar5* rar,
struct archive_entry* entry, size_t block_flags)
{

int ret = process_head_file(a, rar, entry, block_flags);
if(ret != ARCHIVE_OK)
return ret;

rar->file.service = 1;




ret = rar5_read_data_skip(a);
if(ret != ARCHIVE_OK)
return ret;


return ARCHIVE_RETRY;
}

static int process_head_main(struct archive_read* a, struct rar5* rar,
struct archive_entry* entry, size_t block_flags)
{
int ret;
size_t extra_data_size = 0;
size_t extra_field_size = 0;
size_t extra_field_id = 0;
size_t archive_flags = 0;

enum MAIN_FLAGS {
VOLUME = 0x0001,
VOLUME_NUMBER = 0x0002,

SOLID = 0x0004,
PROTECT = 0x0008,
LOCK = 0x0010,
};

enum MAIN_EXTRA {

LOCATOR = 0x01,
};

(void) entry;

if(block_flags & HFL_EXTRA_DATA) {
if(!read_var_sized(a, &extra_data_size, NULL))
return ARCHIVE_EOF;
} else {
extra_data_size = 0;
}

if(!read_var_sized(a, &archive_flags, NULL)) {
return ARCHIVE_EOF;
}

rar->main.volume = (archive_flags & VOLUME) > 0;
rar->main.solid = (archive_flags & SOLID) > 0;

if(archive_flags & VOLUME_NUMBER) {
size_t v = 0;
if(!read_var_sized(a, &v, NULL)) {
return ARCHIVE_EOF;
}

if (v > UINT_MAX) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid volume number");
return ARCHIVE_FATAL;
}

rar->main.vol_no = (unsigned int) v;
} else {
rar->main.vol_no = 0;
}

if(rar->vol.expected_vol_no > 0 &&
rar->main.vol_no != rar->vol.expected_vol_no)
{





return ARCHIVE_EOF;
}

if(extra_data_size == 0) {

return ARCHIVE_OK;
}

if(!read_var_sized(a, &extra_field_size, NULL)) {
return ARCHIVE_EOF;
}

if(!read_var_sized(a, &extra_field_id, NULL)) {
return ARCHIVE_EOF;
}

if(extra_field_size == 0) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid extra field size");
return ARCHIVE_FATAL;
}

switch(extra_field_id) {
case LOCATOR:
ret = process_main_locator_extra_block(a, rar);
if(ret != ARCHIVE_OK) {


return ret;
}

break;
default:
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Unsupported extra type (0x%x)",
(int) extra_field_id);
return ARCHIVE_FATAL;
}

return ARCHIVE_OK;
}

static int skip_unprocessed_bytes(struct archive_read* a) {
struct rar5* rar = get_context(a);
int ret;

if(rar->file.bytes_remaining) {





if(rar->merge_mode) {




ret = consume(a, rar->file.bytes_remaining);
if(ret != ARCHIVE_OK) {
return ret;
}
rar->file.bytes_remaining = 0;
} else {



ret = rar5_read_data_skip(a);
if(ret != ARCHIVE_OK) {
return ret;
}
}
}

return ARCHIVE_OK;
}

static int scan_for_signature(struct archive_read* a);














































static int process_base_block(struct archive_read* a,
struct archive_entry* entry)
{
const size_t SMALLEST_RAR5_BLOCK_SIZE = 3;

struct rar5* rar = get_context(a);
uint32_t hdr_crc, computed_crc;
size_t raw_hdr_size = 0, hdr_size_len, hdr_size;
size_t header_id = 0;
size_t header_flags = 0;
const uint8_t* p;
int ret;

enum HEADER_TYPE {
HEAD_MARK = 0x00, HEAD_MAIN = 0x01, HEAD_FILE = 0x02,
HEAD_SERVICE = 0x03, HEAD_CRYPT = 0x04, HEAD_ENDARC = 0x05,
HEAD_UNKNOWN = 0xff,
};


ret = skip_unprocessed_bytes(a);
if(ret != ARCHIVE_OK)
return ret;


if(!read_u32(a, &hdr_crc)) {
return ARCHIVE_EOF;
}


if(!read_var_sized(a, &raw_hdr_size, &hdr_size_len)) {
return ARCHIVE_EOF;
}

hdr_size = raw_hdr_size + hdr_size_len;


if(hdr_size > (2 * 1024 * 1024)) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Base block header is too large");

return ARCHIVE_FATAL;
}


if(raw_hdr_size == 0 || hdr_size_len == 0 ||
hdr_size < SMALLEST_RAR5_BLOCK_SIZE)
{
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Too small block encountered (%zu bytes)",
raw_hdr_size);

return ARCHIVE_FATAL;
}



if(!read_ahead(a, hdr_size, &p)) {
return ARCHIVE_EOF;
}


computed_crc = (uint32_t) crc32(0, p, (int) hdr_size);
if(computed_crc != hdr_crc) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Header CRC error");

return ARCHIVE_FATAL;
}


if(ARCHIVE_OK != consume(a, hdr_size_len)) {
return ARCHIVE_EOF;
}

if(!read_var_sized(a, &header_id, NULL))
return ARCHIVE_EOF;

if(!read_var_sized(a, &header_flags, NULL))
return ARCHIVE_EOF;

rar->generic.split_after = (header_flags & HFL_SPLIT_AFTER) > 0;
rar->generic.split_before = (header_flags & HFL_SPLIT_BEFORE) > 0;
rar->generic.size = (int)hdr_size;
rar->generic.last_header_id = (int)header_id;
rar->main.endarc = 0;


switch(header_id) {
case HEAD_MAIN:
ret = process_head_main(a, rar, entry, header_flags);




if(ret == ARCHIVE_OK)
return ARCHIVE_RETRY;

return ret;
case HEAD_SERVICE:
ret = process_head_service(a, rar, entry, header_flags);
return ret;
case HEAD_FILE:
ret = process_head_file(a, rar, entry, header_flags);
return ret;
case HEAD_CRYPT:
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Encryption is not supported");
return ARCHIVE_FATAL;
case HEAD_ENDARC:
rar->main.endarc = 1;





if(rar->main.volume) {



ret = scan_for_signature(a);
if(ret == ARCHIVE_FATAL) {
return ARCHIVE_EOF;
} else {
if(rar->vol.expected_vol_no ==
UINT_MAX) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Header error");
return ARCHIVE_FATAL;
}

rar->vol.expected_vol_no =
rar->main.vol_no + 1;
return ARCHIVE_OK;
}
} else {
return ARCHIVE_EOF;
}
case HEAD_MARK:
return ARCHIVE_EOF;
default:
if((header_flags & HFL_SKIP_IF_UNKNOWN) == 0) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Header type error");
return ARCHIVE_FATAL;
} else {



return ARCHIVE_RETRY;
}
}

#if !defined WIN32

archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Internal unpacker error");
return ARCHIVE_FATAL;
#endif
}

static int skip_base_block(struct archive_read* a) {
int ret;
struct rar5* rar = get_context(a);




struct archive_entry* entry = archive_entry_new();
ret = process_base_block(a, entry);


archive_entry_free(entry);
if(ret == ARCHIVE_FATAL)
return ret;

if(rar->generic.last_header_id == 2 && rar->generic.split_before > 0)
return ARCHIVE_OK;

if(ret == ARCHIVE_OK)
return ARCHIVE_RETRY;
else
return ret;
}

static int rar5_read_header(struct archive_read *a,
struct archive_entry *entry)
{
struct rar5* rar = get_context(a);
int ret;

if(rar->header_initialized == 0) {
init_header(a);
rar->header_initialized = 1;
}

if(rar->skipped_magic == 0) {
if(ARCHIVE_OK != consume(a, sizeof(rar5_signature_xor))) {
return ARCHIVE_EOF;
}

rar->skipped_magic = 1;
}

do {
ret = process_base_block(a, entry);
} while(ret == ARCHIVE_RETRY ||
(rar->main.endarc > 0 && ret == ARCHIVE_OK));

return ret;
}

static void init_unpack(struct rar5* rar) {
rar->file.calculated_crc32 = 0;
init_window_mask(rar);

free(rar->cstate.window_buf);
free(rar->cstate.filtered_buf);

if(rar->cstate.window_size > 0) {
rar->cstate.window_buf = calloc(1, rar->cstate.window_size);
rar->cstate.filtered_buf = calloc(1, rar->cstate.window_size);
} else {
rar->cstate.window_buf = NULL;
rar->cstate.filtered_buf = NULL;
}

rar->cstate.write_ptr = 0;
rar->cstate.last_write_ptr = 0;

memset(&rar->cstate.bd, 0, sizeof(rar->cstate.bd));
memset(&rar->cstate.ld, 0, sizeof(rar->cstate.ld));
memset(&rar->cstate.dd, 0, sizeof(rar->cstate.dd));
memset(&rar->cstate.ldd, 0, sizeof(rar->cstate.ldd));
memset(&rar->cstate.rd, 0, sizeof(rar->cstate.rd));
}

static void update_crc(struct rar5* rar, const uint8_t* p, size_t to_read) {
int verify_crc;

if(rar->skip_mode) {
#if defined CHECK_CRC_ON_SOLID_SKIP
verify_crc = 1;
#else
verify_crc = 0;
#endif
} else
verify_crc = 1;

if(verify_crc) {


if(rar->file.stored_crc32 > 0) {
rar->file.calculated_crc32 =
crc32(rar->file.calculated_crc32, p, to_read);
}



if(rar->file.has_blake2 > 0) {


(void) blake2sp_update(&rar->file.b2state, p, to_read);
}
}
}

static int create_decode_tables(uint8_t* bit_length,
struct decode_table* table, int size)
{
int code, upper_limit = 0, i, lc[16];
uint32_t decode_pos_clone[rar5_countof(table->decode_pos)];
ssize_t cur_len, quick_data_size;

memset(&lc, 0, sizeof(lc));
memset(table->decode_num, 0, sizeof(table->decode_num));
table->size = size;
table->quick_bits = size == HUFF_NC ? 10 : 7;

for(i = 0; i < size; i++) {
lc[bit_length[i] & 15]++;
}

lc[0] = 0;
table->decode_pos[0] = 0;
table->decode_len[0] = 0;

for(i = 1; i < 16; i++) {
upper_limit += lc[i];

table->decode_len[i] = upper_limit << (16 - i);
table->decode_pos[i] = table->decode_pos[i - 1] + lc[i - 1];

upper_limit <<= 1;
}

memcpy(decode_pos_clone, table->decode_pos, sizeof(decode_pos_clone));

for(i = 0; i < size; i++) {
uint8_t clen = bit_length[i] & 15;
if(clen > 0) {
int last_pos = decode_pos_clone[clen];
table->decode_num[last_pos] = i;
decode_pos_clone[clen]++;
}
}

quick_data_size = (int64_t)1 << table->quick_bits;
cur_len = 1;
for(code = 0; code < quick_data_size; code++) {
int bit_field = code << (16 - table->quick_bits);
int dist, pos;

while(cur_len < rar5_countof(table->decode_len) &&
bit_field >= table->decode_len[cur_len]) {
cur_len++;
}

table->quick_len[code] = (uint8_t) cur_len;

dist = bit_field - table->decode_len[cur_len - 1];
dist >>= (16 - cur_len);

pos = table->decode_pos[cur_len & 15] + dist;
if(cur_len < rar5_countof(table->decode_pos) && pos < size) {
table->quick_num[code] = table->decode_num[pos];
} else {
table->quick_num[code] = 0;
}
}

return ARCHIVE_OK;
}

static int decode_number(struct archive_read* a, struct decode_table* table,
const uint8_t* p, uint16_t* num)
{
int i, bits, dist;
uint16_t bitfield;
uint32_t pos;
struct rar5* rar = get_context(a);

if(ARCHIVE_OK != read_bits_16(rar, p, &bitfield)) {
return ARCHIVE_EOF;
}

bitfield &= 0xfffe;

if(bitfield < table->decode_len[table->quick_bits]) {
int code = bitfield >> (16 - table->quick_bits);
skip_bits(rar, table->quick_len[code]);
*num = table->quick_num[code];
return ARCHIVE_OK;
}

bits = 15;

for(i = table->quick_bits + 1; i < 15; i++) {
if(bitfield < table->decode_len[i]) {
bits = i;
break;
}
}

skip_bits(rar, bits);

dist = bitfield - table->decode_len[bits - 1];
dist >>= (16 - bits);
pos = table->decode_pos[bits] + dist;

if(pos >= table->size)
pos = 0;

*num = table->decode_num[pos];
return ARCHIVE_OK;
}


static int parse_tables(struct archive_read* a, struct rar5* rar,
const uint8_t* p)
{
int ret, value, i, w, idx = 0;
uint8_t bit_length[HUFF_BC],
table[HUFF_TABLE_SIZE],
nibble_mask = 0xF0,
nibble_shift = 4;

enum { ESCAPE = 15 };



for(w = 0, i = 0; w < HUFF_BC;) {
if(i >= rar->cstate.cur_block_size) {

archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Truncated data in huffman tables");
return ARCHIVE_FATAL;
}

value = (p[i] & nibble_mask) >> nibble_shift;

if(nibble_mask == 0x0F)
++i;

nibble_mask ^= 0xFF;
nibble_shift ^= 4;




if(value == ESCAPE) {
value = (p[i] & nibble_mask) >> nibble_shift;
if(nibble_mask == 0x0F)
++i;
nibble_mask ^= 0xFF;
nibble_shift ^= 4;

if(value == 0) {


bit_length[w++] = ESCAPE;
} else {
int k;


for(k = 0; (k < value + 2) && (w < HUFF_BC);
k++) {
bit_length[w++] = 0;
}
}
} else {
bit_length[w++] = value;
}
}

rar->bits.in_addr = i;
rar->bits.bit_addr = nibble_shift ^ 4;

ret = create_decode_tables(bit_length, &rar->cstate.bd, HUFF_BC);
if(ret != ARCHIVE_OK) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Decoding huffman tables failed");
return ARCHIVE_FATAL;
}

for(i = 0; i < HUFF_TABLE_SIZE;) {
uint16_t num;

if((rar->bits.in_addr + 6) >= rar->cstate.cur_block_size) {

archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Truncated data in huffman tables (#2)");
return ARCHIVE_FATAL;
}

ret = decode_number(a, &rar->cstate.bd, p, &num);
if(ret != ARCHIVE_OK) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Decoding huffman tables failed");
return ARCHIVE_FATAL;
}

if(num < 16) {

table[i] = (uint8_t) num;
i++;
} else if(num < 18) {

uint16_t n;

if(ARCHIVE_OK != read_bits_16(rar, p, &n))
return ARCHIVE_EOF;

if(num == 16) {
n >>= 13;
n += 3;
skip_bits(rar, 3);
} else {
n >>= 9;
n += 11;
skip_bits(rar, 7);
}

if(i > 0) {
while(n-- > 0 && i < HUFF_TABLE_SIZE) {
table[i] = table[i - 1];
i++;
}
} else {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Unexpected error when decoding "
"huffman tables");
return ARCHIVE_FATAL;
}
} else {

uint16_t n;

if(ARCHIVE_OK != read_bits_16(rar, p, &n))
return ARCHIVE_EOF;

if(num == 18) {
n >>= 13;
n += 3;
skip_bits(rar, 3);
} else {
n >>= 9;
n += 11;
skip_bits(rar, 7);
}

while(n-- > 0 && i < HUFF_TABLE_SIZE)
table[i++] = 0;
}
}

ret = create_decode_tables(&table[idx], &rar->cstate.ld, HUFF_NC);
if(ret != ARCHIVE_OK) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Failed to create literal table");
return ARCHIVE_FATAL;
}

idx += HUFF_NC;

ret = create_decode_tables(&table[idx], &rar->cstate.dd, HUFF_DC);
if(ret != ARCHIVE_OK) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Failed to create distance table");
return ARCHIVE_FATAL;
}

idx += HUFF_DC;

ret = create_decode_tables(&table[idx], &rar->cstate.ldd, HUFF_LDC);
if(ret != ARCHIVE_OK) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Failed to create lower bits of distances table");
return ARCHIVE_FATAL;
}

idx += HUFF_LDC;

ret = create_decode_tables(&table[idx], &rar->cstate.rd, HUFF_RC);
if(ret != ARCHIVE_OK) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Failed to create repeating distances table");
return ARCHIVE_FATAL;
}

return ARCHIVE_OK;
}



static int parse_block_header(struct archive_read* a, const uint8_t* p,
ssize_t* block_size, struct compressed_block_header* hdr)
{
uint8_t calculated_cksum;
memcpy(hdr, p, sizeof(struct compressed_block_header));

if(bf_byte_count(hdr) > 2) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Unsupported block header size (was %d, max is 2)",
bf_byte_count(hdr));
return ARCHIVE_FATAL;
}



*block_size = 0;
switch(bf_byte_count(hdr)) {

case 0:
*block_size = *(const uint8_t*) &p[2];
break;


case 1:
*block_size = archive_le16dec(&p[2]);
break;


case 2:
*block_size = archive_le32dec(&p[2]);
*block_size &= 0x00FFFFFF;
break;




default:
return ARCHIVE_FATAL;
}



calculated_cksum = 0x5A
^ (uint8_t) hdr->block_flags_u8
^ (uint8_t) *block_size
^ (uint8_t) (*block_size >> 8)
^ (uint8_t) (*block_size >> 16);

if(calculated_cksum != hdr->block_cksum) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Block checksum error: got 0x%x, expected 0x%x",
hdr->block_cksum, calculated_cksum);

return ARCHIVE_FATAL;
}

return ARCHIVE_OK;
}


static int parse_filter_data(struct rar5* rar, const uint8_t* p,
uint32_t* filter_data)
{
int i, bytes;
uint32_t data = 0;

if(ARCHIVE_OK != read_consume_bits(rar, p, 2, &bytes))
return ARCHIVE_EOF;

bytes++;

for(i = 0; i < bytes; i++) {
uint16_t byte;

if(ARCHIVE_OK != read_bits_16(rar, p, &byte)) {
return ARCHIVE_EOF;
}



data += ((uint32_t) byte >> 8) << (i * 8);
skip_bits(rar, 8);
}

*filter_data = data;
return ARCHIVE_OK;
}


static int is_valid_filter_block_start(struct rar5* rar,
uint32_t start)
{
const int64_t block_start = (ssize_t) start + rar->cstate.write_ptr;
const int64_t last_bs = rar->cstate.last_block_start;
const ssize_t last_bl = rar->cstate.last_block_length;

if(last_bs == 0 || last_bl == 0) {

return 1;
}

if(block_start >= last_bs + last_bl) {


return 1;
}


return 0;
}



static int parse_filter(struct archive_read* ar, const uint8_t* p) {
uint32_t block_start, block_length;
uint16_t filter_type;
struct filter_info* filt = NULL;
struct rar5* rar = get_context(ar);


if(ARCHIVE_OK != parse_filter_data(rar, p, &block_start))
return ARCHIVE_EOF;

if(ARCHIVE_OK != parse_filter_data(rar, p, &block_length))
return ARCHIVE_EOF;

if(ARCHIVE_OK != read_bits_16(rar, p, &filter_type))
return ARCHIVE_EOF;

filter_type >>= 13;
skip_bits(rar, 3);





if(block_length < 4 ||
block_length > 0x400000 ||
filter_type > FILTER_ARM ||
!is_valid_filter_block_start(rar, block_start))
{
archive_set_error(&ar->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Invalid filter encountered");
return ARCHIVE_FATAL;
}


filt = add_new_filter(rar);
if(filt == NULL) {
archive_set_error(&ar->archive, ENOMEM,
"Can't allocate memory for a filter descriptor.");
return ARCHIVE_FATAL;
}

filt->type = filter_type;
filt->block_start = rar->cstate.write_ptr + block_start;
filt->block_length = block_length;

rar->cstate.last_block_start = filt->block_start;
rar->cstate.last_block_length = filt->block_length;




if(filter_type == FILTER_DELTA) {
int channels;

if(ARCHIVE_OK != read_consume_bits(rar, p, 5, &channels))
return ARCHIVE_EOF;

filt->channels = channels + 1;
}

return ARCHIVE_OK;
}

static int decode_code_length(struct rar5* rar, const uint8_t* p,
uint16_t code)
{
int lbits, length = 2;
if(code < 8) {
lbits = 0;
length += code;
} else {
lbits = code / 4 - 1;
length += (4 | (code & 3)) << lbits;
}

if(lbits > 0) {
int add;

if(ARCHIVE_OK != read_consume_bits(rar, p, lbits, &add))
return -1;

length += add;
}

return length;
}

static int copy_string(struct archive_read* a, int len, int dist) {
struct rar5* rar = get_context(a);
const uint64_t cmask = rar->cstate.window_mask;
const uint64_t write_ptr = rar->cstate.write_ptr +
rar->cstate.solid_offset;
int i;

if (rar->cstate.window_buf == NULL)
return ARCHIVE_FATAL;








for(i = 0; i < len; i++) {
const ssize_t write_idx = (write_ptr + i) & cmask;
const ssize_t read_idx = (write_ptr + i - dist) & cmask;
rar->cstate.window_buf[write_idx] =
rar->cstate.window_buf[read_idx];
}

rar->cstate.write_ptr += len;
return ARCHIVE_OK;
}

static int do_uncompress_block(struct archive_read* a, const uint8_t* p) {
struct rar5* rar = get_context(a);
uint16_t num;
int ret;

const uint64_t cmask = rar->cstate.window_mask;
const struct compressed_block_header* hdr = &rar->last_block_hdr;
const uint8_t bit_size = 1 + bf_bit_size(hdr);

while(1) {
if(rar->cstate.write_ptr - rar->cstate.last_write_ptr >
(rar->cstate.window_size >> 1)) {




break;
}

if(rar->bits.in_addr > rar->cstate.cur_block_size - 1 ||
(rar->bits.in_addr == rar->cstate.cur_block_size - 1 &&
rar->bits.bit_addr >= bit_size))
{


rar->cstate.block_parsing_finished = 1;
break;
}


if(ARCHIVE_OK != decode_number(a, &rar->cstate.ld, p, &num)) {
return ARCHIVE_EOF;
}















if(num < 256) {

int64_t write_idx = rar->cstate.solid_offset +
rar->cstate.write_ptr++;

rar->cstate.window_buf[write_idx & cmask] =
(uint8_t) num;
continue;
} else if(num >= 262) {
uint16_t dist_slot;
int len = decode_code_length(rar, p, num - 262),
dbits,
dist = 1;

if(len == -1) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_PROGRAMMER,
"Failed to decode the code length");

return ARCHIVE_FATAL;
}

if(ARCHIVE_OK != decode_number(a, &rar->cstate.dd, p,
&dist_slot))
{
archive_set_error(&a->archive,
ARCHIVE_ERRNO_PROGRAMMER,
"Failed to decode the distance slot");

return ARCHIVE_FATAL;
}

if(dist_slot < 4) {
dbits = 0;
dist += dist_slot;
} else {
dbits = dist_slot / 2 - 1;





dist += (uint32_t) (2 |
(dist_slot & 1)) << dbits;
}

if(dbits > 0) {
if(dbits >= 4) {
uint32_t add = 0;
uint16_t low_dist;

if(dbits > 4) {
if(ARCHIVE_OK != read_bits_32(
rar, p, &add)) {



return ARCHIVE_EOF;
}

skip_bits(rar, dbits - 4);
add = (add >> (
36 - dbits)) << 4;
dist += add;
}

if(ARCHIVE_OK != decode_number(a,
&rar->cstate.ldd, p, &low_dist))
{
archive_set_error(&a->archive,
ARCHIVE_ERRNO_PROGRAMMER,
"Failed to decode the "
"distance slot");

return ARCHIVE_FATAL;
}

if(dist >= INT_MAX - low_dist - 1) {


archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Distance pointer "
"overflow");
return ARCHIVE_FATAL;
}

dist += low_dist;
} else {

int add;

if(ARCHIVE_OK != read_consume_bits(rar,
p, dbits, &add)) {


return ARCHIVE_EOF;
}

dist += add;
}
}

if(dist > 0x100) {
len++;

if(dist > 0x2000) {
len++;

if(dist > 0x40000) {
len++;
}
}
}

dist_cache_push(rar, dist);
rar->cstate.last_len = len;

if(ARCHIVE_OK != copy_string(a, len, dist))
return ARCHIVE_FATAL;

continue;
} else if(num == 256) {

ret = parse_filter(a, p);
if(ret != ARCHIVE_OK)
return ret;

continue;
} else if(num == 257) {
if(rar->cstate.last_len != 0) {
if(ARCHIVE_OK != copy_string(a,
rar->cstate.last_len,
rar->cstate.dist_cache[0]))
{
return ARCHIVE_FATAL;
}
}

continue;
} else {

const int idx = num - 258;
const int dist = dist_cache_touch(rar, idx);

uint16_t len_slot;
int len;

if(ARCHIVE_OK != decode_number(a, &rar->cstate.rd, p,
&len_slot)) {
return ARCHIVE_FATAL;
}

len = decode_code_length(rar, p, len_slot);
rar->cstate.last_len = len;

if(ARCHIVE_OK != copy_string(a, len, dist))
return ARCHIVE_FATAL;

continue;
}
}

return ARCHIVE_OK;
}


static int scan_for_signature(struct archive_read* a) {
const uint8_t* p;
const int chunk_size = 512;
ssize_t i;
char signature[sizeof(rar5_signature_xor)];













rar5_signature(signature);

while(1) {
if(!read_ahead(a, chunk_size, &p))
return ARCHIVE_EOF;

for(i = 0; i < chunk_size - (int)sizeof(rar5_signature_xor);
i++) {
if(memcmp(&p[i], signature,
sizeof(rar5_signature_xor)) == 0) {





(void) consume(a,
i + sizeof(rar5_signature_xor));
return ARCHIVE_OK;
}
}

consume(a, chunk_size);
}

return ARCHIVE_FATAL;
}



static int advance_multivolume(struct archive_read* a) {
int lret;
struct rar5* rar = get_context(a);














while(1) {
if(rar->main.endarc == 1) {
int looping = 1;

rar->main.endarc = 0;

while(looping) {
lret = skip_base_block(a);
switch(lret) {
case ARCHIVE_RETRY:

break;
case ARCHIVE_OK:

looping = 0;
break;
default:


return lret;
}
}

break;
} else {




lret = skip_base_block(a);
if(lret == ARCHIVE_FATAL || lret == ARCHIVE_FAILED)
return lret;






if(lret != ARCHIVE_RETRY) {



if(rar->main.endarc == 0) {
return lret;
} else {
continue;
}
}
}
}

return ARCHIVE_OK;
}





static int merge_block(struct archive_read* a, ssize_t block_size,
const uint8_t** p)
{
struct rar5* rar = get_context(a);
ssize_t cur_block_size, partial_offset = 0;
const uint8_t* lp;
int ret;

if(rar->merge_mode) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Recursive merge is not allowed");

return ARCHIVE_FATAL;
}


rar->cstate.switch_multivolume = 1;


if(rar->vol.push_buf)
free((void*) rar->vol.push_buf);





rar->vol.push_buf = malloc(block_size + 8);
if(!rar->vol.push_buf) {
archive_set_error(&a->archive, ENOMEM,
"Can't allocate memory for a merge block buffer.");
return ARCHIVE_FATAL;
}



memset(&rar->vol.push_buf[block_size], 0, 8);





while(1) {


cur_block_size = rar5_min(rar->file.bytes_remaining,
block_size - partial_offset);

if(cur_block_size == 0) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Encountered block size == 0 during block merge");
return ARCHIVE_FATAL;
}

if(!read_ahead(a, cur_block_size, &lp))
return ARCHIVE_EOF;



if(partial_offset + cur_block_size > block_size) {
archive_set_error(&a->archive,
ARCHIVE_ERRNO_PROGRAMMER,
"Consumed too much data when merging blocks.");
return ARCHIVE_FATAL;
}




memcpy(&rar->vol.push_buf[partial_offset], lp, cur_block_size);


if(ARCHIVE_OK != consume(a, cur_block_size))
return ARCHIVE_EOF;



partial_offset += cur_block_size;
rar->file.bytes_remaining -= cur_block_size;




if(partial_offset == block_size) {
break;
}



if(rar->file.bytes_remaining == 0) {
rar->merge_mode++;
ret = advance_multivolume(a);
rar->merge_mode--;
if(ret != ARCHIVE_OK) {
return ret;
}
}
}

*p = rar->vol.push_buf;




return ARCHIVE_OK;
}

static int process_block(struct archive_read* a) {
const uint8_t* p;
struct rar5* rar = get_context(a);
int ret;



if(rar->main.volume && rar->file.bytes_remaining == 0) {
ret = advance_multivolume(a);
if(ret != ARCHIVE_OK)
return ret;
}

if(rar->cstate.block_parsing_finished) {
ssize_t block_size;
ssize_t to_skip;
ssize_t cur_block_size;


if(!read_ahead(a, 6, &p)) {

return ARCHIVE_EOF;
}









ret = parse_block_header(a, p, &block_size,
&rar->last_block_hdr);
if(ret != ARCHIVE_OK) {
return ret;
}



to_skip = sizeof(struct compressed_block_header) +
bf_byte_count(&rar->last_block_hdr) + 1;

if(ARCHIVE_OK != consume(a, to_skip))
return ARCHIVE_EOF;

rar->file.bytes_remaining -= to_skip;







cur_block_size =
rar5_min(rar->file.bytes_remaining, block_size);

if(block_size > rar->file.bytes_remaining) {










ret = merge_block(a, block_size, &p);
if(ret != ARCHIVE_OK) {
return ret;
}

cur_block_size = block_size;






} else {
rar->cstate.switch_multivolume = 0;





if(!read_ahead(a, 4 + cur_block_size, &p)) {

return ARCHIVE_EOF;
}
}

rar->cstate.block_buf = p;
rar->cstate.cur_block_size = cur_block_size;
rar->cstate.block_parsing_finished = 0;

rar->bits.in_addr = 0;
rar->bits.bit_addr = 0;

if(bf_is_table_present(&rar->last_block_hdr)) {

ret = parse_tables(a, rar, p);
if(ret != ARCHIVE_OK) {


return ret;
}
}
} else {

p = rar->cstate.block_buf;
}






ret = do_uncompress_block(a, p);
if(ret != ARCHIVE_OK) {
return ret;
}

if(rar->cstate.block_parsing_finished &&
rar->cstate.switch_multivolume == 0 &&
rar->cstate.cur_block_size > 0)
{



if(ARCHIVE_OK != consume(a, rar->cstate.cur_block_size))
return ARCHIVE_FATAL;

rar->file.bytes_remaining -= rar->cstate.cur_block_size;
} else if(rar->cstate.switch_multivolume) {



rar->cstate.switch_multivolume = 0;
}

return ARCHIVE_OK;
}





static int use_data(struct rar5* rar, const void** buf, size_t* size,
int64_t* offset)
{
int i;

for(i = 0; i < rar5_countof(rar->cstate.dready); i++) {
struct data_ready *d = &rar->cstate.dready[i];

if(d->used) {
if(buf) *buf = d->buf;
if(size) *size = d->size;
if(offset) *offset = d->offset;

d->used = 0;
return ARCHIVE_OK;
}
}

return ARCHIVE_RETRY;
}




static int push_data_ready(struct archive_read* a, struct rar5* rar,
const uint8_t* buf, size_t size, int64_t offset)
{
int i;







if(rar->skip_mode)
return ARCHIVE_OK;


if(offset != rar->file.last_offset + rar->file.last_size) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Sanity check error: output stream is not continuous");
return ARCHIVE_FATAL;
}

for(i = 0; i < rar5_countof(rar->cstate.dready); i++) {
struct data_ready* d = &rar->cstate.dready[i];
if(!d->used) {
d->used = 1;
d->buf = buf;
d->size = size;
d->offset = offset;


rar->file.last_offset = offset;
rar->file.last_size = size;



update_crc(rar, d->buf, d->size);

return ARCHIVE_OK;
}
}






archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Error: premature end of data_ready stack");
return ARCHIVE_FATAL;
}











































static int do_uncompress_file(struct archive_read* a) {
struct rar5* rar = get_context(a);
int ret;
int64_t max_end_pos;

if(!rar->cstate.initialized) {


if(!rar->main.solid || !rar->cstate.window_buf) {
init_unpack(rar);
}

rar->cstate.initialized = 1;
}

if(rar->cstate.all_filters_applied == 1) {





while(1) {
ret = process_block(a);
if(ret == ARCHIVE_EOF || ret == ARCHIVE_FATAL)
return ret;

if(rar->cstate.last_write_ptr ==
rar->cstate.write_ptr) {


continue;
}



break;
}
}



ret = apply_filters(a);
if(ret == ARCHIVE_RETRY) {
return ARCHIVE_OK;
} else if(ret == ARCHIVE_FATAL) {
return ARCHIVE_FATAL;
}



if(cdeque_size(&rar->cstate.filters) > 0) {


struct filter_info* flt;


if(CDE_OK != cdeque_front(&rar->cstate.filters,
cdeque_filter_p(&flt)))
{
archive_set_error(&a->archive,
ARCHIVE_ERRNO_PROGRAMMER,
"Can't read first filter");
return ARCHIVE_FATAL;
}

max_end_pos = rar5_min(flt->block_start,
rar->cstate.write_ptr);
} else {



max_end_pos = rar->cstate.write_ptr;
}

if(max_end_pos == rar->cstate.last_write_ptr) {










return ARCHIVE_RETRY;
} else {




push_window_data(a, rar, rar->cstate.last_write_ptr,
max_end_pos);
rar->cstate.last_write_ptr = max_end_pos;
}

return ARCHIVE_OK;
}

static int uncompress_file(struct archive_read* a) {
int ret;

while(1) {



ret = do_uncompress_file(a);
if(ret != ARCHIVE_RETRY)
return ret;
}
}


static int do_unstore_file(struct archive_read* a,
struct rar5* rar, const void** buf, size_t* size, int64_t* offset)
{
size_t to_read;
const uint8_t* p;

if(rar->file.bytes_remaining == 0 && rar->main.volume > 0 &&
rar->generic.split_after > 0)
{
int ret;

rar->cstate.switch_multivolume = 1;
ret = advance_multivolume(a);
rar->cstate.switch_multivolume = 0;

if(ret != ARCHIVE_OK) {


return ret;
}
}

to_read = rar5_min(rar->file.bytes_remaining, 64 * 1024);
if(to_read == 0) {
return ARCHIVE_EOF;
}

if(!read_ahead(a, to_read, &p)) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"I/O error when unstoring file");
return ARCHIVE_FATAL;
}

if(ARCHIVE_OK != consume(a, to_read)) {
return ARCHIVE_EOF;
}

if(buf) *buf = p;
if(size) *size = to_read;
if(offset) *offset = rar->cstate.last_unstore_ptr;

rar->file.bytes_remaining -= to_read;
rar->cstate.last_unstore_ptr += to_read;

update_crc(rar, p, to_read);
return ARCHIVE_OK;
}

static int do_unpack(struct archive_read* a, struct rar5* rar,
const void** buf, size_t* size, int64_t* offset)
{
enum COMPRESSION_METHOD {
STORE = 0, FASTEST = 1, FAST = 2, NORMAL = 3, GOOD = 4,
BEST = 5
};

if(rar->file.service > 0) {
return do_unstore_file(a, rar, buf, size, offset);
} else {
switch(rar->cstate.method) {
case STORE:
return do_unstore_file(a, rar, buf, size,
offset);
case FASTEST:

case FAST:

case NORMAL:

case GOOD:

case BEST:
return uncompress_file(a);
default:
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Compression method not supported: 0x%x",
rar->cstate.method);

return ARCHIVE_FATAL;
}
}

#if !defined WIN32

return ARCHIVE_OK;
#endif
}

static int verify_checksums(struct archive_read* a) {
int verify_crc;
struct rar5* rar = get_context(a);






if(!rar->skip_mode) {

verify_crc = 1;
} else {





#if defined CHECK_CRC_ON_SOLID_SKIP

verify_crc = 1;
#else

verify_crc = 0;
#endif
}

if(verify_crc) {





if(rar->file.stored_crc32 > 0) {



if(rar->file.calculated_crc32 !=
rar->file.stored_crc32) {



DEBUG_CODE {
printf("Checksum error: CRC32 "
"(was: %08" PRIx32 ", expected: %08" PRIx32 ")\n",
rar->file.calculated_crc32,
rar->file.stored_crc32);
}

#if !defined(DONT_FAIL_ON_CRC_ERROR)
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Checksum error: CRC32");
return ARCHIVE_FATAL;
#endif
} else {
DEBUG_CODE {
printf("Checksum OK: CRC32 "
"(%08" PRIx32 "/%08" PRIx32 ")\n",
rar->file.stored_crc32,
rar->file.calculated_crc32);
}
}
}

if(rar->file.has_blake2 > 0) {













uint8_t b2_buf[32];
(void) blake2sp_final(&rar->file.b2state, b2_buf, 32);

if(memcmp(&rar->file.blake2sp, b2_buf, 32) != 0) {
#if !defined(DONT_FAIL_ON_CRC_ERROR)
archive_set_error(&a->archive,
ARCHIVE_ERRNO_FILE_FORMAT,
"Checksum error: BLAKE2");

return ARCHIVE_FATAL;
#endif
}
}
}


return ARCHIVE_OK;
}

static int verify_global_checksums(struct archive_read* a) {
return verify_checksums(a);
}





static void rar5_signature(char *buf) {
size_t i;

for(i = 0; i < sizeof(rar5_signature_xor); i++) {
buf[i] = rar5_signature_xor[i] ^ 0xA1;
}
}

static int rar5_read_data(struct archive_read *a, const void **buff,
size_t *size, int64_t *offset) {
int ret;
struct rar5* rar = get_context(a);

if (size)
*size = 0;

if(rar->file.dir > 0) {




archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
"Can't decompress an entry marked as a directory");
return ARCHIVE_FAILED;
}

if(!rar->skip_mode && (rar->cstate.last_write_ptr > rar->file.unpacked_size)) {
archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
"Unpacker has written too many bytes");
return ARCHIVE_FATAL;
}

ret = use_data(rar, buff, size, offset);
if(ret == ARCHIVE_OK) {
return ret;
}

if(rar->file.eof == 1) {
return ARCHIVE_EOF;
}

ret = do_unpack(a, rar, buff, size, offset);
if(ret != ARCHIVE_OK) {
return ret;
}

if(rar->file.bytes_remaining == 0 &&
rar->cstate.last_write_ptr == rar->file.unpacked_size)
{








rar->file.eof = 1;
return verify_global_checksums(a);
}

return ARCHIVE_OK;
}

static int rar5_read_data_skip(struct archive_read *a) {
struct rar5* rar = get_context(a);

if(rar->main.solid) {





int ret;


while(rar->file.bytes_remaining > 0) {








rar->skip_mode++;



ret = rar5_read_data(a, NULL, NULL, NULL);


rar->skip_mode--;

if(ret < 0 || ret == ARCHIVE_EOF) {


return ret;
}
}
} else {




if(ARCHIVE_OK != consume(a, rar->file.bytes_remaining)) {
return ARCHIVE_FATAL;
}

rar->file.bytes_remaining = 0;
}

return ARCHIVE_OK;
}

static int64_t rar5_seek_data(struct archive_read *a, int64_t offset,
int whence)
{
(void) a;
(void) offset;
(void) whence;



return ARCHIVE_FATAL;
}

static int rar5_cleanup(struct archive_read *a) {
struct rar5* rar = get_context(a);

free(rar->cstate.window_buf);
free(rar->cstate.filtered_buf);

free(rar->vol.push_buf);

free_filters(rar);
cdeque_free(&rar->cstate.filters);

free(rar);
a->format->data = NULL;

return ARCHIVE_OK;
}

static int rar5_capabilities(struct archive_read * a) {
(void) a;
return 0;
}

static int rar5_has_encrypted_entries(struct archive_read *_a) {
(void) _a;


return ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED;
}

static int rar5_init(struct rar5* rar) {
memset(rar, 0, sizeof(struct rar5));

if(CDE_OK != cdeque_init(&rar->cstate.filters, 8192))
return ARCHIVE_FATAL;

return ARCHIVE_OK;
}

int archive_read_support_format_rar5(struct archive *_a) {
struct archive_read* ar;
int ret;
struct rar5* rar;

if(ARCHIVE_OK != (ret = get_archive_read(_a, &ar)))
return ret;

rar = malloc(sizeof(*rar));
if(rar == NULL) {
archive_set_error(&ar->archive, ENOMEM,
"Can't allocate rar5 data");
return ARCHIVE_FATAL;
}

if(ARCHIVE_OK != rar5_init(rar)) {
archive_set_error(&ar->archive, ENOMEM,
"Can't allocate rar5 filter buffer");
free(rar);
return ARCHIVE_FATAL;
}

ret = __archive_read_register_format(ar,
rar,
"rar5",
rar5_bid,
rar5_options,
rar5_read_header,
rar5_read_data,
rar5_read_data_skip,
rar5_seek_data,
rar5_cleanup,
rar5_capabilities,
rar5_has_encrypted_entries);

if(ret != ARCHIVE_OK) {
(void) rar5_cleanup(ar);
}

return ret;
}
