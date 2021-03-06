






#if !defined(LIBCBOR_MAPS_H)
#define LIBCBOR_MAPS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif












CBOR_EXPORT size_t cbor_map_size(const cbor_item_t *item);






CBOR_EXPORT size_t cbor_map_allocated(const cbor_item_t *item);






CBOR_EXPORT cbor_item_t *cbor_new_definite_map(size_t size);






CBOR_EXPORT cbor_item_t *cbor_new_indefinite_map();











CBOR_EXPORT bool cbor_map_add(cbor_item_t *item, struct cbor_pair pair);










CBOR_EXPORT bool _cbor_map_add_key(cbor_item_t *item, cbor_item_t *key);










CBOR_EXPORT bool _cbor_map_add_value(cbor_item_t *item, cbor_item_t *value);






CBOR_EXPORT bool cbor_map_is_definite(const cbor_item_t *item);






CBOR_EXPORT bool cbor_map_is_indefinite(const cbor_item_t *item);







CBOR_EXPORT struct cbor_pair *cbor_map_handle(const cbor_item_t *item);

#if defined(__cplusplus)
}
#endif

#endif
