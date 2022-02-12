


































#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "listpack.h"
#include "listpack_malloc.h"
#include "redisassert.h"
#include "util.h"

#define LP_HDR_SIZE 6
#define LP_HDR_NUMELE_UNKNOWN UINT16_MAX
#define LP_MAX_INT_ENCODING_LEN 9
#define LP_MAX_BACKLEN_SIZE 5
#define LP_MAX_ENTRY_BACKLEN 34359738367ULL
#define LP_ENCODING_INT 0
#define LP_ENCODING_STRING 1

#define LP_ENCODING_7BIT_UINT 0
#define LP_ENCODING_7BIT_UINT_MASK 0x80
#define LP_ENCODING_IS_7BIT_UINT(byte) (((byte)&LP_ENCODING_7BIT_UINT_MASK)==LP_ENCODING_7BIT_UINT)
#define LP_ENCODING_7BIT_UINT_ENTRY_SIZE 2

#define LP_ENCODING_6BIT_STR 0x80
#define LP_ENCODING_6BIT_STR_MASK 0xC0
#define LP_ENCODING_IS_6BIT_STR(byte) (((byte)&LP_ENCODING_6BIT_STR_MASK)==LP_ENCODING_6BIT_STR)

#define LP_ENCODING_13BIT_INT 0xC0
#define LP_ENCODING_13BIT_INT_MASK 0xE0
#define LP_ENCODING_IS_13BIT_INT(byte) (((byte)&LP_ENCODING_13BIT_INT_MASK)==LP_ENCODING_13BIT_INT)
#define LP_ENCODING_13BIT_INT_ENTRY_SIZE 3

#define LP_ENCODING_12BIT_STR 0xE0
#define LP_ENCODING_12BIT_STR_MASK 0xF0
#define LP_ENCODING_IS_12BIT_STR(byte) (((byte)&LP_ENCODING_12BIT_STR_MASK)==LP_ENCODING_12BIT_STR)

#define LP_ENCODING_16BIT_INT 0xF1
#define LP_ENCODING_16BIT_INT_MASK 0xFF
#define LP_ENCODING_IS_16BIT_INT(byte) (((byte)&LP_ENCODING_16BIT_INT_MASK)==LP_ENCODING_16BIT_INT)
#define LP_ENCODING_16BIT_INT_ENTRY_SIZE 4

#define LP_ENCODING_24BIT_INT 0xF2
#define LP_ENCODING_24BIT_INT_MASK 0xFF
#define LP_ENCODING_IS_24BIT_INT(byte) (((byte)&LP_ENCODING_24BIT_INT_MASK)==LP_ENCODING_24BIT_INT)
#define LP_ENCODING_24BIT_INT_ENTRY_SIZE 5

#define LP_ENCODING_32BIT_INT 0xF3
#define LP_ENCODING_32BIT_INT_MASK 0xFF
#define LP_ENCODING_IS_32BIT_INT(byte) (((byte)&LP_ENCODING_32BIT_INT_MASK)==LP_ENCODING_32BIT_INT)
#define LP_ENCODING_32BIT_INT_ENTRY_SIZE 6

#define LP_ENCODING_64BIT_INT 0xF4
#define LP_ENCODING_64BIT_INT_MASK 0xFF
#define LP_ENCODING_IS_64BIT_INT(byte) (((byte)&LP_ENCODING_64BIT_INT_MASK)==LP_ENCODING_64BIT_INT)
#define LP_ENCODING_64BIT_INT_ENTRY_SIZE 10

#define LP_ENCODING_32BIT_STR 0xF0
#define LP_ENCODING_32BIT_STR_MASK 0xFF
#define LP_ENCODING_IS_32BIT_STR(byte) (((byte)&LP_ENCODING_32BIT_STR_MASK)==LP_ENCODING_32BIT_STR)

#define LP_EOF 0xFF

#define LP_ENCODING_6BIT_STR_LEN(p) ((p)[0] & 0x3F)
#define LP_ENCODING_12BIT_STR_LEN(p) ((((p)[0] & 0xF) << 8) | (p)[1])
#define LP_ENCODING_32BIT_STR_LEN(p) (((uint32_t)(p)[1]<<0) | ((uint32_t)(p)[2]<<8) | ((uint32_t)(p)[3]<<16) | ((uint32_t)(p)[4]<<24))




#define lpGetTotalBytes(p) (((uint32_t)(p)[0]<<0) | ((uint32_t)(p)[1]<<8) | ((uint32_t)(p)[2]<<16) | ((uint32_t)(p)[3]<<24))




#define lpGetNumElements(p) (((uint32_t)(p)[4]<<0) | ((uint32_t)(p)[5]<<8))

#define lpSetTotalBytes(p,v) do { (p)[0] = (v)&0xff; (p)[1] = ((v)>>8)&0xff; (p)[2] = ((v)>>16)&0xff; (p)[3] = ((v)>>24)&0xff; } while(0)






#define lpSetNumElements(p,v) do { (p)[4] = (v)&0xff; (p)[5] = ((v)>>8)&0xff; } while(0)









#define ASSERT_INTEGRITY(lp, p) do { assert((p) >= (lp)+LP_HDR_SIZE && (p) < (lp)+lpGetTotalBytes((lp))); } while (0)





#define ASSERT_INTEGRITY_LEN(lp, p, len) do { assert((p) >= (lp)+LP_HDR_SIZE && (p)+(len) < (lp)+lpGetTotalBytes((lp))); } while (0)



static inline void lpAssertValidEntry(unsigned char* lp, size_t lpbytes, unsigned char *p);



#define LISTPACK_MAX_SAFETY_SIZE (1<<30)
int lpSafeToAdd(unsigned char* lp, size_t add) {
size_t len = lp? lpGetTotalBytes(lp): 0;
if (len + add > LISTPACK_MAX_SAFETY_SIZE)
return 0;
return 1;
}

























int lpStringToInt64(const char *s, unsigned long slen, int64_t *value) {
const char *p = s;
unsigned long plen = 0;
int negative = 0;
uint64_t v;

if (plen == slen)
return 0;


if (slen == 1 && p[0] == '0') {
if (value != NULL) *value = 0;
return 1;
}

if (p[0] == '-') {
negative = 1;
p++; plen++;


if (plen == slen)
return 0;
}


if (p[0] >= '1' && p[0] <= '9') {
v = p[0]-'0';
p++; plen++;
} else {
return 0;
}

while (plen < slen && p[0] >= '0' && p[0] <= '9') {
if (v > (UINT64_MAX / 10))
return 0;
v *= 10;

if (v > (UINT64_MAX - (p[0]-'0')))
return 0;
v += p[0]-'0';

p++; plen++;
}


if (plen < slen)
return 0;

if (negative) {
if (v > ((uint64_t)(-(INT64_MIN+1))+1))
return 0;
if (value != NULL) *value = -v;
} else {
if (v > INT64_MAX)
return 0;
if (value != NULL) *value = v;
}
return 1;
}






unsigned char *lpNew(size_t capacity) {
unsigned char *lp = lp_malloc(capacity > LP_HDR_SIZE+1 ? capacity : LP_HDR_SIZE+1);
if (lp == NULL) return NULL;
lpSetTotalBytes(lp,LP_HDR_SIZE+1);
lpSetNumElements(lp,0);
lp[LP_HDR_SIZE] = LP_EOF;
return lp;
}


void lpFree(unsigned char *lp) {
lp_free(lp);
}


unsigned char* lpShrinkToFit(unsigned char *lp) {
size_t size = lpGetTotalBytes(lp);
if (size < lp_malloc_size(lp)) {
return lp_realloc(lp, size);
} else {
return lp;
}
}


static inline void lpEncodeIntegerGetType(int64_t v, unsigned char *intenc, uint64_t *enclen) {
if (v >= 0 && v <= 127) {

intenc[0] = v;
*enclen = 1;
} else if (v >= -4096 && v <= 4095) {

if (v < 0) v = ((int64_t)1<<13)+v;
intenc[0] = (v>>8)|LP_ENCODING_13BIT_INT;
intenc[1] = v&0xff;
*enclen = 2;
} else if (v >= -32768 && v <= 32767) {

if (v < 0) v = ((int64_t)1<<16)+v;
intenc[0] = LP_ENCODING_16BIT_INT;
intenc[1] = v&0xff;
intenc[2] = v>>8;
*enclen = 3;
} else if (v >= -8388608 && v <= 8388607) {

if (v < 0) v = ((int64_t)1<<24)+v;
intenc[0] = LP_ENCODING_24BIT_INT;
intenc[1] = v&0xff;
intenc[2] = (v>>8)&0xff;
intenc[3] = v>>16;
*enclen = 4;
} else if (v >= -2147483648 && v <= 2147483647) {

if (v < 0) v = ((int64_t)1<<32)+v;
intenc[0] = LP_ENCODING_32BIT_INT;
intenc[1] = v&0xff;
intenc[2] = (v>>8)&0xff;
intenc[3] = (v>>16)&0xff;
intenc[4] = v>>24;
*enclen = 5;
} else {

uint64_t uv = v;
intenc[0] = LP_ENCODING_64BIT_INT;
intenc[1] = uv&0xff;
intenc[2] = (uv>>8)&0xff;
intenc[3] = (uv>>16)&0xff;
intenc[4] = (uv>>24)&0xff;
intenc[5] = (uv>>32)&0xff;
intenc[6] = (uv>>40)&0xff;
intenc[7] = (uv>>48)&0xff;
intenc[8] = uv>>56;
*enclen = 9;
}
}












static inline int lpEncodeGetType(unsigned char *ele, uint32_t size, unsigned char *intenc, uint64_t *enclen) {
int64_t v;
if (lpStringToInt64((const char*)ele, size, &v)) {
lpEncodeIntegerGetType(v, intenc, enclen);
return LP_ENCODING_INT;
} else {
if (size < 64) *enclen = 1+size;
else if (size < 4096) *enclen = 2+size;
else *enclen = 5+(uint64_t)size;
return LP_ENCODING_STRING;
}
}






static inline unsigned long lpEncodeBacklen(unsigned char *buf, uint64_t l) {
if (l <= 127) {
if (buf) buf[0] = l;
return 1;
} else if (l < 16383) {
if (buf) {
buf[0] = l>>7;
buf[1] = (l&127)|128;
}
return 2;
} else if (l < 2097151) {
if (buf) {
buf[0] = l>>14;
buf[1] = ((l>>7)&127)|128;
buf[2] = (l&127)|128;
}
return 3;
} else if (l < 268435455) {
if (buf) {
buf[0] = l>>21;
buf[1] = ((l>>14)&127)|128;
buf[2] = ((l>>7)&127)|128;
buf[3] = (l&127)|128;
}
return 4;
} else {
if (buf) {
buf[0] = l>>28;
buf[1] = ((l>>21)&127)|128;
buf[2] = ((l>>14)&127)|128;
buf[3] = ((l>>7)&127)|128;
buf[4] = (l&127)|128;
}
return 5;
}
}



static inline uint64_t lpDecodeBacklen(unsigned char *p) {
uint64_t val = 0;
uint64_t shift = 0;
do {
val |= (uint64_t)(p[0] & 127) << shift;
if (!(p[0] & 128)) break;
shift += 7;
p--;
if (shift > 28) return UINT64_MAX;
} while(1);
return val;
}





static inline void lpEncodeString(unsigned char *buf, unsigned char *s, uint32_t len) {
if (len < 64) {
buf[0] = len | LP_ENCODING_6BIT_STR;
memcpy(buf+1,s,len);
} else if (len < 4096) {
buf[0] = (len >> 8) | LP_ENCODING_12BIT_STR;
buf[1] = len & 0xff;
memcpy(buf+2,s,len);
} else {
buf[0] = LP_ENCODING_32BIT_STR;
buf[1] = len & 0xff;
buf[2] = (len >> 8) & 0xff;
buf[3] = (len >> 16) & 0xff;
buf[4] = (len >> 24) & 0xff;
memcpy(buf+5,s,len);
}
}








static inline uint32_t lpCurrentEncodedSizeUnsafe(unsigned char *p) {
if (LP_ENCODING_IS_7BIT_UINT(p[0])) return 1;
if (LP_ENCODING_IS_6BIT_STR(p[0])) return 1+LP_ENCODING_6BIT_STR_LEN(p);
if (LP_ENCODING_IS_13BIT_INT(p[0])) return 2;
if (LP_ENCODING_IS_16BIT_INT(p[0])) return 3;
if (LP_ENCODING_IS_24BIT_INT(p[0])) return 4;
if (LP_ENCODING_IS_32BIT_INT(p[0])) return 5;
if (LP_ENCODING_IS_64BIT_INT(p[0])) return 9;
if (LP_ENCODING_IS_12BIT_STR(p[0])) return 2+LP_ENCODING_12BIT_STR_LEN(p);
if (LP_ENCODING_IS_32BIT_STR(p[0])) return 5+LP_ENCODING_32BIT_STR_LEN(p);
if (p[0] == LP_EOF) return 1;
return 0;
}





static inline uint32_t lpCurrentEncodedSizeBytes(unsigned char *p) {
if (LP_ENCODING_IS_7BIT_UINT(p[0])) return 1;
if (LP_ENCODING_IS_6BIT_STR(p[0])) return 1;
if (LP_ENCODING_IS_13BIT_INT(p[0])) return 1;
if (LP_ENCODING_IS_16BIT_INT(p[0])) return 1;
if (LP_ENCODING_IS_24BIT_INT(p[0])) return 1;
if (LP_ENCODING_IS_32BIT_INT(p[0])) return 1;
if (LP_ENCODING_IS_64BIT_INT(p[0])) return 1;
if (LP_ENCODING_IS_12BIT_STR(p[0])) return 2;
if (LP_ENCODING_IS_32BIT_STR(p[0])) return 5;
if (p[0] == LP_EOF) return 1;
return 0;
}





unsigned char *lpSkip(unsigned char *p) {
unsigned long entrylen = lpCurrentEncodedSizeUnsafe(p);
entrylen += lpEncodeBacklen(NULL,entrylen);
p += entrylen;
return p;
}




unsigned char *lpNext(unsigned char *lp, unsigned char *p) {
assert(p);
p = lpSkip(p);
if (p[0] == LP_EOF) return NULL;
lpAssertValidEntry(lp, lpBytes(lp), p);
return p;
}




unsigned char *lpPrev(unsigned char *lp, unsigned char *p) {
assert(p);
if (p-lp == LP_HDR_SIZE) return NULL;
p--;
uint64_t prevlen = lpDecodeBacklen(p);
prevlen += lpEncodeBacklen(NULL,prevlen);
p -= prevlen-1;
lpAssertValidEntry(lp, lpBytes(lp), p);
return p;
}



unsigned char *lpFirst(unsigned char *lp) {
unsigned char *p = lp + LP_HDR_SIZE;
if (p[0] == LP_EOF) return NULL;
lpAssertValidEntry(lp, lpBytes(lp), p);
return p;
}



unsigned char *lpLast(unsigned char *lp) {
unsigned char *p = lp+lpGetTotalBytes(lp)-1;
return lpPrev(lp,p);
}






unsigned long lpLength(unsigned char *lp) {
uint32_t numele = lpGetNumElements(lp);
if (numele != LP_HDR_NUMELE_UNKNOWN) return numele;



uint32_t count = 0;
unsigned char *p = lpFirst(lp);
while(p) {
count++;
p = lpNext(lp,p);
}



if (count < LP_HDR_NUMELE_UNKNOWN) lpSetNumElements(lp,count);
return count;
}







































static inline unsigned char *lpGetWithSize(unsigned char *p, int64_t *count, unsigned char *intbuf, uint64_t *entry_size) {
int64_t val;
uint64_t uval, negstart, negmax;

assert(p);
if (LP_ENCODING_IS_7BIT_UINT(p[0])) {
negstart = UINT64_MAX;
negmax = 0;
uval = p[0] & 0x7f;
if (entry_size) *entry_size = LP_ENCODING_7BIT_UINT_ENTRY_SIZE;
} else if (LP_ENCODING_IS_6BIT_STR(p[0])) {
*count = LP_ENCODING_6BIT_STR_LEN(p);
if (entry_size) *entry_size = 1 + *count + lpEncodeBacklen(NULL, *count + 1);
return p+1;
} else if (LP_ENCODING_IS_13BIT_INT(p[0])) {
uval = ((p[0]&0x1f)<<8) | p[1];
negstart = (uint64_t)1<<12;
negmax = 8191;
if (entry_size) *entry_size = LP_ENCODING_13BIT_INT_ENTRY_SIZE;
} else if (LP_ENCODING_IS_16BIT_INT(p[0])) {
uval = (uint64_t)p[1] |
(uint64_t)p[2]<<8;
negstart = (uint64_t)1<<15;
negmax = UINT16_MAX;
if (entry_size) *entry_size = LP_ENCODING_16BIT_INT_ENTRY_SIZE;
} else if (LP_ENCODING_IS_24BIT_INT(p[0])) {
uval = (uint64_t)p[1] |
(uint64_t)p[2]<<8 |
(uint64_t)p[3]<<16;
negstart = (uint64_t)1<<23;
negmax = UINT32_MAX>>8;
if (entry_size) *entry_size = LP_ENCODING_24BIT_INT_ENTRY_SIZE;
} else if (LP_ENCODING_IS_32BIT_INT(p[0])) {
uval = (uint64_t)p[1] |
(uint64_t)p[2]<<8 |
(uint64_t)p[3]<<16 |
(uint64_t)p[4]<<24;
negstart = (uint64_t)1<<31;
negmax = UINT32_MAX;
if (entry_size) *entry_size = LP_ENCODING_32BIT_INT_ENTRY_SIZE;
} else if (LP_ENCODING_IS_64BIT_INT(p[0])) {
uval = (uint64_t)p[1] |
(uint64_t)p[2]<<8 |
(uint64_t)p[3]<<16 |
(uint64_t)p[4]<<24 |
(uint64_t)p[5]<<32 |
(uint64_t)p[6]<<40 |
(uint64_t)p[7]<<48 |
(uint64_t)p[8]<<56;
negstart = (uint64_t)1<<63;
negmax = UINT64_MAX;
if (entry_size) *entry_size = LP_ENCODING_64BIT_INT_ENTRY_SIZE;
} else if (LP_ENCODING_IS_12BIT_STR(p[0])) {
*count = LP_ENCODING_12BIT_STR_LEN(p);
if (entry_size) *entry_size = 2 + *count + lpEncodeBacklen(NULL, *count + 2);
return p+2;
} else if (LP_ENCODING_IS_32BIT_STR(p[0])) {
*count = LP_ENCODING_32BIT_STR_LEN(p);
if (entry_size) *entry_size = 5 + *count + lpEncodeBacklen(NULL, *count + 5);
return p+5;
} else {
uval = 12345678900000000ULL + p[0];
negstart = UINT64_MAX;
negmax = 0;
}




if (uval >= negstart) {


uval = negmax-uval;
val = uval;
val = -val-1;
} else {
val = uval;
}



if (intbuf) {
*count = ll2string((char*)intbuf,LP_INTBUF_SIZE,(long long)val);
return intbuf;
} else {
*count = val;
return NULL;
}
}

unsigned char *lpGet(unsigned char *p, int64_t *count, unsigned char *intbuf) {
return lpGetWithSize(p, count, intbuf, NULL);
}






unsigned char *lpGetValue(unsigned char *p, unsigned int *slen, long long *lval) {
unsigned char *vstr;
int64_t ele_len;

vstr = lpGet(p, &ele_len, NULL);
if (vstr) {
*slen = ele_len;
} else {
*lval = ele_len;
}
return vstr;
}



unsigned char *lpFind(unsigned char *lp, unsigned char *p, unsigned char *s,
uint32_t slen, unsigned int skip) {
int skipcnt = 0;
unsigned char vencoding = 0;
unsigned char *value;
int64_t ll, vll;
uint64_t entry_size = 123456789;
uint32_t lp_bytes = lpBytes(lp);

assert(p);
while (p) {
if (skipcnt == 0) {
value = lpGetWithSize(p, &ll, NULL, &entry_size);
if (value) {

assert(p >= lp + LP_HDR_SIZE && p + entry_size < lp + lp_bytes);
if (slen == ll && memcmp(value, s, slen) == 0) {
return p;
}
} else {



if (vencoding == 0) {



if (slen >= 32 || slen == 0 || !lpStringToInt64((const char*)s, slen, &vll)) {
vencoding = UCHAR_MAX;
} else {
vencoding = 1;
}
}




if (vencoding != UCHAR_MAX && ll == vll) {
return p;
}
}


skipcnt = skip;
p += entry_size;
} else {

skipcnt--;



p = lpSkip(p);
}



if (p + 8 >= lp + lp_bytes)
lpAssertValidEntry(lp, lp_bytes, p);
else
assert(p >= lp + LP_HDR_SIZE && p < lp + lp_bytes);
if (p[0] == LP_EOF) break;
}

return NULL;
}





























unsigned char *lpInsert(unsigned char *lp, unsigned char *elestr, unsigned char *eleint,
uint32_t size, unsigned char *p, int where, unsigned char **newp)
{
unsigned char intenc[LP_MAX_INT_ENCODING_LEN];
unsigned char backlen[LP_MAX_BACKLEN_SIZE];

uint64_t enclen;
int delete = (elestr == NULL && eleint == NULL);




if (delete) where = LP_REPLACE;





if (where == LP_AFTER) {
p = lpSkip(p);
where = LP_BEFORE;
ASSERT_INTEGRITY(lp, p);
}



unsigned long poff = p-lp;

int enctype;
if (elestr) {








enctype = lpEncodeGetType(elestr,size,intenc,&enclen);
if (enctype == LP_ENCODING_INT) eleint = intenc;
} else if (eleint) {
enctype = LP_ENCODING_INT;
enclen = size;
} else {
enctype = -1;
enclen = 0;
}




unsigned long backlen_size = (!delete) ? lpEncodeBacklen(backlen,enclen) : 0;
uint64_t old_listpack_bytes = lpGetTotalBytes(lp);
uint32_t replaced_len = 0;
if (where == LP_REPLACE) {
replaced_len = lpCurrentEncodedSizeUnsafe(p);
replaced_len += lpEncodeBacklen(NULL,replaced_len);
ASSERT_INTEGRITY_LEN(lp, p, replaced_len);
}

uint64_t new_listpack_bytes = old_listpack_bytes + enclen + backlen_size
- replaced_len;
if (new_listpack_bytes > UINT32_MAX) return NULL;







unsigned char *dst = lp + poff;


if (new_listpack_bytes > old_listpack_bytes &&
new_listpack_bytes > lp_malloc_size(lp)) {
if ((lp = lp_realloc(lp,new_listpack_bytes)) == NULL) return NULL;
dst = lp + poff;
}



if (where == LP_BEFORE) {
memmove(dst+enclen+backlen_size,dst,old_listpack_bytes-poff);
} else {
long lendiff = (enclen+backlen_size)-replaced_len;
memmove(dst+replaced_len+lendiff,
dst+replaced_len,
old_listpack_bytes-poff-replaced_len);
}


if (new_listpack_bytes < old_listpack_bytes) {
if ((lp = lp_realloc(lp,new_listpack_bytes)) == NULL) return NULL;
dst = lp + poff;
}


if (newp) {
*newp = dst;


if (delete && dst[0] == LP_EOF) *newp = NULL;
}
if (!delete) {
if (enctype == LP_ENCODING_INT) {
memcpy(dst,eleint,enclen);
} else {
lpEncodeString(dst,elestr,size);
}
dst += enclen;
memcpy(dst,backlen,backlen_size);
dst += backlen_size;
}


if (where != LP_REPLACE || delete) {
uint32_t num_elements = lpGetNumElements(lp);
if (num_elements != LP_HDR_NUMELE_UNKNOWN) {
if (!delete)
lpSetNumElements(lp,num_elements+1);
else
lpSetNumElements(lp,num_elements-1);
}
}
lpSetTotalBytes(lp,new_listpack_bytes);

#if 0






unsigned char *oldlp = lp;
lp = lp_malloc(new_listpack_bytes);
memcpy(lp,oldlp,new_listpack_bytes);
if (newp) {
unsigned long offset = (*newp)-oldlp;
*newp = lp + offset;
}

memset(oldlp,'A',new_listpack_bytes);
lp_free(oldlp);
#endif

return lp;
}


unsigned char *lpInsertString(unsigned char *lp, unsigned char *s, uint32_t slen,
unsigned char *p, int where, unsigned char **newp)
{
return lpInsert(lp, s, NULL, slen, p, where, newp);
}



unsigned char *lpInsertInteger(unsigned char *lp, long long lval, unsigned char *p, int where, unsigned char **newp) {
uint64_t enclen;
unsigned char intenc[LP_MAX_INT_ENCODING_LEN];

lpEncodeIntegerGetType(lval, intenc, &enclen);
return lpInsert(lp, NULL, intenc, enclen, p, where, newp);
}


unsigned char *lpPrepend(unsigned char *lp, unsigned char *s, uint32_t slen) {
unsigned char *p = lpFirst(lp);
if (!p) return lpAppend(lp, s, slen);
return lpInsert(lp, s, NULL, slen, p, LP_BEFORE, NULL);
}


unsigned char *lpPrependInteger(unsigned char *lp, long long lval) {
unsigned char *p = lpFirst(lp);
if (!p) return lpAppendInteger(lp, lval);
return lpInsertInteger(lp, lval, p, LP_BEFORE, NULL);
}




unsigned char *lpAppend(unsigned char *lp, unsigned char *ele, uint32_t size) {
uint64_t listpack_bytes = lpGetTotalBytes(lp);
unsigned char *eofptr = lp + listpack_bytes - 1;
return lpInsert(lp,ele,NULL,size,eofptr,LP_BEFORE,NULL);
}


unsigned char *lpAppendInteger(unsigned char *lp, long long lval) {
uint64_t listpack_bytes = lpGetTotalBytes(lp);
unsigned char *eofptr = lp + listpack_bytes - 1;
return lpInsertInteger(lp, lval, eofptr, LP_BEFORE, NULL);
}




unsigned char *lpReplace(unsigned char *lp, unsigned char **p, unsigned char *s, uint32_t slen) {
return lpInsert(lp, s, NULL, slen, *p, LP_REPLACE, p);
}





unsigned char *lpReplaceInteger(unsigned char *lp, unsigned char **p, long long lval) {
return lpInsertInteger(lp, lval, *p, LP_REPLACE, p);
}





unsigned char *lpDelete(unsigned char *lp, unsigned char *p, unsigned char **newp) {
return lpInsert(lp,NULL,NULL,0,p,LP_REPLACE,newp);
}


unsigned char *lpDeleteRangeWithEntry(unsigned char *lp, unsigned char **p, unsigned long num) {
size_t bytes = lpBytes(lp);
unsigned long deleted = 0;
unsigned char *eofptr = lp + bytes - 1;
unsigned char *first, *tail;
first = tail = *p;

if (num == 0) return lp;




while (num--) {
deleted++;
tail = lpSkip(tail);
if (tail[0] == LP_EOF) break;
lpAssertValidEntry(lp, bytes, tail);
}



unsigned long poff = first-lp;


memmove(first, tail, eofptr - tail + 1);
lpSetTotalBytes(lp, bytes - (tail - first));
uint32_t numele = lpGetNumElements(lp);
if (numele != LP_HDR_NUMELE_UNKNOWN)
lpSetNumElements(lp, numele-deleted);
lp = lpShrinkToFit(lp);


*p = lp+poff;
if ((*p)[0] == LP_EOF) *p = NULL;

return lp;
}


unsigned char *lpDeleteRange(unsigned char *lp, long index, unsigned long num) {
unsigned char *p;
uint32_t numele = lpGetNumElements(lp);

if (num == 0) return lp;
if ((p = lpSeek(lp, index)) == NULL) return lp;








if (numele != LP_HDR_NUMELE_UNKNOWN && index < 0) index = (long)numele + index;
if (numele != LP_HDR_NUMELE_UNKNOWN && (numele - (unsigned long)index) <= num) {
p[0] = LP_EOF;
lpSetTotalBytes(lp, p - lp + 1);
lpSetNumElements(lp, index);
lp = lpShrinkToFit(lp);
} else {
lp = lpDeleteRangeWithEntry(lp, &p, num);
}

return lp;
}
















unsigned char *lpMerge(unsigned char **first, unsigned char **second) {

if (first == NULL || *first == NULL || second == NULL || *second == NULL)
return NULL;


if (*first == *second)
return NULL;

size_t first_bytes = lpBytes(*first);
unsigned long first_len = lpLength(*first);

size_t second_bytes = lpBytes(*second);
unsigned long second_len = lpLength(*second);

int append;
unsigned char *source, *target;
size_t target_bytes, source_bytes;



if (first_bytes >= second_bytes) {

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


unsigned long long lpbytes = (unsigned long long)first_bytes + second_bytes - LP_HDR_SIZE - 1;
assert(lpbytes < UINT32_MAX);
unsigned long lplength = first_len + second_len;


lplength = lplength < UINT16_MAX ? lplength : UINT16_MAX;


target = zrealloc(target, lpbytes);
if (append) {



memcpy(target + target_bytes - 1,
source + LP_HDR_SIZE,
source_bytes - LP_HDR_SIZE);
} else {




memmove(target + source_bytes - 1,
target + LP_HDR_SIZE,
target_bytes - LP_HDR_SIZE);
memcpy(target, source, source_bytes - 1);
}

lpSetNumElements(target, lplength);
lpSetTotalBytes(target, lpbytes);


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


size_t lpBytes(unsigned char *lp) {
return lpGetTotalBytes(lp);
}






unsigned char *lpSeek(unsigned char *lp, long index) {
int forward = 1;





uint32_t numele = lpGetNumElements(lp);
if (numele != LP_HDR_NUMELE_UNKNOWN) {
if (index < 0) index = (long)numele+index;
if (index < 0) return NULL;
if (index >= (long)numele) return NULL;


if (index > (long)numele/2) {
forward = 0;


index -= numele;
}
} else {


if (index < 0) forward = 0;
}


if (forward) {
unsigned char *ele = lpFirst(lp);
while (index > 0 && ele) {
ele = lpNext(lp,ele);
index--;
}
return ele;
} else {
unsigned char *ele = lpLast(lp);
while (index < -1 && ele) {
ele = lpPrev(lp,ele);
index++;
}
return ele;
}
}


unsigned char *lpValidateFirst(unsigned char *lp) {
unsigned char *p = lp + LP_HDR_SIZE;
if (p[0] == LP_EOF) return NULL;
return p;
}




int lpValidateNext(unsigned char *lp, unsigned char **pp, size_t lpbytes) {
#define OUT_OF_RANGE(p) ( (p) < lp + LP_HDR_SIZE || (p) > lp + lpbytes - 1)


unsigned char *p = *pp;
if (!p)
return 0;


if (OUT_OF_RANGE(p))
return 0;

if (*p == LP_EOF) {
*pp = NULL;
return 1;
}


uint32_t lenbytes = lpCurrentEncodedSizeBytes(p);
if (!lenbytes)
return 0;


if (OUT_OF_RANGE(p + lenbytes))
return 0;


unsigned long entrylen = lpCurrentEncodedSizeUnsafe(p);
unsigned long encodedBacklen = lpEncodeBacklen(NULL,entrylen);
entrylen += encodedBacklen;


if (OUT_OF_RANGE(p + entrylen))
return 0;


p += entrylen;


uint64_t prevlen = lpDecodeBacklen(p-1);
if (prevlen + encodedBacklen != entrylen)
return 0;

*pp = p;
return 1;
#undef OUT_OF_RANGE
}


static inline void lpAssertValidEntry(unsigned char* lp, size_t lpbytes, unsigned char *p) {
assert(lpValidateNext(lp, &p, lpbytes));
}




int lpValidateIntegrity(unsigned char *lp, size_t size, int deep,
listpackValidateEntryCB entry_cb, void *cb_userdata) {

if (size < LP_HDR_SIZE + 1)
return 0;


size_t bytes = lpGetTotalBytes(lp);
if (bytes != size)
return 0;


if (lp[size-1] != LP_EOF)
return 0;

if (!deep)
return 1;


uint32_t count = 0;
uint32_t numele = lpGetNumElements(lp);
unsigned char *p = lp + LP_HDR_SIZE;
while(p && p[0] != LP_EOF) {
unsigned char *prev = p;



if (!lpValidateNext(lp, &p, bytes))
return 0;


if (entry_cb && !entry_cb(prev, numele, cb_userdata))
return 0;

count++;
}


if (p != lp + size - 1)
return 0;


if (numele != LP_HDR_NUMELE_UNKNOWN && numele != count)
return 0;

return 1;
}



unsigned int lpCompare(unsigned char *p, unsigned char *s, uint32_t slen) {
unsigned char *value;
int64_t sz;
if (p[0] == LP_EOF) return 0;

value = lpGet(p, &sz, NULL);
if (value) {
return (slen == sz) && memcmp(value,s,slen) == 0;
} else {



int64_t sval;
if (lpStringToInt64((const char*)s, slen, &sval))
return sz == sval;
}

return 0;
}


static int uintCompare(const void *a, const void *b) {
return (*(unsigned int *) a - *(unsigned int *) b);
}


static inline void lpSaveValue(unsigned char *val, unsigned int len, int64_t lval, listpackEntry *dest) {
dest->sval = val;
dest->slen = len;
dest->lval = lval;
}





void lpRandomPair(unsigned char *lp, unsigned long total_count, listpackEntry *key, listpackEntry *val) {
unsigned char *p;


assert(total_count);


int r = (rand() % total_count) * 2;
assert((p = lpSeek(lp, r)));
key->sval = lpGetValue(p, &(key->slen), &(key->lval));

if (!val)
return;
assert((p = lpNext(lp, p)));
val->sval = lpGetValue(p, &(val->slen), &(val->lval));
}





void lpRandomPairs(unsigned char *lp, unsigned int count, listpackEntry *keys, listpackEntry *vals) {
unsigned char *p, *key, *value;
unsigned int klen = 0, vlen = 0;
long long klval = 0, vlval = 0;


typedef struct {
unsigned int index;
unsigned int order;
} rand_pick;
rand_pick *picks = zmalloc(sizeof(rand_pick)*count);
unsigned int total_size = lpLength(lp)/2;


assert(total_size);


for (unsigned int i = 0; i < count; i++) {
picks[i].index = (rand() % total_size) * 2;

picks[i].order = i;
}


qsort(picks, count, sizeof(rand_pick), uintCompare);


unsigned int lpindex = picks[0].index, pickindex = 0;
p = lpSeek(lp, lpindex);
while (p && pickindex < count) {
key = lpGetValue(p, &klen, &klval);
assert((p = lpNext(lp, p)));
value = lpGetValue(p, &vlen, &vlval);
while (pickindex < count && lpindex == picks[pickindex].index) {
int storeorder = picks[pickindex].order;
lpSaveValue(key, klen, klval, &keys[storeorder]);
if (vals)
lpSaveValue(value, vlen, vlval, &vals[storeorder]);
pickindex++;
}
lpindex += 2;
p = lpNext(lp, p);
}

zfree(picks);
}







unsigned int lpRandomPairsUnique(unsigned char *lp, unsigned int count, listpackEntry *keys, listpackEntry *vals) {
unsigned char *p, *key;
unsigned int klen = 0;
long long klval = 0;
unsigned int total_size = lpLength(lp)/2;
unsigned int index = 0;
if (count > total_size)
count = total_size;





p = lpFirst(lp);
unsigned int picked = 0, remaining = count;
while (picked < count && p) {
double randomDouble = ((double)rand()) / RAND_MAX;
double threshold = ((double)remaining) / (total_size - index);
if (randomDouble <= threshold) {
key = lpGetValue(p, &klen, &klval);
lpSaveValue(key, klen, klval, &keys[picked]);
assert((p = lpNext(lp, p)));
if (vals) {
key = lpGetValue(p, &klen, &klval);
lpSaveValue(key, klen, klval, &vals[picked]);
}
remaining--;
picked++;
} else {
assert((p = lpNext(lp, p)));
}
p = lpNext(lp, p);
index++;
}
return picked;
}


void lpRepr(unsigned char *lp) {
unsigned char *p, *vstr;
int64_t vlen;
unsigned char intbuf[LP_INTBUF_SIZE];
int index = 0;

printf("{total bytes %zu} {num entries %lu}\n", lpBytes(lp), lpLength(lp));

p = lpFirst(lp);
while(p) {
uint32_t encoded_size_bytes = lpCurrentEncodedSizeBytes(p);
uint32_t encoded_size = lpCurrentEncodedSizeUnsafe(p);
unsigned long back_len = lpEncodeBacklen(NULL, encoded_size);
printf(
"{\n"
"\taddr: 0x%08lx,\n"
"\tindex: %2d,\n"
"\toffset: %1lu,\n"
"\thdr+entrylen+backlen: %2lu,\n"
"\thdrlen: %3u,\n"
"\tbacklen: %2lu,\n"
"\tpayload: %1u\n",
(long unsigned)p,
index,
(unsigned long) (p-lp),
encoded_size + back_len,
encoded_size_bytes,
back_len,
encoded_size - encoded_size_bytes);
printf("\tbytes: ");
for (unsigned int i = 0; i < (encoded_size + back_len); i++) {
printf("%02x|",p[i]);
}
printf("\n");

vstr = lpGet(p, &vlen, intbuf);
printf("\t[str]");
if (vlen > 40) {
if (fwrite(vstr, 40, 1, stdout) == 0) perror("fwrite");
printf("...");
} else {
if (fwrite(vstr, vlen, 1, stdout) == 0) perror("fwrite");
}
printf("\n}\n");
index++;
p = lpNext(lp, p);
}
printf("{end}\n\n");
}

#if defined(REDIS_TEST)

#include <sys/time.h>
#include "adlist.h"
#include "sds.h"
#include "testhelp.h"

#define UNUSED(x) (void)(x)
#define TEST(name) printf("test — %s\n", name);

char *mixlist[] = {"hello", "foo", "quux", "1024"};
char *intlist[] = {"4294967296", "-100", "100", "128000",
"non integer", "much much longer non integer"};

static unsigned char *createList() {
unsigned char *lp = lpNew(0);
lp = lpAppend(lp, (unsigned char*)mixlist[1], strlen(mixlist[1]));
lp = lpAppend(lp, (unsigned char*)mixlist[2], strlen(mixlist[2]));
lp = lpPrepend(lp, (unsigned char*)mixlist[0], strlen(mixlist[0]));
lp = lpAppend(lp, (unsigned char*)mixlist[3], strlen(mixlist[3]));
return lp;
}

static unsigned char *createIntList() {
unsigned char *lp = lpNew(0);
lp = lpAppend(lp, (unsigned char*)intlist[2], strlen(intlist[2]));
lp = lpAppend(lp, (unsigned char*)intlist[3], strlen(intlist[3]));
lp = lpPrepend(lp, (unsigned char*)intlist[1], strlen(intlist[1]));
lp = lpPrepend(lp, (unsigned char*)intlist[0], strlen(intlist[0]));
lp = lpAppend(lp, (unsigned char*)intlist[4], strlen(intlist[4]));
lp = lpAppend(lp, (unsigned char*)intlist[5], strlen(intlist[5]));
return lp;
}

static long long usec(void) {
struct timeval tv;
gettimeofday(&tv, NULL);
return (((long long)tv.tv_sec)*1000000)+tv.tv_usec;
}

static void stress(int pos, int num, int maxsize, int dnum) {
int i, j, k;
unsigned char *lp;
char posstr[2][5] = { "HEAD", "TAIL" };
long long start;
for (i = 0; i < maxsize; i+=dnum) {
lp = lpNew(0);
for (j = 0; j < i; j++) {
lp = lpAppend(lp, (unsigned char*)"quux", 4);
}


start = usec();
for (k = 0; k < num; k++) {
if (pos == 0) {
lp = lpPrepend(lp, (unsigned char*)"quux", 4);
} else {
lp = lpAppend(lp, (unsigned char*)"quux", 4);

}
lp = lpDelete(lp, lpFirst(lp), NULL);
}
printf("List size: %8d, bytes: %8zu, %dx push+pop (%s): %6lld usec\n",
i, lpBytes(lp), num, posstr[pos], usec()-start);
lpFree(lp);
}
}

static unsigned char *pop(unsigned char *lp, int where) {
unsigned char *p, *vstr;
int64_t vlen;

p = lpSeek(lp, where == 0 ? 0 : -1);
vstr = lpGet(p, &vlen, NULL);
if (where == 0)
printf("Pop head: ");
else
printf("Pop tail: ");

if (vstr) {
if (vlen && fwrite(vstr, vlen, 1, stdout) == 0) perror("fwrite");
} else {
printf("%lld", (long long)vlen);
}

printf("\n");
return lpDelete(lp, p, &p);
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

static void verifyEntry(unsigned char *p, unsigned char *s, size_t slen) {
assert(lpCompare(p, s, slen));
}

static int lpValidation(unsigned char *p, unsigned int head_count, void *userdata) {
UNUSED(p);
UNUSED(head_count);

int ret;
long *count = userdata;
ret = lpCompare(p, (unsigned char *)mixlist[*count], strlen(mixlist[*count]));
(*count)++;
return ret;
}

int listpackTest(int argc, char *argv[], int flags) {
UNUSED(argc);
UNUSED(argv);

int i;
unsigned char *lp, *p, *vstr;
int64_t vlen;
unsigned char intbuf[LP_INTBUF_SIZE];
int accurate = (flags & REDIS_TEST_ACCURATE);

TEST("Create int list") {
lp = createIntList();
assert(lpLength(lp) == 6);
lpFree(lp);
}

TEST("Create list") {
lp = createList();
assert(lpLength(lp) == 4);
lpFree(lp);
}

TEST("Test lpPrepend") {
lp = lpNew(0);
lp = lpPrepend(lp, (unsigned char*)"abc", 3);
lp = lpPrepend(lp, (unsigned char*)"1024", 4);
verifyEntry(lpSeek(lp, 0), (unsigned char*)"1024", 4);
verifyEntry(lpSeek(lp, 1), (unsigned char*)"abc", 3);
lpFree(lp);
}

TEST("Test lpPrependInteger") {
lp = lpNew(0);
lp = lpPrependInteger(lp, 127);
lp = lpPrependInteger(lp, 4095);
lp = lpPrependInteger(lp, 32767);
lp = lpPrependInteger(lp, 8388607);
lp = lpPrependInteger(lp, 2147483647);
lp = lpPrependInteger(lp, 9223372036854775807);
verifyEntry(lpSeek(lp, 0), (unsigned char*)"9223372036854775807", 19);
verifyEntry(lpSeek(lp, -1), (unsigned char*)"127", 3);
lpFree(lp);
}

TEST("Get element at index") {
lp = createList();
verifyEntry(lpSeek(lp, 0), (unsigned char*)"hello", 5);
verifyEntry(lpSeek(lp, 3), (unsigned char*)"1024", 4);
verifyEntry(lpSeek(lp, -1), (unsigned char*)"1024", 4);
verifyEntry(lpSeek(lp, -4), (unsigned char*)"hello", 5);
assert(lpSeek(lp, 4) == NULL);
assert(lpSeek(lp, -5) == NULL);
lpFree(lp);
}

TEST("Pop list") {
lp = createList();
lp = pop(lp, 1);
lp = pop(lp, 0);
lp = pop(lp, 1);
lp = pop(lp, 1);
lpFree(lp);
}

TEST("Get element at index") {
lp = createList();
verifyEntry(lpSeek(lp, 0), (unsigned char*)"hello", 5);
verifyEntry(lpSeek(lp, 3), (unsigned char*)"1024", 4);
verifyEntry(lpSeek(lp, -1), (unsigned char*)"1024", 4);
verifyEntry(lpSeek(lp, -4), (unsigned char*)"hello", 5);
assert(lpSeek(lp, 4) == NULL);
assert(lpSeek(lp, -5) == NULL);
lpFree(lp);
}

TEST("Iterate list from 0 to end") {
lp = createList();
p = lpFirst(lp);
i = 0;
while (p) {
verifyEntry(p, (unsigned char*)mixlist[i], strlen(mixlist[i]));
p = lpNext(lp, p);
i++;
}
lpFree(lp);
}

TEST("Iterate list from 1 to end") {
lp = createList();
i = 1;
p = lpSeek(lp, i);
while (p) {
verifyEntry(p, (unsigned char*)mixlist[i], strlen(mixlist[i]));
p = lpNext(lp, p);
i++;
}
lpFree(lp);
}

TEST("Iterate list from 2 to end") {
lp = createList();
i = 2;
p = lpSeek(lp, i);
while (p) {
verifyEntry(p, (unsigned char*)mixlist[i], strlen(mixlist[i]));
p = lpNext(lp, p);
i++;
}
lpFree(lp);
}

TEST("Iterate from back to front") {
lp = createList();
p = lpLast(lp);
i = 3;
while (p) {
verifyEntry(p, (unsigned char*)mixlist[i], strlen(mixlist[i]));
p = lpPrev(lp, p);
i--;
}
lpFree(lp);
}

TEST("Iterate from back to front, deleting all items") {
lp = createList();
p = lpLast(lp);
i = 3;
while ((p = lpLast(lp))) {
verifyEntry(p, (unsigned char*)mixlist[i], strlen(mixlist[i]));
lp = lpDelete(lp, p, &p);
assert(p == NULL);
i--;
}
lpFree(lp);
}

TEST("Delete whole listpack when num == -1");
{
lp = createList();
lp = lpDeleteRange(lp, 0, -1);
assert(lpLength(lp) == 0);
assert(lp[LP_HDR_SIZE] == LP_EOF);
assert(lpBytes(lp) == (LP_HDR_SIZE + 1));
zfree(lp);

lp = createList();
unsigned char *ptr = lpFirst(lp);
lp = lpDeleteRangeWithEntry(lp, &ptr, -1);
assert(lpLength(lp) == 0);
assert(lp[LP_HDR_SIZE] == LP_EOF);
assert(lpBytes(lp) == (LP_HDR_SIZE + 1));
zfree(lp);
}

TEST("Delete whole listpack with negative index");
{
lp = createList();
lp = lpDeleteRange(lp, -4, 4);
assert(lpLength(lp) == 0);
assert(lp[LP_HDR_SIZE] == LP_EOF);
assert(lpBytes(lp) == (LP_HDR_SIZE + 1));
zfree(lp);

lp = createList();
unsigned char *ptr = lpSeek(lp, -4);
lp = lpDeleteRangeWithEntry(lp, &ptr, 4);
assert(lpLength(lp) == 0);
assert(lp[LP_HDR_SIZE] == LP_EOF);
assert(lpBytes(lp) == (LP_HDR_SIZE + 1));
zfree(lp);
}

TEST("Delete inclusive range 0,0");
{
lp = createList();
lp = lpDeleteRange(lp, 0, 1);
assert(lpLength(lp) == 3);
assert(lpSkip(lpLast(lp))[0] == LP_EOF);
zfree(lp);

lp = createList();
unsigned char *ptr = lpFirst(lp);
lp = lpDeleteRangeWithEntry(lp, &ptr, 1);
assert(lpLength(lp) == 3);
assert(lpSkip(lpLast(lp))[0] == LP_EOF);
zfree(lp);
}

TEST("Delete inclusive range 0,1");
{
lp = createList();
lp = lpDeleteRange(lp, 0, 2);
assert(lpLength(lp) == 2);
verifyEntry(lpFirst(lp), (unsigned char*)mixlist[2], strlen(mixlist[2]));
zfree(lp);

lp = createList();
unsigned char *ptr = lpFirst(lp);
lp = lpDeleteRangeWithEntry(lp, &ptr, 2);
assert(lpLength(lp) == 2);
verifyEntry(lpFirst(lp), (unsigned char*)mixlist[2], strlen(mixlist[2]));
zfree(lp);
}

TEST("Delete inclusive range 1,2");
{
lp = createList();
lp = lpDeleteRange(lp, 1, 2);
assert(lpLength(lp) == 2);
verifyEntry(lpFirst(lp), (unsigned char*)mixlist[0], strlen(mixlist[0]));
zfree(lp);

lp = createList();
unsigned char *ptr = lpSeek(lp, 1);
lp = lpDeleteRangeWithEntry(lp, &ptr, 2);
assert(lpLength(lp) == 2);
verifyEntry(lpFirst(lp), (unsigned char*)mixlist[0], strlen(mixlist[0]));
zfree(lp);
}

TEST("Delete with start index out of range");
{
lp = createList();
lp = lpDeleteRange(lp, 5, 1);
assert(lpLength(lp) == 4);
zfree(lp);
}

TEST("Delete with num overflow");
{
lp = createList();
lp = lpDeleteRange(lp, 1, 5);
assert(lpLength(lp) == 1);
verifyEntry(lpFirst(lp), (unsigned char*)mixlist[0], strlen(mixlist[0]));
zfree(lp);

lp = createList();
unsigned char *ptr = lpSeek(lp, 1);
lp = lpDeleteRangeWithEntry(lp, &ptr, 5);
assert(lpLength(lp) == 1);
verifyEntry(lpFirst(lp), (unsigned char*)mixlist[0], strlen(mixlist[0]));
zfree(lp);
}

TEST("Delete foo while iterating") {
lp = createList();
p = lpFirst(lp);
while (p) {
if (lpCompare(p, (unsigned char*)"foo", 3)) {
lp = lpDelete(lp, p, &p);
} else {
p = lpNext(lp, p);
}
}
lpFree(lp);
}

TEST("Replace with same size") {
lp = createList();
unsigned char *orig_lp = lp;
p = lpSeek(lp, 0);
lp = lpReplace(lp, &p, (unsigned char*)"zoink", 5);
p = lpSeek(lp, 3);
lp = lpReplace(lp, &p, (unsigned char*)"y", 1);
p = lpSeek(lp, 1);
lp = lpReplace(lp, &p, (unsigned char*)"65536", 5);
p = lpSeek(lp, 0);
assert(!memcmp((char*)p,
"\x85zoink\x06"
"\xf2\x00\x00\x01\x04"
"\x84quux\05" "\x81y\x02" "\xff",
22));
assert(lp == orig_lp);
lpFree(lp);
}

TEST("Replace with different size") {
lp = createList();
p = lpSeek(lp, 1);
lp = lpReplace(lp, &p, (unsigned char*)"squirrel", 8);
p = lpSeek(lp, 0);
assert(!strncmp((char*)p,
"\x85hello\x06" "\x88squirrel\x09" "\x84quux\x05"
"\xc4\x00\x02" "\xff",
27));
lpFree(lp);
}

TEST("Regression test for >255 byte strings") {
char v1[257] = {0}, v2[257] = {0};
memset(v1,'x',256);
memset(v2,'y',256);
lp = lpNew(0);
lp = lpAppend(lp, (unsigned char*)v1 ,strlen(v1));
lp = lpAppend(lp, (unsigned char*)v2 ,strlen(v2));


p = lpFirst(lp);
vstr = lpGet(p, &vlen, NULL);
assert(strncmp(v1, (char*)vstr, vlen) == 0);
p = lpSeek(lp, 1);
vstr = lpGet(p, &vlen, NULL);
assert(strncmp(v2, (char*)vstr, vlen) == 0);
lpFree(lp);
}

TEST("Create long list and check indices") {
lp = lpNew(0);
char buf[32];
int i,len;
for (i = 0; i < 1000; i++) {
len = sprintf(buf, "%d", i);
lp = lpAppend(lp, (unsigned char*)buf, len);
}
for (i = 0; i < 1000; i++) {
p = lpSeek(lp, i);
vstr = lpGet(p, &vlen, NULL);
assert(i == vlen);

p = lpSeek(lp, -i-1);
vstr = lpGet(p, &vlen, NULL);
assert(999-i == vlen);
}
lpFree(lp);
}

TEST("Compare strings with listpack entries") {
lp = createList();
p = lpSeek(lp,0);
assert(lpCompare(p,(unsigned char*)"hello",5));
assert(!lpCompare(p,(unsigned char*)"hella",5));

p = lpSeek(lp,3);
assert(lpCompare(p,(unsigned char*)"1024",4));
assert(!lpCompare(p,(unsigned char*)"1025",4));
lpFree(lp);
}

TEST("lpMerge two empty listpacks") {
unsigned char *lp1 = lpNew(0);
unsigned char *lp2 = lpNew(0);


lp1 = lpMerge(&lp1, &lp2);
assert(lpLength(lp1) == 0);
zfree(lp1);
}

TEST("lpMerge two listpacks - first larger than second") {
unsigned char *lp1 = createIntList();
unsigned char *lp2 = createList();

size_t lp1_bytes = lpBytes(lp1);
size_t lp2_bytes = lpBytes(lp2);
unsigned long lp1_len = lpLength(lp1);
unsigned long lp2_len = lpLength(lp2);

unsigned char *lp3 = lpMerge(&lp1, &lp2);
assert(lp3 == lp1);
assert(lp2 == NULL);
assert(lpLength(lp3) == (lp1_len + lp2_len));
assert(lpBytes(lp3) == (lp1_bytes + lp2_bytes - LP_HDR_SIZE - 1));
verifyEntry(lpSeek(lp3, 0), (unsigned char*)"4294967296", 10);
verifyEntry(lpSeek(lp3, 5), (unsigned char*)"much much longer non integer", 28);
verifyEntry(lpSeek(lp3, 6), (unsigned char*)"hello", 5);
verifyEntry(lpSeek(lp3, -1), (unsigned char*)"1024", 4);
zfree(lp3);
}

TEST("lpMerge two listpacks - second larger than first") {
unsigned char *lp1 = createList();
unsigned char *lp2 = createIntList();

size_t lp1_bytes = lpBytes(lp1);
size_t lp2_bytes = lpBytes(lp2);
unsigned long lp1_len = lpLength(lp1);
unsigned long lp2_len = lpLength(lp2);

unsigned char *lp3 = lpMerge(&lp1, &lp2);
assert(lp3 == lp2);
assert(lp1 == NULL);
assert(lpLength(lp3) == (lp1_len + lp2_len));
assert(lpBytes(lp3) == (lp1_bytes + lp2_bytes - LP_HDR_SIZE - 1));
verifyEntry(lpSeek(lp3, 0), (unsigned char*)"hello", 5);
verifyEntry(lpSeek(lp3, 3), (unsigned char*)"1024", 4);
verifyEntry(lpSeek(lp3, 4), (unsigned char*)"4294967296", 10);
verifyEntry(lpSeek(lp3, -1), (unsigned char*)"much much longer non integer", 28);
zfree(lp3);
}

TEST("Random pair with one element") {
listpackEntry key, val;
unsigned char *lp = lpNew(0);
lp = lpAppend(lp, (unsigned char*)"abc", 3);
lp = lpAppend(lp, (unsigned char*)"123", 3);
lpRandomPair(lp, 1, &key, &val);
assert(memcmp(key.sval, "abc", key.slen) == 0);
assert(val.lval == 123);
lpFree(lp);
}

TEST("Random pair with many elements") {
listpackEntry key, val;
unsigned char *lp = lpNew(0);
lp = lpAppend(lp, (unsigned char*)"abc", 3);
lp = lpAppend(lp, (unsigned char*)"123", 3);
lp = lpAppend(lp, (unsigned char*)"456", 3);
lp = lpAppend(lp, (unsigned char*)"def", 3);
lpRandomPair(lp, 2, &key, &val);
if (key.sval) {
assert(!memcmp(key.sval, "abc", key.slen));
assert(key.slen == 3);
assert(val.lval == 123);
}
if (!key.sval) {
assert(key.lval == 456);
assert(!memcmp(val.sval, "def", val.slen));
}
lpFree(lp);
}

TEST("Random pairs with one element") {
int count = 5;
unsigned char *lp = lpNew(0);
listpackEntry *keys = zmalloc(sizeof(listpackEntry) * count);
listpackEntry *vals = zmalloc(sizeof(listpackEntry) * count);

lp = lpAppend(lp, (unsigned char*)"abc", 3);
lp = lpAppend(lp, (unsigned char*)"123", 3);
lpRandomPairs(lp, count, keys, vals);
assert(memcmp(keys[4].sval, "abc", keys[4].slen) == 0);
assert(vals[4].lval == 123);
zfree(keys);
zfree(vals);
lpFree(lp);
}

TEST("Random pairs with many elements") {
int count = 5;
lp = lpNew(0);
listpackEntry *keys = zmalloc(sizeof(listpackEntry) * count);
listpackEntry *vals = zmalloc(sizeof(listpackEntry) * count);

lp = lpAppend(lp, (unsigned char*)"abc", 3);
lp = lpAppend(lp, (unsigned char*)"123", 3);
lp = lpAppend(lp, (unsigned char*)"456", 3);
lp = lpAppend(lp, (unsigned char*)"def", 3);
lpRandomPairs(lp, count, keys, vals);
for (int i = 0; i < count; i++) {
if (keys[i].sval) {
assert(!memcmp(keys[i].sval, "abc", keys[i].slen));
assert(keys[i].slen == 3);
assert(vals[i].lval == 123);
}
if (!keys[i].sval) {
assert(keys[i].lval == 456);
assert(!memcmp(vals[i].sval, "def", vals[i].slen));
}
}
zfree(keys);
zfree(vals);
lpFree(lp);
}

TEST("Random pairs unique with one element") {
unsigned picked;
int count = 5;
lp = lpNew(0);
listpackEntry *keys = zmalloc(sizeof(listpackEntry) * count);
listpackEntry *vals = zmalloc(sizeof(listpackEntry) * count);

lp = lpAppend(lp, (unsigned char*)"abc", 3);
lp = lpAppend(lp, (unsigned char*)"123", 3);
picked = lpRandomPairsUnique(lp, count, keys, vals);
assert(picked == 1);
assert(memcmp(keys[0].sval, "abc", keys[0].slen) == 0);
assert(vals[0].lval == 123);
zfree(keys);
zfree(vals);
lpFree(lp);
}

TEST("Random pairs unique with many elements") {
unsigned picked;
int count = 5;
lp = lpNew(0);
listpackEntry *keys = zmalloc(sizeof(listpackEntry) * count);
listpackEntry *vals = zmalloc(sizeof(listpackEntry) * count);

lp = lpAppend(lp, (unsigned char*)"abc", 3);
lp = lpAppend(lp, (unsigned char*)"123", 3);
lp = lpAppend(lp, (unsigned char*)"456", 3);
lp = lpAppend(lp, (unsigned char*)"def", 3);
picked = lpRandomPairsUnique(lp, count, keys, vals);
assert(picked == 2);
for (int i = 0; i < 2; i++) {
if (keys[i].sval) {
assert(!memcmp(keys[i].sval, "abc", keys[i].slen));
assert(keys[i].slen == 3);
assert(vals[i].lval == 123);
}
if (!keys[i].sval) {
assert(keys[i].lval == 456);
assert(!memcmp(vals[i].sval, "def", vals[i].slen));
}
}
zfree(keys);
zfree(vals);
lpFree(lp);
}

TEST("push various encodings") {
lp = lpNew(0);


lp = lpAppend(lp, (unsigned char*)"127", 3);
assert(LP_ENCODING_IS_7BIT_UINT(lpLast(lp)[0]));
lp = lpAppend(lp, (unsigned char*)"4095", 4);
assert(LP_ENCODING_IS_13BIT_INT(lpLast(lp)[0]));
lp = lpAppend(lp, (unsigned char*)"32767", 5);
assert(LP_ENCODING_IS_16BIT_INT(lpLast(lp)[0]));
lp = lpAppend(lp, (unsigned char*)"8388607", 7);
assert(LP_ENCODING_IS_24BIT_INT(lpLast(lp)[0]));
lp = lpAppend(lp, (unsigned char*)"2147483647", 10);
assert(LP_ENCODING_IS_32BIT_INT(lpLast(lp)[0]));
lp = lpAppend(lp, (unsigned char*)"9223372036854775807", 19);
assert(LP_ENCODING_IS_64BIT_INT(lpLast(lp)[0]));


lp = lpAppendInteger(lp, 127);
assert(LP_ENCODING_IS_7BIT_UINT(lpLast(lp)[0]));
verifyEntry(lpLast(lp), (unsigned char*)"127", 3);
lp = lpAppendInteger(lp, 4095);
verifyEntry(lpLast(lp), (unsigned char*)"4095", 4);
assert(LP_ENCODING_IS_13BIT_INT(lpLast(lp)[0]));
lp = lpAppendInteger(lp, 32767);
verifyEntry(lpLast(lp), (unsigned char*)"32767", 5);
assert(LP_ENCODING_IS_16BIT_INT(lpLast(lp)[0]));
lp = lpAppendInteger(lp, 8388607);
verifyEntry(lpLast(lp), (unsigned char*)"8388607", 7);
assert(LP_ENCODING_IS_24BIT_INT(lpLast(lp)[0]));
lp = lpAppendInteger(lp, 2147483647);
verifyEntry(lpLast(lp), (unsigned char*)"2147483647", 10);
assert(LP_ENCODING_IS_32BIT_INT(lpLast(lp)[0]));
lp = lpAppendInteger(lp, 9223372036854775807);
verifyEntry(lpLast(lp), (unsigned char*)"9223372036854775807", 19);
assert(LP_ENCODING_IS_64BIT_INT(lpLast(lp)[0]));


unsigned char *str = zmalloc(65535);
memset(str, 0, 65535);
lp = lpAppend(lp, (unsigned char*)str, 63);
assert(LP_ENCODING_IS_6BIT_STR(lpLast(lp)[0]));
lp = lpAppend(lp, (unsigned char*)str, 4095);
assert(LP_ENCODING_IS_12BIT_STR(lpLast(lp)[0]));
lp = lpAppend(lp, (unsigned char*)str, 65535);
assert(LP_ENCODING_IS_32BIT_STR(lpLast(lp)[0]));
zfree(str);
lpFree(lp);
}

TEST("Test lpFind") {
lp = createList();
assert(lpFind(lp, lpFirst(lp), (unsigned char*)"abc", 3, 0) == NULL);
verifyEntry(lpFind(lp, lpFirst(lp), (unsigned char*)"hello", 5, 0), (unsigned char*)"hello", 5);
verifyEntry(lpFind(lp, lpFirst(lp), (unsigned char*)"1024", 4, 0), (unsigned char*)"1024", 4);
lpFree(lp);
}

TEST("Test lpValidateIntegrity") {
lp = createList();
long count = 0;
assert(lpValidateIntegrity(lp, lpBytes(lp), 1, lpValidation, &count) == 1);
lpFree(lp);
}

TEST("Test number of elements exceeds LP_HDR_NUMELE_UNKNOWN") {
lp = lpNew(0);
for (int i = 0; i < LP_HDR_NUMELE_UNKNOWN + 1; i++)
lp = lpAppend(lp, (unsigned char*)"1", 1);

assert(lpGetNumElements(lp) == LP_HDR_NUMELE_UNKNOWN);
assert(lpLength(lp) == LP_HDR_NUMELE_UNKNOWN+1);

lp = lpDeleteRange(lp, -2, 2);
assert(lpGetNumElements(lp) == LP_HDR_NUMELE_UNKNOWN);
assert(lpLength(lp) == LP_HDR_NUMELE_UNKNOWN-1);
assert(lpGetNumElements(lp) == LP_HDR_NUMELE_UNKNOWN-1);
lpFree(lp);
}

TEST("Stress with random payloads of different encoding") {
unsigned long long start = usec();
int i,j,len,where;
unsigned char *p;
char buf[1024];
int buflen;
list *ref;
listNode *refnode;

int iteration = accurate ? 20000 : 20;
for (i = 0; i < iteration; i++) {
lp = lpNew(0);
ref = listCreate();
listSetFreeMethod(ref,(void (*)(void*))sdsfree);
len = rand() % 256;


for (j = 0; j < len; j++) {
where = (rand() & 1) ? 0 : 1;
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


if (where == 0) {
lp = lpPrepend(lp, (unsigned char*)buf, buflen);
} else {
lp = lpAppend(lp, (unsigned char*)buf, buflen);
}


if (where == 0) {
listAddNodeHead(ref,sdsnewlen(buf, buflen));
} else if (where == 1) {
listAddNodeTail(ref,sdsnewlen(buf, buflen));
} else {
assert(NULL);
}
}

assert(listLength(ref) == lpLength(lp));
for (j = 0; j < len; j++) {


p = lpSeek(lp,j);
refnode = listIndex(ref,j);

vstr = lpGet(p, &vlen, intbuf);
assert(memcmp(vstr,listNodeValue(refnode),vlen) == 0);
}
lpFree(lp);
listRelease(ref);
}
printf("Done. usec=%lld\n\n", usec()-start);
}

TEST("Stress with variable listpack size") {
unsigned long long start = usec();
int maxsize = accurate ? 16384 : 16;
stress(0,100000,maxsize,256);
stress(1,100000,maxsize,256);
printf("Done. usec=%lld\n\n", usec()-start);
}


{
int iteration = accurate ? 100000 : 100;
lp = lpNew(0);
TEST("Benchmark lpAppend") {
unsigned long long start = usec();
for (int i=0; i<iteration; i++) {
char buf[4096] = "asdf";
lp = lpAppend(lp, (unsigned char*)buf, 4);
lp = lpAppend(lp, (unsigned char*)buf, 40);
lp = lpAppend(lp, (unsigned char*)buf, 400);
lp = lpAppend(lp, (unsigned char*)buf, 4000);
lp = lpAppend(lp, (unsigned char*)"1", 1);
lp = lpAppend(lp, (unsigned char*)"10", 2);
lp = lpAppend(lp, (unsigned char*)"100", 3);
lp = lpAppend(lp, (unsigned char*)"1000", 4);
lp = lpAppend(lp, (unsigned char*)"10000", 5);
lp = lpAppend(lp, (unsigned char*)"100000", 6);
}
printf("Done. usec=%lld\n", usec()-start);
}

TEST("Benchmark lpFind string") {
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
unsigned char *fptr = lpFirst(lp);
fptr = lpFind(lp, fptr, (unsigned char*)"nothing", 7, 1);
}
printf("Done. usec=%lld\n", usec()-start);
}

TEST("Benchmark lpFind number") {
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
unsigned char *fptr = lpFirst(lp);
fptr = lpFind(lp, fptr, (unsigned char*)"99999", 5, 1);
}
printf("Done. usec=%lld\n", usec()-start);
}

TEST("Benchmark lpSeek") {
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
lpSeek(lp, 99999);
}
printf("Done. usec=%lld\n", usec()-start);
}

TEST("Benchmark lpValidateIntegrity") {
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
lpValidateIntegrity(lp, lpBytes(lp), 1, NULL, NULL);
}
printf("Done. usec=%lld\n", usec()-start);
}

TEST("Benchmark lpCompare with string") {
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
unsigned char *eptr = lpSeek(lp,0);
while (eptr != NULL) {
lpCompare(eptr,(unsigned char*)"nothing",7);
eptr = lpNext(lp,eptr);
}
}
printf("Done. usec=%lld\n", usec()-start);
}

TEST("Benchmark lpCompare with number") {
unsigned long long start = usec();
for (int i = 0; i < 2000; i++) {
unsigned char *eptr = lpSeek(lp,0);
while (eptr != NULL) {
lpCompare(lp, (unsigned char*)"99999", 5);
eptr = lpNext(lp,eptr);
}
}
printf("Done. usec=%lld\n", usec()-start);
}

lpFree(lp);
}

return 0;
}

#endif
