






























#include "server.h"

#include <stdint.h>
#include <math.h>


















































































































































struct hllhdr {
char magic[4];
uint8_t encoding;
uint8_t notused[3];
uint8_t card[8];
uint8_t registers[];
};


#define HLL_INVALIDATE_CACHE(hdr) (hdr)->card[7] |= (1<<7)
#define HLL_VALID_CACHE(hdr) (((hdr)->card[7] & (1<<7)) == 0)

#define HLL_P 14
#define HLL_Q (64-HLL_P)

#define HLL_REGISTERS (1<<HLL_P)
#define HLL_P_MASK (HLL_REGISTERS-1)
#define HLL_BITS 6
#define HLL_REGISTER_MAX ((1<<HLL_BITS)-1)
#define HLL_HDR_SIZE sizeof(struct hllhdr)
#define HLL_DENSE_SIZE (HLL_HDR_SIZE+((HLL_REGISTERS*HLL_BITS+7)/8))
#define HLL_DENSE 0
#define HLL_SPARSE 1
#define HLL_RAW 255
#define HLL_MAX_ENCODING 1

static char *invalid_hll_err = "-INVALIDOBJ Corrupted HLL object detected";


































































































































#define HLL_DENSE_GET_REGISTER(target,p,regnum) do { uint8_t *_p = (uint8_t*) p; unsigned long _byte = regnum*HLL_BITS/8; unsigned long _fb = regnum*HLL_BITS&7; unsigned long _fb8 = 8 - _fb; unsigned long b0 = _p[_byte]; unsigned long b1 = _p[_byte+1]; target = ((b0 >> _fb) | (b1 << _fb8)) & HLL_REGISTER_MAX; } while(0)











#define HLL_DENSE_SET_REGISTER(p,regnum,val) do { uint8_t *_p = (uint8_t*) p; unsigned long _byte = regnum*HLL_BITS/8; unsigned long _fb = regnum*HLL_BITS&7; unsigned long _fb8 = 8 - _fb; unsigned long _v = val; _p[_byte] &= ~(HLL_REGISTER_MAX << _fb); _p[_byte] |= _v << _fb; _p[_byte+1] &= ~(HLL_REGISTER_MAX >> _fb8); _p[_byte+1] |= _v >> _fb8; } while(0)













#define HLL_SPARSE_XZERO_BIT 0x40
#define HLL_SPARSE_VAL_BIT 0x80
#define HLL_SPARSE_IS_ZERO(p) (((*(p)) & 0xc0) == 0)
#define HLL_SPARSE_IS_XZERO(p) (((*(p)) & 0xc0) == HLL_SPARSE_XZERO_BIT)
#define HLL_SPARSE_IS_VAL(p) ((*(p)) & HLL_SPARSE_VAL_BIT)
#define HLL_SPARSE_ZERO_LEN(p) (((*(p)) & 0x3f)+1)
#define HLL_SPARSE_XZERO_LEN(p) (((((*(p)) & 0x3f) << 8) | (*((p)+1)))+1)
#define HLL_SPARSE_VAL_VALUE(p) ((((*(p)) >> 2) & 0x1f)+1)
#define HLL_SPARSE_VAL_LEN(p) (((*(p)) & 0x3)+1)
#define HLL_SPARSE_VAL_MAX_VALUE 32
#define HLL_SPARSE_VAL_MAX_LEN 4
#define HLL_SPARSE_ZERO_MAX_LEN 64
#define HLL_SPARSE_XZERO_MAX_LEN 16384
#define HLL_SPARSE_VAL_SET(p,val,len) do { *(p) = (((val)-1)<<2|((len)-1))|HLL_SPARSE_VAL_BIT; } while(0)


#define HLL_SPARSE_ZERO_SET(p,len) do { *(p) = (len)-1; } while(0)


#define HLL_SPARSE_XZERO_SET(p,len) do { int _l = (len)-1; *(p) = (_l>>8) | HLL_SPARSE_XZERO_BIT; *((p)+1) = (_l&0xff); } while(0)




#define HLL_ALPHA_INF 0.721347520444481703680






REDIS_NO_SANITIZE("alignment")
uint64_t MurmurHash64A (const void * key, int len, unsigned int seed) {
const uint64_t m = 0xc6a4a7935bd1e995;
const int r = 47;
uint64_t h = seed ^ (len * m);
const uint8_t *data = (const uint8_t *)key;
const uint8_t *end = data + (len-(len&7));

while(data != end) {
uint64_t k;

#if (BYTE_ORDER == LITTLE_ENDIAN)
#if defined(USE_ALIGNED_ACCESS)
memcpy(&k,data,sizeof(uint64_t));
#else
k = *((uint64_t*)data);
#endif
#else
k = (uint64_t) data[0];
k |= (uint64_t) data[1] << 8;
k |= (uint64_t) data[2] << 16;
k |= (uint64_t) data[3] << 24;
k |= (uint64_t) data[4] << 32;
k |= (uint64_t) data[5] << 40;
k |= (uint64_t) data[6] << 48;
k |= (uint64_t) data[7] << 56;
#endif

k *= m;
k ^= k >> r;
k *= m;
h ^= k;
h *= m;
data += 8;
}

switch(len & 7) {
case 7: h ^= (uint64_t)data[6] << 48;
case 6: h ^= (uint64_t)data[5] << 40;
case 5: h ^= (uint64_t)data[4] << 32;
case 4: h ^= (uint64_t)data[3] << 24;
case 3: h ^= (uint64_t)data[2] << 16;
case 2: h ^= (uint64_t)data[1] << 8;
case 1: h ^= (uint64_t)data[0];
h *= m;
};

h ^= h >> r;
h *= m;
h ^= h >> r;
return h;
}




int hllPatLen(unsigned char *ele, size_t elesize, long *regp) {
uint64_t hash, bit, index;
int count;












hash = MurmurHash64A(ele,elesize,0xadc83b19ULL);
index = hash & HLL_P_MASK;
hash >>= HLL_P;
hash |= ((uint64_t)1<<HLL_Q);

bit = 1;
count = 1;
while((hash & bit) == 0) {
count++;
bit <<= 1;
}
*regp = (int) index;
return count;
}













int hllDenseSet(uint8_t *registers, long index, uint8_t count) {
uint8_t oldcount;

HLL_DENSE_GET_REGISTER(oldcount,registers,index);
if (count > oldcount) {
HLL_DENSE_SET_REGISTER(registers,index,count);
return 1;
} else {
return 0;
}
}







int hllDenseAdd(uint8_t *registers, unsigned char *ele, size_t elesize) {
long index;
uint8_t count = hllPatLen(ele,elesize,&index);

return hllDenseSet(registers,index,count);
}


void hllDenseRegHisto(uint8_t *registers, int* reghisto) {
int j;




if (HLL_REGISTERS == 16384 && HLL_BITS == 6) {
uint8_t *r = registers;
unsigned long r0, r1, r2, r3, r4, r5, r6, r7, r8, r9,
r10, r11, r12, r13, r14, r15;
for (j = 0; j < 1024; j++) {

r0 = r[0] & 63;
r1 = (r[0] >> 6 | r[1] << 2) & 63;
r2 = (r[1] >> 4 | r[2] << 4) & 63;
r3 = (r[2] >> 2) & 63;
r4 = r[3] & 63;
r5 = (r[3] >> 6 | r[4] << 2) & 63;
r6 = (r[4] >> 4 | r[5] << 4) & 63;
r7 = (r[5] >> 2) & 63;
r8 = r[6] & 63;
r9 = (r[6] >> 6 | r[7] << 2) & 63;
r10 = (r[7] >> 4 | r[8] << 4) & 63;
r11 = (r[8] >> 2) & 63;
r12 = r[9] & 63;
r13 = (r[9] >> 6 | r[10] << 2) & 63;
r14 = (r[10] >> 4 | r[11] << 4) & 63;
r15 = (r[11] >> 2) & 63;

reghisto[r0]++;
reghisto[r1]++;
reghisto[r2]++;
reghisto[r3]++;
reghisto[r4]++;
reghisto[r5]++;
reghisto[r6]++;
reghisto[r7]++;
reghisto[r8]++;
reghisto[r9]++;
reghisto[r10]++;
reghisto[r11]++;
reghisto[r12]++;
reghisto[r13]++;
reghisto[r14]++;
reghisto[r15]++;

r += 12;
}
} else {
for(j = 0; j < HLL_REGISTERS; j++) {
unsigned long reg;
HLL_DENSE_GET_REGISTER(reg,registers,j);
reghisto[reg]++;
}
}
}









int hllSparseToDense(robj *o) {
sds sparse = o->ptr, dense;
struct hllhdr *hdr, *oldhdr = (struct hllhdr*)sparse;
int idx = 0, runlen, regval;
uint8_t *p = (uint8_t*)sparse, *end = p+sdslen(sparse);


hdr = (struct hllhdr*) sparse;
if (hdr->encoding == HLL_DENSE) return C_OK;




dense = sdsnewlen(NULL,HLL_DENSE_SIZE);
hdr = (struct hllhdr*) dense;
*hdr = *oldhdr;
hdr->encoding = HLL_DENSE;



p += HLL_HDR_SIZE;
while(p < end) {
if (HLL_SPARSE_IS_ZERO(p)) {
runlen = HLL_SPARSE_ZERO_LEN(p);
idx += runlen;
p++;
} else if (HLL_SPARSE_IS_XZERO(p)) {
runlen = HLL_SPARSE_XZERO_LEN(p);
idx += runlen;
p += 2;
} else {
runlen = HLL_SPARSE_VAL_LEN(p);
regval = HLL_SPARSE_VAL_VALUE(p);
if ((runlen + idx) > HLL_REGISTERS) break;
while(runlen--) {
HLL_DENSE_SET_REGISTER(hdr->registers,idx,regval);
idx++;
}
p++;
}
}



if (idx != HLL_REGISTERS) {
sdsfree(dense);
return C_ERR;
}


sdsfree(o->ptr);
o->ptr = dense;
return C_OK;
}
















int hllSparseSet(robj *o, long index, uint8_t count) {
struct hllhdr *hdr;
uint8_t oldcount, *sparse, *end, *p, *prev, *next;
long first, span;
long is_zero = 0, is_xzero = 0, is_val = 0, runlen = 0;



if (count > HLL_SPARSE_VAL_MAX_VALUE) goto promote;






o->ptr = sdsMakeRoomFor(o->ptr,3);



sparse = p = ((uint8_t*)o->ptr) + HLL_HDR_SIZE;
end = p + sdslen(o->ptr) - HLL_HDR_SIZE;

first = 0;
prev = NULL;
next = NULL;
span = 0;
while(p < end) {
long oplen;






oplen = 1;
if (HLL_SPARSE_IS_ZERO(p)) {
span = HLL_SPARSE_ZERO_LEN(p);
} else if (HLL_SPARSE_IS_VAL(p)) {
span = HLL_SPARSE_VAL_LEN(p);
} else {
span = HLL_SPARSE_XZERO_LEN(p);
oplen = 2;
}

if (index <= first+span-1) break;
prev = p;
p += oplen;
first += span;
}
if (span == 0 || p >= end) return -1;

next = HLL_SPARSE_IS_XZERO(p) ? p+2 : p+1;
if (next >= end) next = NULL;




if (HLL_SPARSE_IS_ZERO(p)) {
is_zero = 1;
runlen = HLL_SPARSE_ZERO_LEN(p);
} else if (HLL_SPARSE_IS_XZERO(p)) {
is_xzero = 1;
runlen = HLL_SPARSE_XZERO_LEN(p);
} else {
is_val = 1;
runlen = HLL_SPARSE_VAL_LEN(p);
}






















if (is_val) {
oldcount = HLL_SPARSE_VAL_VALUE(p);

if (oldcount >= count) return 0;


if (runlen == 1) {
HLL_SPARSE_VAL_SET(p,count,1);
goto updated;
}
}



if (is_zero && runlen == 1) {
HLL_SPARSE_VAL_SET(p,count,1);
goto updated;
}
















uint8_t seq[5], *n = seq;
int last = first+span-1;
int len;

if (is_zero || is_xzero) {

if (index != first) {
len = index-first;
if (len > HLL_SPARSE_ZERO_MAX_LEN) {
HLL_SPARSE_XZERO_SET(n,len);
n += 2;
} else {
HLL_SPARSE_ZERO_SET(n,len);
n++;
}
}
HLL_SPARSE_VAL_SET(n,count,1);
n++;
if (index != last) {
len = last-index;
if (len > HLL_SPARSE_ZERO_MAX_LEN) {
HLL_SPARSE_XZERO_SET(n,len);
n += 2;
} else {
HLL_SPARSE_ZERO_SET(n,len);
n++;
}
}
} else {

int curval = HLL_SPARSE_VAL_VALUE(p);

if (index != first) {
len = index-first;
HLL_SPARSE_VAL_SET(n,curval,len);
n++;
}
HLL_SPARSE_VAL_SET(n,count,1);
n++;
if (index != last) {
len = last-index;
HLL_SPARSE_VAL_SET(n,curval,len);
n++;
}
}





int seqlen = n-seq;
int oldlen = is_xzero ? 2 : 1;
int deltalen = seqlen-oldlen;

if (deltalen > 0 &&
sdslen(o->ptr)+deltalen > server.hll_sparse_max_bytes) goto promote;
if (deltalen && next) memmove(next+deltalen,next,end-next);
sdsIncrLen(o->ptr,deltalen);
memcpy(p,seq,seqlen);
end += deltalen;

updated:





p = prev ? prev : sparse;
int scanlen = 5;
while (p < end && scanlen--) {
if (HLL_SPARSE_IS_XZERO(p)) {
p += 2;
continue;
} else if (HLL_SPARSE_IS_ZERO(p)) {
p++;
continue;
}


if (p+1 < end && HLL_SPARSE_IS_VAL(p+1)) {
int v1 = HLL_SPARSE_VAL_VALUE(p);
int v2 = HLL_SPARSE_VAL_VALUE(p+1);
if (v1 == v2) {
int len = HLL_SPARSE_VAL_LEN(p)+HLL_SPARSE_VAL_LEN(p+1);
if (len <= HLL_SPARSE_VAL_MAX_LEN) {
HLL_SPARSE_VAL_SET(p+1,v1,len);
memmove(p,p+1,end-p);
sdsIncrLen(o->ptr,-1);
end--;



continue;
}
}
}
p++;
}


hdr = o->ptr;
HLL_INVALIDATE_CACHE(hdr);
return 1;

promote:
if (hllSparseToDense(o) == C_ERR) return -1;
hdr = o->ptr;








int dense_retval = hllDenseSet(hdr->registers,index,count);
serverAssert(dense_retval == 1);
return dense_retval;
}







int hllSparseAdd(robj *o, unsigned char *ele, size_t elesize) {
long index;
uint8_t count = hllPatLen(ele,elesize,&index);

return hllSparseSet(o,index,count);
}


void hllSparseRegHisto(uint8_t *sparse, int sparselen, int *invalid, int* reghisto) {
int idx = 0, runlen, regval;
uint8_t *end = sparse+sparselen, *p = sparse;

while(p < end) {
if (HLL_SPARSE_IS_ZERO(p)) {
runlen = HLL_SPARSE_ZERO_LEN(p);
idx += runlen;
reghisto[0] += runlen;
p++;
} else if (HLL_SPARSE_IS_XZERO(p)) {
runlen = HLL_SPARSE_XZERO_LEN(p);
idx += runlen;
reghisto[0] += runlen;
p += 2;
} else {
runlen = HLL_SPARSE_VAL_LEN(p);
regval = HLL_SPARSE_VAL_VALUE(p);
idx += runlen;
reghisto[regval] += runlen;
p++;
}
}
if (idx != HLL_REGISTERS && invalid) *invalid = 1;
}









void hllRawRegHisto(uint8_t *registers, int* reghisto) {
uint64_t *word = (uint64_t*) registers;
uint8_t *bytes;
int j;

for (j = 0; j < HLL_REGISTERS/8; j++) {
if (*word == 0) {
reghisto[0] += 8;
} else {
bytes = (uint8_t*) word;
reghisto[bytes[0]]++;
reghisto[bytes[1]]++;
reghisto[bytes[2]]++;
reghisto[bytes[3]]++;
reghisto[bytes[4]]++;
reghisto[bytes[5]]++;
reghisto[bytes[6]]++;
reghisto[bytes[7]]++;
}
word++;
}
}




double hllSigma(double x) {
if (x == 1.) return INFINITY;
double zPrime;
double y = 1;
double z = x;
do {
x *= x;
zPrime = z;
z += x * y;
y += y;
} while(zPrime != z);
return z;
}




double hllTau(double x) {
if (x == 0. || x == 1.) return 0.;
double zPrime;
double y = 1.0;
double z = 1 - x;
do {
x = sqrt(x);
zPrime = z;
y *= 0.5;
z -= pow(1 - x, 2)*y;
} while(zPrime != z);
return z / 3;
}












uint64_t hllCount(struct hllhdr *hdr, int *invalid) {
double m = HLL_REGISTERS;
double E;
int j;





int reghisto[64] = {0};


if (hdr->encoding == HLL_DENSE) {
hllDenseRegHisto(hdr->registers,reghisto);
} else if (hdr->encoding == HLL_SPARSE) {
hllSparseRegHisto(hdr->registers,
sdslen((sds)hdr)-HLL_HDR_SIZE,invalid,reghisto);
} else if (hdr->encoding == HLL_RAW) {
hllRawRegHisto(hdr->registers,reghisto);
} else {
serverPanic("Unknown HyperLogLog encoding in hllCount()");
}




double z = m * hllTau((m-reghisto[HLL_Q+1])/(double)m);
for (j = HLL_Q; j >= 1; --j) {
z += reghisto[j];
z *= 0.5;
}
z += m * hllSigma(reghisto[0]/(double)m);
E = llroundl(HLL_ALPHA_INF*m*m/z);

return (uint64_t) E;
}


int hllAdd(robj *o, unsigned char *ele, size_t elesize) {
struct hllhdr *hdr = o->ptr;
switch(hdr->encoding) {
case HLL_DENSE: return hllDenseAdd(hdr->registers,ele,elesize);
case HLL_SPARSE: return hllSparseAdd(o,ele,elesize);
default: return -1;
}
}









int hllMerge(uint8_t *max, robj *hll) {
struct hllhdr *hdr = hll->ptr;
int i;

if (hdr->encoding == HLL_DENSE) {
uint8_t val;

for (i = 0; i < HLL_REGISTERS; i++) {
HLL_DENSE_GET_REGISTER(val,hdr->registers,i);
if (val > max[i]) max[i] = val;
}
} else {
uint8_t *p = hll->ptr, *end = p + sdslen(hll->ptr);
long runlen, regval;

p += HLL_HDR_SIZE;
i = 0;
while(p < end) {
if (HLL_SPARSE_IS_ZERO(p)) {
runlen = HLL_SPARSE_ZERO_LEN(p);
i += runlen;
p++;
} else if (HLL_SPARSE_IS_XZERO(p)) {
runlen = HLL_SPARSE_XZERO_LEN(p);
i += runlen;
p += 2;
} else {
runlen = HLL_SPARSE_VAL_LEN(p);
regval = HLL_SPARSE_VAL_VALUE(p);
if ((runlen + i) > HLL_REGISTERS) break;
while(runlen--) {
if (regval > max[i]) max[i] = regval;
i++;
}
p++;
}
}
if (i != HLL_REGISTERS) return C_ERR;
}
return C_OK;
}





robj *createHLLObject(void) {
robj *o;
struct hllhdr *hdr;
sds s;
uint8_t *p;
int sparselen = HLL_HDR_SIZE +
(((HLL_REGISTERS+(HLL_SPARSE_XZERO_MAX_LEN-1)) /
HLL_SPARSE_XZERO_MAX_LEN)*2);
int aux;



aux = HLL_REGISTERS;
s = sdsnewlen(NULL,sparselen);
p = (uint8_t*)s + HLL_HDR_SIZE;
while(aux) {
int xzero = HLL_SPARSE_XZERO_MAX_LEN;
if (xzero > aux) xzero = aux;
HLL_SPARSE_XZERO_SET(p,xzero);
p += 2;
aux -= xzero;
}
serverAssert((p-(uint8_t*)s) == sparselen);


o = createObject(OBJ_STRING,s);
hdr = o->ptr;
memcpy(hdr->magic,"HYLL",4);
hdr->encoding = HLL_SPARSE;
return o;
}




int isHLLObjectOrReply(client *c, robj *o) {
struct hllhdr *hdr;


if (checkType(c,o,OBJ_STRING))
return C_ERR;

if (!sdsEncodedObject(o)) goto invalid;
if (stringObjectLen(o) < sizeof(*hdr)) goto invalid;
hdr = o->ptr;


if (hdr->magic[0] != 'H' || hdr->magic[1] != 'Y' ||
hdr->magic[2] != 'L' || hdr->magic[3] != 'L') goto invalid;

if (hdr->encoding > HLL_MAX_ENCODING) goto invalid;


if (hdr->encoding == HLL_DENSE &&
stringObjectLen(o) != HLL_DENSE_SIZE) goto invalid;


return C_OK;

invalid:
addReplyError(c,"-WRONGTYPE Key is not a valid "
"HyperLogLog string value.");
return C_ERR;
}


void pfaddCommand(client *c) {
robj *o = lookupKeyWrite(c->db,c->argv[1]);
struct hllhdr *hdr;
int updated = 0, j;

if (o == NULL) {



o = createHLLObject();
dbAdd(c->db,c->argv[1],o);
updated++;
} else {
if (isHLLObjectOrReply(c,o) != C_OK) return;
o = dbUnshareStringValue(c->db,c->argv[1],o);
}

for (j = 2; j < c->argc; j++) {
int retval = hllAdd(o, (unsigned char*)c->argv[j]->ptr,
sdslen(c->argv[j]->ptr));
switch(retval) {
case 1:
updated++;
break;
case -1:
addReplyError(c,invalid_hll_err);
return;
}
}
hdr = o->ptr;
if (updated) {
signalModifiedKey(c,c->db,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_STRING,"pfadd",c->argv[1],c->db->id);
server.dirty += updated;
HLL_INVALIDATE_CACHE(hdr);
}
addReply(c, updated ? shared.cone : shared.czero);
}


void pfcountCommand(client *c) {
robj *o;
struct hllhdr *hdr;
uint64_t card;





if (c->argc > 2) {
uint8_t max[HLL_HDR_SIZE+HLL_REGISTERS], *registers;
int j;


memset(max,0,sizeof(max));
hdr = (struct hllhdr*) max;
hdr->encoding = HLL_RAW;
registers = max + HLL_HDR_SIZE;
for (j = 1; j < c->argc; j++) {

robj *o = lookupKeyRead(c->db,c->argv[j]);
if (o == NULL) continue;
if (isHLLObjectOrReply(c,o) != C_OK) return;



if (hllMerge(registers,o) == C_ERR) {
addReplyError(c,invalid_hll_err);
return;
}
}


addReplyLongLong(c,hllCount(hdr,NULL));
return;
}












o = lookupKeyRead(c->db,c->argv[1]);
if (o == NULL) {


addReply(c,shared.czero);
} else {
if (isHLLObjectOrReply(c,o) != C_OK) return;
o = dbUnshareStringValue(c->db,c->argv[1],o);


hdr = o->ptr;
if (HLL_VALID_CACHE(hdr)) {

card = (uint64_t)hdr->card[0];
card |= (uint64_t)hdr->card[1] << 8;
card |= (uint64_t)hdr->card[2] << 16;
card |= (uint64_t)hdr->card[3] << 24;
card |= (uint64_t)hdr->card[4] << 32;
card |= (uint64_t)hdr->card[5] << 40;
card |= (uint64_t)hdr->card[6] << 48;
card |= (uint64_t)hdr->card[7] << 56;
} else {
int invalid = 0;

card = hllCount(hdr,&invalid);
if (invalid) {
addReplyError(c,invalid_hll_err);
return;
}
hdr->card[0] = card & 0xff;
hdr->card[1] = (card >> 8) & 0xff;
hdr->card[2] = (card >> 16) & 0xff;
hdr->card[3] = (card >> 24) & 0xff;
hdr->card[4] = (card >> 32) & 0xff;
hdr->card[5] = (card >> 40) & 0xff;
hdr->card[6] = (card >> 48) & 0xff;
hdr->card[7] = (card >> 56) & 0xff;



signalModifiedKey(c,c->db,c->argv[1]);
server.dirty++;
}
addReplyLongLong(c,card);
}
}


void pfmergeCommand(client *c) {
uint8_t max[HLL_REGISTERS];
struct hllhdr *hdr;
int j;
int use_dense = 0;




memset(max,0,sizeof(max));
for (j = 1; j < c->argc; j++) {

robj *o = lookupKeyRead(c->db,c->argv[j]);
if (o == NULL) continue;
if (isHLLObjectOrReply(c,o) != C_OK) return;



hdr = o->ptr;
if (hdr->encoding == HLL_DENSE) use_dense = 1;



if (hllMerge(max,o) == C_ERR) {
addReplyError(c,invalid_hll_err);
return;
}
}


robj *o = lookupKeyWrite(c->db,c->argv[1]);
if (o == NULL) {



o = createHLLObject();
dbAdd(c->db,c->argv[1],o);
} else {



o = dbUnshareStringValue(c->db,c->argv[1],o);
}



if (use_dense && hllSparseToDense(o) == C_ERR) {
addReplyError(c,invalid_hll_err);
return;
}



for (j = 0; j < HLL_REGISTERS; j++) {
if (max[j] == 0) continue;
hdr = o->ptr;
switch(hdr->encoding) {
case HLL_DENSE: hllDenseSet(hdr->registers,j,max[j]); break;
case HLL_SPARSE: hllSparseSet(o,j,max[j]); break;
}
}
hdr = o->ptr;

HLL_INVALIDATE_CACHE(hdr);

signalModifiedKey(c,c->db,c->argv[1]);


notifyKeyspaceEvent(NOTIFY_STRING,"pfadd",c->argv[1],c->db->id);
server.dirty++;
addReply(c,shared.ok);
}






#define HLL_TEST_CYCLES 1000
void pfselftestCommand(client *c) {
unsigned int j, i;
sds bitcounters = sdsnewlen(NULL,HLL_DENSE_SIZE);
struct hllhdr *hdr = (struct hllhdr*) bitcounters, *hdr2;
robj *o = NULL;
uint8_t bytecounters[HLL_REGISTERS];





for (j = 0; j < HLL_TEST_CYCLES; j++) {


for (i = 0; i < HLL_REGISTERS; i++) {
unsigned int r = rand() & HLL_REGISTER_MAX;

bytecounters[i] = r;
HLL_DENSE_SET_REGISTER(hdr->registers,i,r);
}

for (i = 0; i < HLL_REGISTERS; i++) {
unsigned int val;

HLL_DENSE_GET_REGISTER(val,hdr->registers,i);
if (val != bytecounters[i]) {
addReplyErrorFormat(c,
"TESTFAILED Register %d should be %d but is %d",
i, (int) bytecounters[i], (int) val);
goto cleanup;
}
}
}











memset(hdr->registers,0,HLL_DENSE_SIZE-HLL_HDR_SIZE);
o = createHLLObject();
double relerr = 1.04/sqrt(HLL_REGISTERS);
int64_t checkpoint = 1;
uint64_t seed = (uint64_t)rand() | (uint64_t)rand() << 32;
uint64_t ele;
for (j = 1; j <= 10000000; j++) {
ele = j ^ seed;
hllDenseAdd(hdr->registers,(unsigned char*)&ele,sizeof(ele));
hllAdd(o,(unsigned char*)&ele,sizeof(ele));



if (j == checkpoint && j < server.hll_sparse_max_bytes/2) {
hdr2 = o->ptr;
if (hdr2->encoding != HLL_SPARSE) {
addReplyError(c, "TESTFAILED sparse encoding not used");
goto cleanup;
}
}


if (j == checkpoint && hllCount(hdr,NULL) != hllCount(o->ptr,NULL)) {
addReplyError(c, "TESTFAILED dense/sparse disagree");
goto cleanup;
}


if (j == checkpoint) {
int64_t abserr = checkpoint - (int64_t)hllCount(hdr,NULL);
uint64_t maxerr = ceil(relerr*6*checkpoint);





if (j == 10) maxerr = 1;

if (abserr < 0) abserr = -abserr;
if (abserr > (int64_t)maxerr) {
addReplyErrorFormat(c,
"TESTFAILED Too big error. card:%llu abserr:%llu",
(unsigned long long) checkpoint,
(unsigned long long) abserr);
goto cleanup;
}
checkpoint *= 10;
}
}


addReply(c,shared.ok);

cleanup:
sdsfree(bitcounters);
if (o) decrRefCount(o);
}



void pfdebugCommand(client *c) {
char *cmd = c->argv[1]->ptr;
struct hllhdr *hdr;
robj *o;
int j;

o = lookupKeyWrite(c->db,c->argv[2]);
if (o == NULL) {
addReplyError(c,"The specified key does not exist");
return;
}
if (isHLLObjectOrReply(c,o) != C_OK) return;
o = dbUnshareStringValue(c->db,c->argv[2],o);
hdr = o->ptr;


if (!strcasecmp(cmd,"getreg")) {
if (c->argc != 3) goto arityerr;

if (hdr->encoding == HLL_SPARSE) {
if (hllSparseToDense(o) == C_ERR) {
addReplyError(c,invalid_hll_err);
return;
}
server.dirty++;
}

hdr = o->ptr;
addReplyArrayLen(c,HLL_REGISTERS);
for (j = 0; j < HLL_REGISTERS; j++) {
uint8_t val;

HLL_DENSE_GET_REGISTER(val,hdr->registers,j);
addReplyLongLong(c,val);
}
}

else if (!strcasecmp(cmd,"decode")) {
if (c->argc != 3) goto arityerr;

uint8_t *p = o->ptr, *end = p+sdslen(o->ptr);
sds decoded = sdsempty();

if (hdr->encoding != HLL_SPARSE) {
sdsfree(decoded);
addReplyError(c,"HLL encoding is not sparse");
return;
}

p += HLL_HDR_SIZE;
while(p < end) {
int runlen, regval;

if (HLL_SPARSE_IS_ZERO(p)) {
runlen = HLL_SPARSE_ZERO_LEN(p);
p++;
decoded = sdscatprintf(decoded,"z:%d ",runlen);
} else if (HLL_SPARSE_IS_XZERO(p)) {
runlen = HLL_SPARSE_XZERO_LEN(p);
p += 2;
decoded = sdscatprintf(decoded,"Z:%d ",runlen);
} else {
runlen = HLL_SPARSE_VAL_LEN(p);
regval = HLL_SPARSE_VAL_VALUE(p);
p++;
decoded = sdscatprintf(decoded,"v:%d,%d ",regval,runlen);
}
}
decoded = sdstrim(decoded," ");
addReplyBulkCBuffer(c,decoded,sdslen(decoded));
sdsfree(decoded);
}

else if (!strcasecmp(cmd,"encoding")) {
char *encodingstr[2] = {"dense","sparse"};
if (c->argc != 3) goto arityerr;

addReplyStatus(c,encodingstr[hdr->encoding]);
}

else if (!strcasecmp(cmd,"todense")) {
int conv = 0;
if (c->argc != 3) goto arityerr;

if (hdr->encoding == HLL_SPARSE) {
if (hllSparseToDense(o) == C_ERR) {
addReplyError(c,invalid_hll_err);
return;
}
conv = 1;
server.dirty++;
}
addReply(c,conv ? shared.cone : shared.czero);
} else {
addReplyErrorFormat(c,"Unknown PFDEBUG subcommand '%s'", cmd);
}
return;

arityerr:
addReplyErrorFormat(c,
"Wrong number of arguments for the '%s' subcommand",cmd);
}

