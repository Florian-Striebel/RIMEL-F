
























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
#include "archive_private.h"
#include "archive_string.h"
#include "archive_write_private.h"

#define LBYTES 45

struct private_uuencode {
int mode;
struct archive_string name;
struct archive_string encoded_buff;
size_t bs;
size_t hold_len;
unsigned char hold[LBYTES];
};

static int archive_filter_uuencode_options(struct archive_write_filter *,
const char *, const char *);
static int archive_filter_uuencode_open(struct archive_write_filter *);
static int archive_filter_uuencode_write(struct archive_write_filter *,
const void *, size_t);
static int archive_filter_uuencode_close(struct archive_write_filter *);
static int archive_filter_uuencode_free(struct archive_write_filter *);
static void uu_encode(struct archive_string *, const unsigned char *, size_t);
static int64_t atol8(const char *, size_t);




int
archive_write_add_filter_uuencode(struct archive *_a)
{
struct archive_write *a = (struct archive_write *)_a;
struct archive_write_filter *f = __archive_write_allocate_filter(_a);
struct private_uuencode *state;

archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
ARCHIVE_STATE_NEW, "archive_write_add_filter_uu");

state = (struct private_uuencode *)calloc(1, sizeof(*state));
if (state == NULL) {
archive_set_error(f->archive, ENOMEM,
"Can't allocate data for uuencode filter");
return (ARCHIVE_FATAL);
}
archive_strcpy(&state->name, "-");
state->mode = 0644;

f->data = state;
f->name = "uuencode";
f->code = ARCHIVE_FILTER_UU;
f->open = archive_filter_uuencode_open;
f->options = archive_filter_uuencode_options;
f->write = archive_filter_uuencode_write;
f->close = archive_filter_uuencode_close;
f->free = archive_filter_uuencode_free;

return (ARCHIVE_OK);
}




static int
archive_filter_uuencode_options(struct archive_write_filter *f, const char *key,
const char *value)
{
struct private_uuencode *state = (struct private_uuencode *)f->data;

if (strcmp(key, "mode") == 0) {
if (value == NULL) {
archive_set_error(f->archive, ARCHIVE_ERRNO_MISC,
"mode option requires octal digits");
return (ARCHIVE_FAILED);
}
state->mode = (int)atol8(value, strlen(value)) & 0777;
return (ARCHIVE_OK);
} else if (strcmp(key, "name") == 0) {
if (value == NULL) {
archive_set_error(f->archive, ARCHIVE_ERRNO_MISC,
"name option requires a string");
return (ARCHIVE_FAILED);
}
archive_strcpy(&state->name, value);
return (ARCHIVE_OK);
}




return (ARCHIVE_WARN);
}




static int
archive_filter_uuencode_open(struct archive_write_filter *f)
{
struct private_uuencode *state = (struct private_uuencode *)f->data;
size_t bs = 65536, bpb;

if (f->archive->magic == ARCHIVE_WRITE_MAGIC) {


bpb = archive_write_get_bytes_per_block(f->archive);
if (bpb > bs)
bs = bpb;
else if (bpb != 0)
bs -= bs % bpb;
}

state->bs = bs;
if (archive_string_ensure(&state->encoded_buff, bs + 512) == NULL) {
archive_set_error(f->archive, ENOMEM,
"Can't allocate data for uuencode buffer");
return (ARCHIVE_FATAL);
}

archive_string_sprintf(&state->encoded_buff, "begin %o %s\n",
state->mode, state->name.s);

f->data = state;
return (0);
}

static void
uu_encode(struct archive_string *as, const unsigned char *p, size_t len)
{
int c;

c = (int)len;
archive_strappend_char(as, c?c + 0x20:'`');
for (; len >= 3; p += 3, len -= 3) {
c = p[0] >> 2;
archive_strappend_char(as, c?c + 0x20:'`');
c = ((p[0] & 0x03) << 4) | ((p[1] & 0xf0) >> 4);
archive_strappend_char(as, c?c + 0x20:'`');
c = ((p[1] & 0x0f) << 2) | ((p[2] & 0xc0) >> 6);
archive_strappend_char(as, c?c + 0x20:'`');
c = p[2] & 0x3f;
archive_strappend_char(as, c?c + 0x20:'`');
}
if (len > 0) {
c = p[0] >> 2;
archive_strappend_char(as, c?c + 0x20:'`');
c = (p[0] & 0x03) << 4;
if (len == 1) {
archive_strappend_char(as, c?c + 0x20:'`');
archive_strappend_char(as, '`');
archive_strappend_char(as, '`');
} else {
c |= (p[1] & 0xf0) >> 4;
archive_strappend_char(as, c?c + 0x20:'`');
c = (p[1] & 0x0f) << 2;
archive_strappend_char(as, c?c + 0x20:'`');
archive_strappend_char(as, '`');
}
}
archive_strappend_char(as, '\n');
}




static int
archive_filter_uuencode_write(struct archive_write_filter *f, const void *buff,
size_t length)
{
struct private_uuencode *state = (struct private_uuencode *)f->data;
const unsigned char *p = buff;
int ret = ARCHIVE_OK;

if (length == 0)
return (ret);

if (state->hold_len) {
while (state->hold_len < LBYTES && length > 0) {
state->hold[state->hold_len++] = *p++;
length--;
}
if (state->hold_len < LBYTES)
return (ret);
uu_encode(&state->encoded_buff, state->hold, LBYTES);
state->hold_len = 0;
}

for (; length >= LBYTES; length -= LBYTES, p += LBYTES)
uu_encode(&state->encoded_buff, p, LBYTES);


if (length > 0) {
memcpy(state->hold, p, length);
state->hold_len = length;
}
while (archive_strlen(&state->encoded_buff) >= state->bs) {
ret = __archive_write_filter(f->next_filter,
state->encoded_buff.s, state->bs);
memmove(state->encoded_buff.s,
state->encoded_buff.s + state->bs,
state->encoded_buff.length - state->bs);
state->encoded_buff.length -= state->bs;
}

return (ret);
}





static int
archive_filter_uuencode_close(struct archive_write_filter *f)
{
struct private_uuencode *state = (struct private_uuencode *)f->data;


if (state->hold_len != 0)
uu_encode(&state->encoded_buff, state->hold, state->hold_len);
archive_string_sprintf(&state->encoded_buff, "`\nend\n");

archive_write_set_bytes_in_last_block(f->archive, 1);
return __archive_write_filter(f->next_filter,
state->encoded_buff.s, archive_strlen(&state->encoded_buff));
}

static int
archive_filter_uuencode_free(struct archive_write_filter *f)
{
struct private_uuencode *state = (struct private_uuencode *)f->data;

archive_string_free(&state->name);
archive_string_free(&state->encoded_buff);
free(state);
return (ARCHIVE_OK);
}

static int64_t
atol8(const char *p, size_t char_cnt)
{
int64_t l;
int digit;

l = 0;
while (char_cnt-- > 0) {
if (*p >= '0' && *p <= '7')
digit = *p - '0';
else
break;
p++;
l <<= 3;
l |= digit;
}
return (l);
}

