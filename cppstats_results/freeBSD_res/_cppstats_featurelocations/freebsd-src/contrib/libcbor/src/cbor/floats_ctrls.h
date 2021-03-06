






#if !defined(LIBCBOR_FLOATS_CTRLS_H)
#define LIBCBOR_FLOATS_CTRLS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif












CBOR_EXPORT bool cbor_float_ctrl_is_ctrl(const cbor_item_t *item);






CBOR_EXPORT cbor_float_width cbor_float_get_width(const cbor_item_t *item);








CBOR_EXPORT float cbor_float_get_float2(const cbor_item_t *item);








CBOR_EXPORT float cbor_float_get_float4(const cbor_item_t *item);








CBOR_EXPORT double cbor_float_get_float8(const cbor_item_t *item);








CBOR_EXPORT double cbor_float_get_float(const cbor_item_t *item);






CBOR_EXPORT bool cbor_get_bool(const cbor_item_t *item);







CBOR_EXPORT cbor_item_t *cbor_new_ctrl();







CBOR_EXPORT cbor_item_t *cbor_new_float2();







CBOR_EXPORT cbor_item_t *cbor_new_float4();







CBOR_EXPORT cbor_item_t *cbor_new_float8();





CBOR_EXPORT cbor_item_t *cbor_new_null();





CBOR_EXPORT cbor_item_t *cbor_new_undef();






CBOR_EXPORT cbor_item_t *cbor_build_bool(bool value);












CBOR_EXPORT void cbor_set_ctrl(cbor_item_t *item, uint8_t value);






CBOR_EXPORT void cbor_set_bool(cbor_item_t *item, bool value);






CBOR_EXPORT void cbor_set_float2(cbor_item_t *item, float value);






CBOR_EXPORT void cbor_set_float4(cbor_item_t *item, float value);






CBOR_EXPORT void cbor_set_float8(cbor_item_t *item, double value);






CBOR_EXPORT uint8_t cbor_ctrl_value(const cbor_item_t *item);






CBOR_EXPORT cbor_item_t *cbor_build_float2(float value);






CBOR_EXPORT cbor_item_t *cbor_build_float4(float value);






CBOR_EXPORT cbor_item_t *cbor_build_float8(double value);






CBOR_EXPORT cbor_item_t *cbor_build_ctrl(uint8_t value);

#if defined(__cplusplus)
}
#endif

#endif
