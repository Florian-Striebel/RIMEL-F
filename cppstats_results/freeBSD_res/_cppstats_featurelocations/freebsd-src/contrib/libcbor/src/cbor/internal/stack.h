






#if !defined(LIBCBOR_STACK_H)
#define LIBCBOR_STACK_H

#include "cbor/common.h"

#if defined(__cplusplus)
extern "C" {
#endif


struct _cbor_stack_record {
struct _cbor_stack_record *lower;
cbor_item_t *item;
size_t subitems;
};


struct _cbor_stack {
struct _cbor_stack_record *top;
size_t size;
};

struct _cbor_stack _cbor_stack_init();

void _cbor_stack_pop(struct _cbor_stack *);

struct _cbor_stack_record *_cbor_stack_push(struct _cbor_stack *, cbor_item_t *,
size_t);

#if defined(__cplusplus)
}
#endif

#endif
