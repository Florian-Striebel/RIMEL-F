






#include <stdio.h>
#include "cbor.h"

int main(int argc, char* argv[]) {

cbor_item_t* root = cbor_new_definite_map(2);

cbor_map_add(root,
(struct cbor_pair){
.key = cbor_move(cbor_build_string("Is CBOR awesome?")),
.value = cbor_move(cbor_build_bool(true))});
cbor_map_add(root,
(struct cbor_pair){
.key = cbor_move(cbor_build_uint8(42)),
.value = cbor_move(cbor_build_string("Is the answer"))});

unsigned char* buffer;
size_t buffer_size,
length = cbor_serialize_alloc(root, &buffer, &buffer_size);

fwrite(buffer, 1, length, stdout);
free(buffer);

fflush(stdout);
cbor_decref(&root);
}
