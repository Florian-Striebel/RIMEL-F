





#include "openbsd-compat.h"

#if defined(_WIN32) && !defined(HAVE_ENDIAN_H)









uint32_t
htole32(uint32_t in)
{
uint32_t out = 0;
uint8_t *b = (uint8_t *)&out;

b[0] = (uint8_t)((in >> 0) & 0xff);
b[1] = (uint8_t)((in >> 8) & 0xff);
b[2] = (uint8_t)((in >> 16) & 0xff);
b[3] = (uint8_t)((in >> 24) & 0xff);

return (out);
}

uint64_t
htole64(uint64_t in)
{
uint64_t out = 0;
uint8_t *b = (uint8_t *)&out;

b[0] = (uint8_t)((in >> 0) & 0xff);
b[1] = (uint8_t)((in >> 8) & 0xff);
b[2] = (uint8_t)((in >> 16) & 0xff);
b[3] = (uint8_t)((in >> 24) & 0xff);
b[4] = (uint8_t)((in >> 32) & 0xff);
b[5] = (uint8_t)((in >> 40) & 0xff);
b[6] = (uint8_t)((in >> 48) & 0xff);
b[7] = (uint8_t)((in >> 56) & 0xff);

return (out);
}

#endif
