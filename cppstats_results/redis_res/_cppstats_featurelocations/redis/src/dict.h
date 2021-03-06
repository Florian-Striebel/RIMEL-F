


































#if !defined(__DICT_H)
#define __DICT_H

#include "mt19937-64.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#define DICT_OK 0
#define DICT_ERR 1

typedef struct dictEntry {
void *key;
union {
void *val;
uint64_t u64;
int64_t s64;
double d;
} v;
struct dictEntry *next;
void *metadata[];


} dictEntry;

typedef struct dict dict;

typedef struct dictType {
uint64_t (*hashFunction)(const void *key);
void *(*keyDup)(dict *d, const void *key);
void *(*valDup)(dict *d, const void *obj);
int (*keyCompare)(dict *d, const void *key1, const void *key2);
void (*keyDestructor)(dict *d, void *key);
void (*valDestructor)(dict *d, void *obj);
int (*expandAllowed)(size_t moreMem, double usedRatio);


size_t (*dictEntryMetadataBytes)(dict *d);
} dictType;

#define DICTHT_SIZE(exp) ((exp) == -1 ? 0 : (unsigned long)1<<(exp))
#define DICTHT_SIZE_MASK(exp) ((exp) == -1 ? 0 : (DICTHT_SIZE(exp))-1)

struct dict {
dictType *type;

dictEntry **ht_table[2];
unsigned long ht_used[2];

long rehashidx;


int16_t pauserehash;
signed char ht_size_exp[2];
};





typedef struct dictIterator {
dict *d;
long index;
int table, safe;
dictEntry *entry, *nextEntry;

unsigned long long fingerprint;
} dictIterator;

typedef void (dictScanFunction)(void *privdata, const dictEntry *de);
typedef void (dictScanBucketFunction)(dict *d, dictEntry **bucketref);


#define DICT_HT_INITIAL_EXP 2
#define DICT_HT_INITIAL_SIZE (1<<(DICT_HT_INITIAL_EXP))


#define dictFreeVal(d, entry) if ((d)->type->valDestructor) (d)->type->valDestructor((d), (entry)->v.val)



#define dictSetVal(d, entry, _val_) do { if ((d)->type->valDup) (entry)->v.val = (d)->type->valDup((d), _val_); else (entry)->v.val = (_val_); } while(0)






#define dictSetSignedIntegerVal(entry, _val_) do { (entry)->v.s64 = _val_; } while(0)


#define dictSetUnsignedIntegerVal(entry, _val_) do { (entry)->v.u64 = _val_; } while(0)


#define dictSetDoubleVal(entry, _val_) do { (entry)->v.d = _val_; } while(0)


#define dictFreeKey(d, entry) if ((d)->type->keyDestructor) (d)->type->keyDestructor((d), (entry)->key)



#define dictSetKey(d, entry, _key_) do { if ((d)->type->keyDup) (entry)->key = (d)->type->keyDup((d), _key_); else (entry)->key = (_key_); } while(0)






#define dictCompareKeys(d, key1, key2) (((d)->type->keyCompare) ? (d)->type->keyCompare((d), key1, key2) : (key1) == (key2))




#define dictMetadata(entry) (&(entry)->metadata)
#define dictMetadataSize(d) ((d)->type->dictEntryMetadataBytes ? (d)->type->dictEntryMetadataBytes(d) : 0)


#define dictHashKey(d, key) (d)->type->hashFunction(key)
#define dictGetKey(he) ((he)->key)
#define dictGetVal(he) ((he)->v.val)
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
#define dictGetDoubleVal(he) ((he)->v.d)
#define dictSlots(d) (DICTHT_SIZE((d)->ht_size_exp[0])+DICTHT_SIZE((d)->ht_size_exp[1]))
#define dictSize(d) ((d)->ht_used[0]+(d)->ht_used[1])
#define dictIsRehashing(d) ((d)->rehashidx != -1)
#define dictPauseRehashing(d) (d)->pauserehash++
#define dictResumeRehashing(d) (d)->pauserehash--


#if ULONG_MAX >= 0xffffffffffffffff
#define randomULong() ((unsigned long) genrand64_int64())
#else
#define randomULong() random()
#endif


dict *dictCreate(dictType *type);
int dictExpand(dict *d, unsigned long size);
int dictTryExpand(dict *d, unsigned long size);
int dictAdd(dict *d, void *key, void *val);
dictEntry *dictAddRaw(dict *d, void *key, dictEntry **existing);
dictEntry *dictAddOrFind(dict *d, void *key);
int dictReplace(dict *d, void *key, void *val);
int dictDelete(dict *d, const void *key);
dictEntry *dictUnlink(dict *d, const void *key);
void dictFreeUnlinkedEntry(dict *d, dictEntry *he);
void dictRelease(dict *d);
dictEntry * dictFind(dict *d, const void *key);
void *dictFetchValue(dict *d, const void *key);
int dictResize(dict *d);
dictIterator *dictGetIterator(dict *d);
dictIterator *dictGetSafeIterator(dict *d);
dictEntry *dictNext(dictIterator *iter);
void dictReleaseIterator(dictIterator *iter);
dictEntry *dictGetRandomKey(dict *d);
dictEntry *dictGetFairRandomKey(dict *d);
unsigned int dictGetSomeKeys(dict *d, dictEntry **des, unsigned int count);
void dictGetStats(char *buf, size_t bufsize, dict *d);
uint64_t dictGenHashFunction(const void *key, size_t len);
uint64_t dictGenCaseHashFunction(const unsigned char *buf, size_t len);
void dictEmpty(dict *d, void(callback)(dict*));
void dictEnableResize(void);
void dictDisableResize(void);
int dictRehash(dict *d, int n);
int dictRehashMilliseconds(dict *d, int ms);
void dictSetHashFunctionSeed(uint8_t *seed);
uint8_t *dictGetHashFunctionSeed(void);
unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, dictScanBucketFunction *bucketfn, void *privdata);
uint64_t dictGetHash(dict *d, const void *key);
dictEntry **dictFindEntryRefByPtrAndHash(dict *d, const void *oldptr, uint64_t hash);

#if defined(REDIS_TEST)
int dictTest(int argc, char *argv[], int flags);
#endif

#endif
