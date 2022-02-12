






#if !defined(LIBCBOR_COMMON_H)
#define LIBCBOR_COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "cbor/cbor_export.h"
#include "cbor/configuration.h"
#include "data.h"

#if defined(__cplusplus)
extern "C" {








#define CBOR_RESTRICT_POINTER

#else


#define CBOR_RESTRICT_POINTER CBOR_RESTRICT_SPECIFIER

#endif

static const uint8_t cbor_major_version = CBOR_MAJOR_VERSION;
static const uint8_t cbor_minor_version = CBOR_MINOR_VERSION;
static const uint8_t cbor_patch_version = CBOR_PATCH_VERSION;

#define CBOR_VERSION TO_STR(CBOR_MAJOR_VERSION) "." TO_STR(CBOR_MINOR_VERSION) "." TO_STR(CBOR_PATCH_VERSION)


#define CBOR_HEX_VERSION ((CBOR_MAJOR_VERSION << 16) | (CBOR_MINOR_VERSION << 8) | CBOR_PATCH_VERSION)




#if defined(DEBUG)
#include <stdio.h>
#define debug_print(fmt, ...) do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)





#else
#define debug_print(fmt, ...) do { } while (0)


#endif

#define TO_STR_(x) #x
#define TO_STR(x) TO_STR_(x)


#define _CBOR_NOTNULL(cbor_item) do { if (cbor_item == NULL) { return NULL; } } while (0)







#define _CBOR_DEPENDENT_NOTNULL(cbor_item, pointer) do { if (pointer == NULL) { _CBOR_FREE(cbor_item); return NULL; } } while (0)







#if CBOR_CUSTOM_ALLOC

typedef void *(*_cbor_malloc_t)(size_t);
typedef void *(*_cbor_realloc_t)(void *, size_t);
typedef void (*_cbor_free_t)(void *);

CBOR_EXPORT extern _cbor_malloc_t _cbor_malloc;
CBOR_EXPORT extern _cbor_realloc_t _cbor_realloc;
CBOR_EXPORT extern _cbor_free_t _cbor_free;




















CBOR_EXPORT void cbor_set_allocs(_cbor_malloc_t custom_malloc,
_cbor_realloc_t custom_realloc,
_cbor_free_t custom_free);

#define _CBOR_MALLOC _cbor_malloc
#define _CBOR_REALLOC _cbor_realloc
#define _CBOR_FREE _cbor_free

#else

#define _CBOR_MALLOC malloc
#define _CBOR_REALLOC realloc
#define _CBOR_FREE free

#endif












CBOR_EXPORT cbor_type cbor_typeof(
const cbor_item_t *item);







CBOR_EXPORT bool cbor_isa_uint(const cbor_item_t *item);





CBOR_EXPORT bool cbor_isa_negint(const cbor_item_t *item);





CBOR_EXPORT bool cbor_isa_bytestring(const cbor_item_t *item);





CBOR_EXPORT bool cbor_isa_string(const cbor_item_t *item);





CBOR_EXPORT bool cbor_isa_array(const cbor_item_t *item);





CBOR_EXPORT bool cbor_isa_map(const cbor_item_t *item);





CBOR_EXPORT bool cbor_isa_tag(const cbor_item_t *item);





CBOR_EXPORT bool cbor_isa_float_ctrl(const cbor_item_t *item);







CBOR_EXPORT bool cbor_is_int(const cbor_item_t *item);





CBOR_EXPORT bool cbor_is_float(const cbor_item_t *item);





CBOR_EXPORT bool cbor_is_bool(const cbor_item_t *item);











CBOR_EXPORT bool cbor_is_null(const cbor_item_t *item);











CBOR_EXPORT bool cbor_is_undef(const cbor_item_t *item);














CBOR_EXPORT cbor_item_t *cbor_incref(cbor_item_t *item);








CBOR_EXPORT void cbor_decref(cbor_item_t **item);








CBOR_EXPORT void cbor_intermediate_decref(cbor_item_t *item);










CBOR_EXPORT size_t cbor_refcount(const cbor_item_t *item);
















CBOR_EXPORT cbor_item_t *cbor_move(cbor_item_t *item);

#if defined(__cplusplus)
}
#endif

#endif
