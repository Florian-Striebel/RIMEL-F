






#if !defined(LIBCBOR_BUILDER_CALLBACKS_H)
#define LIBCBOR_BUILDER_CALLBACKS_H

#include "../callbacks.h"
#include "cbor/common.h"
#include "stack.h"

#if defined(__cplusplus)
extern "C" {
#endif


struct _cbor_decoder_context {

bool creation_failed;

bool syntax_error;
cbor_item_t *root;
struct _cbor_stack *stack;
};

void cbor_builder_uint8_callback(void *, uint8_t);

void cbor_builder_uint16_callback(void *, uint16_t);

void cbor_builder_uint32_callback(void *, uint32_t);

void cbor_builder_uint64_callback(void *, uint64_t);

void cbor_builder_negint8_callback(void *, uint8_t);

void cbor_builder_negint16_callback(void *, uint16_t);

void cbor_builder_negint32_callback(void *, uint32_t);

void cbor_builder_negint64_callback(void *, uint64_t);

void cbor_builder_string_callback(void *, cbor_data, size_t);

void cbor_builder_string_start_callback(void *);

void cbor_builder_byte_string_callback(void *, cbor_data, size_t);

void cbor_builder_byte_string_start_callback(void *);

void cbor_builder_array_start_callback(void *, size_t);

void cbor_builder_indef_array_start_callback(void *);

void cbor_builder_map_start_callback(void *, size_t);

void cbor_builder_indef_map_start_callback(void *);

void cbor_builder_tag_callback(void *, uint64_t);

void cbor_builder_float2_callback(void *, float);

void cbor_builder_float4_callback(void *, float);

void cbor_builder_float8_callback(void *, double);

void cbor_builder_null_callback(void *);

void cbor_builder_undefined_callback(void *);

void cbor_builder_boolean_callback(void *, bool);

void cbor_builder_indef_break_callback(void *);

#if defined(__cplusplus)
}
#endif

#endif
