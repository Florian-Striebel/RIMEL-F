






#include <stdio.h>
#include "cbor.h"

void usage() {
printf("Usage: readfile [input file]\n");
exit(1);
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


struct cbor_load_result result;
cbor_item_t* item = cbor_load(buffer, length, &result);

if (result.error.code != CBOR_ERR_NONE) {
printf(
"There was an error while reading the input near byte %zu (read %zu "
"bytes in total): ",
result.error.position, result.read);
switch (result.error.code) {
case CBOR_ERR_MALFORMATED: {
printf("Malformed data\n");
break;
}
case CBOR_ERR_MEMERROR: {
printf("Memory error -- perhaps the input is too large?\n");
break;
}
case CBOR_ERR_NODATA: {
printf("The input is empty\n");
break;
}
case CBOR_ERR_NOTENOUGHDATA: {
printf("Data seem to be missing -- is the input complete?\n");
break;
}
case CBOR_ERR_SYNTAXERROR: {
printf(
"Syntactically malformed data -- see "
"http://tools.ietf.org/html/rfc7049\n");
break;
}
case CBOR_ERR_NONE: {

break;
}
}
exit(1);
}


cbor_describe(item, stdout);
fflush(stdout);

cbor_decref(&item);

fclose(f);
}
