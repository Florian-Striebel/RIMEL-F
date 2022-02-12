





























#include <stdint.h>

#if !defined(__QUICKLIST_H__)
#define __QUICKLIST_H__











typedef struct quicklistNode {
struct quicklistNode *prev;
struct quicklistNode *next;
unsigned char *entry;
size_t sz;
unsigned int count : 16;
unsigned int encoding : 2;
unsigned int container : 2;
unsigned int recompress : 1;
unsigned int attempted_compress : 1;
unsigned int extra : 10;
} quicklistNode;






typedef struct quicklistLZF {
size_t sz;
char compressed[];
} quicklistLZF;









typedef struct quicklistBookmark {
quicklistNode *node;
char *name;
} quicklistBookmark;

#if UINTPTR_MAX == 0xffffffff

#define QL_FILL_BITS 14
#define QL_COMP_BITS 14
#define QL_BM_BITS 4
#elif UINTPTR_MAX == 0xffffffffffffffff

#define QL_FILL_BITS 16
#define QL_COMP_BITS 16
#define QL_BM_BITS 4

#else
#error unknown arch bits count
#endif









typedef struct quicklist {
quicklistNode *head;
quicklistNode *tail;
unsigned long count;
unsigned long len;
signed int fill : QL_FILL_BITS;
unsigned int compress : QL_COMP_BITS;
unsigned int bookmark_count: QL_BM_BITS;
quicklistBookmark bookmarks[];
} quicklist;

typedef struct quicklistIter {
quicklist *quicklist;
quicklistNode *current;
unsigned char *zi;
long offset;
int direction;
} quicklistIter;

typedef struct quicklistEntry {
const quicklist *quicklist;
quicklistNode *node;
unsigned char *zi;
unsigned char *value;
long long longval;
size_t sz;
int offset;
} quicklistEntry;

#define QUICKLIST_HEAD 0
#define QUICKLIST_TAIL -1


#define QUICKLIST_NODE_ENCODING_RAW 1
#define QUICKLIST_NODE_ENCODING_LZF 2


#define QUICKLIST_NOCOMPRESS 0


#define QUICKLIST_NODE_CONTAINER_PLAIN 1
#define QUICKLIST_NODE_CONTAINER_PACKED 2

#define QL_NODE_IS_PLAIN(node) ((node)->container == QUICKLIST_NODE_CONTAINER_PLAIN)

#define quicklistNodeIsCompressed(node) ((node)->encoding == QUICKLIST_NODE_ENCODING_LZF)



quicklist *quicklistCreate(void);
quicklist *quicklistNew(int fill, int compress);
void quicklistSetCompressDepth(quicklist *quicklist, int depth);
void quicklistSetFill(quicklist *quicklist, int fill);
void quicklistSetOptions(quicklist *quicklist, int fill, int depth);
void quicklistRelease(quicklist *quicklist);
int quicklistPushHead(quicklist *quicklist, void *value, const size_t sz);
int quicklistPushTail(quicklist *quicklist, void *value, const size_t sz);
void quicklistPush(quicklist *quicklist, void *value, const size_t sz,
int where);
void quicklistAppendListpack(quicklist *quicklist, unsigned char *zl);
void quicklistAppendPlainNode(quicklist *quicklist, unsigned char *data, size_t sz);
void quicklistInsertAfter(quicklistIter *iter, quicklistEntry *entry,
void *value, const size_t sz);
void quicklistInsertBefore(quicklistIter *iter, quicklistEntry *entry,
void *value, const size_t sz);
void quicklistDelEntry(quicklistIter *iter, quicklistEntry *entry);
void quicklistReplaceEntry(quicklistIter *iter, quicklistEntry *entry,
void *data, size_t sz);
int quicklistReplaceAtIndex(quicklist *quicklist, long index, void *data,
const size_t sz);
int quicklistDelRange(quicklist *quicklist, const long start, const long stop);
quicklistIter *quicklistGetIterator(quicklist *quicklist, int direction);
quicklistIter *quicklistGetIteratorAtIdx(quicklist *quicklist,
int direction, const long long idx);
quicklistIter *quicklistGetIteratorEntryAtIdx(quicklist *quicklist, const long long index,
quicklistEntry *entry);
int quicklistNext(quicklistIter *iter, quicklistEntry *entry);
void quicklistSetDirection(quicklistIter *iter, int direction);
void quicklistReleaseIterator(quicklistIter *iter);
quicklist *quicklistDup(quicklist *orig);
void quicklistRotate(quicklist *quicklist);
int quicklistPopCustom(quicklist *quicklist, int where, unsigned char **data,
size_t *sz, long long *sval,
void *(*saver)(unsigned char *data, size_t sz));
int quicklistPop(quicklist *quicklist, int where, unsigned char **data,
size_t *sz, long long *slong);
unsigned long quicklistCount(const quicklist *ql);
int quicklistCompare(quicklistEntry *entry, unsigned char *p2, const size_t p2_len);
size_t quicklistGetLzf(const quicklistNode *node, void **data);
void quicklistRepr(unsigned char *ql, int full);


int quicklistBookmarkCreate(quicklist **ql_ref, const char *name, quicklistNode *node);
int quicklistBookmarkDelete(quicklist *ql, const char *name);
quicklistNode *quicklistBookmarkFind(quicklist *ql, const char *name);
void quicklistBookmarksClear(quicklist *ql);
int quicklistisSetPackedThreshold(size_t sz);

#if defined(REDIS_TEST)
int quicklistTest(int argc, char *argv[], int flags);
#endif


#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif
