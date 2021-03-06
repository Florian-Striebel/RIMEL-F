






#if !defined(LIBCBOR_INTS_H)
#define LIBCBOR_INTS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif












CBOR_EXPORT uint8_t cbor_get_uint8(const cbor_item_t *item);






CBOR_EXPORT uint16_t cbor_get_uint16(const cbor_item_t *item);






CBOR_EXPORT uint32_t cbor_get_uint32(const cbor_item_t *item);






CBOR_EXPORT uint64_t cbor_get_uint64(const cbor_item_t *item);






CBOR_EXPORT uint64_t cbor_get_int(const cbor_item_t *item);







CBOR_EXPORT void cbor_set_uint8(cbor_item_t *item, uint8_t value);







CBOR_EXPORT void cbor_set_uint16(cbor_item_t *item, uint16_t value);







CBOR_EXPORT void cbor_set_uint32(cbor_item_t *item, uint32_t value);







CBOR_EXPORT void cbor_set_uint64(cbor_item_t *item, uint64_t value);






CBOR_EXPORT cbor_int_width cbor_int_get_width(const cbor_item_t *item);







CBOR_EXPORT void cbor_mark_uint(cbor_item_t *item);







CBOR_EXPORT void cbor_mark_negint(cbor_item_t *item);








CBOR_EXPORT cbor_item_t *cbor_new_int8();








CBOR_EXPORT cbor_item_t *cbor_new_int16();








CBOR_EXPORT cbor_item_t *cbor_new_int32();








CBOR_EXPORT cbor_item_t *cbor_new_int64();






CBOR_EXPORT cbor_item_t *cbor_build_uint8(uint8_t value);






CBOR_EXPORT cbor_item_t *cbor_build_uint16(uint16_t value);






CBOR_EXPORT cbor_item_t *cbor_build_uint32(uint32_t value);






CBOR_EXPORT cbor_item_t *cbor_build_uint64(uint64_t value);






CBOR_EXPORT cbor_item_t *cbor_build_negint8(uint8_t value);






CBOR_EXPORT cbor_item_t *cbor_build_negint16(uint16_t value);






CBOR_EXPORT cbor_item_t *cbor_build_negint32(uint32_t value);






CBOR_EXPORT cbor_item_t *cbor_build_negint64(uint64_t value);

#if defined(__cplusplus)
}
#endif

#endif
