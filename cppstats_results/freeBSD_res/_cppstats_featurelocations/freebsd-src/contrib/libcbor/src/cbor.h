






#if !defined(LIBCBOR_H_)
#define LIBCBOR_H_

#include "cbor/common.h"
#include "cbor/data.h"

#include "cbor/arrays.h"
#include "cbor/bytestrings.h"
#include "cbor/floats_ctrls.h"
#include "cbor/ints.h"
#include "cbor/maps.h"
#include "cbor/strings.h"
#include "cbor/tags.h"

#include "cbor/callbacks.h"
#include "cbor/cbor_export.h"
#include "cbor/encoding.h"
#include "cbor/serialization.h"
#include "cbor/streaming.h"

#if defined(__cplusplus)
extern "C" {
#endif















CBOR_EXPORT cbor_item_t* cbor_load(cbor_data source, size_t source_size,
struct cbor_load_result* result);








CBOR_EXPORT cbor_item_t* cbor_copy(cbor_item_t* item);

#if CBOR_PRETTY_PRINTER
#include <stdio.h>

CBOR_EXPORT void cbor_describe(cbor_item_t* item, FILE* out);
#endif

#if defined(__cplusplus)
}
#endif

#endif
