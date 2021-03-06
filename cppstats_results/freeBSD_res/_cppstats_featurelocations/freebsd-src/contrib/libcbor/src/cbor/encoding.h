






#if !defined(LIBCBOR_ENCODING_H)
#define LIBCBOR_ENCODING_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif







CBOR_EXPORT size_t cbor_encode_uint8(uint8_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_uint16(uint16_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_uint32(uint32_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_uint64(uint64_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_uint(uint64_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_negint8(uint8_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_negint16(uint16_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_negint32(uint32_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_negint64(uint64_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_negint(uint64_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_bytestring_start(size_t, unsigned char *,
size_t);

CBOR_EXPORT size_t cbor_encode_indef_bytestring_start(unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_string_start(size_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_indef_string_start(unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_array_start(size_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_indef_array_start(unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_map_start(size_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_indef_map_start(unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_tag(uint64_t, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_bool(bool, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_null(unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_undef(unsigned char *, size_t);



























CBOR_EXPORT size_t cbor_encode_half(float, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_single(float, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_double(double, unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_break(unsigned char *, size_t);

CBOR_EXPORT size_t cbor_encode_ctrl(uint8_t, unsigned char *, size_t);

#if defined(__cplusplus)
}
#endif

#endif
