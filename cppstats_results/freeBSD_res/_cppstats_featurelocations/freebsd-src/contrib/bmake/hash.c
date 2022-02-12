








































































#include "make.h"


MAKE_RCSID("$NetBSD: hash.c,v 1.66 2021/12/07 21:58:01 rillig Exp $");





#define rebuildLimit 3


static unsigned int
Hash_String(const char *key, size_t *out_keylen)
{
unsigned int h;
const char *p;

h = 0;
for (p = key; *p != '\0'; p++)
h = 31 * h + (unsigned char)*p;

if (out_keylen != NULL)
*out_keylen = (size_t)(p - key);
return h;
}


unsigned int
Hash_Substring(Substring key)
{
unsigned int h;
const char *p;

h = 0;
for (p = key.start; p != key.end; p++)
h = 31 * h + (unsigned char)*p;
return h;
}

static HashEntry *
HashTable_Find(HashTable *t, unsigned int h, const char *key)
{
HashEntry *e;
unsigned int chainlen = 0;

#if defined(DEBUG_HASH_LOOKUP)
DEBUG4(HASH, "%s: %p h=%08x key=%s\n", __func__, t, h, key);
#endif

for (e = t->buckets[h & t->bucketsMask]; e != NULL; e = e->next) {
chainlen++;
if (e->key_hash == h && strcmp(e->key, key) == 0)
break;
}

if (chainlen > t->maxchain)
t->maxchain = chainlen;

return e;
}

static bool
HashEntry_KeyEquals(const HashEntry *he, Substring key)
{
const char *heKey, *p;

heKey = he->key;
for (p = key.start; p != key.end; p++, heKey++)
if (*p != *heKey || *heKey == '\0')
return false;
return *heKey == '\0';
}

static HashEntry *
HashTable_FindEntryBySubstring(HashTable *t, Substring key, unsigned int h)
{
HashEntry *e;
unsigned int chainlen = 0;

#if defined(DEBUG_HASH_LOOKUP)
DEBUG5(HASH, "%s: %p h=%08x key=%.*s\n", __func__, t, h,
(int)Substring_Length(key), key.start);
#endif

for (e = t->buckets[h & t->bucketsMask]; e != NULL; e = e->next) {
chainlen++;
if (e->key_hash == h && HashEntry_KeyEquals(e, key))
break;
}

if (chainlen > t->maxchain)
t->maxchain = chainlen;

return e;
}


void
HashTable_Init(HashTable *t)
{
unsigned int n = 16, i;
HashEntry **buckets = bmake_malloc(sizeof *buckets * n);
for (i = 0; i < n; i++)
buckets[i] = NULL;

t->buckets = buckets;
t->bucketsSize = n;
t->numEntries = 0;
t->bucketsMask = n - 1;
t->maxchain = 0;
}





void
HashTable_Done(HashTable *t)
{
HashEntry **buckets = t->buckets;
size_t i, n = t->bucketsSize;

for (i = 0; i < n; i++) {
HashEntry *he = buckets[i];
while (he != NULL) {
HashEntry *next = he->next;
free(he);
he = next;
}
}

free(t->buckets);
#if defined(CLEANUP)
t->buckets = NULL;
#endif
}


HashEntry *
HashTable_FindEntry(HashTable *t, const char *key)
{
unsigned int h = Hash_String(key, NULL);
return HashTable_Find(t, h, key);
}


void *
HashTable_FindValue(HashTable *t, const char *key)
{
HashEntry *he = HashTable_FindEntry(t, key);
return he != NULL ? he->value : NULL;
}





void *
HashTable_FindValueBySubstringHash(HashTable *t, Substring key, unsigned int h)
{
HashEntry *he = HashTable_FindEntryBySubstring(t, key, h);
return he != NULL ? he->value : NULL;
}





static void
HashTable_Enlarge(HashTable *t)
{
unsigned int oldSize = t->bucketsSize;
HashEntry **oldBuckets = t->buckets;
unsigned int newSize = 2 * oldSize;
unsigned int newMask = newSize - 1;
HashEntry **newBuckets = bmake_malloc(sizeof *newBuckets * newSize);
size_t i;

for (i = 0; i < newSize; i++)
newBuckets[i] = NULL;

for (i = 0; i < oldSize; i++) {
HashEntry *he = oldBuckets[i];
while (he != NULL) {
HashEntry *next = he->next;
he->next = newBuckets[he->key_hash & newMask];
newBuckets[he->key_hash & newMask] = he;
he = next;
}
}

free(oldBuckets);

t->bucketsSize = newSize;
t->bucketsMask = newMask;
t->buckets = newBuckets;
DEBUG5(HASH, "%s: %p size=%d entries=%d maxchain=%d\n",
__func__, (void *)t, t->bucketsSize, t->numEntries, t->maxchain);
t->maxchain = 0;
}





HashEntry *
HashTable_CreateEntry(HashTable *t, const char *key, bool *out_isNew)
{
size_t keylen;
unsigned int h = Hash_String(key, &keylen);
HashEntry *he = HashTable_Find(t, h, key);

if (he != NULL) {
if (out_isNew != NULL)
*out_isNew = false;
return he;
}

if (t->numEntries >= rebuildLimit * t->bucketsSize)
HashTable_Enlarge(t);

he = bmake_malloc(sizeof *he + keylen);
he->value = NULL;
he->key_hash = h;
memcpy(he->key, key, keylen + 1);

he->next = t->buckets[h & t->bucketsMask];
t->buckets[h & t->bucketsMask] = he;
t->numEntries++;

if (out_isNew != NULL)
*out_isNew = true;
return he;
}

HashEntry *
HashTable_Set(HashTable *t, const char *key, void *value)
{
HashEntry *he = HashTable_CreateEntry(t, key, NULL);
HashEntry_Set(he, value);
return he;
}


void
HashTable_DeleteEntry(HashTable *t, HashEntry *he)
{
HashEntry **ref = &t->buckets[he->key_hash & t->bucketsMask];
HashEntry *p;

for (; (p = *ref) != NULL; ref = &p->next) {
if (p == he) {
*ref = p->next;
free(p);
t->numEntries--;
return;
}
}
abort();
}





HashEntry *
HashIter_Next(HashIter *hi)
{
HashTable *t = hi->table;
HashEntry *he = hi->entry;
HashEntry **buckets = t->buckets;
unsigned int bucketsSize = t->bucketsSize;

if (he != NULL)
he = he->next;

while (he == NULL) {
if (hi->nextBucket >= bucketsSize)
return NULL;
he = buckets[hi->nextBucket++];
}
hi->entry = he;
return he;
}

void
HashTable_DebugStats(HashTable *t, const char *name)
{
DEBUG4(HASH, "HashTable %s: size=%u numEntries=%u maxchain=%u\n",
name, t->bucketsSize, t->numEntries, t->maxchain);
}
