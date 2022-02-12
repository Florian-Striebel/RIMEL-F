


































#include "fmacros.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/time.h>

#include "dict.h"
#include "zmalloc.h"
#include "redisassert.h"









static int dict_can_resize = 1;
static unsigned int dict_force_resize_ratio = 5;



static int _dictExpandIfNeeded(dict *d);
static signed char _dictNextExp(unsigned long size);
static long _dictKeyIndex(dict *d, const void *key, uint64_t hash, dictEntry **existing);
static int _dictInit(dict *d, dictType *type);



static uint8_t dict_hash_function_seed[16];

void dictSetHashFunctionSeed(uint8_t *seed) {
memcpy(dict_hash_function_seed,seed,sizeof(dict_hash_function_seed));
}

uint8_t *dictGetHashFunctionSeed(void) {
return dict_hash_function_seed;
}




uint64_t siphash(const uint8_t *in, const size_t inlen, const uint8_t *k);
uint64_t siphash_nocase(const uint8_t *in, const size_t inlen, const uint8_t *k);

uint64_t dictGenHashFunction(const void *key, size_t len) {
return siphash(key,len,dict_hash_function_seed);
}

uint64_t dictGenCaseHashFunction(const unsigned char *buf, size_t len) {
return siphash_nocase(buf,len,dict_hash_function_seed);
}




static void _dictReset(dict *d, int htidx)
{
d->ht_table[htidx] = NULL;
d->ht_size_exp[htidx] = -1;
d->ht_used[htidx] = 0;
}


dict *dictCreate(dictType *type)
{
dict *d = zmalloc(sizeof(*d));

_dictInit(d,type);
return d;
}


int _dictInit(dict *d, dictType *type)
{
_dictReset(d, 0);
_dictReset(d, 1);
d->type = type;
d->rehashidx = -1;
d->pauserehash = 0;
return DICT_OK;
}



int dictResize(dict *d)
{
unsigned long minimal;

if (!dict_can_resize || dictIsRehashing(d)) return DICT_ERR;
minimal = d->ht_used[0];
if (minimal < DICT_HT_INITIAL_SIZE)
minimal = DICT_HT_INITIAL_SIZE;
return dictExpand(d, minimal);
}




int _dictExpand(dict *d, unsigned long size, int* malloc_failed)
{
if (malloc_failed) *malloc_failed = 0;



if (dictIsRehashing(d) || d->ht_used[0] > size)
return DICT_ERR;


dictEntry **new_ht_table;
unsigned long new_ht_used;
signed char new_ht_size_exp = _dictNextExp(size);


size_t newsize = 1ul<<new_ht_size_exp;
if (newsize < size || newsize * sizeof(dictEntry*) < newsize)
return DICT_ERR;


if (new_ht_size_exp == d->ht_size_exp[0]) return DICT_ERR;


if (malloc_failed) {
new_ht_table = ztrycalloc(newsize*sizeof(dictEntry*));
*malloc_failed = new_ht_table == NULL;
if (*malloc_failed)
return DICT_ERR;
} else
new_ht_table = zcalloc(newsize*sizeof(dictEntry*));

new_ht_used = 0;



if (d->ht_table[0] == NULL) {
d->ht_size_exp[0] = new_ht_size_exp;
d->ht_used[0] = new_ht_used;
d->ht_table[0] = new_ht_table;
return DICT_OK;
}


d->ht_size_exp[1] = new_ht_size_exp;
d->ht_used[1] = new_ht_used;
d->ht_table[1] = new_ht_table;
d->rehashidx = 0;
return DICT_OK;
}


int dictExpand(dict *d, unsigned long size) {
return _dictExpand(d, size, NULL);
}


int dictTryExpand(dict *d, unsigned long size) {
int malloc_failed;
_dictExpand(d, size, &malloc_failed);
return malloc_failed? DICT_ERR : DICT_OK;
}










int dictRehash(dict *d, int n) {
int empty_visits = n*10;
if (!dictIsRehashing(d)) return 0;

while(n-- && d->ht_used[0] != 0) {
dictEntry *de, *nextde;



assert(DICTHT_SIZE(d->ht_size_exp[0]) > (unsigned long)d->rehashidx);
while(d->ht_table[0][d->rehashidx] == NULL) {
d->rehashidx++;
if (--empty_visits == 0) return 1;
}
de = d->ht_table[0][d->rehashidx];

while(de) {
uint64_t h;

nextde = de->next;

h = dictHashKey(d, de->key) & DICTHT_SIZE_MASK(d->ht_size_exp[1]);
de->next = d->ht_table[1][h];
d->ht_table[1][h] = de;
d->ht_used[0]--;
d->ht_used[1]++;
de = nextde;
}
d->ht_table[0][d->rehashidx] = NULL;
d->rehashidx++;
}


if (d->ht_used[0] == 0) {
zfree(d->ht_table[0]);

d->ht_table[0] = d->ht_table[1];
d->ht_used[0] = d->ht_used[1];
d->ht_size_exp[0] = d->ht_size_exp[1];
_dictReset(d, 1);
d->rehashidx = -1;
return 0;
}


return 1;
}

long long timeInMilliseconds(void) {
struct timeval tv;

gettimeofday(&tv,NULL);
return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}




int dictRehashMilliseconds(dict *d, int ms) {
if (d->pauserehash > 0) return 0;

long long start = timeInMilliseconds();
int rehashes = 0;

while(dictRehash(d,100)) {
rehashes += 100;
if (timeInMilliseconds()-start > ms) break;
}
return rehashes;
}









static void _dictRehashStep(dict *d) {
if (d->pauserehash == 0) dictRehash(d,1);
}


int dictAdd(dict *d, void *key, void *val)
{
dictEntry *entry = dictAddRaw(d,key,NULL);

if (!entry) return DICT_ERR;
dictSetVal(d, entry, val);
return DICT_OK;
}



















dictEntry *dictAddRaw(dict *d, void *key, dictEntry **existing)
{
long index;
dictEntry *entry;
int htidx;

if (dictIsRehashing(d)) _dictRehashStep(d);



if ((index = _dictKeyIndex(d, key, dictHashKey(d,key), existing)) == -1)
return NULL;





htidx = dictIsRehashing(d) ? 1 : 0;
size_t metasize = dictMetadataSize(d);
entry = zmalloc(sizeof(*entry) + metasize);
if (metasize > 0) {
memset(dictMetadata(entry), 0, metasize);
}
entry->next = d->ht_table[htidx][index];
d->ht_table[htidx][index] = entry;
d->ht_used[htidx]++;


dictSetKey(d, entry, key);
return entry;
}






int dictReplace(dict *d, void *key, void *val)
{
dictEntry *entry, *existing, auxentry;



entry = dictAddRaw(d,key,&existing);
if (entry) {
dictSetVal(d, entry, val);
return 1;
}






auxentry = *existing;
dictSetVal(d, existing, val);
dictFreeVal(d, &auxentry);
return 0;
}








dictEntry *dictAddOrFind(dict *d, void *key) {
dictEntry *entry, *existing;
entry = dictAddRaw(d,key,&existing);
return entry ? entry : existing;
}




static dictEntry *dictGenericDelete(dict *d, const void *key, int nofree) {
uint64_t h, idx;
dictEntry *he, *prevHe;
int table;


if (dictSize(d) == 0) return NULL;

if (dictIsRehashing(d)) _dictRehashStep(d);
h = dictHashKey(d, key);

for (table = 0; table <= 1; table++) {
idx = h & DICTHT_SIZE_MASK(d->ht_size_exp[table]);
he = d->ht_table[table][idx];
prevHe = NULL;
while(he) {
if (key==he->key || dictCompareKeys(d, key, he->key)) {

if (prevHe)
prevHe->next = he->next;
else
d->ht_table[table][idx] = he->next;
if (!nofree) {
dictFreeUnlinkedEntry(d, he);
}
d->ht_used[table]--;
return he;
}
prevHe = he;
he = he->next;
}
if (!dictIsRehashing(d)) break;
}
return NULL;
}



int dictDelete(dict *ht, const void *key) {
return dictGenericDelete(ht,key,0) ? DICT_OK : DICT_ERR;
}






















dictEntry *dictUnlink(dict *d, const void *key) {
return dictGenericDelete(d,key,1);
}



void dictFreeUnlinkedEntry(dict *d, dictEntry *he) {
if (he == NULL) return;
dictFreeKey(d, he);
dictFreeVal(d, he);
zfree(he);
}


int _dictClear(dict *d, int htidx, void(callback)(dict*)) {
unsigned long i;


for (i = 0; i < DICTHT_SIZE(d->ht_size_exp[htidx]) && d->ht_used[htidx] > 0; i++) {
dictEntry *he, *nextHe;

if (callback && (i & 65535) == 0) callback(d);

if ((he = d->ht_table[htidx][i]) == NULL) continue;
while(he) {
nextHe = he->next;
dictFreeKey(d, he);
dictFreeVal(d, he);
zfree(he);
d->ht_used[htidx]--;
he = nextHe;
}
}

zfree(d->ht_table[htidx]);

_dictReset(d, htidx);
return DICT_OK;
}


void dictRelease(dict *d)
{
_dictClear(d,0,NULL);
_dictClear(d,1,NULL);
zfree(d);
}

dictEntry *dictFind(dict *d, const void *key)
{
dictEntry *he;
uint64_t h, idx, table;

if (dictSize(d) == 0) return NULL;
if (dictIsRehashing(d)) _dictRehashStep(d);
h = dictHashKey(d, key);
for (table = 0; table <= 1; table++) {
idx = h & DICTHT_SIZE_MASK(d->ht_size_exp[table]);
he = d->ht_table[table][idx];
while(he) {
if (key==he->key || dictCompareKeys(d, key, he->key))
return he;
he = he->next;
}
if (!dictIsRehashing(d)) return NULL;
}
return NULL;
}

void *dictFetchValue(dict *d, const void *key) {
dictEntry *he;

he = dictFind(d,key);
return he ? dictGetVal(he) : NULL;
}







unsigned long long dictFingerprint(dict *d) {
unsigned long long integers[6], hash = 0;
int j;

integers[0] = (long) d->ht_table[0];
integers[1] = d->ht_size_exp[0];
integers[2] = d->ht_used[0];
integers[3] = (long) d->ht_table[1];
integers[4] = d->ht_size_exp[1];
integers[5] = d->ht_used[1];








for (j = 0; j < 6; j++) {
hash += integers[j];

hash = (~hash) + (hash << 21);
hash = hash ^ (hash >> 24);
hash = (hash + (hash << 3)) + (hash << 8);
hash = hash ^ (hash >> 14);
hash = (hash + (hash << 2)) + (hash << 4);
hash = hash ^ (hash >> 28);
hash = hash + (hash << 31);
}
return hash;
}

dictIterator *dictGetIterator(dict *d)
{
dictIterator *iter = zmalloc(sizeof(*iter));

iter->d = d;
iter->table = 0;
iter->index = -1;
iter->safe = 0;
iter->entry = NULL;
iter->nextEntry = NULL;
return iter;
}

dictIterator *dictGetSafeIterator(dict *d) {
dictIterator *i = dictGetIterator(d);

i->safe = 1;
return i;
}

dictEntry *dictNext(dictIterator *iter)
{
while (1) {
if (iter->entry == NULL) {
if (iter->index == -1 && iter->table == 0) {
if (iter->safe)
dictPauseRehashing(iter->d);
else
iter->fingerprint = dictFingerprint(iter->d);
}
iter->index++;
if (iter->index >= (long) DICTHT_SIZE(iter->d->ht_size_exp[iter->table])) {
if (dictIsRehashing(iter->d) && iter->table == 0) {
iter->table++;
iter->index = 0;
} else {
break;
}
}
iter->entry = iter->d->ht_table[iter->table][iter->index];
} else {
iter->entry = iter->nextEntry;
}
if (iter->entry) {


iter->nextEntry = iter->entry->next;
return iter->entry;
}
}
return NULL;
}

void dictReleaseIterator(dictIterator *iter)
{
if (!(iter->index == -1 && iter->table == 0)) {
if (iter->safe)
dictResumeRehashing(iter->d);
else
assert(iter->fingerprint == dictFingerprint(iter->d));
}
zfree(iter);
}



dictEntry *dictGetRandomKey(dict *d)
{
dictEntry *he, *orighe;
unsigned long h;
int listlen, listele;

if (dictSize(d) == 0) return NULL;
if (dictIsRehashing(d)) _dictRehashStep(d);
if (dictIsRehashing(d)) {
unsigned long s0 = DICTHT_SIZE(d->ht_size_exp[0]);
do {


h = d->rehashidx + (randomULong() % (dictSlots(d) - d->rehashidx));
he = (h >= s0) ? d->ht_table[1][h - s0] : d->ht_table[0][h];
} while(he == NULL);
} else {
unsigned long m = DICTHT_SIZE_MASK(d->ht_size_exp[0]);
do {
h = randomULong() & m;
he = d->ht_table[0][h];
} while(he == NULL);
}





listlen = 0;
orighe = he;
while(he) {
he = he->next;
listlen++;
}
listele = random() % listlen;
he = orighe;
while(listele--) he = he->next;
return he;
}























unsigned int dictGetSomeKeys(dict *d, dictEntry **des, unsigned int count) {
unsigned long j;
unsigned long tables;
unsigned long stored = 0, maxsizemask;
unsigned long maxsteps;

if (dictSize(d) < count) count = dictSize(d);
maxsteps = count*10;


for (j = 0; j < count; j++) {
if (dictIsRehashing(d))
_dictRehashStep(d);
else
break;
}

tables = dictIsRehashing(d) ? 2 : 1;
maxsizemask = DICTHT_SIZE_MASK(d->ht_size_exp[0]);
if (tables > 1 && maxsizemask < DICTHT_SIZE_MASK(d->ht_size_exp[1]))
maxsizemask = DICTHT_SIZE_MASK(d->ht_size_exp[1]);


unsigned long i = randomULong() & maxsizemask;
unsigned long emptylen = 0;
while(stored < count && maxsteps--) {
for (j = 0; j < tables; j++) {



if (tables == 2 && j == 0 && i < (unsigned long) d->rehashidx) {




if (i >= DICTHT_SIZE(d->ht_size_exp[1]))
i = d->rehashidx;
else
continue;
}
if (i >= DICTHT_SIZE(d->ht_size_exp[j])) continue;
dictEntry *he = d->ht_table[j][i];



if (he == NULL) {
emptylen++;
if (emptylen >= 5 && emptylen > count) {
i = randomULong() & maxsizemask;
emptylen = 0;
}
} else {
emptylen = 0;
while (he) {


*des = he;
des++;
he = he->next;
stored++;
if (stored == count) return stored;
}
}
}
i = (i+1) & maxsizemask;
}
return stored;
}












#define GETFAIR_NUM_ENTRIES 15
dictEntry *dictGetFairRandomKey(dict *d) {
dictEntry *entries[GETFAIR_NUM_ENTRIES];
unsigned int count = dictGetSomeKeys(d,entries,GETFAIR_NUM_ENTRIES);




if (count == 0) return dictGetRandomKey(d);
unsigned int idx = rand() % count;
return entries[idx];
}



static unsigned long rev(unsigned long v) {
unsigned long s = CHAR_BIT * sizeof(v);
unsigned long mask = ~0UL;
while ((s >>= 1) > 0) {
mask ^= (mask << s);
v = ((v >> s) & mask) | ((v << s) & ~mask);
}
return v;
}





















































































unsigned long dictScan(dict *d,
unsigned long v,
dictScanFunction *fn,
dictScanBucketFunction* bucketfn,
void *privdata)
{
int htidx0, htidx1;
const dictEntry *de, *next;
unsigned long m0, m1;

if (dictSize(d) == 0) return 0;


dictPauseRehashing(d);

if (!dictIsRehashing(d)) {
htidx0 = 0;
m0 = DICTHT_SIZE_MASK(d->ht_size_exp[htidx0]);


if (bucketfn) bucketfn(d, &d->ht_table[htidx0][v & m0]);
de = d->ht_table[htidx0][v & m0];
while (de) {
next = de->next;
fn(privdata, de);
de = next;
}



v |= ~m0;


v = rev(v);
v++;
v = rev(v);

} else {
htidx0 = 0;
htidx1 = 1;


if (DICTHT_SIZE(d->ht_size_exp[htidx0]) > DICTHT_SIZE(d->ht_size_exp[htidx1])) {
htidx0 = 1;
htidx1 = 0;
}

m0 = DICTHT_SIZE_MASK(d->ht_size_exp[htidx0]);
m1 = DICTHT_SIZE_MASK(d->ht_size_exp[htidx1]);


if (bucketfn) bucketfn(d, &d->ht_table[htidx0][v & m0]);
de = d->ht_table[htidx0][v & m0];
while (de) {
next = de->next;
fn(privdata, de);
de = next;
}



do {

if (bucketfn) bucketfn(d, &d->ht_table[htidx1][v & m1]);
de = d->ht_table[htidx1][v & m1];
while (de) {
next = de->next;
fn(privdata, de);
de = next;
}


v |= ~m1;
v = rev(v);
v++;
v = rev(v);


} while (v & (m0 ^ m1));
}

dictResumeRehashing(d);

return v;
}






static int dictTypeExpandAllowed(dict *d) {
if (d->type->expandAllowed == NULL) return 1;
return d->type->expandAllowed(
DICTHT_SIZE(_dictNextExp(d->ht_used[0] + 1)) * sizeof(dictEntry*),
(double)d->ht_used[0] / DICTHT_SIZE(d->ht_size_exp[0]));
}


static int _dictExpandIfNeeded(dict *d)
{

if (dictIsRehashing(d)) return DICT_OK;


if (DICTHT_SIZE(d->ht_size_exp[0]) == 0) return dictExpand(d, DICT_HT_INITIAL_SIZE);





if (d->ht_used[0] >= DICTHT_SIZE(d->ht_size_exp[0]) &&
(dict_can_resize ||
d->ht_used[0]/ DICTHT_SIZE(d->ht_size_exp[0]) > dict_force_resize_ratio) &&
dictTypeExpandAllowed(d))
{
return dictExpand(d, d->ht_used[0] + 1);
}
return DICT_OK;
}



static signed char _dictNextExp(unsigned long size)
{
unsigned char e = DICT_HT_INITIAL_EXP;

if (size >= LONG_MAX) return (8*sizeof(long)-1);
while(1) {
if (((unsigned long)1<<e) >= size)
return e;
e++;
}
}








static long _dictKeyIndex(dict *d, const void *key, uint64_t hash, dictEntry **existing)
{
unsigned long idx, table;
dictEntry *he;
if (existing) *existing = NULL;


if (_dictExpandIfNeeded(d) == DICT_ERR)
return -1;
for (table = 0; table <= 1; table++) {
idx = hash & DICTHT_SIZE_MASK(d->ht_size_exp[table]);

he = d->ht_table[table][idx];
while(he) {
if (key==he->key || dictCompareKeys(d, key, he->key)) {
if (existing) *existing = he;
return -1;
}
he = he->next;
}
if (!dictIsRehashing(d)) break;
}
return idx;
}

void dictEmpty(dict *d, void(callback)(dict*)) {
_dictClear(d,0,callback);
_dictClear(d,1,callback);
d->rehashidx = -1;
d->pauserehash = 0;
}

void dictEnableResize(void) {
dict_can_resize = 1;
}

void dictDisableResize(void) {
dict_can_resize = 0;
}

uint64_t dictGetHash(dict *d, const void *key) {
return dictHashKey(d, key);
}






dictEntry **dictFindEntryRefByPtrAndHash(dict *d, const void *oldptr, uint64_t hash) {
dictEntry *he, **heref;
unsigned long idx, table;

if (dictSize(d) == 0) return NULL;
for (table = 0; table <= 1; table++) {
idx = hash & DICTHT_SIZE_MASK(d->ht_size_exp[table]);
heref = &d->ht_table[table][idx];
he = *heref;
while(he) {
if (oldptr==he->key)
return heref;
heref = &he->next;
he = *heref;
}
if (!dictIsRehashing(d)) return NULL;
}
return NULL;
}



#define DICT_STATS_VECTLEN 50
size_t _dictGetStatsHt(char *buf, size_t bufsize, dict *d, int htidx) {
unsigned long i, slots = 0, chainlen, maxchainlen = 0;
unsigned long totchainlen = 0;
unsigned long clvector[DICT_STATS_VECTLEN];
size_t l = 0;

if (d->ht_used[htidx] == 0) {
return snprintf(buf,bufsize,
"No stats available for empty dictionaries\n");
}


for (i = 0; i < DICT_STATS_VECTLEN; i++) clvector[i] = 0;
for (i = 0; i < DICTHT_SIZE(d->ht_size_exp[htidx]); i++) {
dictEntry *he;

if (d->ht_table[htidx][i] == NULL) {
clvector[0]++;
continue;
}
slots++;

chainlen = 0;
he = d->ht_table[htidx][i];
while(he) {
chainlen++;
he = he->next;
}
clvector[(chainlen < DICT_STATS_VECTLEN) ? chainlen : (DICT_STATS_VECTLEN-1)]++;
if (chainlen > maxchainlen) maxchainlen = chainlen;
totchainlen += chainlen;
}


l += snprintf(buf+l,bufsize-l,
"Hash table %d stats (%s):\n"
" table size: %lu\n"
" number of elements: %lu\n"
" different slots: %lu\n"
" max chain length: %lu\n"
" avg chain length (counted): %.02f\n"
" avg chain length (computed): %.02f\n"
" Chain length distribution:\n",
htidx, (htidx == 0) ? "main hash table" : "rehashing target",
DICTHT_SIZE(d->ht_size_exp[htidx]), d->ht_used[htidx], slots, maxchainlen,
(float)totchainlen/slots, (float)d->ht_used[htidx]/slots);

for (i = 0; i < DICT_STATS_VECTLEN-1; i++) {
if (clvector[i] == 0) continue;
if (l >= bufsize) break;
l += snprintf(buf+l,bufsize-l,
" %ld: %ld (%.02f%%)\n",
i, clvector[i], ((float)clvector[i]/DICTHT_SIZE(d->ht_size_exp[htidx]))*100);
}


if (bufsize) buf[bufsize-1] = '\0';
return strlen(buf);
}

void dictGetStats(char *buf, size_t bufsize, dict *d) {
size_t l;
char *orig_buf = buf;
size_t orig_bufsize = bufsize;

l = _dictGetStatsHt(buf,bufsize,d,0);
buf += l;
bufsize -= l;
if (dictIsRehashing(d) && bufsize > 0) {
_dictGetStatsHt(buf,bufsize,d,1);
}

if (orig_bufsize) orig_buf[orig_bufsize-1] = '\0';
}



#if defined(REDIS_TEST)
#include "testhelp.h"

#define UNUSED(V) ((void) V)

uint64_t hashCallback(const void *key) {
return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

int compareCallback(dict *d, const void *key1, const void *key2) {
int l1,l2;
UNUSED(d);

l1 = strlen((char*)key1);
l2 = strlen((char*)key2);
if (l1 != l2) return 0;
return memcmp(key1, key2, l1) == 0;
}

void freeCallback(dict *d, void *val) {
UNUSED(d);

zfree(val);
}

char *stringFromLongLong(long long value) {
char buf[32];
int len;
char *s;

len = sprintf(buf,"%lld",value);
s = zmalloc(len+1);
memcpy(s, buf, len);
s[len] = '\0';
return s;
}

dictType BenchmarkDictType = {
hashCallback,
NULL,
NULL,
compareCallback,
freeCallback,
NULL,
NULL
};

#define start_benchmark() start = timeInMilliseconds()
#define end_benchmark(msg) do { elapsed = timeInMilliseconds()-start; printf(msg ": %ld items in %lld ms\n", count, elapsed); } while(0)





int dictTest(int argc, char **argv, int flags) {
long j;
long long start, elapsed;
dict *dict = dictCreate(&BenchmarkDictType);
long count = 0;
int accurate = (flags & REDIS_TEST_ACCURATE);

if (argc == 4) {
if (accurate) {
count = 5000000;
} else {
count = strtol(argv[3],NULL,10);
}
} else {
count = 5000;
}

start_benchmark();
for (j = 0; j < count; j++) {
int retval = dictAdd(dict,stringFromLongLong(j),(void*)j);
assert(retval == DICT_OK);
}
end_benchmark("Inserting");
assert((long)dictSize(dict) == count);


while (dictIsRehashing(dict)) {
dictRehashMilliseconds(dict,100);
}

start_benchmark();
for (j = 0; j < count; j++) {
char *key = stringFromLongLong(j);
dictEntry *de = dictFind(dict,key);
assert(de != NULL);
zfree(key);
}
end_benchmark("Linear access of existing elements");

start_benchmark();
for (j = 0; j < count; j++) {
char *key = stringFromLongLong(j);
dictEntry *de = dictFind(dict,key);
assert(de != NULL);
zfree(key);
}
end_benchmark("Linear access of existing elements (2nd round)");

start_benchmark();
for (j = 0; j < count; j++) {
char *key = stringFromLongLong(rand() % count);
dictEntry *de = dictFind(dict,key);
assert(de != NULL);
zfree(key);
}
end_benchmark("Random access of existing elements");

start_benchmark();
for (j = 0; j < count; j++) {
dictEntry *de = dictGetRandomKey(dict);
assert(de != NULL);
}
end_benchmark("Accessing random keys");

start_benchmark();
for (j = 0; j < count; j++) {
char *key = stringFromLongLong(rand() % count);
key[0] = 'X';
dictEntry *de = dictFind(dict,key);
assert(de == NULL);
zfree(key);
}
end_benchmark("Accessing missing");

start_benchmark();
for (j = 0; j < count; j++) {
char *key = stringFromLongLong(j);
int retval = dictDelete(dict,key);
assert(retval == DICT_OK);
key[0] += 17;
retval = dictAdd(dict,key,(void*)j);
assert(retval == DICT_OK);
}
end_benchmark("Removing and adding");
dictRelease(dict);
return 0;
}
#endif
