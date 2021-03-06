











































































#if !defined(MAKE_HASH_H)
#define MAKE_HASH_H


typedef struct HashEntry {
struct HashEntry *next;

void *value;
unsigned int key_hash;
char key[1];
} HashEntry;


typedef struct HashTable {
HashEntry **buckets;

unsigned int bucketsSize;
unsigned int numEntries;
unsigned int bucketsMask;
unsigned int maxchain;
} HashTable;


typedef struct HashIter {
HashTable *table;
unsigned int nextBucket;
HashEntry *entry;
} HashIter;


typedef struct HashSet {
HashTable tbl;
} HashSet;

MAKE_INLINE void *
HashEntry_Get(HashEntry *h)
{
return h->value;
}

MAKE_INLINE void
HashEntry_Set(HashEntry *h, void *datum)
{
h->value = datum;
}


MAKE_INLINE void
HashIter_Init(HashIter *hi, HashTable *t)
{
hi->table = t;
hi->nextBucket = 0;
hi->entry = NULL;
}

void HashTable_Init(HashTable *);
void HashTable_Done(HashTable *);
HashEntry *HashTable_FindEntry(HashTable *, const char *);
void *HashTable_FindValue(HashTable *, const char *);
unsigned int Hash_Substring(Substring);
void *HashTable_FindValueBySubstringHash(HashTable *, Substring, unsigned int);
HashEntry *HashTable_CreateEntry(HashTable *, const char *, bool *);
HashEntry *HashTable_Set(HashTable *, const char *, void *);
void HashTable_DeleteEntry(HashTable *, HashEntry *);
void HashTable_DebugStats(HashTable *, const char *);

void HashIter_Init(HashIter *, HashTable *);
HashEntry *HashIter_Next(HashIter *);

MAKE_INLINE void
HashSet_Init(HashSet *set)
{
HashTable_Init(&set->tbl);
}

MAKE_INLINE void
HashSet_Done(HashSet *set)
{
HashTable_Done(&set->tbl);
}

MAKE_INLINE bool
HashSet_Add(HashSet *set, const char *key)
{
bool isNew;

(void)HashTable_CreateEntry(&set->tbl, key, &isNew);
return isNew;
}

MAKE_INLINE bool
HashSet_Contains(HashSet *set, const char *key)
{
return HashTable_FindEntry(&set->tbl, key) != NULL;
}

MAKE_INLINE void
HashIter_InitSet(HashIter *hi, HashSet *set)
{
HashIter_Init(hi, &set->tbl);
}

#endif
