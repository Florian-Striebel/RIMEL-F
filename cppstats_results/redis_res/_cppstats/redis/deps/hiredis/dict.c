#include "fmacros.h"
#include "alloc.h"
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "dict.h"
static int _dictExpandIfNeeded(dict *ht);
static unsigned long _dictNextPower(unsigned long size);
static int _dictKeyIndex(dict *ht, const void *key);
static int _dictInit(dict *ht, dictType *type, void *privDataPtr);
static unsigned int dictGenHashFunction(const unsigned char *buf, int len) {
unsigned int hash = 5381;
while (len--)
hash = ((hash << 5) + hash) + (*buf++);
return hash;
}
static void _dictReset(dict *ht) {
ht->table = NULL;
ht->size = 0;
ht->sizemask = 0;
ht->used = 0;
}
static dict *dictCreate(dictType *type, void *privDataPtr) {
dict *ht = hi_malloc(sizeof(*ht));
if (ht == NULL)
return NULL;
_dictInit(ht,type,privDataPtr);
return ht;
}
static int _dictInit(dict *ht, dictType *type, void *privDataPtr) {
_dictReset(ht);
ht->type = type;
ht->privdata = privDataPtr;
return DICT_OK;
}
static int dictExpand(dict *ht, unsigned long size) {
dict n;
unsigned long realsize = _dictNextPower(size), i;
if (ht->used > size)
return DICT_ERR;
_dictInit(&n, ht->type, ht->privdata);
n.size = realsize;
n.sizemask = realsize-1;
n.table = hi_calloc(realsize,sizeof(dictEntry*));
if (n.table == NULL)
return DICT_ERR;
n.used = ht->used;
for (i = 0; i < ht->size && ht->used > 0; i++) {
dictEntry *he, *nextHe;
if (ht->table[i] == NULL) continue;
he = ht->table[i];
while(he) {
unsigned int h;
nextHe = he->next;
h = dictHashKey(ht, he->key) & n.sizemask;
he->next = n.table[h];
n.table[h] = he;
ht->used--;
he = nextHe;
}
}
assert(ht->used == 0);
hi_free(ht->table);
*ht = n;
return DICT_OK;
}
static int dictAdd(dict *ht, void *key, void *val) {
int index;
dictEntry *entry;
if ((index = _dictKeyIndex(ht, key)) == -1)
return DICT_ERR;
entry = hi_malloc(sizeof(*entry));
if (entry == NULL)
return DICT_ERR;
entry->next = ht->table[index];
ht->table[index] = entry;
dictSetHashKey(ht, entry, key);
dictSetHashVal(ht, entry, val);
ht->used++;
return DICT_OK;
}
static int dictReplace(dict *ht, void *key, void *val) {
dictEntry *entry, auxentry;
if (dictAdd(ht, key, val) == DICT_OK)
return 1;
entry = dictFind(ht, key);
if (entry == NULL)
return 0;
auxentry = *entry;
dictSetHashVal(ht, entry, val);
dictFreeEntryVal(ht, &auxentry);
return 0;
}
static int dictDelete(dict *ht, const void *key) {
unsigned int h;
dictEntry *de, *prevde;
if (ht->size == 0)
return DICT_ERR;
h = dictHashKey(ht, key) & ht->sizemask;
de = ht->table[h];
prevde = NULL;
while(de) {
if (dictCompareHashKeys(ht,key,de->key)) {
if (prevde)
prevde->next = de->next;
else
ht->table[h] = de->next;
dictFreeEntryKey(ht,de);
dictFreeEntryVal(ht,de);
hi_free(de);
ht->used--;
return DICT_OK;
}
prevde = de;
de = de->next;
}
return DICT_ERR;
}
static int _dictClear(dict *ht) {
unsigned long i;
for (i = 0; i < ht->size && ht->used > 0; i++) {
dictEntry *he, *nextHe;
if ((he = ht->table[i]) == NULL) continue;
while(he) {
nextHe = he->next;
dictFreeEntryKey(ht, he);
dictFreeEntryVal(ht, he);
hi_free(he);
ht->used--;
he = nextHe;
}
}
hi_free(ht->table);
_dictReset(ht);
return DICT_OK;
}
static void dictRelease(dict *ht) {
_dictClear(ht);
hi_free(ht);
}
static dictEntry *dictFind(dict *ht, const void *key) {
dictEntry *he;
unsigned int h;
if (ht->size == 0) return NULL;
h = dictHashKey(ht, key) & ht->sizemask;
he = ht->table[h];
while(he) {
if (dictCompareHashKeys(ht, key, he->key))
return he;
he = he->next;
}
return NULL;
}
static dictIterator *dictGetIterator(dict *ht) {
dictIterator *iter = hi_malloc(sizeof(*iter));
if (iter == NULL)
return NULL;
iter->ht = ht;
iter->index = -1;
iter->entry = NULL;
iter->nextEntry = NULL;
return iter;
}
static dictEntry *dictNext(dictIterator *iter) {
while (1) {
if (iter->entry == NULL) {
iter->index++;
if (iter->index >=
(signed)iter->ht->size) break;
iter->entry = iter->ht->table[iter->index];
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
static void dictReleaseIterator(dictIterator *iter) {
hi_free(iter);
}
static int _dictExpandIfNeeded(dict *ht) {
if (ht->size == 0)
return dictExpand(ht, DICT_HT_INITIAL_SIZE);
if (ht->used == ht->size)
return dictExpand(ht, ht->size*2);
return DICT_OK;
}
static unsigned long _dictNextPower(unsigned long size) {
unsigned long i = DICT_HT_INITIAL_SIZE;
if (size >= LONG_MAX) return LONG_MAX;
while(1) {
if (i >= size)
return i;
i *= 2;
}
}
static int _dictKeyIndex(dict *ht, const void *key) {
unsigned int h;
dictEntry *he;
if (_dictExpandIfNeeded(ht) == DICT_ERR)
return -1;
h = dictHashKey(ht, key) & ht->sizemask;
he = ht->table[h];
while(he) {
if (dictCompareHashKeys(ht, key, he->key))
return -1;
he = he->next;
}
return h;
}
