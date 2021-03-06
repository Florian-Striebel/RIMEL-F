





















































































































































































#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "zmalloc.h"
#include "util.h"
#include "ziplist.h"
#include "config.h"
#include "endianconv.h"
#include "redisassert.h"

#define ZIP_END 255
#define ZIP_BIG_PREVLEN 254







#define ZIP_STR_MASK 0xc0
#define ZIP_INT_MASK 0x30
#define ZIP_STR_06B (0 << 6)
#define ZIP_STR_14B (1 << 6)
#define ZIP_STR_32B (2 << 6)
#define ZIP_INT_16B (0xc0 | 0<<4)
#define ZIP_INT_32B (0xc0 | 1<<4)
#define ZIP_INT_64B (0xc0 | 2<<4)
#define ZIP_INT_24B (0xc0 | 3<<4)
#define ZIP_INT_8B 0xfe



#define ZIP_INT_IMM_MASK 0x0f

#define ZIP_INT_IMM_MIN 0xf1
#define ZIP_INT_IMM_MAX 0xfd

#define INT24_MAX 0x7fffff
#define INT24_MIN (-INT24_MAX - 1)



#define ZIP_IS_STR(enc) (((enc) & ZIP_STR_MASK) < ZIP_STR_MASK)




#define ZIPLIST_BYTES(zl) (*((uint32_t*)(zl)))


#define ZIPLIST_TAIL_OFFSET(zl) (*((uint32_t*)((zl)+sizeof(uint32_t))))



#define ZIPLIST_LENGTH(zl) (*((uint16_t*)((zl)+sizeof(uint32_t)*2)))




#define ZIPLIST_HEADER_SIZE (sizeof(uint32_t)*2+sizeof(uint16_t))


#define ZIPLIST_END_SIZE (sizeof(uint8_t))


#define ZIPLIST_ENTRY_HEAD(zl) ((zl)+ZIPLIST_HEADER_SIZE)



#define ZIPLIST_ENTRY_TAIL(zl) ((zl)+intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl)))



#define ZIPLIST_ENTRY_END(zl) ((zl)+intrev32ifbe(ZIPLIST_BYTES(zl))-1)






#define ZIPLIST_INCR_LENGTH(zl,incr) { if (intrev16ifbe(ZIPLIST_LENGTH(zl)) < UINT16_MAX) ZIPLIST_LENGTH(zl) = intrev16ifbe(intrev16ifbe(ZIPLIST_LENGTH(zl))+incr); }






#define ZIPLIST_MAX_SAFETY_SIZE (1<<30)
int ziplistSafeToAdd(unsigned char* zl, size_t add) {
size_t len = zl? ziplistBlobLen(zl): 0;
if (len + add > ZIPLIST_MAX_SAFETY_SIZE)
return 0;
return 1;
}





typedef struct zlentry {
unsigned int prevrawlensize;
unsigned int prevrawlen;
unsigned int lensize;


unsigned int len;




unsigned int headersize;
unsigned char encoding;



unsigned char *p;

} zlentry;

#define ZIPLIST_ENTRY_ZERO(zle) { (zle)->prevrawlensize = (zle)->prevrawlen = 0; (zle)->lensize = (zle)->len = (zle)->headersize = 0; (zle)->encoding = 0; (zle)->p = NULL; }








#define ZIP_ENTRY_ENCODING(ptr, encoding) do { (encoding) = ((ptr)[0]); if ((encoding) < ZIP_STR_MASK) (encoding) &= ZIP_STR_MASK; } while(0)




#define ZIP_ENCODING_SIZE_INVALID 0xff


static inline unsigned int zipEncodingLenSize(unsigned char encoding) {
if (encoding == ZIP_INT_16B || encoding == ZIP_INT_32B ||
encoding == ZIP_INT_24B || encoding == ZIP_INT_64B ||
encoding == ZIP_INT_8B)
return 1;
if (encoding >= ZIP_INT_IMM_MIN && encoding <= ZIP_INT_IMM_MAX)
return 1;
if (encoding == ZIP_STR_06B)
return 1;
if (encoding == ZIP_STR_14B)
return 2;
if (encoding == ZIP_STR_32B)
return 5;
return ZIP_ENCODING_SIZE_INVALID;
}

#define ZIP_ASSERT_ENCODING(encoding) do { assert(zipEncodingLenSize(encoding) != ZIP_ENCODING_SIZE_INVALID); } while (0)




static inline unsigned int zipIntSize(unsigned char encoding) {
switch(encoding) {
case ZIP_INT_8B: return 1;
case ZIP_INT_16B: return 2;
case ZIP_INT_24B: return 3;
case ZIP_INT_32B: return 4;
case ZIP_INT_64B: return 8;
}
if (encoding >= ZIP_INT_IMM_MIN && encoding <= ZIP_INT_IMM_MAX)
return 0;

redis_unreachable();
return 0;
}













unsigned int zipStoreEntryEncoding(unsigned char *p, unsigned char encoding, unsigned int rawlen) {
unsigned char len = 1, buf[5];

if (ZIP_IS_STR(encoding)) {


if (rawlen <= 0x3f) {
if (!p) return len;
buf[0] = ZIP_STR_06B | rawlen;
} else if (rawlen <= 0x3fff) {
len += 1;
if (!p) return len;
buf[0] = ZIP_STR_14B | ((rawlen >> 8) & 0x3f);
buf[1] = rawlen & 0xff;
} else {
len += 4;
if (!p) return len;
buf[0] = ZIP_STR_32B;
buf[1] = (rawlen >> 24) & 0xff;
buf[2] = (rawlen >> 16) & 0xff;
buf[3] = (rawlen >> 8) & 0xff;
buf[4] = rawlen & 0xff;
}
} else {

if (!p) return len;
buf[0] = encoding;
}


memcpy(p,buf,len);
return len;
}







#define ZIP_DECODE_LENGTH(ptr, encoding, lensize, len) do { if ((encoding) < ZIP_STR_MASK) { if ((encoding) == ZIP_STR_06B) { (lensize) = 1; (len) = (ptr)[0] & 0x3f; } else if ((encoding) == ZIP_STR_14B) { (lensize) = 2; (len) = (((ptr)[0] & 0x3f) << 8) | (ptr)[1]; } else if ((encoding) == ZIP_STR_32B) { (lensize) = 5; (len) = ((uint32_t)(ptr)[1] << 24) | ((uint32_t)(ptr)[2] << 16) | ((uint32_t)(ptr)[3] << 8) | ((uint32_t)(ptr)[4]); } else { (lensize) = 0; (len) = 0; } } else { (lensize) = 1; if ((encoding) == ZIP_INT_8B) (len) = 1; else if ((encoding) == ZIP_INT_16B) (len) = 2; else if ((encoding) == ZIP_INT_24B) (len) = 3; else if ((encoding) == ZIP_INT_32B) (len) = 4; else if ((encoding) == ZIP_INT_64B) (len) = 8; else if (encoding >= ZIP_INT_IMM_MIN && encoding <= ZIP_INT_IMM_MAX) (len) = 0; else (lensize) = (len) = 0; } } while(0)


































int zipStorePrevEntryLengthLarge(unsigned char *p, unsigned int len) {
uint32_t u32;
if (p != NULL) {
p[0] = ZIP_BIG_PREVLEN;
u32 = len;
memcpy(p+1,&u32,sizeof(u32));
memrev32ifbe(p+1);
}
return 1 + sizeof(uint32_t);
}



unsigned int zipStorePrevEntryLength(unsigned char *p, unsigned int len) {
if (p == NULL) {
return (len < ZIP_BIG_PREVLEN) ? 1 : sizeof(uint32_t) + 1;
} else {
if (len < ZIP_BIG_PREVLEN) {
p[0] = len;
return 1;
} else {
return zipStorePrevEntryLengthLarge(p,len);
}
}
}



#define ZIP_DECODE_PREVLENSIZE(ptr, prevlensize) do { if ((ptr)[0] < ZIP_BIG_PREVLEN) { (prevlensize) = 1; } else { (prevlensize) = 5; } } while(0)














#define ZIP_DECODE_PREVLEN(ptr, prevlensize, prevlen) do { ZIP_DECODE_PREVLENSIZE(ptr, prevlensize); if ((prevlensize) == 1) { (prevlen) = (ptr)[0]; } else { (prevlen) = ((ptr)[4] << 24) | ((ptr)[3] << 16) | ((ptr)[2] << 8) | ((ptr)[1]); } } while(0)


























int zipPrevLenByteDiff(unsigned char *p, unsigned int len) {
unsigned int prevlensize;
ZIP_DECODE_PREVLENSIZE(p, prevlensize);
return zipStorePrevEntryLength(NULL, len) - prevlensize;
}



int zipTryEncoding(unsigned char *entry, unsigned int entrylen, long long *v, unsigned char *encoding) {
long long value;

if (entrylen >= 32 || entrylen == 0) return 0;
if (string2ll((char*)entry,entrylen,&value)) {


if (value >= 0 && value <= 12) {
*encoding = ZIP_INT_IMM_MIN+value;
} else if (value >= INT8_MIN && value <= INT8_MAX) {
*encoding = ZIP_INT_8B;
} else if (value >= INT16_MIN && value <= INT16_MAX) {
*encoding = ZIP_INT_16B;
} else if (value >= INT24_MIN && value <= INT24_MAX) {
*encoding = ZIP_INT_24B;
} else if (value >= INT32_MIN && value <= INT32_MAX) {
*encoding = ZIP_INT_32B;
} else {
*encoding = ZIP_INT_64B;
}
*v = value;
return 1;
}
return 0;
}


void zipSaveInteger(unsigned char *p, int64_t value, unsigned char encoding) {
int16_t i16;
int32_t i32;
int64_t i64;
if (encoding == ZIP_INT_8B) {
((int8_t*)p)[0] = (int8_t)value;
} else if (encoding == ZIP_INT_16B) {
i16 = value;
memcpy(p,&i16,sizeof(i16));
memrev16ifbe(p);
} else if (encoding == ZIP_INT_24B) {
i32 = ((uint64_t)value)<<8;
memrev32ifbe(&i32);
memcpy(p,((uint8_t*)&i32)+1,sizeof(i32)-sizeof(uint8_t));
} else if (encoding == ZIP_INT_32B) {
i32 = value;
memcpy(p,&i32,sizeof(i32));
memrev32ifbe(p);
} else if (encoding == ZIP_INT_64B) {
i64 = value;
memcpy(p,&i64,sizeof(i64));
memrev64ifbe(p);
} else if (encoding >= ZIP_INT_IMM_MIN && encoding <= ZIP_INT_IMM_MAX) {

} else {
assert(NULL);
}
}


int64_t zipLoadInteger(unsigned char *p, unsigned char encoding) {
int16_t i16;
int32_t i32;
int64_t i64, ret = 0;
if (encoding == ZIP_INT_8B) {
ret = ((int8_t*)p)[0];
} else if (encoding == ZIP_INT_16B) {
memcpy(&i16,p,sizeof(i16));
memrev16ifbe(&i16);
ret = i16;
} else if (encoding == ZIP_INT_32B) {
memcpy(&i32,p,sizeof(i32));
memrev32ifbe(&i32);
ret = i32;
} else if (encoding == ZIP_INT_24B) {
i32 = 0;
memcpy(((uint8_t*)&i32)+1,p,sizeof(i32)-sizeof(uint8_t));
memrev32ifbe(&i32);
ret = i32>>8;
} else if (encoding == ZIP_INT_64B) {
memcpy(&i64,p,sizeof(i64));
memrev64ifbe(&i64);
ret = i64;
} else if (encoding >= ZIP_INT_IMM_MIN && encoding <= ZIP_INT_IMM_MAX) {
ret = (encoding & ZIP_INT_IMM_MASK)-1;
} else {
assert(NULL);
}
return ret;
}







static inline void zipEntry(unsigned char *p, zlentry *e) {
ZIP_DECODE_PREVLEN(p, e->prevrawlensize, e->prevrawlen);
ZIP_ENTRY_ENCODING(p + e->prevrawlensize, e->encoding);
ZIP_DECODE_LENGTH(p + e->prevrawlensize, e->encoding, e->lensize, e->len);
assert(e->lensize != 0);
e->headersize = e->prevrawlensize + e->lensize;
e->p = p;
}





static inline int zipEntrySafe(unsigned char* zl, size_t zlbytes, unsigned char *p, zlentry *e, int validate_prevlen) {
unsigned char *zlfirst = zl + ZIPLIST_HEADER_SIZE;
unsigned char *zllast = zl + zlbytes - ZIPLIST_END_SIZE;
#define OUT_OF_RANGE(p) (unlikely((p) < zlfirst || (p) > zllast))



if (p >= zlfirst && p + 10 < zllast) {
ZIP_DECODE_PREVLEN(p, e->prevrawlensize, e->prevrawlen);
ZIP_ENTRY_ENCODING(p + e->prevrawlensize, e->encoding);
ZIP_DECODE_LENGTH(p + e->prevrawlensize, e->encoding, e->lensize, e->len);
e->headersize = e->prevrawlensize + e->lensize;
e->p = p;

if (unlikely(e->lensize == 0))
return 0;

if (OUT_OF_RANGE(p + e->headersize + e->len))
return 0;

if (validate_prevlen && OUT_OF_RANGE(p - e->prevrawlen))
return 0;
return 1;
}


if (OUT_OF_RANGE(p))
return 0;


ZIP_DECODE_PREVLENSIZE(p, e->prevrawlensize);
if (OUT_OF_RANGE(p + e->prevrawlensize))
return 0;


ZIP_ENTRY_ENCODING(p + e->prevrawlensize, e->encoding);
e->lensize = zipEncodingLenSize(e->encoding);
if (unlikely(e->lensize == ZIP_ENCODING_SIZE_INVALID))
return 0;


if (OUT_OF_RANGE(p + e->prevrawlensize + e->lensize))
return 0;


ZIP_DECODE_PREVLEN(p, e->prevrawlensize, e->prevrawlen);
ZIP_DECODE_LENGTH(p + e->prevrawlensize, e->encoding, e->lensize, e->len);
e->headersize = e->prevrawlensize + e->lensize;


if (OUT_OF_RANGE(p + e->headersize + e->len))
return 0;


if (validate_prevlen && OUT_OF_RANGE(p - e->prevrawlen))
return 0;

e->p = p;
return 1;
#undef OUT_OF_RANGE
}


static inline unsigned int zipRawEntryLengthSafe(unsigned char* zl, size_t zlbytes, unsigned char *p) {
zlentry e;
assert(zipEntrySafe(zl, zlbytes, p, &e, 0));
return e.headersize + e.len;
}


static inline unsigned int zipRawEntryLength(unsigned char *p) {
zlentry e;
zipEntry(p, &e);
return e.headersize + e.len;
}


static inline void zipAssertValidEntry(unsigned char* zl, size_t zlbytes, unsigned char *p) {
zlentry e;
assert(zipEntrySafe(zl, zlbytes, p, &e, 1));
}


unsigned char *ziplistNew(void) {
unsigned int bytes = ZIPLIST_HEADER_SIZE+ZIPLIST_END_SIZE;
unsigned char *zl = zmalloc(bytes);
ZIPLIST_BYTES(zl) = intrev32ifbe(bytes);
ZIPLIST_TAIL_OFFSET(zl) = intrev32ifbe(ZIPLIST_HEADER_SIZE);
ZIPLIST_LENGTH(zl) = 0;
zl[bytes-1] = ZIP_END;
return zl;
}


unsigned char *ziplistResize(unsigned char *zl, size_t len) {
assert(len < UINT32_MAX);
zl = zrealloc(zl,len);
ZIPLIST_BYTES(zl) = intrev32ifbe(len);
zl[len-1] = ZIP_END;
return zl;
}





















unsigned char *__ziplistCascadeUpdate(unsigned char *zl, unsigned char *p) {
zlentry cur;
size_t prevlen, prevlensize, prevoffset;
size_t firstentrylen;
size_t rawlen, curlen = intrev32ifbe(ZIPLIST_BYTES(zl));
size_t extra = 0, cnt = 0, offset;
size_t delta = 4;
unsigned char *tail = zl + intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl));


if (p[0] == ZIP_END) return zl;

zipEntry(p, &cur);
firstentrylen = prevlen = cur.headersize + cur.len;
prevlensize = zipStorePrevEntryLength(NULL, prevlen);
prevoffset = p - zl;
p += prevlen;


while (p[0] != ZIP_END) {
assert(zipEntrySafe(zl, curlen, p, &cur, 0));


if (cur.prevrawlen == prevlen) break;


if (cur.prevrawlensize >= prevlensize) {
if (cur.prevrawlensize == prevlensize) {
zipStorePrevEntryLength(p, prevlen);
} else {


zipStorePrevEntryLengthLarge(p, prevlen);
}
break;
}


assert(cur.prevrawlen == 0 || cur.prevrawlen + delta == prevlen);


rawlen = cur.headersize + cur.len;
prevlen = rawlen + delta;
prevlensize = zipStorePrevEntryLength(NULL, prevlen);
prevoffset = p - zl;
p += rawlen;
extra += delta;
cnt++;
}


if (extra == 0) return zl;


if (tail == zl + prevoffset) {


if (extra - delta != 0) {
ZIPLIST_TAIL_OFFSET(zl) =
intrev32ifbe(intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl))+extra-delta);
}
} else {

ZIPLIST_TAIL_OFFSET(zl) =
intrev32ifbe(intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl))+extra);
}



offset = p - zl;
zl = ziplistResize(zl, curlen + extra);
p = zl + offset;
memmove(p + extra, p, curlen - offset - 1);
p += extra;


while (cnt) {
zipEntry(zl + prevoffset, &cur);
rawlen = cur.headersize + cur.len;

memmove(p - (rawlen - cur.prevrawlensize),
zl + prevoffset + cur.prevrawlensize,
rawlen - cur.prevrawlensize);
p -= (rawlen + delta);
if (cur.prevrawlen == 0) {

zipStorePrevEntryLength(p, firstentrylen);
} else {

zipStorePrevEntryLength(p, cur.prevrawlen+delta);
}

prevoffset -= cur.prevrawlen;
cnt--;
}
return zl;
}


unsigned char *__ziplistDelete(unsigned char *zl, unsigned char *p, unsigned int num) {
unsigned int i, totlen, deleted = 0;
size_t offset;
int nextdiff = 0;
zlentry first, tail;
size_t zlbytes = intrev32ifbe(ZIPLIST_BYTES(zl));

zipEntry(p, &first);
for (i = 0; p[0] != ZIP_END && i < num; i++) {
p += zipRawEntryLengthSafe(zl, zlbytes, p);
deleted++;
}

assert(p >= first.p);
totlen = p-first.p;
if (totlen > 0) {
uint32_t set_tail;
if (p[0] != ZIP_END) {




nextdiff = zipPrevLenByteDiff(p,first.prevrawlen);





p -= nextdiff;
assert(p >= first.p && p<zl+zlbytes-1);
zipStorePrevEntryLength(p,first.prevrawlen);


set_tail = intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl))-totlen;




assert(zipEntrySafe(zl, zlbytes, p, &tail, 1));
if (p[tail.headersize+tail.len] != ZIP_END) {
set_tail = set_tail + nextdiff;
}





size_t bytes_to_move = zlbytes-(p-zl)-1;
memmove(first.p,p,bytes_to_move);
} else {

set_tail = (first.p-zl)-first.prevrawlen;
}


offset = first.p-zl;
zlbytes -= totlen - nextdiff;
zl = ziplistResize(zl, zlbytes);
p = zl+offset;


ZIPLIST_INCR_LENGTH(zl,-deleted);


assert(set_tail <= zlbytes - ZIPLIST_END_SIZE);
ZIPLIST_TAIL_OFFSET(zl) = intrev32ifbe(set_tail);



if (nextdiff != 0)
zl = __ziplistCascadeUpdate(zl,p);
}
return zl;
}


unsigned char *__ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen) {
size_t curlen = intrev32ifbe(ZIPLIST_BYTES(zl)), reqlen, newlen;
unsigned int prevlensize, prevlen = 0;
size_t offset;
int nextdiff = 0;
unsigned char encoding = 0;
long long value = 123456789;


zlentry tail;


if (p[0] != ZIP_END) {
ZIP_DECODE_PREVLEN(p, prevlensize, prevlen);
} else {
unsigned char *ptail = ZIPLIST_ENTRY_TAIL(zl);
if (ptail[0] != ZIP_END) {
prevlen = zipRawEntryLengthSafe(zl, curlen, ptail);
}
}


if (zipTryEncoding(s,slen,&value,&encoding)) {

reqlen = zipIntSize(encoding);
} else {


reqlen = slen;
}


reqlen += zipStorePrevEntryLength(NULL,prevlen);
reqlen += zipStoreEntryEncoding(NULL,encoding,slen);




int forcelarge = 0;
nextdiff = (p[0] != ZIP_END) ? zipPrevLenByteDiff(p,reqlen) : 0;
if (nextdiff == -4 && reqlen < 4) {
nextdiff = 0;
forcelarge = 1;
}


offset = p-zl;
newlen = curlen+reqlen+nextdiff;
zl = ziplistResize(zl,newlen);
p = zl+offset;


if (p[0] != ZIP_END) {

memmove(p+reqlen,p-nextdiff,curlen-offset-1+nextdiff);


if (forcelarge)
zipStorePrevEntryLengthLarge(p+reqlen,reqlen);
else
zipStorePrevEntryLength(p+reqlen,reqlen);


ZIPLIST_TAIL_OFFSET(zl) =
intrev32ifbe(intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl))+reqlen);




assert(zipEntrySafe(zl, newlen, p+reqlen, &tail, 1));
if (p[reqlen+tail.headersize+tail.len] != ZIP_END) {
ZIPLIST_TAIL_OFFSET(zl) =
intrev32ifbe(intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl))+nextdiff);
}
} else {

ZIPLIST_TAIL_OFFSET(zl) = intrev32ifbe(p-zl);
}



if (nextdiff != 0) {
offset = p-zl;
zl = __ziplistCascadeUpdate(zl,p+reqlen);
p = zl+offset;
}


p += zipStorePrevEntryLength(p,prevlen);
p += zipStoreEntryEncoding(p,encoding,slen);
if (ZIP_IS_STR(encoding)) {
memcpy(p,s,slen);
} else {
zipSaveInteger(p,value,encoding);
}
ZIPLIST_INCR_LENGTH(zl,1);
return zl;
}
















unsigned char *ziplistMerge(unsigned char **first, unsigned char **second) {

if (first == NULL || *first == NULL || second == NULL || *second == NULL)
return NULL;


if (*first == *second)
return NULL;

size_t first_bytes = intrev32ifbe(ZIPLIST_BYTES(*first));
size_t first_len = intrev16ifbe(ZIPLIST_LENGTH(*first));

size_t second_bytes = intrev32ifbe(ZIPLIST_BYTES(*second));
size_t second_len = intrev16ifbe(ZIPLIST_LENGTH(*second));

int append;
unsigned char *source, *target;
size_t target_bytes, source_bytes;



if (first_len >= second_len) {

target = *first;
target_bytes = first_bytes;
source = *second;
source_bytes = second_bytes;
append = 1;
} else {

target = *second;
target_bytes = second_bytes;
source = *first;
source_bytes = first_bytes;
append = 0;
}


size_t zlbytes = first_bytes + second_bytes -
ZIPLIST_HEADER_SIZE - ZIPLIST_END_SIZE;
size_t zllength = first_len + second_len;


zllength = zllength < UINT16_MAX ? zllength : UINT16_MAX;


assert(zlbytes < UINT32_MAX);


size_t first_offset = intrev32ifbe(ZIPLIST_TAIL_OFFSET(*first));
size_t second_offset = intrev32ifbe(ZIPLIST_TAIL_OFFSET(*second));


target = zrealloc(target, zlbytes);
if (append) {



memcpy(target + target_bytes - ZIPLIST_END_SIZE,
source + ZIPLIST_HEADER_SIZE,
source_bytes - ZIPLIST_HEADER_SIZE);
} else {




memmove(target + source_bytes - ZIPLIST_END_SIZE,
target + ZIPLIST_HEADER_SIZE,
target_bytes - ZIPLIST_HEADER_SIZE);
memcpy(target, source, source_bytes - ZIPLIST_END_SIZE);
}


ZIPLIST_BYTES(target) = intrev32ifbe(zlbytes);
ZIPLIST_LENGTH(target) = intrev16ifbe(zllength);





ZIPLIST_TAIL_OFFSET(target) = intrev32ifbe(
(first_bytes - ZIPLIST_END_SIZE) +
(second_offset - ZIPLIST_HEADER_SIZE));





target = __ziplistCascadeUpdate(target, target+first_offset);


if (append) {
zfree(*second);
*second = NULL;
*first = target;
} else {
zfree(*first);
*first = NULL;
*second = target;
}
return target;
}

unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where) {
unsigned char *p;
p = (where == ZIPLIST_HEAD) ? ZIPLIST_ENTRY_HEAD(zl) : ZIPLIST_ENTRY_END(zl);
return __ziplistInsert(zl,p,s,slen);
}




unsigned char *ziplistIndex(unsigned char *zl, int index) {
unsigned char *p;
unsigned int prevlensize, prevlen = 0;
size_t zlbytes = intrev32ifbe(ZIPLIST_BYTES(zl));
if (index < 0) {
index = (-index)-1;
p = ZIPLIST_ENTRY_TAIL(zl);
if (p[0] != ZIP_END) {



ZIP_DECODE_PREVLENSIZE(p, prevlensize);
assert(p + prevlensize < zl + zlbytes - ZIPLIST_END_SIZE);
ZIP_DECODE_PREVLEN(p, prevlensize, prevlen);
while (prevlen > 0 && index--) {
p -= prevlen;
assert(p >= zl + ZIPLIST_HEADER_SIZE && p < zl + zlbytes - ZIPLIST_END_SIZE);
ZIP_DECODE_PREVLEN(p, prevlensize, prevlen);
}
}
} else {
p = ZIPLIST_ENTRY_HEAD(zl);
while (index--) {


p += zipRawEntryLengthSafe(zl, zlbytes, p);
if (p[0] == ZIP_END)
break;
}
}
if (p[0] == ZIP_END || index > 0)
return NULL;
zipAssertValidEntry(zl, zlbytes, p);
return p;
}







unsigned char *ziplistNext(unsigned char *zl, unsigned char *p) {
((void) zl);
size_t zlbytes = intrev32ifbe(ZIPLIST_BYTES(zl));




if (p[0] == ZIP_END) {
return NULL;
}

p += zipRawEntryLength(p);
if (p[0] == ZIP_END) {
return NULL;
}

zipAssertValidEntry(zl, zlbytes, p);
return p;
}


unsigned char *ziplistPrev(unsigned char *zl, unsigned char *p) {
unsigned int prevlensize, prevlen = 0;




if (p[0] == ZIP_END) {
p = ZIPLIST_ENTRY_TAIL(zl);
return (p[0] == ZIP_END) ? NULL : p;
} else if (p == ZIPLIST_ENTRY_HEAD(zl)) {
return NULL;
} else {
ZIP_DECODE_PREVLEN(p, prevlensize, prevlen);
assert(prevlen > 0);
p-=prevlen;
size_t zlbytes = intrev32ifbe(ZIPLIST_BYTES(zl));
zipAssertValidEntry(zl, zlbytes, p);
return p;
}
}





unsigned int ziplistGet(unsigned char *p, unsigned char **sstr, unsigned int *slen, long long *sval) {
zlentry entry;
if (p == NULL || p[0] == ZIP_END) return 0;
if (sstr) *sstr = NULL;

zipEntry(p, &entry);
if (ZIP_IS_STR(entry.encoding)) {
if (sstr) {
*slen = entry.len;
*sstr = p+entry.headersize;
}
} else {
if (sval) {
*sval = zipLoadInteger(p+entry.headersize,entry.encoding);
}
}
return 1;
}


unsigned char *ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen) {
return __ziplistInsert(zl,p,s,slen);
}




unsigned char *ziplistDelete(unsigned char *zl, unsigned char **p) {
size_t offset = *p-zl;
zl = __ziplistDelete(zl,*p,1);





*p = zl+offset;
return zl;
}


unsigned char *ziplistDeleteRange(unsigned char *zl, int index, unsigned int num) {
unsigned char *p = ziplistIndex(zl,index);
return (p == NULL) ? zl : __ziplistDelete(zl,p,num);
}



unsigned char *ziplistReplace(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen) {


zlentry entry;
zipEntry(p, &entry);


unsigned int reqlen;
unsigned char encoding = 0;
long long value = 123456789;
if (zipTryEncoding(s,slen,&value,&encoding)) {
reqlen = zipIntSize(encoding);
} else {
reqlen = slen;
}
reqlen += zipStoreEntryEncoding(NULL,encoding,slen);

if (reqlen == entry.lensize + entry.len) {

p += entry.prevrawlensize;
p += zipStoreEntryEncoding(p,encoding,slen);
if (ZIP_IS_STR(encoding)) {
memcpy(p,s,slen);
} else {
zipSaveInteger(p,value,encoding);
}
} else {

zl = ziplistDelete(zl,&p);
zl = ziplistInsert(zl,p,s,slen);
}
return zl;
}



unsigned int ziplistCompare(unsigned char *p, unsigned char *sstr, unsigned int slen) {
zlentry entry;
unsigned char sencoding;
long long zval, sval;
if (p[0] == ZIP_END) return 0;

zipEntry(p, &entry);
if (ZIP_IS_STR(entry.encoding)) {

if (entry.len == slen) {
return memcmp(p+entry.headersize,sstr,slen) == 0;
} else {
return 0;
}
} else {


if (zipTryEncoding(sstr,slen,&sval,&sencoding)) {
zval = zipLoadInteger(p+entry.headersize,entry.encoding);
return zval == sval;
}
}
return 0;
}



unsigned char *ziplistFind(unsigned char *zl, unsigned char *p, unsigned char *vstr, unsigned int vlen, unsigned int skip) {
int skipcnt = 0;
unsigned char vencoding = 0;
long long vll = 0;
size_t zlbytes = ziplistBlobLen(zl);

while (p[0] != ZIP_END) {
struct zlentry e;
unsigned char *q;

assert(zipEntrySafe(zl, zlbytes, p, &e, 1));
q = p + e.prevrawlensize + e.lensize;

if (skipcnt == 0) {

if (ZIP_IS_STR(e.encoding)) {
if (e.len == vlen && memcmp(q, vstr, vlen) == 0) {
return p;
}
} else {



if (vencoding == 0) {
if (!zipTryEncoding(vstr, vlen, &vll, &vencoding)) {



vencoding = UCHAR_MAX;
}

assert(vencoding);
}




if (vencoding != UCHAR_MAX) {
long long ll = zipLoadInteger(q, e.encoding);
if (ll == vll) {
return p;
}
}
}


skipcnt = skip;
} else {

skipcnt--;
}


p = q + e.len;
}

return NULL;
}


unsigned int ziplistLen(unsigned char *zl) {
unsigned int len = 0;
if (intrev16ifbe(ZIPLIST_LENGTH(zl)) < UINT16_MAX) {
len = intrev16ifbe(ZIPLIST_LENGTH(zl));
} else {
unsigned char *p = zl+ZIPLIST_HEADER_SIZE;
size_t zlbytes = intrev32ifbe(ZIPLIST_BYTES(zl));
while (*p != ZIP_END) {
p += zipRawEntryLengthSafe(zl, zlbytes, p);
len++;
}


if (len < UINT16_MAX) ZIPLIST_LENGTH(zl) = intrev16ifbe(len);
}
return len;
}


size_t ziplistBlobLen(unsigned char *zl) {
return intrev32ifbe(ZIPLIST_BYTES(zl));
}

void ziplistRepr(unsigned char *zl) {
unsigned char *p;
int index = 0;
zlentry entry;
size_t zlbytes = ziplistBlobLen(zl);

printf(
"{total bytes %u} "
"{num entries %u}\n"
"{tail offset %u}\n",
intrev32ifbe(ZIPLIST_BYTES(zl)),
intrev16ifbe(ZIPLIST_LENGTH(zl)),
intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl)));
p = ZIPLIST_ENTRY_HEAD(zl);
while(*p != ZIP_END) {
assert(zipEntrySafe(zl, zlbytes, p, &entry, 1));
printf(
"{\n"
"\taddr 0x%08lx,\n"
"\tindex %2d,\n"
"\toffset %5lu,\n"
"\thdr+entry len: %5u,\n"
"\thdr len%2u,\n"
"\tprevrawlen: %5u,\n"
"\tprevrawlensize: %2u,\n"
"\tpayload %5u\n",
(long unsigned)p,
index,
(unsigned long) (p-zl),
entry.headersize+entry.len,
entry.headersize,
entry.prevrawlen,
entry.prevrawlensize,
entry.len);
printf("\tbytes: ");
for (unsigned int i = 0; i < entry.headersize+entry.len; i++) {
printf("%02x|",p[i]);
}
printf("\n");
p += entry.headersize;
if (ZIP_IS_STR(entry.encoding)) {
printf("\t[str]");
if (entry.len > 40) {
if (fwrite(p,40,1,stdout) == 0) perror("fwrite");
printf("...");
} else {
if (entry.len &&
fwrite(p,entry.len,1,stdout) == 0) perror("fwrite");
}
} else {
printf("\t[int]%lld", (long long) zipLoadInteger(p,entry.encoding));
}
printf("\n}\n");
p += entry.len;
index++;
}
printf("{end}\n\n");
}




int ziplistValidateIntegrity(unsigned char *zl, size_t size, int deep,
ziplistValidateEntryCB entry_cb, void *cb_userdata) {

if (size < ZIPLIST_HEADER_SIZE + ZIPLIST_END_SIZE)
return 0;


size_t bytes = intrev32ifbe(ZIPLIST_BYTES(zl));
if (bytes != size)
return 0;


if (zl[size - ZIPLIST_END_SIZE] != ZIP_END)
return 0;


if (intrev32ifbe(ZIPLIST_TAIL_OFFSET(zl)) > size - ZIPLIST_END_SIZE)
return 0;

if (!deep)
return 1;

unsigned int count = 0;
unsigned int header_count = intrev16ifbe(ZIPLIST_LENGTH(zl));
unsigned char *p = ZIPLIST_ENTRY_HEAD(zl);
unsigned char *prev = NULL;
size_t prev_raw_size = 0;
while(*p != ZIP_END) {
struct zlentry e;

if (!zipEntrySafe(zl, size, p, &e, 1))
return 0;


if (e.prevrawlen != prev_raw_size)
return 0;


if (entry_cb && !entry_cb(p, header_count, cb_userdata))
return 0;


prev_raw_size = e.headersize + e.len;
prev = p;
p += e.headersize + e.len;
count++;
}


if (p != zl + bytes - ZIPLIST_END_SIZE)
return 0;


if (prev != NULL && prev != ZIPLIST_ENTRY_TAIL(zl))
return 0;


if (header_count != UINT16_MAX && count != header_count)
return 0;

return 1;
}





void ziplistRandomPair(unsigned char *zl, unsigned long total_count, ziplistEntry *key, ziplistEntry *val) {
int ret;
unsigned char *p;


assert(total_count);


int r = (rand() % total_count) * 2;
p = ziplistIndex(zl, r);
ret = ziplistGet(p, &key->sval, &key->slen, &key->lval);
assert(ret != 0);

if (!val)
return;
p = ziplistNext(zl, p);
ret = ziplistGet(p, &val->sval, &val->slen, &val->lval);
assert(ret != 0);
}


int uintCompare(const void *a, const void *b) {
return (*(unsigned int *) a - *(unsigned int *) b);
}


static inline void ziplistSaveValue(unsigned char *val, unsigned int len, long long lval, ziplistEntry *dest) {
dest->sval = val;
dest->slen = len;
dest->lval = lval;
}





void ziplistRandomPairs(unsigned char *zl, unsigned int count, ziplistEntry *keys, ziplistEntry *vals) {
unsigned char *p, *key, *value;
unsigned int klen = 0, vlen = 0;
long long klval = 0, vlval = 0;


typedef struct {
unsigned int index;
unsigned int order;
} rand_pick;
rand_pick *picks = zmalloc(sizeof(rand_pick)*count);
unsigned int total_size = ziplistLen(zl)/2;


assert(total_size);


for (unsigned int i = 0; i < count; i++) {
picks[i].index = (rand() % total_size) * 2;

picks[i].order = i;
}


qsort(picks, count, sizeof(rand_pick), uintCompare);


unsigned int zipindex = picks[0].index, pickindex = 0;
p = ziplistIndex(zl, zipindex);
while (ziplistGet(p, &key, &klen, &klval) && pickindex < count) {
p = ziplistNext(zl, p);
assert(ziplistGet(p, &value, &vlen, &vlval));
while (pickindex < count && zipindex == picks[pickindex].index) {
int storeorder = picks[pickindex].order;
ziplistSaveValue(key, klen, klval, &keys[storeorder]);
if (vals)
ziplistSaveValue(value, vlen, vlval, &vals[storeorder]);
pickindex++;
}
zipindex += 2;
p = ziplistNext(zl, p);
}

zfree(picks);
}







unsigned int ziplistRandomPairsUnique(unsigned char *zl, unsigned int count, ziplistEntry *keys, ziplistEntry *vals) {
unsigned char *p, *key;
unsigned int klen = 0;
long long klval = 0;
unsigned int total_size = ziplistLen(zl)/2;
unsigned int index = 0;
if (count > total_size)
count = total_size;





p = ziplistIndex(zl, 0);
unsigned int picked = 0, remaining = count;
while (picked < count && p) {
double randomDouble = ((double)rand()) / RAND_MAX;
double threshold = ((double)remaining) / (total_size - index);
if (randomDouble <= threshold) {
assert(ziplistGet(p, &key, &klen, &klval));
ziplistSaveValue(key, klen, klval, &keys[picked]);
p = ziplistNext(zl, p);
assert(p);
if (vals) {
assert(ziplistGet(p, &key, &klen, &klval));
ziplistSaveValue(key, klen, klval, &vals[picked]);
}
remaining--;
picked++;
} else {
p = ziplistNext(zl, p);
assert(p);
}
p = ziplistNext(zl, p);
index++;
}
return picked;
}

#if defined(REDIS_TEST)
#include <sys/time.h>
#include "adlist.h"
#include "sds.h"
#include "testhelp.h"

#define debug(f, ...) { if (DEBUG) printf(f, __VA_ARGS__); }

static unsigned char *createList() {
unsigned char *zl = ziplistNew();
zl = ziplistPush(zl, (unsigned char*)"foo", 3, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"quux", 4, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"hello", 5, ZIPLIST_HEAD);
zl = ziplistPush(zl, (unsigned char*)"1024", 4, ZIPLIST_TAIL);
return zl;
}

static unsigned char *createIntList() {
unsigned char *zl = ziplistNew();
char buf[32];

sprintf(buf, "100");
zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_TAIL);
sprintf(buf, "128000");
zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_TAIL);
sprintf(buf, "-100");
zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_HEAD);
sprintf(buf, "4294967296");
zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_HEAD);
sprintf(buf, "non integer");
zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_TAIL);
sprintf(buf, "much much longer non integer");
zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_TAIL);
return zl;
}

static long long usec(void) {
struct timeval tv;
gettimeofday(&tv,NULL);
return (((long long)tv.tv_sec)*1000000)+tv.tv_usec;
}

static void stress(int pos, int num, int maxsize, int dnum) {
int i,j,k;
unsigned char *zl;
char posstr[2][5] = { "HEAD", "TAIL" };
long long start;
for (i = 0; i < maxsize; i+=dnum) {
zl = ziplistNew();
for (j = 0; j < i; j++) {
zl = ziplistPush(zl,(unsigned char*)"quux",4,ZIPLIST_TAIL);
}


start = usec();
for (k = 0; k < num; k++) {
zl = ziplistPush(zl,(unsigned char*)"quux",4,pos);
zl = ziplistDeleteRange(zl,0,1);
}
printf("List size: %8d, bytes: %8d, %dx push+pop (%s): %6lld usec\n",
i,intrev32ifbe(ZIPLIST_BYTES(zl)),num,posstr[pos],usec()-start);
zfree(zl);
}
}

static unsigned char *pop(unsigned char *zl, int where) {
unsigned char *p, *vstr;
unsigned int vlen;
long long vlong;

p = ziplistIndex(zl,where == ZIPLIST_HEAD ? 0 : -1);
if (ziplistGet(p,&vstr,&vlen,&vlong)) {
if (where == ZIPLIST_HEAD)
printf("Pop head: ");
else
printf("Pop tail: ");

if (vstr) {
if (vlen && fwrite(vstr,vlen,1,stdout) == 0) perror("fwrite");
}
else {
printf("%lld", vlong);
}

printf("\n");
return ziplistDelete(zl,&p);
} else {
printf("ERROR: Could not pop\n");
exit(1);
}
}

static int randstring(char *target, unsigned int min, unsigned int max) {
int p = 0;
int len = min+rand()%(max-min+1);
int minval, maxval;
switch(rand() % 3) {
case 0:
minval = 0;
maxval = 255;
break;
case 1:
minval = 48;
maxval = 122;
break;
case 2:
minval = 48;
maxval = 52;
break;
default:
assert(NULL);
}

while(p < len)
target[p++] = minval+rand()%(maxval-minval+1);
return len;
}

static void verify(unsigned char *zl, zlentry *e) {
int len = ziplistLen(zl);
zlentry _e;

ZIPLIST_ENTRY_ZERO(&_e);

for (int i = 0; i < len; i++) {
memset(&e[i], 0, sizeof(zlentry));
zipEntry(ziplistIndex(zl, i), &e[i]);

memset(&_e, 0, sizeof(zlentry));
zipEntry(ziplistIndex(zl, -len+i), &_e);

assert(memcmp(&e[i], &_e, sizeof(zlentry)) == 0);
}
}

static unsigned char *insertHelper(unsigned char *zl, char ch, size_t len, unsigned char *pos) {
assert(len <= ZIP_BIG_PREVLEN);
unsigned char data[ZIP_BIG_PREVLEN] = {0};
memset(data, ch, len);
return ziplistInsert(zl, pos, data, len);
}

static int compareHelper(unsigned char *zl, char ch, size_t len, int index) {
assert(len <= ZIP_BIG_PREVLEN);
unsigned char data[ZIP_BIG_PREVLEN] = {0};
memset(data, ch, len);
unsigned char *p = ziplistIndex(zl, index);
assert(p != NULL);
return ziplistCompare(p, data, len);
}

static size_t strEntryBytesSmall(size_t slen) {
return slen + zipStorePrevEntryLength(NULL, 0) + zipStoreEntryEncoding(NULL, 0, slen);
}

static size_t strEntryBytesLarge(size_t slen) {
return slen + zipStorePrevEntryLength(NULL, ZIP_BIG_PREVLEN) + zipStoreEntryEncoding(NULL, 0, slen);
}


int ziplistTest(int argc, char **argv, int flags) {
int accurate = (flags & REDIS_TEST_ACCURATE);
unsigned char *zl, *p;
unsigned char *entry;
unsigned int elen;
long long value;
int iteration;


if (argc >= 4)
srand(atoi(argv[3]));

zl = createIntList();
ziplistRepr(zl);

zfree(zl);

zl = createList();
ziplistRepr(zl);

zl = pop(zl,ZIPLIST_TAIL);
ziplistRepr(zl);

zl = pop(zl,ZIPLIST_HEAD);
ziplistRepr(zl);

zl = pop(zl,ZIPLIST_TAIL);
ziplistRepr(zl);

zl = pop(zl,ZIPLIST_TAIL);
ziplistRepr(zl);

zfree(zl);

printf("Get element at index 3:\n");
{
zl = createList();
p = ziplistIndex(zl, 3);
if (!ziplistGet(p, &entry, &elen, &value)) {
printf("ERROR: Could not access index 3\n");
return 1;
}
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
printf("\n");
} else {
printf("%lld\n", value);
}
printf("\n");
zfree(zl);
}

printf("Get element at index 4 (out of range):\n");
{
zl = createList();
p = ziplistIndex(zl, 4);
if (p == NULL) {
printf("No entry\n");
} else {
printf("ERROR: Out of range index should return NULL, returned offset: %ld\n", (long)(p-zl));
return 1;
}
printf("\n");
zfree(zl);
}

printf("Get element at index -1 (last element):\n");
{
zl = createList();
p = ziplistIndex(zl, -1);
if (!ziplistGet(p, &entry, &elen, &value)) {
printf("ERROR: Could not access index -1\n");
return 1;
}
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
printf("\n");
} else {
printf("%lld\n", value);
}
printf("\n");
zfree(zl);
}

printf("Get element at index -4 (first element):\n");
{
zl = createList();
p = ziplistIndex(zl, -4);
if (!ziplistGet(p, &entry, &elen, &value)) {
printf("ERROR: Could not access index -4\n");
return 1;
}
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
printf("\n");
} else {
printf("%lld\n", value);
}
printf("\n");
zfree(zl);
}

printf("Get element at index -5 (reverse out of range):\n");
{
zl = createList();
p = ziplistIndex(zl, -5);
if (p == NULL) {
printf("No entry\n");
} else {
printf("ERROR: Out of range index should return NULL, returned offset: %ld\n", (long)(p-zl));
return 1;
}
printf("\n");
zfree(zl);
}

printf("Iterate list from 0 to end:\n");
{
zl = createList();
p = ziplistIndex(zl, 0);
while (ziplistGet(p, &entry, &elen, &value)) {
printf("Entry: ");
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
} else {
printf("%lld", value);
}
p = ziplistNext(zl,p);
printf("\n");
}
printf("\n");
zfree(zl);
}

printf("Iterate list from 1 to end:\n");
{
zl = createList();
p = ziplistIndex(zl, 1);
while (ziplistGet(p, &entry, &elen, &value)) {
printf("Entry: ");
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
} else {
printf("%lld", value);
}
p = ziplistNext(zl,p);
printf("\n");
}
printf("\n");
zfree(zl);
}

printf("Iterate list from 2 to end:\n");
{
zl = createList();
p = ziplistIndex(zl, 2);
while (ziplistGet(p, &entry, &elen, &value)) {
printf("Entry: ");
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
} else {
printf("%lld", value);
}
p = ziplistNext(zl,p);
printf("\n");
}
printf("\n");
zfree(zl);
}

printf("Iterate starting out of range:\n");
{
zl = createList();
p = ziplistIndex(zl, 4);
if (!ziplistGet(p, &entry, &elen, &value)) {
printf("No entry\n");
} else {
printf("ERROR\n");
}
printf("\n");
zfree(zl);
}

printf("Iterate from back to front:\n");
{
zl = createList();
p = ziplistIndex(zl, -1);
while (ziplistGet(p, &entry, &elen, &value)) {
printf("Entry: ");
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
} else {
printf("%lld", value);
}
p = ziplistPrev(zl,p);
printf("\n");
}
printf("\n");
zfree(zl);
}

printf("Iterate from back to front, deleting all items:\n");
{
zl = createList();
p = ziplistIndex(zl, -1);
while (ziplistGet(p, &entry, &elen, &value)) {
printf("Entry: ");
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0) perror("fwrite");
} else {
printf("%lld", value);
}
zl = ziplistDelete(zl,&p);
p = ziplistPrev(zl,p);
printf("\n");
}
printf("\n");
zfree(zl);
}

printf("Delete inclusive range 0,0:\n");
{
zl = createList();
zl = ziplistDeleteRange(zl, 0, 1);
ziplistRepr(zl);
zfree(zl);
}

printf("Delete inclusive range 0,1:\n");
{
zl = createList();
zl = ziplistDeleteRange(zl, 0, 2);
ziplistRepr(zl);
zfree(zl);
}

printf("Delete inclusive range 1,2:\n");
{
zl = createList();
zl = ziplistDeleteRange(zl, 1, 2);
ziplistRepr(zl);
zfree(zl);
}

printf("Delete with start index out of range:\n");
{
zl = createList();
zl = ziplistDeleteRange(zl, 5, 1);
ziplistRepr(zl);
zfree(zl);
}

printf("Delete with num overflow:\n");
{
zl = createList();
zl = ziplistDeleteRange(zl, 1, 5);
ziplistRepr(zl);
zfree(zl);
}

printf("Delete foo while iterating:\n");
{
zl = createList();
p = ziplistIndex(zl,0);
while (ziplistGet(p,&entry,&elen,&value)) {
if (entry && strncmp("foo",(char*)entry,elen) == 0) {
printf("Delete foo\n");
zl = ziplistDelete(zl,&p);
} else {
printf("Entry: ");
if (entry) {
if (elen && fwrite(entry,elen,1,stdout) == 0)
perror("fwrite");
} else {
printf("%lld",value);
}
p = ziplistNext(zl,p);
printf("\n");
}
}
printf("\n");
ziplistRepr(zl);
zfree(zl);
}

printf("Replace with same size:\n");
{
zl = createList();
unsigned char *orig_zl = zl;
p = ziplistIndex(zl, 0);
zl = ziplistReplace(zl, p, (unsigned char*)"zoink", 5);
p = ziplistIndex(zl, 3);
zl = ziplistReplace(zl, p, (unsigned char*)"yy", 2);
p = ziplistIndex(zl, 1);
zl = ziplistReplace(zl, p, (unsigned char*)"65536", 5);
p = ziplistIndex(zl, 0);
assert(!memcmp((char*)p,
"\x00\x05zoink"
"\x07\xf0\x00\x00\x01"
"\x05\x04quux" "\x06\x02yy" "\xff",
23));
assert(zl == orig_zl);
zfree(zl);
printf("SUCCESS\n\n");
}

printf("Replace with different size:\n");
{
zl = createList();
p = ziplistIndex(zl, 1);
zl = ziplistReplace(zl, p, (unsigned char*)"squirrel", 8);
p = ziplistIndex(zl, 0);
assert(!strncmp((char*)p,
"\x00\x05hello" "\x07\x08squirrel" "\x0a\x04quux"
"\x06\xc0\x00\x04" "\xff",
28));
zfree(zl);
printf("SUCCESS\n\n");
}

printf("Regression test for >255 byte strings:\n");
{
char v1[257] = {0}, v2[257] = {0};
memset(v1,'x',256);
memset(v2,'y',256);
zl = ziplistNew();
zl = ziplistPush(zl,(unsigned char*)v1,strlen(v1),ZIPLIST_TAIL);
zl = ziplistPush(zl,(unsigned char*)v2,strlen(v2),ZIPLIST_TAIL);


p = ziplistIndex(zl,0);
assert(ziplistGet(p,&entry,&elen,&value));
assert(strncmp(v1,(char*)entry,elen) == 0);
p = ziplistIndex(zl,1);
assert(ziplistGet(p,&entry,&elen,&value));
assert(strncmp(v2,(char*)entry,elen) == 0);
printf("SUCCESS\n\n");
zfree(zl);
}

printf("Regression test deleting next to last entries:\n");
{
char v[3][257] = {{0}};
zlentry e[3] = {{.prevrawlensize = 0, .prevrawlen = 0, .lensize = 0,
.len = 0, .headersize = 0, .encoding = 0, .p = NULL}};
size_t i;

for (i = 0; i < (sizeof(v)/sizeof(v[0])); i++) {
memset(v[i], 'a' + i, sizeof(v[0]));
}

v[0][256] = '\0';
v[1][ 1] = '\0';
v[2][256] = '\0';

zl = ziplistNew();
for (i = 0; i < (sizeof(v)/sizeof(v[0])); i++) {
zl = ziplistPush(zl, (unsigned char *) v[i], strlen(v[i]), ZIPLIST_TAIL);
}

verify(zl, e);

assert(e[0].prevrawlensize == 1);
assert(e[1].prevrawlensize == 5);
assert(e[2].prevrawlensize == 1);


unsigned char *p = e[1].p;
zl = ziplistDelete(zl, &p);

verify(zl, e);

assert(e[0].prevrawlensize == 1);
assert(e[1].prevrawlensize == 5);

printf("SUCCESS\n\n");
zfree(zl);
}

printf("Create long list and check indices:\n");
{
unsigned long long start = usec();
zl = ziplistNew();
char buf[32];
int i,len;
for (i = 0; i < 1000; i++) {
len = sprintf(buf,"%d",i);
zl = ziplistPush(zl,(unsigned char*)buf,len,ZIPLIST_TAIL);
}
for (i = 0; i < 1000; i++) {
p = ziplistIndex(zl,i);
assert(ziplistGet(p,NULL,NULL,&value));
assert(i == value);

p = ziplistIndex(zl,-i-1);
assert(ziplistGet(p,NULL,NULL,&value));
assert(999-i == value);
}
printf("SUCCESS. usec=%lld\n\n", usec()-start);
zfree(zl);
}

printf("Compare strings with ziplist entries:\n");
{
zl = createList();
p = ziplistIndex(zl,0);
if (!ziplistCompare(p,(unsigned char*)"hello",5)) {
printf("ERROR: not \"hello\"\n");
return 1;
}
if (ziplistCompare(p,(unsigned char*)"hella",5)) {
printf("ERROR: \"hella\"\n");
return 1;
}

p = ziplistIndex(zl,3);
if (!ziplistCompare(p,(unsigned char*)"1024",4)) {
printf("ERROR: not \"1024\"\n");
return 1;
}
if (ziplistCompare(p,(unsigned char*)"1025",4)) {
printf("ERROR: \"1025\"\n");
return 1;
}
printf("SUCCESS\n\n");
zfree(zl);
}

printf("Merge test:\n");
{

zl = createList();
unsigned char *zl2 = createList();

unsigned char *zl3 = ziplistNew();
unsigned char *zl4 = ziplistNew();

if (ziplistMerge(&zl4, &zl4)) {
printf("ERROR: Allowed merging of one ziplist into itself.\n");
return 1;
}


zl4 = ziplistMerge(&zl3, &zl4);
ziplistRepr(zl4);
if (ziplistLen(zl4)) {
printf("ERROR: Merging two empty ziplists created entries.\n");
return 1;
}
zfree(zl4);

zl2 = ziplistMerge(&zl, &zl2);

ziplistRepr(zl2);

if (ziplistLen(zl2) != 8) {
printf("ERROR: Merged length not 8, but: %u\n", ziplistLen(zl2));
return 1;
}

p = ziplistIndex(zl2,0);
if (!ziplistCompare(p,(unsigned char*)"hello",5)) {
printf("ERROR: not \"hello\"\n");
return 1;
}
if (ziplistCompare(p,(unsigned char*)"hella",5)) {
printf("ERROR: \"hella\"\n");
return 1;
}

p = ziplistIndex(zl2,3);
if (!ziplistCompare(p,(unsigned char*)"1024",4)) {
printf("ERROR: not \"1024\"\n");
return 1;
}
if (ziplistCompare(p,(unsigned char*)"1025",4)) {
printf("ERROR: \"1025\"\n");
return 1;
}

p = ziplistIndex(zl2,4);
if (!ziplistCompare(p,(unsigned char*)"hello",5)) {
printf("ERROR: not \"hello\"\n");
return 1;
}
if (ziplistCompare(p,(unsigned char*)"hella",5)) {
printf("ERROR: \"hella\"\n");
return 1;
}

p = ziplistIndex(zl2,7);
if (!ziplistCompare(p,(unsigned char*)"1024",4)) {
printf("ERROR: not \"1024\"\n");
return 1;
}
if (ziplistCompare(p,(unsigned char*)"1025",4)) {
printf("ERROR: \"1025\"\n");
return 1;
}
printf("SUCCESS\n\n");
zfree(zl);
}

printf("Stress with random payloads of different encoding:\n");
{
unsigned long long start = usec();
int i,j,len,where;
unsigned char *p;
char buf[1024];
int buflen;
list *ref;
listNode *refnode;


unsigned char *sstr;
unsigned int slen;
long long sval;

iteration = accurate ? 20000 : 20;
for (i = 0; i < iteration; i++) {
zl = ziplistNew();
ref = listCreate();
listSetFreeMethod(ref,(void (*)(void*))sdsfree);
len = rand() % 256;


for (j = 0; j < len; j++) {
where = (rand() & 1) ? ZIPLIST_HEAD : ZIPLIST_TAIL;
if (rand() % 2) {
buflen = randstring(buf,1,sizeof(buf)-1);
} else {
switch(rand() % 3) {
case 0:
buflen = sprintf(buf,"%lld",(0LL + rand()) >> 20);
break;
case 1:
buflen = sprintf(buf,"%lld",(0LL + rand()));
break;
case 2:
buflen = sprintf(buf,"%lld",(0LL + rand()) << 20);
break;
default:
assert(NULL);
}
}


zl = ziplistPush(zl, (unsigned char*)buf, buflen, where);


if (where == ZIPLIST_HEAD) {
listAddNodeHead(ref,sdsnewlen(buf, buflen));
} else if (where == ZIPLIST_TAIL) {
listAddNodeTail(ref,sdsnewlen(buf, buflen));
} else {
assert(NULL);
}
}

assert(listLength(ref) == ziplistLen(zl));
for (j = 0; j < len; j++) {


p = ziplistIndex(zl,j);
refnode = listIndex(ref,j);

assert(ziplistGet(p,&sstr,&slen,&sval));
if (sstr == NULL) {
buflen = sprintf(buf,"%lld",sval);
} else {
buflen = slen;
memcpy(buf,sstr,buflen);
buf[buflen] = '\0';
}
assert(memcmp(buf,listNodeValue(refnode),buflen) == 0);
}
zfree(zl);
listRelease(ref);
}
printf("Done. usec=%lld\n\n", usec()-start);
}

printf("Stress with variable ziplist size:\n");
{
unsigned long long start = usec();
int maxsize = accurate ? 16384 : 16;
stress(ZIPLIST_HEAD,100000,maxsize,256);
stress(ZIPLIST_TAIL,100000,maxsize,256);
printf("Done. usec=%lld\n\n", usec()-start);
}


{
zl = ziplistNew();
iteration = accurate ? 100000 : 100;
for (int i=0; i<iteration; i++) {
char buf[4096] = "asdf";
zl = ziplistPush(zl, (unsigned char*)buf, 4, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)buf, 40, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)buf, 400, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)buf, 4000, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"1", 1, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"10", 2, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"100", 3, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"1000", 4, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"10000", 5, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"100000", 6, ZIPLIST_TAIL);
}

printf("Benchmark ziplistFind:\n");
{
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
unsigned char *fptr = ziplistIndex(zl, ZIPLIST_HEAD);
fptr = ziplistFind(zl, fptr, (unsigned char*)"nothing", 7, 1);
}
printf("%lld\n", usec()-start);
}

printf("Benchmark ziplistIndex:\n");
{
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
ziplistIndex(zl, 99999);
}
printf("%lld\n", usec()-start);
}

printf("Benchmark ziplistValidateIntegrity:\n");
{
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
ziplistValidateIntegrity(zl, ziplistBlobLen(zl), 1, NULL, NULL);
}
printf("%lld\n", usec()-start);
}

printf("Benchmark ziplistCompare with string\n");
{
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
unsigned char *eptr = ziplistIndex(zl,0);
while (eptr != NULL) {
ziplistCompare(eptr,(unsigned char*)"nothing",7);
eptr = ziplistNext(zl,eptr);
}
}
printf("Done. usec=%lld\n", usec()-start);
}

printf("Benchmark ziplistCompare with number\n");
{
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
unsigned char *eptr = ziplistIndex(zl,0);
while (eptr != NULL) {
ziplistCompare(eptr,(unsigned char*)"99999",5);
eptr = ziplistNext(zl,eptr);
}
}
printf("Done. usec=%lld\n", usec()-start);
}

zfree(zl);
}

printf("Stress __ziplistCascadeUpdate:\n");
{
char data[ZIP_BIG_PREVLEN];
zl = ziplistNew();
iteration = accurate ? 100000 : 100;
for (int i = 0; i < iteration; i++) {
zl = ziplistPush(zl, (unsigned char*)data, ZIP_BIG_PREVLEN-4, ZIPLIST_TAIL);
}
unsigned long long start = usec();
zl = ziplistPush(zl, (unsigned char*)data, ZIP_BIG_PREVLEN-3, ZIPLIST_HEAD);
printf("Done. usec=%lld\n\n", usec()-start);
zfree(zl);
}

printf("Edge cases of __ziplistCascadeUpdate:\n");
{


size_t s1 = ZIP_BIG_PREVLEN-4, s2 = ZIP_BIG_PREVLEN-3;
zl = ziplistNew();

zlentry e[4] = {{.prevrawlensize = 0, .prevrawlen = 0, .lensize = 0,
.len = 0, .headersize = 0, .encoding = 0, .p = NULL}};

zl = insertHelper(zl, 'a', s1, ZIPLIST_ENTRY_HEAD(zl));
verify(zl, e);

assert(e[0].prevrawlensize == 1 && e[0].prevrawlen == 0);
assert(compareHelper(zl, 'a', s1, 0));
ziplistRepr(zl);


zl = insertHelper(zl, 'b', s1, ZIPLIST_ENTRY_HEAD(zl));
verify(zl, e);

assert(e[0].prevrawlensize == 1 && e[0].prevrawlen == 0);
assert(compareHelper(zl, 'b', s1, 0));

assert(e[1].prevrawlensize == 1 && e[1].prevrawlen == strEntryBytesSmall(s1));
assert(compareHelper(zl, 'a', s1, 1));

ziplistRepr(zl);


zl = insertHelper(zl, 'c', s2, ZIPLIST_ENTRY_HEAD(zl));
verify(zl, e);

assert(e[0].prevrawlensize == 1 && e[0].prevrawlen == 0);
assert(compareHelper(zl, 'c', s2, 0));

assert(e[1].prevrawlensize == 5 && e[1].prevrawlen == strEntryBytesSmall(s2));
assert(compareHelper(zl, 'b', s1, 1));

assert(e[2].prevrawlensize == 5 && e[2].prevrawlen == strEntryBytesLarge(s1));
assert(compareHelper(zl, 'a', s1, 2));

ziplistRepr(zl);


zl = insertHelper(zl, 'd', s2, ZIPLIST_ENTRY_HEAD(zl));
verify(zl, e);

assert(e[0].prevrawlensize == 1 && e[0].prevrawlen == 0);
assert(compareHelper(zl, 'd', s2, 0));

assert(e[1].prevrawlensize == 5 && e[1].prevrawlen == strEntryBytesSmall(s2));
assert(compareHelper(zl, 'c', s2, 1));

assert(e[2].prevrawlensize == 5 && e[2].prevrawlen == strEntryBytesLarge(s2));
assert(compareHelper(zl, 'b', s1, 2));

assert(e[3].prevrawlensize == 5 && e[3].prevrawlen == strEntryBytesLarge(s1));
assert(compareHelper(zl, 'a', s1, 3));

ziplistRepr(zl);


unsigned char *p = ziplistIndex(zl, 2);
zl = ziplistDelete(zl, &p);
verify(zl, e);

assert(e[0].prevrawlensize == 1 && e[0].prevrawlen == 0);
assert(compareHelper(zl, 'd', s2, 0));

assert(e[1].prevrawlensize == 5 && e[1].prevrawlen == strEntryBytesSmall(s2));
assert(compareHelper(zl, 'c', s2, 1));

assert(e[2].prevrawlensize == 5 && e[2].prevrawlen == strEntryBytesLarge(s2));
assert(compareHelper(zl, 'a', s1, 2));

ziplistRepr(zl);

zfree(zl);
}

printf("__ziplistInsert nextdiff == -4 && reqlen < 4 (issue #7170):\n");
{
zl = ziplistNew();


char A_252[253] = {0}, A_250[251] = {0};
memset(A_252, 'A', 252);
memset(A_250, 'A', 250);


zl = ziplistPush(zl, (unsigned char*)"one", 3, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"two", 3, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)A_252, strlen(A_252), ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)A_250, strlen(A_250), ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"three", 5, ZIPLIST_TAIL);
zl = ziplistPush(zl, (unsigned char*)"10", 2, ZIPLIST_TAIL);
ziplistRepr(zl);

p = ziplistIndex(zl, 2);
if (!ziplistCompare(p, (unsigned char*)A_252, strlen(A_252))) {
printf("ERROR: not \"A_252\"\n");
return 1;
}






zl = ziplistDelete(zl, &p);
ziplistRepr(zl);

p = ziplistIndex(zl, 3);
if (!ziplistCompare(p, (unsigned char*)"three", 5)) {
printf("ERROR: not \"three\"\n");
return 1;
}



zl = ziplistInsert(zl, p, (unsigned char*)"10", 2);
ziplistRepr(zl);


p = ziplistIndex(zl, -1);
if (!ziplistCompare(p, (unsigned char*)"10", 2)) {
printf("ERROR: not \"10\"\n");
return 1;
}

zfree(zl);
}

printf("ALL TESTS PASSED!\n");
return 0;
}
#endif
