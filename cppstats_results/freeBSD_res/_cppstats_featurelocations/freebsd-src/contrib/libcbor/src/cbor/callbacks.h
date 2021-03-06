






#if !defined(LIBCBOR_CALLBACKS_H)
#define LIBCBOR_CALLBACKS_H

#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif


typedef void (*cbor_int8_callback)(void *, uint8_t);


typedef void (*cbor_int16_callback)(void *, uint16_t);


typedef void (*cbor_int32_callback)(void *, uint32_t);


typedef void (*cbor_int64_callback)(void *, uint64_t);


typedef void (*cbor_simple_callback)(void *);


typedef void (*cbor_string_callback)(void *, cbor_data, size_t);


typedef void (*cbor_collection_callback)(void *, size_t);


typedef void (*cbor_float_callback)(void *, float);


typedef void (*cbor_double_callback)(void *, double);


typedef void (*cbor_bool_callback)(void *, bool);


struct cbor_callbacks {

cbor_int8_callback uint8;

cbor_int16_callback uint16;

cbor_int32_callback uint32;

cbor_int64_callback uint64;


cbor_int64_callback negint64;

cbor_int32_callback negint32;

cbor_int16_callback negint16;

cbor_int8_callback negint8;


cbor_simple_callback byte_string_start;

cbor_string_callback byte_string;


cbor_string_callback string;

cbor_simple_callback string_start;


cbor_simple_callback indef_array_start;

cbor_collection_callback array_start;


cbor_simple_callback indef_map_start;

cbor_collection_callback map_start;


cbor_int64_callback tag;


cbor_float_callback float2;

cbor_float_callback float4;

cbor_double_callback float8;

cbor_simple_callback undefined;

cbor_simple_callback null;

cbor_bool_callback boolean;


cbor_simple_callback indef_break;
};


CBOR_EXPORT void cbor_null_uint8_callback(void *, uint8_t);


CBOR_EXPORT void cbor_null_uint16_callback(void *, uint16_t);


CBOR_EXPORT void cbor_null_uint32_callback(void *, uint32_t);


CBOR_EXPORT void cbor_null_uint64_callback(void *, uint64_t);


CBOR_EXPORT void cbor_null_negint8_callback(void *, uint8_t);


CBOR_EXPORT void cbor_null_negint16_callback(void *, uint16_t);


CBOR_EXPORT void cbor_null_negint32_callback(void *, uint32_t);


CBOR_EXPORT void cbor_null_negint64_callback(void *, uint64_t);


CBOR_EXPORT void cbor_null_string_callback(void *, cbor_data, size_t);


CBOR_EXPORT void cbor_null_string_start_callback(void *);


CBOR_EXPORT void cbor_null_byte_string_callback(void *, cbor_data, size_t);


CBOR_EXPORT void cbor_null_byte_string_start_callback(void *);


CBOR_EXPORT void cbor_null_array_start_callback(void *, size_t);


CBOR_EXPORT void cbor_null_indef_array_start_callback(void *);


CBOR_EXPORT void cbor_null_map_start_callback(void *, size_t);


CBOR_EXPORT void cbor_null_indef_map_start_callback(void *);


CBOR_EXPORT void cbor_null_tag_callback(void *, uint64_t);


CBOR_EXPORT void cbor_null_float2_callback(void *, float);


CBOR_EXPORT void cbor_null_float4_callback(void *, float);


CBOR_EXPORT void cbor_null_float8_callback(void *, double);


CBOR_EXPORT void cbor_null_null_callback(void *);


CBOR_EXPORT void cbor_null_undefined_callback(void *);


CBOR_EXPORT void cbor_null_boolean_callback(void *, bool);


CBOR_EXPORT void cbor_null_indef_break_callback(void *);


CBOR_EXPORT extern const struct cbor_callbacks cbor_empty_callbacks;

#if defined(__cplusplus)
}
#endif

#endif
