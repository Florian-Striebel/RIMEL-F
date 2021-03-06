#include <stdio.h>
#include <string.h>
#include "quicklist.h"
#include "zmalloc.h"
#include "config.h"
#include "listpack.h"
#include "util.h"
#include "lzf.h"
#include "redisassert.h"
#if !defined(REDIS_STATIC)
#define REDIS_STATIC static
#endif
static const size_t optimization_level[] = {4096, 8192, 16384, 32768, 65536};
static size_t packed_threshold = (1 << 30);
#define isLargeElement(size) ((size) >= packed_threshold)
int quicklistisSetPackedThreshold(size_t sz) {
if (sz > (1ull<<32) - (1<<20)) {
return 0;
}
packed_threshold = sz;
return 1;
}
#define SIZE_SAFETY_LIMIT 8192
#define SIZE_ESTIMATE_OVERHEAD 8
#define MIN_COMPRESS_BYTES 48
#define MIN_COMPRESS_IMPROVE 8
#if !defined(REDIS_TEST_VERBOSE)
#define D(...)
#else
#define D(...) do { printf("%s:%s:%d:\t", __FILE__, __func__, __LINE__); printf(__VA_ARGS__); printf("\n"); } while (0)
#endif
#define QL_MAX_BM ((1 << QL_BM_BITS)-1)
quicklistBookmark *_quicklistBookmarkFindByName(quicklist *ql, const char *name);
quicklistBookmark *_quicklistBookmarkFindByNode(quicklist *ql, quicklistNode *node);
void _quicklistBookmarkDelete(quicklist *ql, quicklistBookmark *bm);
#define initEntry(e) do { (e)->zi = (e)->value = NULL; (e)->longval = -123456789; (e)->quicklist = NULL; (e)->node = NULL; (e)->offset = 123456789; (e)->sz = 0; } while (0)
#define resetIterator(iter) do { (iter)->current = NULL; (iter)->zi = NULL; } while (0)
quicklist *quicklistCreate(void) {
struct quicklist *quicklist;
quicklist = zmalloc(sizeof(*quicklist));
quicklist->head = quicklist->tail = NULL;
quicklist->len = 0;
quicklist->count = 0;
quicklist->compress = 0;
quicklist->fill = -2;
quicklist->bookmark_count = 0;
return quicklist;
}
#define COMPRESS_MAX ((1 << QL_COMP_BITS)-1)
void quicklistSetCompressDepth(quicklist *quicklist, int compress) {
if (compress > COMPRESS_MAX) {
compress = COMPRESS_MAX;
} else if (compress < 0) {
compress = 0;
}
quicklist->compress = compress;
}
#define FILL_MAX ((1 << (QL_FILL_BITS-1))-1)
void quicklistSetFill(quicklist *quicklist, int fill) {
if (fill > FILL_MAX) {
fill = FILL_MAX;
} else if (fill < -5) {
fill = -5;
}
quicklist->fill = fill;
}
void quicklistSetOptions(quicklist *quicklist, int fill, int depth) {
quicklistSetFill(quicklist, fill);
quicklistSetCompressDepth(quicklist, depth);
}
quicklist *quicklistNew(int fill, int compress) {
quicklist *quicklist = quicklistCreate();
quicklistSetOptions(quicklist, fill, compress);
return quicklist;
}
REDIS_STATIC quicklistNode *quicklistCreateNode(void) {
quicklistNode *node;
node = zmalloc(sizeof(*node));
node->entry = NULL;
node->count = 0;
node->sz = 0;
node->next = node->prev = NULL;
node->encoding = QUICKLIST_NODE_ENCODING_RAW;
node->container = QUICKLIST_NODE_CONTAINER_PACKED;
node->recompress = 0;
return node;
}
unsigned long quicklistCount(const quicklist *ql) { return ql->count; }
void quicklistRelease(quicklist *quicklist) {
unsigned long len;
quicklistNode *current, *next;
current = quicklist->head;
len = quicklist->len;
while (len--) {
next = current->next;
zfree(current->entry);
quicklist->count -= current->count;
zfree(current);
quicklist->len--;
current = next;
}
quicklistBookmarksClear(quicklist);
zfree(quicklist);
}
REDIS_STATIC int __quicklistCompressNode(quicklistNode *node) {
#if defined(REDIS_TEST)
node->attempted_compress = 1;
#endif
assert(node->prev && node->next);
node->recompress = 0;
if (node->sz < MIN_COMPRESS_BYTES)
return 0;
quicklistLZF *lzf = zmalloc(sizeof(*lzf) + node->sz);
if (((lzf->sz = lzf_compress(node->entry, node->sz, lzf->compressed,
node->sz)) == 0) ||
lzf->sz + MIN_COMPRESS_IMPROVE >= node->sz) {
zfree(lzf);
return 0;
}
lzf = zrealloc(lzf, sizeof(*lzf) + lzf->sz);
zfree(node->entry);
node->entry = (unsigned char *)lzf;
node->encoding = QUICKLIST_NODE_ENCODING_LZF;
return 1;
}
#define quicklistCompressNode(_node) do { if ((_node) && (_node)->encoding == QUICKLIST_NODE_ENCODING_RAW) { __quicklistCompressNode((_node)); } } while (0)
REDIS_STATIC int __quicklistDecompressNode(quicklistNode *node) {
#if defined(REDIS_TEST)
node->attempted_compress = 0;
#endif
node->recompress = 0;
void *decompressed = zmalloc(node->sz);
quicklistLZF *lzf = (quicklistLZF *)node->entry;
if (lzf_decompress(lzf->compressed, lzf->sz, decompressed, node->sz) == 0) {
zfree(decompressed);
return 0;
}
zfree(lzf);
node->entry = decompressed;
node->encoding = QUICKLIST_NODE_ENCODING_RAW;
return 1;
}
#define quicklistDecompressNode(_node) do { if ((_node) && (_node)->encoding == QUICKLIST_NODE_ENCODING_LZF) { __quicklistDecompressNode((_node)); } } while (0)
#define quicklistDecompressNodeForUse(_node) do { if ((_node) && (_node)->encoding == QUICKLIST_NODE_ENCODING_LZF) { __quicklistDecompressNode((_node)); (_node)->recompress = 1; } } while (0)
size_t quicklistGetLzf(const quicklistNode *node, void **data) {
quicklistLZF *lzf = (quicklistLZF *)node->entry;
*data = lzf->compressed;
return lzf->sz;
}
#define quicklistAllowsCompression(_ql) ((_ql)->compress != 0)
REDIS_STATIC void __quicklistCompress(const quicklist *quicklist,
quicklistNode *node) {
if (quicklist->len == 0) return;
assert(quicklist->head->recompress == 0 && quicklist->tail->recompress == 0);
if (!quicklistAllowsCompression(quicklist) ||
quicklist->len < (unsigned int)(quicklist->compress * 2))
return;
#if 0
if (quicklist->compress == 1) {
quicklistNode *h = quicklist->head, *t = quicklist->tail;
quicklistDecompressNode(h);
quicklistDecompressNode(t);
if (h != node && t != node)
quicklistCompressNode(node);
return;
} else if (quicklist->compress == 2) {
quicklistNode *h = quicklist->head, *hn = h->next, *hnn = hn->next;
quicklistNode *t = quicklist->tail, *tp = t->prev, *tpp = tp->prev;
quicklistDecompressNode(h);
quicklistDecompressNode(hn);
quicklistDecompressNode(t);
quicklistDecompressNode(tp);
if (h != node && hn != node && t != node && tp != node) {
quicklistCompressNode(node);
}
if (hnn != t) {
quicklistCompressNode(hnn);
}
if (tpp != h) {
quicklistCompressNode(tpp);
}
return;
}
#endif
quicklistNode *forward = quicklist->head;
quicklistNode *reverse = quicklist->tail;
int depth = 0;
int in_depth = 0;
while (depth++ < quicklist->compress) {
quicklistDecompressNode(forward);
quicklistDecompressNode(reverse);
if (forward == node || reverse == node)
in_depth = 1;
if (forward == reverse || forward->next == reverse)
return;
forward = forward->next;
reverse = reverse->prev;
}
if (!in_depth)
quicklistCompressNode(node);
quicklistCompressNode(forward);
quicklistCompressNode(reverse);
}
#define quicklistCompress(_ql, _node) do { if ((_node)->recompress) quicklistCompressNode((_node)); else __quicklistCompress((_ql), (_node)); } while (0)
#define quicklistRecompressOnly(_node) do { if ((_node)->recompress) quicklistCompressNode((_node)); } while (0)
REDIS_STATIC void __quicklistInsertNode(quicklist *quicklist,
quicklistNode *old_node,
quicklistNode *new_node, int after) {
if (after) {
new_node->prev = old_node;
if (old_node) {
new_node->next = old_node->next;
if (old_node->next)
old_node->next->prev = new_node;
old_node->next = new_node;
}
if (quicklist->tail == old_node)
quicklist->tail = new_node;
} else {
new_node->next = old_node;
if (old_node) {
new_node->prev = old_node->prev;
if (old_node->prev)
old_node->prev->next = new_node;
old_node->prev = new_node;
}
if (quicklist->head == old_node)
quicklist->head = new_node;
}
if (quicklist->len == 0) {
quicklist->head = quicklist->tail = new_node;
}
quicklist->len++;
if (old_node)
quicklistCompress(quicklist, old_node);
quicklistCompress(quicklist, new_node);
}
REDIS_STATIC void _quicklistInsertNodeBefore(quicklist *quicklist,
quicklistNode *old_node,
quicklistNode *new_node) {
__quicklistInsertNode(quicklist, old_node, new_node, 0);
}
REDIS_STATIC void _quicklistInsertNodeAfter(quicklist *quicklist,
quicklistNode *old_node,
quicklistNode *new_node) {
__quicklistInsertNode(quicklist, old_node, new_node, 1);
}
REDIS_STATIC int
_quicklistNodeSizeMeetsOptimizationRequirement(const size_t sz,
const int fill) {
if (fill >= 0)
return 0;
size_t offset = (-fill) - 1;
if (offset < (sizeof(optimization_level) / sizeof(*optimization_level))) {
if (sz <= optimization_level[offset]) {
return 1;
} else {
return 0;
}
} else {
return 0;
}
}
#define sizeMeetsSafetyLimit(sz) ((sz) <= SIZE_SAFETY_LIMIT)
REDIS_STATIC int _quicklistNodeAllowInsert(const quicklistNode *node,
const int fill, const size_t sz) {
if (unlikely(!node))
return 0;
if (unlikely(QL_NODE_IS_PLAIN(node) || isLargeElement(sz)))
return 0;
size_t new_sz = node->sz + sz + SIZE_ESTIMATE_OVERHEAD;
if (likely(_quicklistNodeSizeMeetsOptimizationRequirement(new_sz, fill)))
return 1;
else if (!sizeMeetsSafetyLimit(new_sz))
return 0;
else if ((int)node->count < fill)
return 1;
else
return 0;
}
REDIS_STATIC int _quicklistNodeAllowMerge(const quicklistNode *a,
const quicklistNode *b,
const int fill) {
if (!a || !b)
return 0;
if (unlikely(QL_NODE_IS_PLAIN(a) || QL_NODE_IS_PLAIN(b)))
return 0;
unsigned int merge_sz = a->sz + b->sz - 11;
if (likely(_quicklistNodeSizeMeetsOptimizationRequirement(merge_sz, fill)))
return 1;
else if (!sizeMeetsSafetyLimit(merge_sz))
return 0;
else if ((int)(a->count + b->count) <= fill)
return 1;
else
return 0;
}
#define quicklistNodeUpdateSz(node) do { (node)->sz = lpBytes((node)->entry); } while (0)
static quicklistNode* __quicklistCreatePlainNode(void *value, size_t sz) {
quicklistNode *new_node = quicklistCreateNode();
new_node->entry = zmalloc(sz);
new_node->container = QUICKLIST_NODE_CONTAINER_PLAIN;
memcpy(new_node->entry, value, sz);
new_node->sz = sz;
new_node->count++;
return new_node;
}
static void __quicklistInsertPlainNode(quicklist *quicklist, quicklistNode *old_node,
void *value, size_t sz, int after) {
__quicklistInsertNode(quicklist, old_node, __quicklistCreatePlainNode(value, sz), after);
quicklist->count++;
}
int quicklistPushHead(quicklist *quicklist, void *value, size_t sz) {
quicklistNode *orig_head = quicklist->head;
if (unlikely(isLargeElement(sz))) {
__quicklistInsertPlainNode(quicklist, quicklist->head, value, sz, 0);
return 1;
}
if (likely(
_quicklistNodeAllowInsert(quicklist->head, quicklist->fill, sz))) {
quicklist->head->entry = lpPrepend(quicklist->head->entry, value, sz);
quicklistNodeUpdateSz(quicklist->head);
} else {
quicklistNode *node = quicklistCreateNode();
node->entry = lpPrepend(lpNew(0), value, sz);
quicklistNodeUpdateSz(node);
_quicklistInsertNodeBefore(quicklist, quicklist->head, node);
}
quicklist->count++;
quicklist->head->count++;
return (orig_head != quicklist->head);
}
int quicklistPushTail(quicklist *quicklist, void *value, size_t sz) {
quicklistNode *orig_tail = quicklist->tail;
if (unlikely(isLargeElement(sz))) {
__quicklistInsertPlainNode(quicklist, quicklist->tail, value, sz, 1);
return 1;
}
if (likely(
_quicklistNodeAllowInsert(quicklist->tail, quicklist->fill, sz))) {
quicklist->tail->entry = lpAppend(quicklist->tail->entry, value, sz);
quicklistNodeUpdateSz(quicklist->tail);
} else {
quicklistNode *node = quicklistCreateNode();
node->entry = lpAppend(lpNew(0), value, sz);
quicklistNodeUpdateSz(node);
_quicklistInsertNodeAfter(quicklist, quicklist->tail, node);
}
quicklist->count++;
quicklist->tail->count++;
return (orig_tail != quicklist->tail);
}
void quicklistAppendListpack(quicklist *quicklist, unsigned char *zl) {
quicklistNode *node = quicklistCreateNode();
node->entry = zl;
node->count = lpLength(node->entry);
node->sz = lpBytes(zl);
_quicklistInsertNodeAfter(quicklist, quicklist->tail, node);
quicklist->count += node->count;
}
void quicklistAppendPlainNode(quicklist *quicklist, unsigned char *data, size_t sz) {
quicklistNode *node = quicklistCreateNode();
node->entry = data;
node->count = 1;
node->sz = sz;
node->container = QUICKLIST_NODE_CONTAINER_PLAIN;
_quicklistInsertNodeAfter(quicklist, quicklist->tail, node);
quicklist->count += node->count;
}
#define quicklistDeleteIfEmpty(ql, n) do { if ((n)->count == 0) { __quicklistDelNode((ql), (n)); (n) = NULL; } } while (0)
REDIS_STATIC void __quicklistDelNode(quicklist *quicklist,
quicklistNode *node) {
quicklistBookmark *bm = _quicklistBookmarkFindByNode(quicklist, node);
if (bm) {
bm->node = node->next;
if (!bm->node)
_quicklistBookmarkDelete(quicklist, bm);
}
if (node->next)
node->next->prev = node->prev;
if (node->prev)
node->prev->next = node->next;
if (node == quicklist->tail) {
quicklist->tail = node->prev;
}
if (node == quicklist->head) {
quicklist->head = node->next;
}
quicklist->len--;
quicklist->count -= node->count;
__quicklistCompress(quicklist, NULL);
zfree(node->entry);
zfree(node);
}
REDIS_STATIC int quicklistDelIndex(quicklist *quicklist, quicklistNode *node,
unsigned char **p) {
int gone = 0;
if (unlikely(QL_NODE_IS_PLAIN(node))) {
__quicklistDelNode(quicklist, node);
return 1;
}
node->entry = lpDelete(node->entry, *p, p);
node->count--;
if (node->count == 0) {
gone = 1;
__quicklistDelNode(quicklist, node);
} else {
quicklistNodeUpdateSz(node);
}
quicklist->count--;
return gone ? 1 : 0;
}
void quicklistDelEntry(quicklistIter *iter, quicklistEntry *entry) {
quicklistNode *prev = entry->node->prev;
quicklistNode *next = entry->node->next;
int deleted_node = quicklistDelIndex((quicklist *)entry->quicklist,
entry->node, &entry->zi);
iter->zi = NULL;
if (deleted_node) {
if (iter->direction == AL_START_HEAD) {
iter->current = next;
iter->offset = 0;
} else if (iter->direction == AL_START_TAIL) {
iter->current = prev;
iter->offset = -1;
}
}
}
void quicklistReplaceEntry(quicklistIter *iter, quicklistEntry *entry,
void *data, size_t sz)
{
quicklist* quicklist = iter->quicklist;
if (likely(!QL_NODE_IS_PLAIN(entry->node) && !isLargeElement(sz))) {
entry->node->entry = lpReplace(entry->node->entry, &entry->zi, data, sz);
quicklistNodeUpdateSz(entry->node);
quicklistCompress(quicklist, entry->node);
} else if (QL_NODE_IS_PLAIN(entry->node)) {
if (isLargeElement(sz)) {
zfree(entry->node->entry);
entry->node->entry = zmalloc(sz);
entry->node->sz = sz;
memcpy(entry->node->entry, data, sz);
quicklistCompress(quicklist, entry->node);
} else {
quicklistInsertAfter(iter, entry, data, sz);
__quicklistDelNode(quicklist, entry->node);
}
} else {
quicklistInsertAfter(iter, entry, data, sz);
if (entry->node->count == 1) {
__quicklistDelNode(quicklist, entry->node);
} else {
unsigned char *p = lpSeek(entry->node->entry, -1);
quicklistDelIndex(quicklist, entry->node, &p);
quicklistCompress(quicklist, entry->node->next);
}
}
resetIterator(iter);
}
int quicklistReplaceAtIndex(quicklist *quicklist, long index, void *data,
size_t sz) {
quicklistEntry entry;
quicklistIter *iter = quicklistGetIteratorEntryAtIdx(quicklist, index, &entry);
if (likely(iter)) {
quicklistReplaceEntry(iter, &entry, data, sz);
quicklistReleaseIterator(iter);
return 1;
} else {
return 0;
}
}
REDIS_STATIC quicklistNode *_quicklistListpackMerge(quicklist *quicklist,
quicklistNode *a,
quicklistNode *b) {
D("Requested merge (a,b) (%u, %u)", a->count, b->count);
quicklistDecompressNode(a);
quicklistDecompressNode(b);
if ((lpMerge(&a->entry, &b->entry))) {
quicklistNode *keep = NULL, *nokeep = NULL;
if (!a->entry) {
nokeep = a;
keep = b;
} else if (!b->entry) {
nokeep = b;
keep = a;
}
keep->count = lpLength(keep->entry);
quicklistNodeUpdateSz(keep);
nokeep->count = 0;
__quicklistDelNode(quicklist, nokeep);
quicklistCompress(quicklist, keep);
return keep;
} else {
return NULL;
}
}
REDIS_STATIC void _quicklistMergeNodes(quicklist *quicklist,
quicklistNode *center) {
int fill = quicklist->fill;
quicklistNode *prev, *prev_prev, *next, *next_next, *target;
prev = prev_prev = next = next_next = target = NULL;
if (center->prev) {
prev = center->prev;
if (center->prev->prev)
prev_prev = center->prev->prev;
}
if (center->next) {
next = center->next;
if (center->next->next)
next_next = center->next->next;
}
if (_quicklistNodeAllowMerge(prev, prev_prev, fill)) {
_quicklistListpackMerge(quicklist, prev_prev, prev);
prev_prev = prev = NULL;
}
if (_quicklistNodeAllowMerge(next, next_next, fill)) {
_quicklistListpackMerge(quicklist, next, next_next);
next = next_next = NULL;
}
if (_quicklistNodeAllowMerge(center, center->prev, fill)) {
target = _quicklistListpackMerge(quicklist, center->prev, center);
center = NULL;
} else {
target = center;
}
if (_quicklistNodeAllowMerge(target, target->next, fill)) {
_quicklistListpackMerge(quicklist, target, target->next);
}
}
REDIS_STATIC quicklistNode *_quicklistSplitNode(quicklistNode *node, int offset,
int after) {
size_t zl_sz = node->sz;
quicklistNode *new_node = quicklistCreateNode();
new_node->entry = zmalloc(zl_sz);
memcpy(new_node->entry, node->entry, zl_sz);
int orig_start = after ? offset + 1 : 0;
int orig_extent = after ? -1 : offset;
int new_start = after ? 0 : offset;
int new_extent = after ? offset + 1 : -1;
D("After %d (%d); ranges: [%d, %d], [%d, %d]", after, offset, orig_start,
orig_extent, new_start, new_extent);
node->entry = lpDeleteRange(node->entry, orig_start, orig_extent);
node->count = lpLength(node->entry);
quicklistNodeUpdateSz(node);
new_node->entry = lpDeleteRange(new_node->entry, new_start, new_extent);
new_node->count = lpLength(new_node->entry);
quicklistNodeUpdateSz(new_node);
D("After split lengths: orig (%d), new (%d)", node->count, new_node->count);
return new_node;
}
REDIS_STATIC void _quicklistInsert(quicklistIter *iter, quicklistEntry *entry,
void *value, const size_t sz, int after)
{
quicklist *quicklist = iter->quicklist;
int full = 0, at_tail = 0, at_head = 0, avail_next = 0, avail_prev = 0;
int fill = quicklist->fill;
quicklistNode *node = entry->node;
quicklistNode *new_node = NULL;
if (!node) {
D("No node given!");
if (unlikely(isLargeElement(sz))) {
__quicklistInsertPlainNode(quicklist, quicklist->tail, value, sz, after);
return;
}
new_node = quicklistCreateNode();
new_node->entry = lpPrepend(lpNew(0), value, sz);
__quicklistInsertNode(quicklist, NULL, new_node, after);
new_node->count++;
quicklist->count++;
return;
}
if (!_quicklistNodeAllowInsert(node, fill, sz)) {
D("Current node is full with count %d with requested fill %d",
node->count, fill);
full = 1;
}
if (after && (entry->offset == node->count - 1 || entry->offset == -1)) {
D("At Tail of current listpack");
at_tail = 1;
if (_quicklistNodeAllowInsert(node->next, fill, sz)) {
D("Next node is available.");
avail_next = 1;
}
}
if (!after && (entry->offset == 0 || entry->offset == -(node->count))) {
D("At Head");
at_head = 1;
if (_quicklistNodeAllowInsert(node->prev, fill, sz)) {
D("Prev node is available.");
avail_prev = 1;
}
}
if (unlikely(isLargeElement(sz))) {
if (QL_NODE_IS_PLAIN(node) || (at_tail && after) || (at_head && !after)) {
__quicklistInsertPlainNode(quicklist, node, value, sz, after);
} else {
quicklistDecompressNodeForUse(node);
new_node = _quicklistSplitNode(node, entry->offset, after);
quicklistNode *entry_node = __quicklistCreatePlainNode(value, sz);
__quicklistInsertNode(quicklist, node, entry_node, after);
__quicklistInsertNode(quicklist, entry_node, new_node, after);
quicklist->count++;
}
return;
}
if (!full && after) {
D("Not full, inserting after current position.");
quicklistDecompressNodeForUse(node);
node->entry = lpInsertString(node->entry, value, sz, entry->zi, LP_AFTER, NULL);
node->count++;
quicklistNodeUpdateSz(node);
quicklistRecompressOnly(node);
} else if (!full && !after) {
D("Not full, inserting before current position.");
quicklistDecompressNodeForUse(node);
node->entry = lpInsertString(node->entry, value, sz, entry->zi, LP_BEFORE, NULL);
node->count++;
quicklistNodeUpdateSz(node);
quicklistRecompressOnly(node);
} else if (full && at_tail && avail_next && after) {
D("Full and tail, but next isn't full; inserting next node head");
new_node = node->next;
quicklistDecompressNodeForUse(new_node);
new_node->entry = lpPrepend(new_node->entry, value, sz);
new_node->count++;
quicklistNodeUpdateSz(new_node);
quicklistRecompressOnly(new_node);
quicklistRecompressOnly(node);
} else if (full && at_head && avail_prev && !after) {
D("Full and head, but prev isn't full, inserting prev node tail");
new_node = node->prev;
quicklistDecompressNodeForUse(new_node);
new_node->entry = lpAppend(new_node->entry, value, sz);
new_node->count++;
quicklistNodeUpdateSz(new_node);
quicklistRecompressOnly(new_node);
quicklistRecompressOnly(node);
} else if (full && ((at_tail && !avail_next && after) ||
(at_head && !avail_prev && !after))) {
D("\tprovisioning new node...");
new_node = quicklistCreateNode();
new_node->entry = lpPrepend(lpNew(0), value, sz);
new_node->count++;
quicklistNodeUpdateSz(new_node);
__quicklistInsertNode(quicklist, node, new_node, after);
} else if (full) {
D("\tsplitting node...");
quicklistDecompressNodeForUse(node);
new_node = _quicklistSplitNode(node, entry->offset, after);
if (after)
new_node->entry = lpPrepend(new_node->entry, value, sz);
else
new_node->entry = lpAppend(new_node->entry, value, sz);
new_node->count++;
quicklistNodeUpdateSz(new_node);
__quicklistInsertNode(quicklist, node, new_node, after);
_quicklistMergeNodes(quicklist, node);
}
quicklist->count++;
resetIterator(iter);
}
void quicklistInsertBefore(quicklistIter *iter, quicklistEntry *entry,
void *value, const size_t sz)
{
_quicklistInsert(iter, entry, value, sz, 0);
}
void quicklistInsertAfter(quicklistIter *iter, quicklistEntry *entry,
void *value, const size_t sz)
{
_quicklistInsert(iter, entry, value, sz, 1);
}
int quicklistDelRange(quicklist *quicklist, const long start,
const long count) {
if (count <= 0)
return 0;
unsigned long extent = count;
if (start >= 0 && extent > (quicklist->count - start)) {
extent = quicklist->count - start;
} else if (start < 0 && extent > (unsigned long)(-start)) {
extent = -start;
}
quicklistIter *iter = quicklistGetIteratorAtIdx(quicklist, AL_START_TAIL, start);
if (!iter)
return 0;
D("Quicklist delete request for start %ld, count %ld, extent: %ld", start,
count, extent);
quicklistNode *node = iter->current;
long offset = iter->offset;
quicklistReleaseIterator(iter);
while (extent) {
quicklistNode *next = node->next;
unsigned long del;
int delete_entire_node = 0;
if (offset == 0 && extent >= node->count) {
delete_entire_node = 1;
del = node->count;
} else if (offset >= 0 && extent + offset >= node->count) {
del = node->count - offset;
} else if (offset < 0) {
del = -offset;
if (del > extent)
del = extent;
} else {
del = extent;
}
D("[%ld]: asking to del: %ld because offset: %d; (ENTIRE NODE: %d), "
"node count: %u",
extent, del, offset, delete_entire_node, node->count);
if (delete_entire_node || QL_NODE_IS_PLAIN(node)) {
__quicklistDelNode(quicklist, node);
} else {
quicklistDecompressNodeForUse(node);
node->entry = lpDeleteRange(node->entry, offset, del);
quicklistNodeUpdateSz(node);
node->count -= del;
quicklist->count -= del;
quicklistDeleteIfEmpty(quicklist, node);
if (node)
quicklistRecompressOnly(node);
}
extent -= del;
node = next;
offset = 0;
}
return 1;
}
int quicklistCompare(quicklistEntry* entry, unsigned char *p2, const size_t p2_len) {
if (unlikely(QL_NODE_IS_PLAIN(entry->node))) {
return ((entry->sz == p2_len) && (memcmp(entry->value, p2, p2_len) == 0));
}
return lpCompare(entry->zi, p2, p2_len);
}
quicklistIter *quicklistGetIterator(quicklist *quicklist, int direction) {
quicklistIter *iter;
iter = zmalloc(sizeof(*iter));
if (direction == AL_START_HEAD) {
iter->current = quicklist->head;
iter->offset = 0;
} else if (direction == AL_START_TAIL) {
iter->current = quicklist->tail;
iter->offset = -1;
}
iter->direction = direction;
iter->quicklist = quicklist;
iter->zi = NULL;
return iter;
}
quicklistIter *quicklistGetIteratorAtIdx(quicklist *quicklist,
const int direction,
const long long idx)
{
quicklistNode *n;
unsigned long long accum = 0;
unsigned long long index;
int forward = idx < 0 ? 0 : 1;
index = forward ? idx : (-idx) - 1;
if (index >= quicklist->count)
return NULL;
int seek_forward = forward;
unsigned long long seek_index = index;
if (index > (quicklist->count - 1) / 2) {
seek_forward = !forward;
seek_index = quicklist->count - 1 - index;
}
n = seek_forward ? quicklist->head : quicklist->tail;
while (likely(n)) {
if ((accum + n->count) > seek_index) {
break;
} else {
D("Skipping over (%p) %u at accum %lld", (void *)n, n->count,
accum);
accum += n->count;
n = seek_forward ? n->next : n->prev;
}
}
if (!n)
return NULL;
if (seek_forward != forward) accum = quicklist->count - n->count - accum;
D("Found node: %p at accum %llu, idx %llu, sub+ %llu, sub- %llu", (void *)n,
accum, index, index - accum, (-index) - 1 + accum);
quicklistIter *iter = quicklistGetIterator(quicklist, direction);
iter->current = n;
if (forward) {
iter->offset = index - accum;
} else {
iter->offset = (-index) - 1 + accum;
}
return iter;
}
void quicklistReleaseIterator(quicklistIter *iter) {
if (!iter) return;
if (iter->current)
quicklistCompress(iter->quicklist, iter->current);
zfree(iter);
}
int quicklistNext(quicklistIter *iter, quicklistEntry *entry) {
initEntry(entry);
if (!iter) {
D("Returning because no iter!");
return 0;
}
entry->quicklist = iter->quicklist;
entry->node = iter->current;
if (!iter->current) {
D("Returning because current node is NULL");
return 0;
}
unsigned char *(*nextFn)(unsigned char *, unsigned char *) = NULL;
int offset_update = 0;
int plain = QL_NODE_IS_PLAIN(iter->current);
if (!iter->zi) {
quicklistDecompressNodeForUse(iter->current);
if (unlikely(plain))
iter->zi = iter->current->entry;
else
iter->zi = lpSeek(iter->current->entry, iter->offset);
} else if (unlikely(plain)) {
iter->zi = NULL;
} else {
if (iter->direction == AL_START_HEAD) {
nextFn = lpNext;
offset_update = 1;
} else if (iter->direction == AL_START_TAIL) {
nextFn = lpPrev;
offset_update = -1;
}
iter->zi = nextFn(iter->current->entry, iter->zi);
iter->offset += offset_update;
}
entry->zi = iter->zi;
entry->offset = iter->offset;
if (iter->zi) {
if (unlikely(plain)) {
entry->value = entry->node->entry;
entry->sz = entry->node->sz;
return 1;
}
unsigned int sz = 0;
entry->value = lpGetValue(entry->zi, &sz, &entry->longval);
entry->sz = sz;
return 1;
} else {
quicklistCompress(iter->quicklist, iter->current);
if (iter->direction == AL_START_HEAD) {
D("Jumping to start of next node");
iter->current = iter->current->next;
iter->offset = 0;
} else if (iter->direction == AL_START_TAIL) {
D("Jumping to end of previous node");
iter->current = iter->current->prev;
iter->offset = -1;
}
iter->zi = NULL;
return quicklistNext(iter, entry);
}
}
void quicklistSetDirection(quicklistIter *iter, int direction) {
iter->direction = direction;
}
quicklist *quicklistDup(quicklist *orig) {
quicklist *copy;
copy = quicklistNew(orig->fill, orig->compress);
for (quicklistNode *current = orig->head; current;
current = current->next) {
quicklistNode *node = quicklistCreateNode();
if (current->encoding == QUICKLIST_NODE_ENCODING_LZF) {
quicklistLZF *lzf = (quicklistLZF *)current->entry;
size_t lzf_sz = sizeof(*lzf) + lzf->sz;
node->entry = zmalloc(lzf_sz);
memcpy(node->entry, current->entry, lzf_sz);
} else if (current->encoding == QUICKLIST_NODE_ENCODING_RAW) {
node->entry = zmalloc(current->sz);
memcpy(node->entry, current->entry, current->sz);
}
node->count = current->count;
copy->count += node->count;
node->sz = current->sz;
node->encoding = current->encoding;
node->container = current->container;
_quicklistInsertNodeAfter(copy, copy->tail, node);
}
return copy;
}
quicklistIter *quicklistGetIteratorEntryAtIdx(quicklist *quicklist, const long long idx,
quicklistEntry *entry)
{
quicklistIter *iter = quicklistGetIteratorAtIdx(quicklist, AL_START_TAIL, idx);
if (!iter) return NULL;
assert(quicklistNext(iter, entry));
return iter;
}
static void quicklistRotatePlain(quicklist *quicklist) {
quicklistNode *new_head = quicklist->tail;
quicklistNode *new_tail = quicklist->tail->prev;
quicklist->head->prev = new_head;
new_tail->next = NULL;
new_head->next = quicklist->head;
new_head->prev = NULL;
quicklist->head = new_head;
quicklist->tail = new_tail;
}
void quicklistRotate(quicklist *quicklist) {
if (quicklist->count <= 1)
return;
if (unlikely(QL_NODE_IS_PLAIN(quicklist->tail))) {
quicklistRotatePlain(quicklist);
return;
}
unsigned char *p = lpSeek(quicklist->tail->entry, -1);
unsigned char *value, *tmp;
long long longval;
unsigned int sz;
char longstr[32] = {0};
tmp = lpGetValue(p, &sz, &longval);
if (!tmp) {
sz = ll2string(longstr, sizeof(longstr), longval);
value = (unsigned char *)longstr;
} else if (quicklist->len == 1) {
value = zmalloc(sz);
memcpy(value, tmp, sz);
} else {
value = tmp;
}
quicklistPushHead(quicklist, value, sz);
if (quicklist->len == 1) {
p = lpSeek(quicklist->tail->entry, -1);
}
quicklistDelIndex(quicklist, quicklist->tail, &p);
if (value != (unsigned char*)longstr && value != tmp)
zfree(value);
}
int quicklistPopCustom(quicklist *quicklist, int where, unsigned char **data,
size_t *sz, long long *sval,
void *(*saver)(unsigned char *data, size_t sz)) {
unsigned char *p;
unsigned char *vstr;
unsigned int vlen;
long long vlong;
int pos = (where == QUICKLIST_HEAD) ? 0 : -1;
if (quicklist->count == 0)
return 0;
if (data)
*data = NULL;
if (sz)
*sz = 0;
if (sval)
*sval = -123456789;
quicklistNode *node;
if (where == QUICKLIST_HEAD && quicklist->head) {
node = quicklist->head;
} else if (where == QUICKLIST_TAIL && quicklist->tail) {
node = quicklist->tail;
} else {
return 0;
}
assert(node->encoding != QUICKLIST_NODE_ENCODING_LZF);
if (unlikely(QL_NODE_IS_PLAIN(node))) {
if (data)
*data = saver(node->entry, node->sz);
if (sz)
*sz = node->sz;
quicklistDelIndex(quicklist, node, NULL);
return 1;
}
p = lpSeek(node->entry, pos);
vstr = lpGetValue(p, &vlen, &vlong);
if (vstr) {
if (data)
*data = saver(vstr, vlen);
if (sz)
*sz = vlen;
} else {
if (data)
*data = NULL;
if (sval)
*sval = vlong;
}
quicklistDelIndex(quicklist, node, &p);
return 1;
}
REDIS_STATIC void *_quicklistSaver(unsigned char *data, size_t sz) {
unsigned char *vstr;
if (data) {
vstr = zmalloc(sz);
memcpy(vstr, data, sz);
return vstr;
}
return NULL;
}
int quicklistPop(quicklist *quicklist, int where, unsigned char **data,
size_t *sz, long long *slong) {
unsigned char *vstr;
size_t vlen;
long long vlong;
if (quicklist->count == 0)
return 0;
int ret = quicklistPopCustom(quicklist, where, &vstr, &vlen, &vlong,
_quicklistSaver);
if (data)
*data = vstr;
if (slong)
*slong = vlong;
if (sz)
*sz = vlen;
return ret;
}
void quicklistPush(quicklist *quicklist, void *value, const size_t sz,
int where) {
if (quicklist->head)
assert(quicklist->head->encoding != QUICKLIST_NODE_ENCODING_LZF);
if (quicklist->tail)
assert(quicklist->tail->encoding != QUICKLIST_NODE_ENCODING_LZF);
if (where == QUICKLIST_HEAD) {
quicklistPushHead(quicklist, value, sz);
} else if (where == QUICKLIST_TAIL) {
quicklistPushTail(quicklist, value, sz);
}
}
void quicklistRepr(unsigned char *ql, int full) {
int i = 0;
quicklist *quicklist = (struct quicklist*) ql;
printf("{count : %ld}\n", quicklist->count);
printf("{len : %ld}\n", quicklist->len);
printf("{fill : %d}\n", quicklist->fill);
printf("{compress : %d}\n", quicklist->compress);
printf("{bookmark_count : %d}\n", quicklist->bookmark_count);
quicklistNode* node = quicklist->head;
while(node != NULL) {
printf("{quicklist node(%d)\n", i++);
printf("{container : %s, encoding: %s, size: %zu, recompress: %d, attempted_compress: %d}\n",
QL_NODE_IS_PLAIN(node) ? "PLAIN": "PACKED",
(node->encoding == QUICKLIST_NODE_ENCODING_RAW) ? "RAW": "LZF",
node->sz,
node->recompress,
node->attempted_compress);
if (full) {
quicklistDecompressNode(node);
if (node->container == QUICKLIST_NODE_CONTAINER_PACKED) {
printf("{ listpack:\n");
lpRepr(node->entry);
printf("}\n");
} else if (QL_NODE_IS_PLAIN(node)) {
printf("{ entry : %s }\n", node->entry);
}
printf("}\n");
quicklistRecompressOnly(node);
}
node = node->next;
}
}
int quicklistBookmarkCreate(quicklist **ql_ref, const char *name, quicklistNode *node) {
quicklist *ql = *ql_ref;
if (ql->bookmark_count >= QL_MAX_BM)
return 0;
quicklistBookmark *bm = _quicklistBookmarkFindByName(ql, name);
if (bm) {
bm->node = node;
return 1;
}
ql = zrealloc(ql, sizeof(quicklist) + (ql->bookmark_count+1) * sizeof(quicklistBookmark));
*ql_ref = ql;
ql->bookmarks[ql->bookmark_count].node = node;
ql->bookmarks[ql->bookmark_count].name = zstrdup(name);
ql->bookmark_count++;
return 1;
}
quicklistNode *quicklistBookmarkFind(quicklist *ql, const char *name) {
quicklistBookmark *bm = _quicklistBookmarkFindByName(ql, name);
if (!bm) return NULL;
return bm->node;
}
int quicklistBookmarkDelete(quicklist *ql, const char *name) {
quicklistBookmark *bm = _quicklistBookmarkFindByName(ql, name);
if (!bm)
return 0;
_quicklistBookmarkDelete(ql, bm);
return 1;
}
quicklistBookmark *_quicklistBookmarkFindByName(quicklist *ql, const char *name) {
unsigned i;
for (i=0; i<ql->bookmark_count; i++) {
if (!strcmp(ql->bookmarks[i].name, name)) {
return &ql->bookmarks[i];
}
}
return NULL;
}
quicklistBookmark *_quicklistBookmarkFindByNode(quicklist *ql, quicklistNode *node) {
unsigned i;
for (i=0; i<ql->bookmark_count; i++) {
if (ql->bookmarks[i].node == node) {
return &ql->bookmarks[i];
}
}
return NULL;
}
void _quicklistBookmarkDelete(quicklist *ql, quicklistBookmark *bm) {
int index = bm - ql->bookmarks;
zfree(bm->name);
ql->bookmark_count--;
memmove(bm, bm+1, (ql->bookmark_count - index)* sizeof(*bm));
}
void quicklistBookmarksClear(quicklist *ql) {
while (ql->bookmark_count)
zfree(ql->bookmarks[--ql->bookmark_count].name);
}
#if defined(REDIS_TEST)
#include <stdint.h>
#include <sys/time.h>
#include "testhelp.h"
#include <stdlib.h>
#define yell(str, ...) printf("ERROR! " str "\n\n", __VA_ARGS__)
#define ERROR do { printf("\tERROR!\n"); err++; } while (0)
#define ERR(x, ...) do { printf("%s:%s:%d:\t", __FILE__, __func__, __LINE__); printf("ERROR! " x "\n", __VA_ARGS__); err++; } while (0)
#define TEST(name) printf("test ??? %s\n", name);
#define TEST_DESC(name, ...) printf("test ??? " name "\n", __VA_ARGS__);
#define QL_TEST_VERBOSE 0
#define UNUSED(x) (void)(x)
static void ql_info(quicklist *ql) {
#if QL_TEST_VERBOSE
printf("Container length: %lu\n", ql->len);
printf("Container size: %lu\n", ql->count);
if (ql->head)
printf("\t(zsize head: %lu)\n", lpLength(ql->head->entry));
if (ql->tail)
printf("\t(zsize tail: %lu)\n", lpLength(ql->tail->entry));
printf("\n");
#else
UNUSED(ql);
#endif
}
static long long ustime(void) {
struct timeval tv;
long long ust;
gettimeofday(&tv, NULL);
ust = ((long long)tv.tv_sec) * 1000000;
ust += tv.tv_usec;
return ust;
}
static long long mstime(void) { return ustime() / 1000; }
static int _itrprintr(quicklist *ql, int print, int forward) {
quicklistIter *iter =
quicklistGetIterator(ql, forward ? AL_START_HEAD : AL_START_TAIL);
quicklistEntry entry;
int i = 0;
int p = 0;
quicklistNode *prev = NULL;
while (quicklistNext(iter, &entry)) {
if (entry.node != prev) {
p++;
prev = entry.node;
}
if (print) {
int size = (entry.sz > (1<<20)) ? 1<<20 : entry.sz;
printf("[%3d (%2d)]: [%.*s] (%lld)\n", i, p, size,
(char *)entry.value, entry.longval);
}
i++;
}
quicklistReleaseIterator(iter);
return i;
}
static int itrprintr(quicklist *ql, int print) {
return _itrprintr(ql, print, 1);
}
static int itrprintr_rev(quicklist *ql, int print) {
return _itrprintr(ql, print, 0);
}
#define ql_verify(a, b, c, d, e) do { err += _ql_verify((a), (b), (c), (d), (e)); } while (0)
static int _ql_verify_compress(quicklist *ql) {
int errors = 0;
if (quicklistAllowsCompression(ql)) {
quicklistNode *node = ql->head;
unsigned int low_raw = ql->compress;
unsigned int high_raw = ql->len - ql->compress;
for (unsigned int at = 0; at < ql->len; at++, node = node->next) {
if (node && (at < low_raw || at >= high_raw)) {
if (node->encoding != QUICKLIST_NODE_ENCODING_RAW) {
yell("Incorrect compression: node %d is "
"compressed at depth %d ((%u, %u); total "
"nodes: %lu; size: %zu; recompress: %d)",
at, ql->compress, low_raw, high_raw, ql->len, node->sz,
node->recompress);
errors++;
}
} else {
if (node->encoding != QUICKLIST_NODE_ENCODING_LZF &&
!node->attempted_compress) {
yell("Incorrect non-compression: node %d is NOT "
"compressed at depth %d ((%u, %u); total "
"nodes: %lu; size: %zu; recompress: %d; attempted: %d)",
at, ql->compress, low_raw, high_raw, ql->len, node->sz,
node->recompress, node->attempted_compress);
errors++;
}
}
}
}
return errors;
}
static int _ql_verify(quicklist *ql, uint32_t len, uint32_t count,
uint32_t head_count, uint32_t tail_count) {
int errors = 0;
ql_info(ql);
if (len != ql->len) {
yell("quicklist length wrong: expected %d, got %lu", len, ql->len);
errors++;
}
if (count != ql->count) {
yell("quicklist count wrong: expected %d, got %lu", count, ql->count);
errors++;
}
int loopr = itrprintr(ql, 0);
if (loopr != (int)ql->count) {
yell("quicklist cached count not match actual count: expected %lu, got "
"%d",
ql->count, loopr);
errors++;
}
int rloopr = itrprintr_rev(ql, 0);
if (loopr != rloopr) {
yell("quicklist has different forward count than reverse count! "
"Forward count is %d, reverse count is %d.",
loopr, rloopr);
errors++;
}
if (ql->len == 0 && !errors) {
return errors;
}
if (ql->head && head_count != ql->head->count &&
head_count != lpLength(ql->head->entry)) {
yell("quicklist head count wrong: expected %d, "
"got cached %d vs. actual %lu",
head_count, ql->head->count, lpLength(ql->head->entry));
errors++;
}
if (ql->tail && tail_count != ql->tail->count &&
tail_count != lpLength(ql->tail->entry)) {
yell("quicklist tail count wrong: expected %d, "
"got cached %u vs. actual %lu",
tail_count, ql->tail->count, lpLength(ql->tail->entry));
errors++;
}
errors += _ql_verify_compress(ql);
return errors;
}
static void ql_release_iterator(quicklistIter *iter) {
quicklist *ql = NULL;
if (iter) ql = iter->quicklist;
quicklistReleaseIterator(iter);
if (ql) assert(!_ql_verify_compress(ql));
}
static char *genstr(char *prefix, int i) {
static char result[64] = {0};
snprintf(result, sizeof(result), "%s%d", prefix, i);
return result;
}
static void randstring(unsigned char *target, size_t sz) {
size_t p = 0;
int minval, maxval;
switch(rand() % 3) {
case 0:
minval = 'a';
maxval = 'z';
break;
case 1:
minval = '0';
maxval = '9';
break;
case 2:
minval = 'A';
maxval = 'Z';
break;
default:
assert(NULL);
}
while(p < sz)
target[p++] = minval+rand()%(maxval-minval+1);
}
int quicklistTest(int argc, char *argv[], int flags) {
UNUSED(argc);
UNUSED(argv);
int accurate = (flags & REDIS_TEST_ACCURATE);
unsigned int err = 0;
int optimize_start =
-(int)(sizeof(optimization_level) / sizeof(*optimization_level));
printf("Starting optimization offset at: %d\n", optimize_start);
int options[] = {0, 1, 2, 3, 4, 5, 6, 10};
int fills[] = {-5, -4, -3, -2, -1, 0,
1, 2, 32, 66, 128, 999};
size_t option_count = sizeof(options) / sizeof(*options);
int fill_count = (int)(sizeof(fills) / sizeof(*fills));
long long runtime[option_count];
for (int _i = 0; _i < (int)option_count; _i++) {
printf("Testing Compression option %d\n", options[_i]);
long long start = mstime();
quicklistIter *iter;
TEST("create list") {
quicklist *ql = quicklistNew(-2, options[_i]);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("add to tail of empty list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushTail(ql, "hello", 6);
ql_verify(ql, 1, 1, 1, 1);
quicklistRelease(ql);
}
TEST("add to head of empty list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushHead(ql, "hello", 6);
ql_verify(ql, 1, 1, 1, 1);
quicklistRelease(ql);
}
TEST_DESC("add to tail 5x at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 5; i++)
quicklistPushTail(ql, genstr("hello", i), 32);
if (ql->count != 5)
ERROR;
if (fills[f] == 32)
ql_verify(ql, 1, 5, 5, 5);
quicklistRelease(ql);
}
}
TEST_DESC("add to head 5x at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 5; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
if (ql->count != 5)
ERROR;
if (fills[f] == 32)
ql_verify(ql, 1, 5, 5, 5);
quicklistRelease(ql);
}
}
TEST_DESC("add to tail 500x at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i), 64);
if (ql->count != 500)
ERROR;
if (fills[f] == 32)
ql_verify(ql, 16, 500, 32, 20);
quicklistRelease(ql);
}
}
TEST_DESC("add to head 500x at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
if (ql->count != 500)
ERROR;
if (fills[f] == 32)
ql_verify(ql, 16, 500, 20, 32);
quicklistRelease(ql);
}
}
TEST("rotate empty") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistRotate(ql);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("Comprassion Plain node") {
char buf[256];
quicklistisSetPackedThreshold(1);
quicklist *ql = quicklistNew(-2, 1);
for (int i = 0; i < 500; i++) {
snprintf(buf, sizeof(buf), "hello%d", i);
quicklistPushHead(ql, buf, 256);
}
quicklistIter *iter = quicklistGetIterator(ql, AL_START_TAIL);
quicklistEntry entry;
int i = 0;
while (quicklistNext(iter, &entry)) {
snprintf(buf, sizeof(buf), "hello%d", i);
if (strcmp((char *)entry.value, buf))
ERR("value [%s] didn't match [%s] at position %d",
entry.value, buf, i);
i++;
}
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("NEXT plain node")
{
packed_threshold = 3;
quicklist *ql = quicklistNew(-2, options[_i]);
char *strings[] = {"hello1", "hello2", "h3", "h4", "hello5"};
for (int i = 0; i < 5; ++i)
quicklistPushHead(ql, strings[i], strlen(strings[i]));
quicklistEntry entry;
quicklistIter *iter = quicklistGetIterator(ql, AL_START_TAIL);
int j = 0;
while(quicklistNext(iter, &entry) != 0) {
assert(strncmp(strings[j], (char *)entry.value, strlen(strings[j])) == 0);
j++;
}
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("rotate plain node ") {
unsigned char *data = NULL;
size_t sz;
long long lv;
int i =0;
packed_threshold = 5;
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushHead(ql, "hello1", 6);
quicklistPushHead(ql, "hello4", 6);
quicklistPushHead(ql, "hello3", 6);
quicklistPushHead(ql, "hello2", 6);
quicklistRotate(ql);
for(i = 1 ; i < 5; i++) {
quicklistPop(ql, QUICKLIST_HEAD, &data, &sz, &lv);
int temp_char = data[5];
zfree(data);
assert(temp_char == ('0' + i));
}
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
packed_threshold = (1 << 30);
}
TEST("rotate one val once") {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
quicklistPushHead(ql, "hello", 6);
quicklistRotate(ql);
ql_verify(ql, 1, 1, 1, 1);
quicklistRelease(ql);
}
}
TEST_DESC("rotate 500 val 5000 times at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
quicklistPushHead(ql, "900", 3);
quicklistPushHead(ql, "7000", 4);
quicklistPushHead(ql, "-1200", 5);
quicklistPushHead(ql, "42", 2);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 64);
ql_info(ql);
for (int i = 0; i < 5000; i++) {
ql_info(ql);
quicklistRotate(ql);
}
if (fills[f] == 1)
ql_verify(ql, 504, 504, 1, 1);
else if (fills[f] == 2)
ql_verify(ql, 252, 504, 2, 2);
else if (fills[f] == 32)
ql_verify(ql, 16, 504, 32, 24);
quicklistRelease(ql);
}
}
TEST("pop empty") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPop(ql, QUICKLIST_HEAD, NULL, NULL, NULL);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("pop 1 string from 1") {
quicklist *ql = quicklistNew(-2, options[_i]);
char *populate = genstr("hello", 331);
quicklistPushHead(ql, populate, 32);
unsigned char *data;
size_t sz;
long long lv;
ql_info(ql);
assert(quicklistPop(ql, QUICKLIST_HEAD, &data, &sz, &lv));
assert(data != NULL);
assert(sz == 32);
if (strcmp(populate, (char *)data)) {
int size = sz;
ERR("Pop'd value (%.*s) didn't equal original value (%s)", size,
data, populate);
}
zfree(data);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("pop head 1 number from 1") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushHead(ql, "55513", 5);
unsigned char *data;
size_t sz;
long long lv;
ql_info(ql);
assert(quicklistPop(ql, QUICKLIST_HEAD, &data, &sz, &lv));
assert(data == NULL);
assert(lv == 55513);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("pop head 500 from 500") {
quicklist *ql = quicklistNew(-2, options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
ql_info(ql);
for (int i = 0; i < 500; i++) {
unsigned char *data;
size_t sz;
long long lv;
int ret = quicklistPop(ql, QUICKLIST_HEAD, &data, &sz, &lv);
assert(ret == 1);
assert(data != NULL);
assert(sz == 32);
if (strcmp(genstr("hello", 499 - i), (char *)data)) {
int size = sz;
ERR("Pop'd value (%.*s) didn't equal original value (%s)",
size, data, genstr("hello", 499 - i));
}
zfree(data);
}
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("pop head 5000 from 500") {
quicklist *ql = quicklistNew(-2, options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
for (int i = 0; i < 5000; i++) {
unsigned char *data;
size_t sz;
long long lv;
int ret = quicklistPop(ql, QUICKLIST_HEAD, &data, &sz, &lv);
if (i < 500) {
assert(ret == 1);
assert(data != NULL);
assert(sz == 32);
if (strcmp(genstr("hello", 499 - i), (char *)data)) {
int size = sz;
ERR("Pop'd value (%.*s) didn't equal original value "
"(%s)",
size, data, genstr("hello", 499 - i));
}
zfree(data);
} else {
assert(ret == 0);
}
}
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("iterate forward over 500 list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
quicklistIter *iter = quicklistGetIterator(ql, AL_START_HEAD);
quicklistEntry entry;
int i = 499, count = 0;
while (quicklistNext(iter, &entry)) {
char *h = genstr("hello", i);
if (strcmp((char *)entry.value, h))
ERR("value [%s] didn't match [%s] at position %d",
entry.value, h, i);
i--;
count++;
}
if (count != 500)
ERR("Didn't iterate over exactly 500 elements (%d)", i);
ql_verify(ql, 16, 500, 20, 32);
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("iterate reverse over 500 list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
quicklistIter *iter = quicklistGetIterator(ql, AL_START_TAIL);
quicklistEntry entry;
int i = 0;
while (quicklistNext(iter, &entry)) {
char *h = genstr("hello", i);
if (strcmp((char *)entry.value, h))
ERR("value [%s] didn't match [%s] at position %d",
entry.value, h, i);
i++;
}
if (i != 500)
ERR("Didn't iterate over exactly 500 elements (%d)", i);
ql_verify(ql, 16, 500, 20, 32);
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("insert after 1 element") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushHead(ql, "hello", 6);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
quicklistInsertAfter(iter, &entry, "abc", 4);
ql_release_iterator(iter);
ql_verify(ql, 1, 2, 2, 2);
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
int sz = entry.sz;
if (strncmp((char *)entry.value, "hello", 5)) {
ERR("Value 0 didn't match, instead got: %.*s", sz,
entry.value);
}
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 1, &entry);
sz = entry.sz;
if (strncmp((char *)entry.value, "abc", 3)) {
ERR("Value 1 didn't match, instead got: %.*s", sz,
entry.value);
}
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("insert before 1 element") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushHead(ql, "hello", 6);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
quicklistInsertBefore(iter, &entry, "abc", 4);
ql_release_iterator(iter);
ql_verify(ql, 1, 2, 2, 2);
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
int sz = entry.sz;
if (strncmp((char *)entry.value, "abc", 3)) {
ERR("Value 0 didn't match, instead got: %.*s", sz,
entry.value);
}
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 1, &entry);
sz = entry.sz;
if (strncmp((char *)entry.value, "hello", 5)) {
ERR("Value 1 didn't match, instead got: %.*s", sz,
entry.value);
}
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("insert head while head node is full") {
quicklist *ql = quicklistNew(4, options[_i]);
for (int i = 0; i < 10; i++)
quicklistPushTail(ql, genstr("hello", i), 6);
quicklistSetFill(ql, -1);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, -10, &entry);
char buf[4096] = {0};
quicklistInsertBefore(iter, &entry, buf, 4096);
ql_release_iterator(iter);
ql_verify(ql, 4, 11, 1, 2);
quicklistRelease(ql);
}
TEST("insert tail while tail node is full") {
quicklist *ql = quicklistNew(4, options[_i]);
for (int i = 0; i < 10; i++)
quicklistPushHead(ql, genstr("hello", i), 6);
quicklistSetFill(ql, -1);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, -1, &entry);
char buf[4096] = {0};
quicklistInsertAfter(iter, &entry, buf, 4096);
ql_release_iterator(iter);
ql_verify(ql, 4, 11, 2, 1);
quicklistRelease(ql);
}
TEST_DESC("insert once in elements while iterating at compress %d",
options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
quicklistPushTail(ql, "abc", 3);
quicklistSetFill(ql, 1);
quicklistPushTail(ql, "def", 3);
quicklistSetFill(ql, f);
quicklistPushTail(ql, "bob", 3);
quicklistPushTail(ql, "foo", 3);
quicklistPushTail(ql, "zoo", 3);
itrprintr(ql, 0);
quicklistIter *iter = quicklistGetIterator(ql, AL_START_HEAD);
quicklistEntry entry;
while (quicklistNext(iter, &entry)) {
if (!strncmp((char *)entry.value, "bob", 3)) {
quicklistInsertBefore(iter, &entry, "bar", 3);
break;
}
}
ql_release_iterator(iter);
itrprintr(ql, 0);
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
int sz = entry.sz;
if (strncmp((char *)entry.value, "abc", 3))
ERR("Value 0 didn't match, instead got: %.*s", sz,
entry.value);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 1, &entry);
if (strncmp((char *)entry.value, "def", 3))
ERR("Value 1 didn't match, instead got: %.*s", sz,
entry.value);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 2, &entry);
if (strncmp((char *)entry.value, "bar", 3))
ERR("Value 2 didn't match, instead got: %.*s", sz,
entry.value);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 3, &entry);
if (strncmp((char *)entry.value, "bob", 3))
ERR("Value 3 didn't match, instead got: %.*s", sz,
entry.value);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 4, &entry);
if (strncmp((char *)entry.value, "foo", 3))
ERR("Value 4 didn't match, instead got: %.*s", sz,
entry.value);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 5, &entry);
if (strncmp((char *)entry.value, "zoo", 3))
ERR("Value 5 didn't match, instead got: %.*s", sz,
entry.value);
ql_release_iterator(iter);
quicklistRelease(ql);
}
}
TEST_DESC("insert [before] 250 new in middle of 500 elements at compress %d",
options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i), 32);
for (int i = 0; i < 250; i++) {
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 250, &entry);
quicklistInsertBefore(iter, &entry, genstr("abc", i), 32);
ql_release_iterator(iter);
}
if (fills[f] == 32)
ql_verify(ql, 25, 750, 32, 20);
quicklistRelease(ql);
}
}
TEST_DESC("insert [after] 250 new in middle of 500 elements at compress %d",
options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
for (int i = 0; i < 250; i++) {
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 250, &entry);
quicklistInsertAfter(iter, &entry, genstr("abc", i), 32);
ql_release_iterator(iter);
}
if (ql->count != 750)
ERR("List size not 750, but rather %ld", ql->count);
if (fills[f] == 32)
ql_verify(ql, 26, 750, 20, 32);
quicklistRelease(ql);
}
}
TEST("duplicate empty list") {
quicklist *ql = quicklistNew(-2, options[_i]);
ql_verify(ql, 0, 0, 0, 0);
quicklist *copy = quicklistDup(ql);
ql_verify(copy, 0, 0, 0, 0);
quicklistRelease(ql);
quicklistRelease(copy);
}
TEST("duplicate list of 1 element") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushHead(ql, genstr("hello", 3), 32);
ql_verify(ql, 1, 1, 1, 1);
quicklist *copy = quicklistDup(ql);
ql_verify(copy, 1, 1, 1, 1);
quicklistRelease(ql);
quicklistRelease(copy);
}
TEST("duplicate list of 500") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
ql_verify(ql, 16, 500, 20, 32);
quicklist *copy = quicklistDup(ql);
ql_verify(copy, 16, 500, 20, 32);
quicklistRelease(ql);
quicklistRelease(copy);
}
for (int f = 0; f < fill_count; f++) {
TEST_DESC("index 1,200 from 500 list at fill %d at compress %d", f,
options[_i]) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 1, &entry);
if (strcmp((char *)entry.value, "hello2") != 0)
ERR("Value: %s", entry.value);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 200, &entry);
if (strcmp((char *)entry.value, "hello201") != 0)
ERR("Value: %s", entry.value);
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST_DESC("index -1,-2 from 500 list at fill %d at compress %d",
fills[f], options[_i]) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, -1, &entry);
if (strcmp((char *)entry.value, "hello500") != 0)
ERR("Value: %s", entry.value);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -2, &entry);
if (strcmp((char *)entry.value, "hello499") != 0)
ERR("Value: %s", entry.value);
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST_DESC("index -100 from 500 list at fill %d at compress %d",
fills[f], options[_i]) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, -100, &entry);
if (strcmp((char *)entry.value, "hello401") != 0)
ERR("Value: %s", entry.value);
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST_DESC("index too big +1 from 50 list at fill %d at compress %d",
fills[f], options[_i]) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
for (int i = 0; i < 50; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
quicklistEntry entry;
int sz = entry.sz;
iter = quicklistGetIteratorEntryAtIdx(ql, 50, &entry);
if (iter)
ERR("Index found at 50 with 50 list: %.*s", sz,
entry.value);
ql_release_iterator(iter);
quicklistRelease(ql);
}
}
TEST("delete range empty list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistDelRange(ql, 5, 20);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("delete range of entire node in list of one node") {
quicklist *ql = quicklistNew(-2, options[_i]);
for (int i = 0; i < 32; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
ql_verify(ql, 1, 32, 32, 32);
quicklistDelRange(ql, 0, 32);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("delete range of entire node with overflow counts") {
quicklist *ql = quicklistNew(-2, options[_i]);
for (int i = 0; i < 32; i++)
quicklistPushHead(ql, genstr("hello", i), 32);
ql_verify(ql, 1, 32, 32, 32);
quicklistDelRange(ql, 0, 128);
ql_verify(ql, 0, 0, 0, 0);
quicklistRelease(ql);
}
TEST("delete middle 100 of 500 list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
ql_verify(ql, 16, 500, 32, 20);
quicklistDelRange(ql, 200, 100);
ql_verify(ql, 14, 400, 32, 20);
quicklistRelease(ql);
}
TEST("delete less than fill but across nodes") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
ql_verify(ql, 16, 500, 32, 20);
quicklistDelRange(ql, 60, 10);
ql_verify(ql, 16, 490, 32, 20);
quicklistRelease(ql);
}
TEST("delete negative 1 from 500 list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
ql_verify(ql, 16, 500, 32, 20);
quicklistDelRange(ql, -1, 1);
ql_verify(ql, 16, 499, 32, 19);
quicklistRelease(ql);
}
TEST("delete negative 1 from 500 list with overflow counts") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
ql_verify(ql, 16, 500, 32, 20);
quicklistDelRange(ql, -1, 128);
ql_verify(ql, 16, 499, 32, 19);
quicklistRelease(ql);
}
TEST("delete negative 100 from 500 list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 500; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
quicklistDelRange(ql, -100, 100);
ql_verify(ql, 13, 400, 32, 16);
quicklistRelease(ql);
}
TEST("delete -10 count 5 from 50 list") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
for (int i = 0; i < 50; i++)
quicklistPushTail(ql, genstr("hello", i + 1), 32);
ql_verify(ql, 2, 50, 32, 18);
quicklistDelRange(ql, -10, 5);
ql_verify(ql, 2, 45, 32, 13);
quicklistRelease(ql);
}
TEST("numbers only list read") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushTail(ql, "1111", 4);
quicklistPushTail(ql, "2222", 4);
quicklistPushTail(ql, "3333", 4);
quicklistPushTail(ql, "4444", 4);
ql_verify(ql, 1, 4, 4, 4);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
if (entry.longval != 1111)
ERR("Not 1111, %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 1, &entry);
if (entry.longval != 2222)
ERR("Not 2222, %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 2, &entry);
if (entry.longval != 3333)
ERR("Not 3333, %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 3, &entry);
if (entry.longval != 4444)
ERR("Not 4444, %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, 4, &entry);
if (iter)
ERR("Index past elements: %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -1, &entry);
if (entry.longval != 4444)
ERR("Not 4444 (reverse), %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -2, &entry);
if (entry.longval != 3333)
ERR("Not 3333 (reverse), %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -3, &entry);
if (entry.longval != 2222)
ERR("Not 2222 (reverse), %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -4, &entry);
if (entry.longval != 1111)
ERR("Not 1111 (reverse), %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -5, &entry);
if (iter)
ERR("Index past elements (reverse), %lld", entry.longval);
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("numbers larger list read") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistSetFill(ql, 32);
char num[32];
long long nums[5000];
for (int i = 0; i < 5000; i++) {
nums[i] = -5157318210846258176 + i;
int sz = ll2string(num, sizeof(num), nums[i]);
quicklistPushTail(ql, num, sz);
}
quicklistPushTail(ql, "xxxxxxxxxxxxxxxxxxxx", 20);
quicklistEntry entry;
for (int i = 0; i < 5000; i++) {
iter = quicklistGetIteratorEntryAtIdx(ql, i, &entry);
if (entry.longval != nums[i])
ERR("[%d] Not longval %lld but rather %lld", i, nums[i],
entry.longval);
entry.longval = 0xdeadbeef;
ql_release_iterator(iter);
}
iter = quicklistGetIteratorEntryAtIdx(ql, 5000, &entry);
if (strncmp((char *)entry.value, "xxxxxxxxxxxxxxxxxxxx", 20))
ERR("String val not match: %s", entry.value);
ql_verify(ql, 157, 5001, 32, 9);
ql_release_iterator(iter);
quicklistRelease(ql);
}
TEST("numbers larger list read B") {
quicklist *ql = quicklistNew(-2, options[_i]);
quicklistPushTail(ql, "99", 2);
quicklistPushTail(ql, "98", 2);
quicklistPushTail(ql, "xxxxxxxxxxxxxxxxxxxx", 20);
quicklistPushTail(ql, "96", 2);
quicklistPushTail(ql, "95", 2);
quicklistReplaceAtIndex(ql, 1, "foo", 3);
quicklistReplaceAtIndex(ql, -1, "bar", 3);
quicklistRelease(ql);
}
TEST_DESC("lrem test at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
char *words[] = {"abc", "foo", "bar", "foobar", "foobared",
"zap", "bar", "test", "foo"};
char *result[] = {"abc", "foo", "foobar", "foobared",
"zap", "test", "foo"};
char *resultB[] = {"abc", "foo", "foobar",
"foobared", "zap", "test"};
for (int i = 0; i < 9; i++)
quicklistPushTail(ql, words[i], strlen(words[i]));
quicklistIter *iter = quicklistGetIterator(ql, AL_START_HEAD);
quicklistEntry entry;
int i = 0;
while (quicklistNext(iter, &entry)) {
if (quicklistCompare(&entry, (unsigned char *)"bar", 3)) {
quicklistDelEntry(iter, &entry);
}
i++;
}
ql_release_iterator(iter);
iter = quicklistGetIterator(ql, AL_START_HEAD);
i = 0;
while (quicklistNext(iter, &entry)) {
int sz = entry.sz;
if (strncmp((char *)entry.value, result[i], entry.sz)) {
ERR("No match at position %d, got %.*s instead of %s",
i, sz, entry.value, result[i]);
}
i++;
}
ql_release_iterator(iter);
quicklistPushTail(ql, "foo", 3);
iter = quicklistGetIterator(ql, AL_START_TAIL);
i = 0;
int del = 2;
while (quicklistNext(iter, &entry)) {
if (quicklistCompare(&entry, (unsigned char *)"foo", 3)) {
quicklistDelEntry(iter, &entry);
del--;
}
if (!del)
break;
i++;
}
ql_release_iterator(iter);
iter = quicklistGetIterator(ql, AL_START_TAIL);
i = 0;
size_t resB = sizeof(resultB) / sizeof(*resultB);
while (quicklistNext(iter, &entry)) {
int sz = entry.sz;
if (strncmp((char *)entry.value, resultB[resB - 1 - i],
sz)) {
ERR("No match at position %d, got %.*s instead of %s",
i, sz, entry.value, resultB[resB - 1 - i]);
}
i++;
}
ql_release_iterator(iter);
quicklistRelease(ql);
}
}
TEST_DESC("iterate reverse + delete at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
quicklistPushTail(ql, "abc", 3);
quicklistPushTail(ql, "def", 3);
quicklistPushTail(ql, "hij", 3);
quicklistPushTail(ql, "jkl", 3);
quicklistPushTail(ql, "oop", 3);
quicklistEntry entry;
quicklistIter *iter = quicklistGetIterator(ql, AL_START_TAIL);
int i = 0;
while (quicklistNext(iter, &entry)) {
if (quicklistCompare(&entry, (unsigned char *)"hij", 3)) {
quicklistDelEntry(iter, &entry);
}
i++;
}
ql_release_iterator(iter);
if (i != 5)
ERR("Didn't iterate 5 times, iterated %d times.", i);
iter = quicklistGetIterator(ql, AL_START_HEAD);
i = 0;
char *vals[] = {"abc", "def", "jkl", "oop"};
while (quicklistNext(iter, &entry)) {
if (!quicklistCompare(&entry, (unsigned char *)vals[i],
3)) {
ERR("Value at %d didn't match %s\n", i, vals[i]);
}
i++;
}
ql_release_iterator(iter);
quicklistRelease(ql);
}
}
TEST_DESC("iterator at index test at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
char num[32];
long long nums[5000];
for (int i = 0; i < 760; i++) {
nums[i] = -5157318210846258176 + i;
int sz = ll2string(num, sizeof(num), nums[i]);
quicklistPushTail(ql, num, sz);
}
quicklistEntry entry;
quicklistIter *iter =
quicklistGetIteratorAtIdx(ql, AL_START_HEAD, 437);
int i = 437;
while (quicklistNext(iter, &entry)) {
if (entry.longval != nums[i])
ERR("Expected %lld, but got %lld", entry.longval,
nums[i]);
i++;
}
ql_release_iterator(iter);
quicklistRelease(ql);
}
}
TEST_DESC("ltrim test A at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
char num[32];
long long nums[5000];
for (int i = 0; i < 32; i++) {
nums[i] = -5157318210846258176 + i;
int sz = ll2string(num, sizeof(num), nums[i]);
quicklistPushTail(ql, num, sz);
}
if (fills[f] == 32)
ql_verify(ql, 1, 32, 32, 32);
quicklistDelRange(ql, 0, 25);
quicklistDelRange(ql, 0, 0);
quicklistEntry entry;
for (int i = 0; i < 7; i++) {
iter = quicklistGetIteratorEntryAtIdx(ql, i, &entry);
if (entry.longval != nums[25 + i])
ERR("Deleted invalid range! Expected %lld but got "
"%lld",
entry.longval, nums[25 + i]);
ql_release_iterator(iter);
}
if (fills[f] == 32)
ql_verify(ql, 1, 7, 7, 7);
quicklistRelease(ql);
}
}
TEST_DESC("ltrim test B at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], QUICKLIST_NOCOMPRESS);
char num[32];
long long nums[5000];
for (int i = 0; i < 33; i++) {
nums[i] = i;
int sz = ll2string(num, sizeof(num), nums[i]);
quicklistPushTail(ql, num, sz);
}
if (fills[f] == 32)
ql_verify(ql, 2, 33, 32, 1);
quicklistDelRange(ql, 0, 5);
quicklistDelRange(ql, -16, 16);
if (fills[f] == 32)
ql_verify(ql, 1, 12, 12, 12);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
if (entry.longval != 5)
ERR("A: longval not 5, but %lld", entry.longval);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -1, &entry);
if (entry.longval != 16)
ERR("B! got instead: %lld", entry.longval);
quicklistPushTail(ql, "bobobob", 7);
ql_release_iterator(iter);
iter = quicklistGetIteratorEntryAtIdx(ql, -1, &entry);
int sz = entry.sz;
if (strncmp((char *)entry.value, "bobobob", 7))
ERR("Tail doesn't match bobobob, it's %.*s instead",
sz, entry.value);
ql_release_iterator(iter);
for (int i = 0; i < 12; i++) {
iter = quicklistGetIteratorEntryAtIdx(ql, i, &entry);
if (entry.longval != nums[5 + i])
ERR("Deleted invalid range! Expected %lld but got "
"%lld",
entry.longval, nums[5 + i]);
ql_release_iterator(iter);
}
quicklistRelease(ql);
}
}
TEST_DESC("ltrim test C at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
char num[32];
long long nums[5000];
for (int i = 0; i < 33; i++) {
nums[i] = -5157318210846258176 + i;
int sz = ll2string(num, sizeof(num), nums[i]);
quicklistPushTail(ql, num, sz);
}
if (fills[f] == 32)
ql_verify(ql, 2, 33, 32, 1);
quicklistDelRange(ql, 0, 3);
quicklistDelRange(ql, -29,
4000);
if (fills[f] == 32)
ql_verify(ql, 1, 1, 1, 1);
quicklistEntry entry;
iter = quicklistGetIteratorEntryAtIdx(ql, 0, &entry);
if (entry.longval != -5157318210846258173)
ERROR;
ql_release_iterator(iter);
quicklistRelease(ql);
}
}
TEST_DESC("ltrim test D at compress %d", options[_i]) {
for (int f = 0; f < fill_count; f++) {
quicklist *ql = quicklistNew(fills[f], options[_i]);
char num[32];
long long nums[5000];
for (int i = 0; i < 33; i++) {
nums[i] = -5157318210846258176 + i;
int sz = ll2string(num, sizeof(num), nums[i]);
quicklistPushTail(ql, num, sz);
}
if (fills[f] == 32)
ql_verify(ql, 2, 33, 32, 1);
quicklistDelRange(ql, -12, 3);
if (ql->count != 30)
ERR("Didn't delete exactly three elements! Count is: %lu",
ql->count);
quicklistRelease(ql);
}
}
long long stop = mstime();
runtime[_i] = stop - start;
}
int list_sizes[] = {250, 251, 500, 999, 1000};
long long start = mstime();
int list_count = accurate ? (int)(sizeof(list_sizes) / sizeof(*list_sizes)) : 1;
for (int list = 0; list < list_count; list++) {
TEST_DESC("verify specific compression of interior nodes with %d list ",
list_sizes[list]) {
for (int f = 0; f < fill_count; f++) {
for (int depth = 1; depth < 40; depth++) {
quicklist *ql = quicklistNew(fills[f], depth);
for (int i = 0; i < list_sizes[list]; i++) {
quicklistPushTail(ql, genstr("hello TAIL", i + 1), 64);
quicklistPushHead(ql, genstr("hello HEAD", i + 1), 64);
}
for (int step = 0; step < 2; step++) {
if (step == 1) {
for (int i = 0; i < list_sizes[list] / 2; i++) {
unsigned char *data;
assert(quicklistPop(ql, QUICKLIST_HEAD, &data,
NULL, NULL));
zfree(data);
assert(quicklistPop(ql, QUICKLIST_TAIL, &data,
NULL, NULL));
zfree(data);
}
}
quicklistNode *node = ql->head;
unsigned int low_raw = ql->compress;
unsigned int high_raw = ql->len - ql->compress;
for (unsigned int at = 0; at < ql->len;
at++, node = node->next) {
if (at < low_raw || at >= high_raw) {
if (node->encoding != QUICKLIST_NODE_ENCODING_RAW) {
ERR("Incorrect compression: node %d is "
"compressed at depth %d ((%u, %u); total "
"nodes: %lu; size: %zu)",
at, depth, low_raw, high_raw, ql->len,
node->sz);
}
} else {
if (node->encoding != QUICKLIST_NODE_ENCODING_LZF) {
ERR("Incorrect non-compression: node %d is NOT "
"compressed at depth %d ((%u, %u); total "
"nodes: %lu; size: %zu; attempted: %d)",
at, depth, low_raw, high_raw, ql->len,
node->sz, node->attempted_compress);
}
}
}
}
quicklistRelease(ql);
}
}
}
}
long long stop = mstime();
printf("\n");
for (size_t i = 0; i < option_count; i++)
printf("Test Loop %02d: %0.2f seconds.\n", options[i],
(float)runtime[i] / 1000);
printf("Compressions: %0.2f seconds.\n", (float)(stop - start) / 1000);
printf("\n");
TEST("bookmark get updated to next item") {
quicklist *ql = quicklistNew(1, 0);
quicklistPushTail(ql, "1", 1);
quicklistPushTail(ql, "2", 1);
quicklistPushTail(ql, "3", 1);
quicklistPushTail(ql, "4", 1);
quicklistPushTail(ql, "5", 1);
assert(ql->len==5);
assert(quicklistBookmarkCreate(&ql, "_dummy", ql->head->next));
assert(quicklistBookmarkCreate(&ql, "_test", ql->tail->prev));
assert(quicklistBookmarkFind(ql, "_test") == ql->tail->prev);
assert(quicklistDelRange(ql, -2, 1));
assert(quicklistBookmarkFind(ql, "_test") == ql->tail);
assert(quicklistDelRange(ql, -1, 1));
assert(quicklistBookmarkFind(ql, "_test") == NULL);
assert(quicklistBookmarkFind(ql, "_dummy") == ql->head->next);
assert(quicklistBookmarkFind(ql, "_missing") == NULL);
assert(ql->len==3);
quicklistBookmarksClear(ql);
assert(quicklistBookmarkFind(ql, "_dummy") == NULL);
quicklistRelease(ql);
}
TEST("bookmark limit") {
int i;
quicklist *ql = quicklistNew(1, 0);
quicklistPushHead(ql, "1", 1);
for (i=0; i<QL_MAX_BM; i++)
assert(quicklistBookmarkCreate(&ql, genstr("",i), ql->head));
assert(!quicklistBookmarkCreate(&ql, "_test", ql->head));
assert(quicklistBookmarkDelete(ql, "0"));
assert(quicklistBookmarkCreate(&ql, "_test", ql->head));
assert(quicklistBookmarkDelete(ql, "_test"));
for (i=1; i<QL_MAX_BM; i++)
assert(quicklistBookmarkFind(ql, genstr("",i)) == ql->head);
assert(!quicklistBookmarkFind(ql, "0"));
assert(!quicklistBookmarkFind(ql, "_test"));
quicklistRelease(ql);
}
if (flags & REDIS_TEST_LARGE_MEMORY) {
TEST("compress and decompress quicklist listpack node") {
quicklistNode *node = quicklistCreateNode();
node->entry = lpNew(0);
node->prev = quicklistCreateNode();
node->next = quicklistCreateNode();
size_t sz = (1 << 25);
unsigned char *s = zmalloc(sz);
randstring(s, sz);
for (int i = 0; i < 32; i++) {
node->entry = lpAppend(node->entry, s, sz);
quicklistNodeUpdateSz(node);
long long start = mstime();
assert(__quicklistCompressNode(node));
assert(__quicklistDecompressNode(node));
printf("Compress and decompress: %zu MB in %.2f seconds.\n",
node->sz/1024/1024, (float)(mstime() - start) / 1000);
}
zfree(s);
zfree(node->prev);
zfree(node->next);
zfree(node->entry);
zfree(node);
}
#if ULONG_MAX >= 0xffffffffffffffff
TEST("compress and decomress quicklist plain node large than UINT32_MAX") {
size_t sz = (1ull << 32);
unsigned char *s = zmalloc(sz);
randstring(s, sz);
memcpy(s, "helloworld", 10);
memcpy(s + sz - 10, "1234567890", 10);
quicklistNode *node = __quicklistCreatePlainNode(s, sz);
node->prev = quicklistCreateNode();
node->next = quicklistCreateNode();
long long start = mstime();
assert(__quicklistCompressNode(node));
assert(__quicklistDecompressNode(node));
printf("Compress and decompress: %zu MB in %.2f seconds.\n",
node->sz/1024/1024, (float)(mstime() - start) / 1000);
assert(memcmp(node->entry, "helloworld", 10) == 0);
assert(memcmp(node->entry + sz - 10, "1234567890", 10) == 0);
zfree(node->prev);
zfree(node->next);
zfree(node->entry);
zfree(node);
}
#endif
}
if (!err)
printf("ALL TESTS PASSED!\n");
else
ERR("Sorry, not all tests passed! In fact, %d tests failed.", err);
return err;
}
#endif
