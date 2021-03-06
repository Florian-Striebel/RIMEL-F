






#if !defined(LIBCBOR_TAGS_H)
#define LIBCBOR_TAGS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif













CBOR_EXPORT cbor_item_t *cbor_new_tag(uint64_t value);






CBOR_EXPORT cbor_item_t *cbor_tag_item(const cbor_item_t *item);






CBOR_EXPORT uint64_t cbor_tag_value(const cbor_item_t *item);






CBOR_EXPORT void cbor_tag_set_item(cbor_item_t *item, cbor_item_t *tagged_item);







CBOR_EXPORT cbor_item_t *cbor_build_tag(uint64_t value, cbor_item_t *item);

#if defined(__cplusplus)
}
#endif

#endif
