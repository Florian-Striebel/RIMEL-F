






#include "unicode.h"

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static const uint8_t utf8d[] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
0x3, 0x3, 0x4, 0x3, 0x3,
0xb, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
0x8, 0x8, 0x8, 0x8, 0x8,
0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4,
0x6, 0x1, 0x1, 0x1, 0x1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
1, 0, 1, 0, 1, 1, 1, 1, 1, 1,
1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 3, 1, 3, 1, 1, 1, 1, 1, 1,
1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1,
1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};




uint32_t _cbor_unicode_decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
uint32_t type = utf8d[byte];

*codep = (*state != UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6)
: (0xff >> type) & (byte);

*state = utf8d[256 + *state * 16 + type];
return *state;
}

size_t _cbor_unicode_codepoint_count(cbor_data source, size_t source_length,
struct _cbor_unicode_status* status) {
*status =
(struct _cbor_unicode_status){.location = 0, .status = _CBOR_UNICODE_OK};
uint32_t codepoint, state = UTF8_ACCEPT, res;
size_t pos = 0, count = 0;

for (; pos < source_length; pos++) {
res = _cbor_unicode_decode(&state, &codepoint, source[pos]);

if (res == UTF8_ACCEPT) {
count++;
} else if (res == UTF8_REJECT) {
goto error;
}
}


if (state != UTF8_ACCEPT) goto error;

return count;

error:
*status = (struct _cbor_unicode_status){.location = pos,
.status = _CBOR_UNICODE_BADCP};
return -1;
}
