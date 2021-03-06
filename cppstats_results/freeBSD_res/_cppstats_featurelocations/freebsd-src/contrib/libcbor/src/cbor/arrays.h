






#if !defined(LIBCBOR_ARRAYS_H)
#define LIBCBOR_ARRAYS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif






CBOR_EXPORT size_t cbor_array_size(const cbor_item_t* item);






CBOR_EXPORT size_t cbor_array_allocated(const cbor_item_t* item);







CBOR_EXPORT cbor_item_t* cbor_array_get(const cbor_item_t* item, size_t index);










CBOR_EXPORT bool cbor_array_set(cbor_item_t* item, size_t index,
cbor_item_t* value);










CBOR_EXPORT bool cbor_array_replace(cbor_item_t* item, size_t index,
cbor_item_t* value);






CBOR_EXPORT bool cbor_array_is_definite(const cbor_item_t* item);






CBOR_EXPORT bool cbor_array_is_indefinite(const cbor_item_t* item);









CBOR_EXPORT cbor_item_t** cbor_array_handle(const cbor_item_t* item);






CBOR_EXPORT cbor_item_t* cbor_new_definite_array(size_t size);





CBOR_EXPORT cbor_item_t* cbor_new_indefinite_array();










CBOR_EXPORT bool cbor_array_push(cbor_item_t* array, cbor_item_t* pushee);

#if defined(__cplusplus)
}
#endif

#endif
