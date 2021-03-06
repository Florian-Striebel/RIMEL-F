






#include <stdio.h>
#include <string.h>
#include "cbor.h"

void usage() {
printf("Usage: streaming_parser [input file]\n");
exit(1);
}








const char* key = "a secret key";
bool key_found = false;

void find_string(void* _ctx, cbor_data buffer, size_t len) {
if (key_found) {
printf("Found the value: %.*s\n", (int)len, buffer);
key_found = false;
} else if (len == strlen(key)) {
key_found = (memcmp(key, buffer, len) == 0);
}
}

int main(int argc, char* argv[]) {
if (argc != 2) usage();
FILE* f = fopen(argv[1], "rb");
if (f == NULL) usage();
fseek(f, 0, SEEK_END);
size_t length = (size_t)ftell(f);
fseek(f, 0, SEEK_SET);
unsigned char* buffer = malloc(length);
fread(buffer, length, 1, f);

struct cbor_callbacks callbacks = cbor_empty_callbacks;
struct cbor_decoder_result decode_result;
size_t bytes_read = 0;
callbacks.string = find_string;
while (bytes_read < length) {
decode_result = cbor_stream_decode(buffer + bytes_read, length - bytes_read,
&callbacks, NULL);
bytes_read += decode_result.read;
}

free(buffer);
fclose(f);
}
