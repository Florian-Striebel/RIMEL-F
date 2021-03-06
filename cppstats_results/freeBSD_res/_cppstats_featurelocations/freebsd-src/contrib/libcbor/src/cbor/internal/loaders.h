






#if !defined(LIBCBOR_LOADERS_H)
#define LIBCBOR_LOADERS_H

#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif


uint8_t _cbor_load_uint8(const unsigned char *source);

uint16_t _cbor_load_uint16(const unsigned char *source);

uint32_t _cbor_load_uint32(const unsigned char *source);

uint64_t _cbor_load_uint64(const unsigned char *source);

double _cbor_load_half(cbor_data source);

float _cbor_load_float(cbor_data source);

double _cbor_load_double(cbor_data source);

#if defined(__cplusplus)
}
#endif

#endif
