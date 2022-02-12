




























#include "crcspeed.h"


void crcspeed64little_init(crcfn64 crcfn, uint64_t table[8][256]) {
uint64_t crc;


for (int n = 0; n < 256; n++) {
unsigned char v = n;
table[0][n] = crcfn(0, &v, 1);
}


for (int n = 0; n < 256; n++) {
crc = table[0][n];
for (int k = 1; k < 8; k++) {
crc = table[0][crc & 0xff] ^ (crc >> 8);
table[k][n] = crc;
}
}
}

void crcspeed16little_init(crcfn16 crcfn, uint16_t table[8][256]) {
uint16_t crc;


for (int n = 0; n < 256; n++) {
table[0][n] = crcfn(0, &n, 1);
}


for (int n = 0; n < 256; n++) {
crc = table[0][n];
for (int k = 1; k < 8; k++) {
crc = table[0][(crc >> 8) & 0xff] ^ (crc << 8);
table[k][n] = crc;
}
}
}


static inline uint64_t rev8(uint64_t a) {
#if defined(__GNUC__) || defined(__clang__)
return __builtin_bswap64(a);
#else
uint64_t m;

m = UINT64_C(0xff00ff00ff00ff);
a = ((a >> 8) & m) | (a & m) << 8;
m = UINT64_C(0xffff0000ffff);
a = ((a >> 16) & m) | (a & m) << 16;
return a >> 32 | a << 32;
#endif
}



void crcspeed64big_init(crcfn64 fn, uint64_t big_table[8][256]) {

crcspeed64little_init(fn, big_table);
for (int k = 0; k < 8; k++) {
for (int n = 0; n < 256; n++) {
big_table[k][n] = rev8(big_table[k][n]);
}
}
}

void crcspeed16big_init(crcfn16 fn, uint16_t big_table[8][256]) {

crcspeed16little_init(fn, big_table);
for (int k = 0; k < 8; k++) {
for (int n = 0; n < 256; n++) {
big_table[k][n] = rev8(big_table[k][n]);
}
}
}






uint64_t crcspeed64little(uint64_t little_table[8][256], uint64_t crc,
void *buf, size_t len) {
unsigned char *next = buf;


while (len && ((uintptr_t)next & 7) != 0) {
crc = little_table[0][(crc ^ *next++) & 0xff] ^ (crc >> 8);
len--;
}


while (len >= 8) {
crc ^= *(uint64_t *)next;
crc = little_table[7][crc & 0xff] ^
little_table[6][(crc >> 8) & 0xff] ^
little_table[5][(crc >> 16) & 0xff] ^
little_table[4][(crc >> 24) & 0xff] ^
little_table[3][(crc >> 32) & 0xff] ^
little_table[2][(crc >> 40) & 0xff] ^
little_table[1][(crc >> 48) & 0xff] ^
little_table[0][crc >> 56];
next += 8;
len -= 8;
}


while (len) {
crc = little_table[0][(crc ^ *next++) & 0xff] ^ (crc >> 8);
len--;
}

return crc;
}

uint16_t crcspeed16little(uint16_t little_table[8][256], uint16_t crc,
void *buf, size_t len) {
unsigned char *next = buf;


while (len && ((uintptr_t)next & 7) != 0) {
crc = little_table[0][((crc >> 8) ^ *next++) & 0xff] ^ (crc << 8);
len--;
}


while (len >= 8) {
uint64_t n = *(uint64_t *)next;
crc = little_table[7][(n & 0xff) ^ ((crc >> 8) & 0xff)] ^
little_table[6][((n >> 8) & 0xff) ^ (crc & 0xff)] ^
little_table[5][(n >> 16) & 0xff] ^
little_table[4][(n >> 24) & 0xff] ^
little_table[3][(n >> 32) & 0xff] ^
little_table[2][(n >> 40) & 0xff] ^
little_table[1][(n >> 48) & 0xff] ^
little_table[0][n >> 56];
next += 8;
len -= 8;
}


while (len) {
crc = little_table[0][((crc >> 8) ^ *next++) & 0xff] ^ (crc << 8);
len--;
}

return crc;
}




uint64_t crcspeed64big(uint64_t big_table[8][256], uint64_t crc, void *buf,
size_t len) {
unsigned char *next = buf;

crc = rev8(crc);
while (len && ((uintptr_t)next & 7) != 0) {
crc = big_table[0][(crc >> 56) ^ *next++] ^ (crc << 8);
len--;
}

while (len >= 8) {
crc ^= *(uint64_t *)next;
crc = big_table[0][crc & 0xff] ^
big_table[1][(crc >> 8) & 0xff] ^
big_table[2][(crc >> 16) & 0xff] ^
big_table[3][(crc >> 24) & 0xff] ^
big_table[4][(crc >> 32) & 0xff] ^
big_table[5][(crc >> 40) & 0xff] ^
big_table[6][(crc >> 48) & 0xff] ^
big_table[7][crc >> 56];
next += 8;
len -= 8;
}

while (len) {
crc = big_table[0][(crc >> 56) ^ *next++] ^ (crc << 8);
len--;
}

return rev8(crc);
}


uint16_t crcspeed16big(uint16_t big_table[8][256], uint16_t crc_in, void *buf,
size_t len) {
unsigned char *next = buf;
uint64_t crc = crc_in;

crc = rev8(crc);
while (len && ((uintptr_t)next & 7) != 0) {
crc = big_table[0][((crc >> (56 - 8)) ^ *next++) & 0xff] ^ (crc >> 8);
len--;
}

while (len >= 8) {
uint64_t n = *(uint64_t *)next;
crc = big_table[0][(n & 0xff) ^ ((crc >> (56 - 8)) & 0xff)] ^
big_table[1][((n >> 8) & 0xff) ^ (crc & 0xff)] ^
big_table[2][(n >> 16) & 0xff] ^
big_table[3][(n >> 24) & 0xff] ^
big_table[4][(n >> 32) & 0xff] ^
big_table[5][(n >> 40) & 0xff] ^
big_table[6][(n >> 48) & 0xff] ^
big_table[7][n >> 56];
next += 8;
len -= 8;
}

while (len) {
crc = big_table[0][((crc >> (56 - 8)) ^ *next++) & 0xff] ^ (crc >> 8);
len--;
}

return rev8(crc);
}





uint64_t crcspeed64native(uint64_t table[8][256], uint64_t crc, void *buf,
size_t len) {
uint64_t n = 1;

return *(char *)&n ? crcspeed64little(table, crc, buf, len)
: crcspeed64big(table, crc, buf, len);
}

uint16_t crcspeed16native(uint16_t table[8][256], uint16_t crc, void *buf,
size_t len) {
uint64_t n = 1;

return *(char *)&n ? crcspeed16little(table, crc, buf, len)
: crcspeed16big(table, crc, buf, len);
}


void crcspeed64native_init(crcfn64 fn, uint64_t table[8][256]) {
uint64_t n = 1;

*(char *)&n ? crcspeed64little_init(fn, table)
: crcspeed64big_init(fn, table);
}

void crcspeed16native_init(crcfn16 fn, uint16_t table[8][256]) {
uint64_t n = 1;

*(char *)&n ? crcspeed16little_init(fn, table)
: crcspeed16big_init(fn, table);
}
