






#include "memory_utils.h"
#include "cbor/common.h"





size_t _cbor_highest_bit(size_t number) {
size_t bit = 0;
while (number != 0) {
bit++;
number >>= 1;
}

return bit;
}

bool _cbor_safe_to_multiply(size_t a, size_t b) {
return _cbor_highest_bit(a) + _cbor_highest_bit(b) <= sizeof(size_t) * 8;
}

void* _cbor_alloc_multiple(size_t item_size, size_t item_count) {
if (_cbor_safe_to_multiply(item_size, item_count)) {
return _CBOR_MALLOC(item_size * item_count);
} else {
return NULL;
}
}

void* _cbor_realloc_multiple(void* pointer, size_t item_size,
size_t item_count) {
if (_cbor_safe_to_multiply(item_size, item_count)) {
return _CBOR_REALLOC(pointer, item_size * item_count);
} else {
return NULL;
}
}
