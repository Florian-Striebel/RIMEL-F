






#if !defined(LIBCBOR_BYTESTRINGS_H)
#define LIBCBOR_BYTESTRINGS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif














CBOR_EXPORT size_t cbor_bytestring_length(const cbor_item_t *item);






CBOR_EXPORT bool cbor_bytestring_is_definite(const cbor_item_t *item);






CBOR_EXPORT bool cbor_bytestring_is_indefinite(const cbor_item_t *item);










CBOR_EXPORT cbor_mutable_data cbor_bytestring_handle(const cbor_item_t *item);








CBOR_EXPORT void cbor_bytestring_set_handle(
cbor_item_t *item, cbor_mutable_data CBOR_RESTRICT_POINTER data,
size_t length);









CBOR_EXPORT cbor_item_t **cbor_bytestring_chunks_handle(
const cbor_item_t *item);






CBOR_EXPORT size_t cbor_bytestring_chunk_count(const cbor_item_t *item);












CBOR_EXPORT bool cbor_bytestring_add_chunk(cbor_item_t *item,
cbor_item_t *chunk);







CBOR_EXPORT cbor_item_t *cbor_new_definite_bytestring();







CBOR_EXPORT cbor_item_t *cbor_new_indefinite_bytestring();










CBOR_EXPORT cbor_item_t *cbor_build_bytestring(cbor_data handle, size_t length);

#if defined(__cplusplus)
}
#endif

#endif
