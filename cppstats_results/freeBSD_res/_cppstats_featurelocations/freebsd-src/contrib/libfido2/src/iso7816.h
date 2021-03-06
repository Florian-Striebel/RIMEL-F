





#if !defined(_ISO7816_H)
#define _ISO7816_H

#include <stdint.h>
#include <stdlib.h>

#include "packed.h"

#if defined(__cplusplus)
extern "C" {
#endif

PACKED_TYPE(iso7816_header_t,
struct iso7816_header {
uint8_t cla;
uint8_t ins;
uint8_t p1;
uint8_t p2;
uint8_t lc1;
uint8_t lc2;
uint8_t lc3;
})

typedef struct iso7816_apdu {
size_t alloc_len;
uint16_t payload_len;
uint8_t *payload_ptr;
iso7816_header_t header;
uint8_t payload[];
} iso7816_apdu_t;

const unsigned char *iso7816_ptr(const iso7816_apdu_t *);
int iso7816_add(iso7816_apdu_t *, const void *, size_t);
iso7816_apdu_t *iso7816_new(uint8_t, uint8_t, uint8_t, uint16_t);
size_t iso7816_len(const iso7816_apdu_t *);
void iso7816_free(iso7816_apdu_t **);

#if defined(__cplusplus)
}
#endif

#endif
