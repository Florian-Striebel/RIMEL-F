






#if !defined(LIBCBOR_STREAMING_H)
#define LIBCBOR_STREAMING_H

#include "callbacks.h"
#include "cbor/cbor_export.h"
#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif











CBOR_EXPORT struct cbor_decoder_result cbor_stream_decode(
cbor_data buffer, size_t buffer_size,
const struct cbor_callbacks* callbacks, void* context);

#if defined(__cplusplus)
}
#endif

#endif
