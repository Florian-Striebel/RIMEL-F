


























#include "_libdwarf.h"

ELFTC_VCSID("$Id: libdwarf_rw.c 3286 2015-12-31 16:45:46Z emaste $");

uint64_t
_dwarf_read_lsb(uint8_t *data, uint64_t *offsetp, int bytes_to_read)
{
uint64_t ret;
uint8_t *src;

src = data + *offsetp;

ret = 0;
switch (bytes_to_read) {
case 8:
ret |= ((uint64_t) src[4]) << 32 | ((uint64_t) src[5]) << 40;
ret |= ((uint64_t) src[6]) << 48 | ((uint64_t) src[7]) << 56;

case 4:
ret |= ((uint64_t) src[2]) << 16 | ((uint64_t) src[3]) << 24;

case 2:
ret |= ((uint64_t) src[1]) << 8;

case 1:
ret |= src[0];
break;
default:
return (0);
}

*offsetp += bytes_to_read;

return (ret);
}

uint64_t
_dwarf_decode_lsb(uint8_t **data, int bytes_to_read)
{
uint64_t ret;
uint8_t *src;

src = *data;

ret = 0;
switch (bytes_to_read) {
case 8:
ret |= ((uint64_t) src[4]) << 32 | ((uint64_t) src[5]) << 40;
ret |= ((uint64_t) src[6]) << 48 | ((uint64_t) src[7]) << 56;

case 4:
ret |= ((uint64_t) src[2]) << 16 | ((uint64_t) src[3]) << 24;

case 2:
ret |= ((uint64_t) src[1]) << 8;

case 1:
ret |= src[0];
break;
default:
return (0);
}

*data += bytes_to_read;

return (ret);
}

uint64_t
_dwarf_read_msb(uint8_t *data, uint64_t *offsetp, int bytes_to_read)
{
uint64_t ret;
uint8_t *src;

src = data + *offsetp;

switch (bytes_to_read) {
case 1:
ret = src[0];
break;
case 2:
ret = src[1] | ((uint64_t) src[0]) << 8;
break;
case 4:
ret = src[3] | ((uint64_t) src[2]) << 8;
ret |= ((uint64_t) src[1]) << 16 | ((uint64_t) src[0]) << 24;
break;
case 8:
ret = src[7] | ((uint64_t) src[6]) << 8;
ret |= ((uint64_t) src[5]) << 16 | ((uint64_t) src[4]) << 24;
ret |= ((uint64_t) src[3]) << 32 | ((uint64_t) src[2]) << 40;
ret |= ((uint64_t) src[1]) << 48 | ((uint64_t) src[0]) << 56;
break;
default:
return (0);
}

*offsetp += bytes_to_read;

return (ret);
}

uint64_t
_dwarf_decode_msb(uint8_t **data, int bytes_to_read)
{
uint64_t ret;
uint8_t *src;

src = *data;

ret = 0;
switch (bytes_to_read) {
case 1:
ret = src[0];
break;
case 2:
ret = src[1] | ((uint64_t) src[0]) << 8;
break;
case 4:
ret = src[3] | ((uint64_t) src[2]) << 8;
ret |= ((uint64_t) src[1]) << 16 | ((uint64_t) src[0]) << 24;
break;
case 8:
ret = src[7] | ((uint64_t) src[6]) << 8;
ret |= ((uint64_t) src[5]) << 16 | ((uint64_t) src[4]) << 24;
ret |= ((uint64_t) src[3]) << 32 | ((uint64_t) src[2]) << 40;
ret |= ((uint64_t) src[1]) << 48 | ((uint64_t) src[0]) << 56;
break;
default:
return (0);
break;
}

*data += bytes_to_read;

return (ret);
}

void
_dwarf_write_lsb(uint8_t *data, uint64_t *offsetp, uint64_t value,
int bytes_to_write)
{
uint8_t *dst;

dst = data + *offsetp;

switch (bytes_to_write) {
case 8:
dst[7] = (value >> 56) & 0xff;
dst[6] = (value >> 48) & 0xff;
dst[5] = (value >> 40) & 0xff;
dst[4] = (value >> 32) & 0xff;

case 4:
dst[3] = (value >> 24) & 0xff;
dst[2] = (value >> 16) & 0xff;

case 2:
dst[1] = (value >> 8) & 0xff;

case 1:
dst[0] = value & 0xff;
break;
default:
return;
}

*offsetp += bytes_to_write;
}

int
_dwarf_write_lsb_alloc(uint8_t **block, uint64_t *size, uint64_t *offsetp,
uint64_t value, int bytes_to_write, Dwarf_Error *error)
{

assert(*size > 0);

while (*offsetp + bytes_to_write > *size) {
*size *= 2;
*block = realloc(*block, (size_t) *size);
if (*block == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

_dwarf_write_lsb(*block, offsetp, value, bytes_to_write);

return (DW_DLE_NONE);
}

void
_dwarf_write_msb(uint8_t *data, uint64_t *offsetp, uint64_t value,
int bytes_to_write)
{
uint8_t *dst;

dst = data + *offsetp;

switch (bytes_to_write) {
case 8:
dst[7] = value & 0xff;
dst[6] = (value >> 8) & 0xff;
dst[5] = (value >> 16) & 0xff;
dst[4] = (value >> 24) & 0xff;
value >>= 32;

case 4:
dst[3] = value & 0xff;
dst[2] = (value >> 8) & 0xff;
value >>= 16;

case 2:
dst[1] = value & 0xff;
value >>= 8;

case 1:
dst[0] = value & 0xff;
break;
default:
return;
}

*offsetp += bytes_to_write;
}

int
_dwarf_write_msb_alloc(uint8_t **block, uint64_t *size, uint64_t *offsetp,
uint64_t value, int bytes_to_write, Dwarf_Error *error)
{

assert(*size > 0);

while (*offsetp + bytes_to_write > *size) {
*size *= 2;
*block = realloc(*block, (size_t) *size);
if (*block == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

_dwarf_write_msb(*block, offsetp, value, bytes_to_write);

return (DW_DLE_NONE);
}

int64_t
_dwarf_read_sleb128(uint8_t *data, uint64_t *offsetp)
{
int64_t ret = 0;
uint8_t b;
int shift = 0;
uint8_t *src;

src = data + *offsetp;

do {
b = *src++;
ret |= ((b & 0x7f) << shift);
(*offsetp)++;
shift += 7;
} while ((b & 0x80) != 0);

if (shift < 64 && (b & 0x40) != 0)
ret |= (-1 << shift);

return (ret);
}

int
_dwarf_write_sleb128(uint8_t *data, uint8_t *end, int64_t val)
{
uint8_t *p;

p = data;

for (;;) {
if (p >= end)
return (-1);
*p = val & 0x7f;
val >>= 7;
if ((val == 0 && (*p & 0x40) == 0) ||
(val == -1 && (*p & 0x40) != 0)) {
p++;
break;
}
*p++ |= 0x80;
}

return (p - data);
}

int
_dwarf_write_sleb128_alloc(uint8_t **block, uint64_t *size, uint64_t *offsetp,
int64_t val, Dwarf_Error *error)
{
int len;

assert(*size > 0);

while ((len = _dwarf_write_sleb128(*block + *offsetp, *block + *size,
val)) < 0) {
*size *= 2;
*block = realloc(*block, (size_t) *size);
if (*block == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

*offsetp += len;

return (DW_DLE_NONE);
}

uint64_t
_dwarf_read_uleb128(uint8_t *data, uint64_t *offsetp)
{
uint64_t ret = 0;
uint8_t b;
int shift = 0;
uint8_t *src;

src = data + *offsetp;

do {
b = *src++;
ret |= ((b & 0x7f) << shift);
(*offsetp)++;
shift += 7;
} while ((b & 0x80) != 0);

return (ret);
}

int
_dwarf_write_uleb128(uint8_t *data, uint8_t *end, uint64_t val)
{
uint8_t *p;

p = data;

do {
if (p >= end)
return (-1);
*p = val & 0x7f;
val >>= 7;
if (val > 0)
*p |= 0x80;
p++;
} while (val > 0);

return (p - data);
}

int
_dwarf_write_uleb128_alloc(uint8_t **block, uint64_t *size, uint64_t *offsetp,
uint64_t val, Dwarf_Error *error)
{
int len;

assert(*size > 0);

while ((len = _dwarf_write_uleb128(*block + *offsetp, *block + *size,
val)) < 0) {
*size *= 2;
*block = realloc(*block, (size_t) *size);
if (*block == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

*offsetp += len;

return (DW_DLE_NONE);
}

int64_t
_dwarf_decode_sleb128(uint8_t **dp)
{
int64_t ret = 0;
uint8_t b;
int shift = 0;

uint8_t *src = *dp;

do {
b = *src++;
ret |= ((b & 0x7f) << shift);
shift += 7;
} while ((b & 0x80) != 0);

if (shift < 64 && (b & 0x40) != 0)
ret |= (-1 << shift);

*dp = src;

return (ret);
}

uint64_t
_dwarf_decode_uleb128(uint8_t **dp)
{
uint64_t ret = 0;
uint8_t b;
int shift = 0;

uint8_t *src = *dp;

do {
b = *src++;
ret |= ((b & 0x7f) << shift);
shift += 7;
} while ((b & 0x80) != 0);

*dp = src;

return (ret);
}

char *
_dwarf_read_string(void *data, Dwarf_Unsigned size, uint64_t *offsetp)
{
char *ret, *src;

ret = src = (char *) data + *offsetp;

while (*src != '\0' && *offsetp < size) {
src++;
(*offsetp)++;
}

if (*src == '\0' && *offsetp < size)
(*offsetp)++;

return (ret);
}

void
_dwarf_write_string(void *data, uint64_t *offsetp, char *string)
{
char *dst;

dst = (char *) data + *offsetp;
strcpy(dst, string);
(*offsetp) += strlen(string) + 1;
}

int
_dwarf_write_string_alloc(uint8_t **block, uint64_t *size, uint64_t *offsetp,
char *string, Dwarf_Error *error)
{
size_t len;

assert(*size > 0);

len = strlen(string) + 1;
while (*offsetp + len > *size) {
*size *= 2;
*block = realloc(*block, (size_t) *size);
if (*block == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

_dwarf_write_string(*block, offsetp, string);

return (DW_DLE_NONE);
}

uint8_t *
_dwarf_read_block(void *data, uint64_t *offsetp, uint64_t length)
{
uint8_t *ret, *src;

ret = src = (uint8_t *) data + *offsetp;

(*offsetp) += length;

return (ret);
}

void
_dwarf_write_block(void *data, uint64_t *offsetp, uint8_t *blk,
uint64_t length)
{
uint8_t *dst;

dst = (uint8_t *) data + *offsetp;
memcpy(dst, blk, length);
(*offsetp) += length;
}

int
_dwarf_write_block_alloc(uint8_t **block, uint64_t *size, uint64_t *offsetp,
uint8_t *blk, uint64_t length, Dwarf_Error *error)
{

assert(*size > 0);

while (*offsetp + length > *size) {
*size *= 2;
*block = realloc(*block, (size_t) *size);
if (*block == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

_dwarf_write_block(*block, offsetp, blk, length);

return (DW_DLE_NONE);
}

void
_dwarf_write_padding(void *data, uint64_t *offsetp, uint8_t byte,
uint64_t length)
{
uint8_t *dst;

dst = (uint8_t *) data + *offsetp;
memset(dst, byte, length);
(*offsetp) += length;
}

int
_dwarf_write_padding_alloc(uint8_t **block, uint64_t *size, uint64_t *offsetp,
uint8_t byte, uint64_t cnt, Dwarf_Error *error)
{
assert(*size > 0);

while (*offsetp + cnt > *size) {
*size *= 2;
*block = realloc(*block, (size_t) *size);
if (*block == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

_dwarf_write_padding(*block, offsetp, byte, cnt);

return (DW_DLE_NONE);
}
