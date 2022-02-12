






#include "streaming.h"
#include "internal/loaders.h"

bool static claim_bytes(size_t required, size_t provided,
struct cbor_decoder_result *result) {
if (required > (provided - result->read)) {
result->required = required + result->read;
result->read = 0;
result->status = CBOR_DECODER_NEDATA;
return false;
} else {
result->read += required;
result->required = 0;
return true;
}
}

struct cbor_decoder_result cbor_stream_decode(
cbor_data source, size_t source_size,
const struct cbor_callbacks *callbacks, void *context) {

struct cbor_decoder_result result = {.status = CBOR_DECODER_FINISHED};
if (!claim_bytes(1, source_size, &result)) {
return result;
}

switch (*source) {
case 0x00:
case 0x01:
case 0x02:
case 0x03:
case 0x04:
case 0x05:
case 0x06:
case 0x07:
case 0x08:
case 0x09:
case 0x0A:
case 0x0B:
case 0x0C:
case 0x0D:
case 0x0E:
case 0x0F:
case 0x10:
case 0x11:
case 0x12:
case 0x13:
case 0x14:
case 0x15:
case 0x16:
case 0x17:

{
callbacks->uint8(context, _cbor_load_uint8(source));
return result;
}
case 0x18:

{
if (claim_bytes(1, source_size, &result)) {
callbacks->uint8(context, _cbor_load_uint8(source + 1));
}
return result;
}
case 0x19:

{
if (claim_bytes(2, source_size, &result)) {
callbacks->uint16(context, _cbor_load_uint16(source + 1));
}
return result;
}
case 0x1A:

{
if (claim_bytes(4, source_size, &result)) {
callbacks->uint32(context, _cbor_load_uint32(source + 1));
}
return result;
}
case 0x1B:

{
if (claim_bytes(8, source_size, &result)) {
callbacks->uint64(context, _cbor_load_uint64(source + 1));
}
return result;
}
case 0x1C:
case 0x1D:
case 0x1E:
case 0x1F:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0x20:
case 0x21:
case 0x22:
case 0x23:
case 0x24:
case 0x25:
case 0x26:
case 0x27:
case 0x28:
case 0x29:
case 0x2A:
case 0x2B:
case 0x2C:
case 0x2D:
case 0x2E:
case 0x2F:
case 0x30:
case 0x31:
case 0x32:
case 0x33:
case 0x34:
case 0x35:
case 0x36:
case 0x37:

{
callbacks->negint8(context,
_cbor_load_uint8(source) - 0x20);
return result;
}
case 0x38:

{
if (claim_bytes(1, source_size, &result)) {
callbacks->negint8(context, _cbor_load_uint8(source + 1));
}
return result;
}
case 0x39:

{
if (claim_bytes(2, source_size, &result)) {
callbacks->negint16(context, _cbor_load_uint16(source + 1));
}
return result;
}
case 0x3A:

{
if (claim_bytes(4, source_size, &result)) {
callbacks->negint32(context, _cbor_load_uint32(source + 1));
}
return result;
}
case 0x3B:

{
if (claim_bytes(8, source_size, &result)) {
callbacks->negint64(context, _cbor_load_uint64(source + 1));
}
return result;
}
case 0x3C:
case 0x3D:
case 0x3E:
case 0x3F:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0x40:
case 0x41:
case 0x42:
case 0x43:
case 0x44:
case 0x45:
case 0x46:
case 0x47:
case 0x48:
case 0x49:
case 0x4A:
case 0x4B:
case 0x4C:
case 0x4D:
case 0x4E:
case 0x4F:
case 0x50:
case 0x51:
case 0x52:
case 0x53:
case 0x54:
case 0x55:
case 0x56:
case 0x57:

{
size_t length =
(size_t)_cbor_load_uint8(source) - 0x40;
if (claim_bytes(length, source_size, &result)) {
callbacks->byte_string(context, source + 1, length);
}
return result;
}
case 0x58:


{
if (claim_bytes(1, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint8(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->byte_string(context, source + 1 + 1, length);
}
}
return result;
}
case 0x59:

{
if (claim_bytes(2, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint16(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->byte_string(context, source + 1 + 2, length);
}
}
return result;
}
case 0x5A:

{
if (claim_bytes(4, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint32(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->byte_string(context, source + 1 + 4, length);
}
}
return result;
}
case 0x5B:

{
if (claim_bytes(8, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint64(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->byte_string(context, source + 1 + 8, length);
}
}
return result;
}
case 0x5C:
case 0x5D:
case 0x5E:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0x5F:

{
callbacks->byte_string_start(context);
return result;
}
case 0x60:
case 0x61:
case 0x62:
case 0x63:
case 0x64:
case 0x65:
case 0x66:
case 0x67:
case 0x68:
case 0x69:
case 0x6A:
case 0x6B:
case 0x6C:
case 0x6D:
case 0x6E:
case 0x6F:
case 0x70:
case 0x71:
case 0x72:
case 0x73:
case 0x74:
case 0x75:
case 0x76:
case 0x77:

{
size_t length =
(size_t)_cbor_load_uint8(source) - 0x60;
if (claim_bytes(length, source_size, &result)) {
callbacks->string(context, source + 1, length);
}
return result;
}
case 0x78:

{
if (claim_bytes(1, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint8(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->string(context, source + 1 + 1, length);
}
}
return result;
}
case 0x79:

{
if (claim_bytes(2, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint16(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->string(context, source + 1 + 2, length);
}
}
return result;
}
case 0x7A:

{
if (claim_bytes(4, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint32(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->string(context, source + 1 + 4, length);
}
}
return result;
}
case 0x7B:

{
if (claim_bytes(8, source_size, &result)) {
size_t length = (size_t)_cbor_load_uint64(source + 1);
if (claim_bytes(length, source_size, &result)) {
callbacks->string(context, source + 1 + 8, length);
}
}
return result;
}
case 0x7C:
case 0x7D:
case 0x7E:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0x7F:

{
callbacks->string_start(context);
return result;
}
case 0x80:
case 0x81:
case 0x82:
case 0x83:
case 0x84:
case 0x85:
case 0x86:
case 0x87:
case 0x88:
case 0x89:
case 0x8A:
case 0x8B:
case 0x8C:
case 0x8D:
case 0x8E:
case 0x8F:
case 0x90:
case 0x91:
case 0x92:
case 0x93:
case 0x94:
case 0x95:
case 0x96:
case 0x97:

{
callbacks->array_start(
context, (size_t)_cbor_load_uint8(source) - 0x80);
return result;
}
case 0x98:

{
if (claim_bytes(1, source_size, &result)) {
callbacks->array_start(context, (size_t)_cbor_load_uint8(source + 1));
}
return result;
}
case 0x99:

{
if (claim_bytes(2, source_size, &result)) {
callbacks->array_start(context,
(size_t)_cbor_load_uint16(source + 1));
}
return result;
}
case 0x9A:

{
if (claim_bytes(4, source_size, &result)) {
callbacks->array_start(context,
(size_t)_cbor_load_uint32(source + 1));
}
return result;
}
case 0x9B:

{
if (claim_bytes(8, source_size, &result)) {
callbacks->array_start(context,
(size_t)_cbor_load_uint64(source + 1));
}
return result;
}
case 0x9C:
case 0x9D:
case 0x9E:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0x9F:

{
callbacks->indef_array_start(context);
return result;
}
case 0xA0:
case 0xA1:
case 0xA2:
case 0xA3:
case 0xA4:
case 0xA5:
case 0xA6:
case 0xA7:
case 0xA8:
case 0xA9:
case 0xAA:
case 0xAB:
case 0xAC:
case 0xAD:
case 0xAE:
case 0xAF:
case 0xB0:
case 0xB1:
case 0xB2:
case 0xB3:
case 0xB4:
case 0xB5:
case 0xB6:
case 0xB7:

{
callbacks->map_start(
context, (size_t)_cbor_load_uint8(source) - 0xA0);
return result;
}
case 0xB8:

{
if (claim_bytes(1, source_size, &result)) {
callbacks->map_start(context, (size_t)_cbor_load_uint8(source + 1));
}
return result;
}
case 0xB9:

{
if (claim_bytes(2, source_size, &result)) {
callbacks->map_start(context, (size_t)_cbor_load_uint16(source + 1));
}
return result;
}
case 0xBA:

{
if (claim_bytes(4, source_size, &result)) {
callbacks->map_start(context, (size_t)_cbor_load_uint32(source + 1));
}
return result;
}
case 0xBB:

{
if (claim_bytes(8, source_size, &result)) {
callbacks->map_start(context, (size_t)_cbor_load_uint64(source + 1));
}
return result;
}
case 0xBC:
case 0xBD:
case 0xBE:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0xBF:

{
callbacks->indef_map_start(context);
return result;
}
case 0xC0:

case 0xC1:

case 0xC2:

case 0xC3:

case 0xC4:

case 0xC5:

{
callbacks->tag(context,
_cbor_load_uint8(source) - 0xC0);
return result;
}
case 0xC6:
case 0xC7:
case 0xC8:
case 0xC9:
case 0xCA:
case 0xCB:
case 0xCC:
case 0xCD:
case 0xCE:
case 0xCF:
case 0xD0:
case 0xD1:
case 0xD2:
case 0xD3:
case 0xD4:
{
return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR};
}
case 0xD5:
case 0xD6:
case 0xD7:
{
callbacks->tag(context,
_cbor_load_uint8(source) - 0xC0);
return result;
}
case 0xD8:
{
if (claim_bytes(1, source_size, &result)) {
callbacks->tag(context, _cbor_load_uint8(source + 1));
}
return result;
}
case 0xD9:
{
if (claim_bytes(2, source_size, &result)) {
callbacks->tag(context, _cbor_load_uint16(source + 1));
}
return result;
}
case 0xDA:
{
if (claim_bytes(4, source_size, &result)) {
callbacks->tag(context, _cbor_load_uint32(source + 1));
}
return result;
}
case 0xDB:
{
if (claim_bytes(8, source_size, &result)) {
callbacks->tag(context, _cbor_load_uint64(source + 1));
}
return result;
}
case 0xDC:
case 0xDD:
case 0xDE:
case 0xDF:
{
return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR};
}
case 0xE0:
case 0xE1:
case 0xE2:
case 0xE3:
case 0xE4:
case 0xE5:
case 0xE6:
case 0xE7:
case 0xE8:
case 0xE9:
case 0xEA:
case 0xEB:
case 0xEC:
case 0xED:
case 0xEE:
case 0xEF:
case 0xF0:
case 0xF1:
case 0xF2:
case 0xF3:
{
return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR};
}
case 0xF4:

{
callbacks->boolean(context, false);
return result;
}
case 0xF5:

{
callbacks->boolean(context, true);
return result;
}
case 0xF6:

{
callbacks->null(context);
return result;
}
case 0xF7:

{
callbacks->undefined(context);
return result;
}
case 0xF8:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0xF9:

{
if (claim_bytes(2, source_size, &result)) {
callbacks->float2(context, _cbor_load_half(source + 1));
}
return result;
}
case 0xFA:

{
if (claim_bytes(4, source_size, &result)) {
callbacks->float4(context, _cbor_load_float(source + 1));
}
return result;
}
case 0xFB:

{
if (claim_bytes(8, source_size, &result)) {
callbacks->float8(context, _cbor_load_double(source + 1));
}
return result;
}
case 0xFC:
case 0xFD:
case 0xFE:

{ return (struct cbor_decoder_result){0, CBOR_DECODER_ERROR}; }
case 0xFF:

{
callbacks->indef_break(context);
return result;
}
default:
{
return result;
}
}
}
