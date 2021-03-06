






#if !defined(LIBCBOR_SERIALIZATION_H)
#define LIBCBOR_SERIALIZATION_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif














CBOR_EXPORT size_t cbor_serialize(const cbor_item_t *item,
cbor_mutable_data buffer, size_t buffer_size);














CBOR_EXPORT size_t cbor_serialize_alloc(const cbor_item_t *item,
cbor_mutable_data *buffer,
size_t *buffer_size);








CBOR_EXPORT size_t cbor_serialize_uint(const cbor_item_t *, cbor_mutable_data,
size_t);








CBOR_EXPORT size_t cbor_serialize_negint(const cbor_item_t *, cbor_mutable_data,
size_t);








CBOR_EXPORT size_t cbor_serialize_bytestring(const cbor_item_t *,
cbor_mutable_data, size_t);








CBOR_EXPORT size_t cbor_serialize_string(const cbor_item_t *, cbor_mutable_data,
size_t);








CBOR_EXPORT size_t cbor_serialize_array(const cbor_item_t *, cbor_mutable_data,
size_t);








CBOR_EXPORT size_t cbor_serialize_map(const cbor_item_t *, cbor_mutable_data,
size_t);








CBOR_EXPORT size_t cbor_serialize_tag(const cbor_item_t *, cbor_mutable_data,
size_t);








CBOR_EXPORT size_t cbor_serialize_float_ctrl(const cbor_item_t *,
cbor_mutable_data, size_t);

#if defined(__cplusplus)
}
#endif

#endif
