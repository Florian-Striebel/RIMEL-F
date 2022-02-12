






#if !defined(LIBCBOR_ENCODERS_H)
#define LIBCBOR_ENCODERS_H

#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif

size_t _cbor_encode_uint8(uint8_t value, unsigned char *buffer,
size_t buffer_size, uint8_t offset);

size_t _cbor_encode_uint16(uint16_t value, unsigned char *buffer,
size_t buffer_size, uint8_t offset);

size_t _cbor_encode_uint32(uint32_t value, unsigned char *buffer,
size_t buffer_size, uint8_t offset);

size_t _cbor_encode_uint64(uint64_t value, unsigned char *buffer,
size_t buffer_size, uint8_t offset);

size_t _cbor_encode_uint(uint64_t value, unsigned char *buffer,
size_t buffer_size, uint8_t offset);

#if defined(__cplusplus)
}
#endif

#endif
