






#include <stdio.h>
#include <stdlib.h>
#include "cbor.h"






int comparUint(const void *a, const void *b) {
uint8_t av = cbor_get_uint8(*(cbor_item_t **)a),
bv = cbor_get_uint8(*(cbor_item_t **)b);

if (av < bv)
return -1;
else if (av == bv)
return 0;
else
return 1;
}

int main(int argc, char *argv[]) {
cbor_item_t *array = cbor_new_definite_array(4);
cbor_array_push(array, cbor_move(cbor_build_uint8(4)));
cbor_array_push(array, cbor_move(cbor_build_uint8(3)));
cbor_array_push(array, cbor_move(cbor_build_uint8(1)));
cbor_array_push(array, cbor_move(cbor_build_uint8(2)));

qsort(cbor_array_handle(array), cbor_array_size(array), sizeof(cbor_item_t *),
comparUint);

cbor_describe(array, stdout);
fflush(stdout);
}
