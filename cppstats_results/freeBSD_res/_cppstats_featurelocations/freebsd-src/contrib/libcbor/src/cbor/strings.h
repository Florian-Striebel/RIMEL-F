






#if !defined(LIBCBOR_STRINGS_H)
#define LIBCBOR_STRINGS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif














CBOR_EXPORT size_t cbor_string_length(const cbor_item_t *item);








CBOR_EXPORT size_t cbor_string_codepoint_count(const cbor_item_t *item);






CBOR_EXPORT bool cbor_string_is_definite(const cbor_item_t *item);






CBOR_EXPORT bool cbor_string_is_indefinite(const cbor_item_t *item);










CBOR_EXPORT cbor_mutable_data cbor_string_handle(const cbor_item_t *item);















CBOR_EXPORT void cbor_string_set_handle(
cbor_item_t *item, cbor_mutable_data CBOR_RESTRICT_POINTER data,
size_t length);









CBOR_EXPORT cbor_item_t **cbor_string_chunks_handle(const cbor_item_t *item);






CBOR_EXPORT size_t cbor_string_chunk_count(const cbor_item_t *item);












CBOR_EXPORT bool cbor_string_add_chunk(cbor_item_t *item, cbor_item_t *chunk);







CBOR_EXPORT cbor_item_t *cbor_new_definite_string();







CBOR_EXPORT cbor_item_t *cbor_new_indefinite_string();








CBOR_EXPORT cbor_item_t *cbor_build_string(const char *val);








CBOR_EXPORT cbor_item_t *cbor_build_stringn(const char *val, size_t length);

#if defined(__cplusplus)
}
#endif

#endif
