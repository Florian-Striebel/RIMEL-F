#define ZIPLIST_HEAD 0
#define ZIPLIST_TAIL 1
typedef struct {
unsigned char *sval;
unsigned int slen;
long long lval;
} ziplistEntry;
unsigned char *ziplistNew(void);
unsigned char *ziplistMerge(unsigned char **first, unsigned char **second);
unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where);
unsigned char *ziplistIndex(unsigned char *zl, int index);
unsigned char *ziplistNext(unsigned char *zl, unsigned char *p);
unsigned char *ziplistPrev(unsigned char *zl, unsigned char *p);
unsigned int ziplistGet(unsigned char *p, unsigned char **sval, unsigned int *slen, long long *lval);
unsigned char *ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
unsigned char *ziplistDelete(unsigned char *zl, unsigned char **p);
unsigned char *ziplistDeleteRange(unsigned char *zl, int index, unsigned int num);
unsigned char *ziplistReplace(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
unsigned int ziplistCompare(unsigned char *p, unsigned char *s, unsigned int slen);
unsigned char *ziplistFind(unsigned char *zl, unsigned char *p, unsigned char *vstr, unsigned int vlen, unsigned int skip);
unsigned int ziplistLen(unsigned char *zl);
size_t ziplistBlobLen(unsigned char *zl);
void ziplistRepr(unsigned char *zl);
typedef int (*ziplistValidateEntryCB)(unsigned char* p, unsigned int head_count, void* userdata);
int ziplistValidateIntegrity(unsigned char *zl, size_t size, int deep,
ziplistValidateEntryCB entry_cb, void *cb_userdata);
void ziplistRandomPair(unsigned char *zl, unsigned long total_count, ziplistEntry *key, ziplistEntry *val);
void ziplistRandomPairs(unsigned char *zl, unsigned int count, ziplistEntry *keys, ziplistEntry *vals);
unsigned int ziplistRandomPairsUnique(unsigned char *zl, unsigned int count, ziplistEntry *keys, ziplistEntry *vals);
int ziplistSafeToAdd(unsigned char* zl, size_t add);
#if defined(REDIS_TEST)
int ziplistTest(int argc, char *argv[], int flags);
#endif
