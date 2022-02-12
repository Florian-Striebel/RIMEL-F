






#if !defined(LIBCBOR_MEMORY_UTILS_H)
#define LIBCBOR_MEMORY_UTILS_H

#include <stdbool.h>
#include <string.h>


bool _cbor_safe_to_multiply(size_t a, size_t b);








void* _cbor_alloc_multiple(size_t item_size, size_t item_count);











void* _cbor_realloc_multiple(void* pointer, size_t item_size,
size_t item_count);

#endif
