






#include <stdio.h>
#include "cbor.h"

int main(int argc, char* argv[]) {
printf("Hello from libcbor %s\n", CBOR_VERSION);
printf("Custom allocation support: %s\n", CBOR_CUSTOM_ALLOC ? "yes" : "no");
printf("Pretty-printer support: %s\n", CBOR_PRETTY_PRINTER ? "yes" : "no");
printf("Buffer growth factor: %f\n", (float)CBOR_BUFFER_GROWTH);
}
