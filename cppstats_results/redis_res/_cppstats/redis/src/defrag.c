#include "server.h"
#include "cluster.h"
#include <time.h>
#include <assert.h>
#include <stddef.h>
#if defined(HAVE_DEFRAG)
int je_get_defrag_hint(void* ptr);
void defragDictBucketCallback(dict *d, dictEntry **bucketref);
dictEntry* replaceSatelliteDictKeyPtrAndOrDefragDictEntry(dict *d, sds oldkey, sds newkey, uint64_t hash, long *defragged);
void* activeDefragAlloc(void *ptr) {
size_t size;
void *newptr;
if(!je_get_defrag_hint(ptr)) {
server.stat_active_defrag_misses++;
return NULL;
}
size = zmalloc_size(ptr);
newptr = zmalloc_no_tcache(size);
memcpy(newptr, ptr, size);
zfree_no_tcache(ptr);
return newptr;
}
sds activeDefragSds(sds sdsptr) {
void* ptr = sdsAllocPtr(sdsptr);
void* newptr = activeDefragAlloc(ptr);
if (newptr) {
size_t offset = sdsptr - (char*)ptr;
sdsptr = (char*)newptr + offset;
return sdsptr;
}
return NULL;
}
robj *activeDefragStringOb(robj* ob, long *defragged) {
robj *ret = NULL;
if (ob->refcount!=1)
return NULL;
if (ob->type!=OBJ_STRING || ob->encoding!=OBJ_ENCODING_EMBSTR) {
if ((ret = activeDefragAlloc(ob))) {
ob = ret;
(*defragged)++;
}
}
if (ob->type == OBJ_STRING) {
if(ob->encoding==OBJ_ENCODING_RAW) {
sds newsds = activeDefragSds((sds)ob->ptr);
if (newsds) {
ob->ptr = newsds;
(*defragged)++;
}
} else if (ob->encoding==OBJ_ENCODING_EMBSTR) {
long ofs = (intptr_t)ob->ptr - (intptr_t)ob;
if ((ret = activeDefragAlloc(ob))) {
ret->ptr = (void*)((intptr_t)ret + ofs);
(*defragged)++;
}
} else if (ob->encoding!=OBJ_ENCODING_INT) {
serverPanic("Unknown string encoding");
}
}
return ret;
}
long dictIterDefragEntry(dictIterator *iter) {
long defragged = 0;
if (iter->nextEntry) {
dictEntry *newde = activeDefragAlloc(iter->nextEntry);
if (newde) {
defragged++;
iter->nextEntry = newde;
iter->entry->next = newde;
}
}
if (iter->d->ht_table[iter->table][iter->index] == iter->entry) {
dictEntry *newde = activeDefragAlloc(iter->entry);
if (newde) {
iter->entry = newde;
iter->d->ht_table[iter->table][iter->index] = newde;
defragged++;
}
}
return defragged;
}
long dictDefragTables(dict* d) {
dictEntry **newtable;
long defragged = 0;
newtable = activeDefragAlloc(d->ht_table[0]);
if (newtable)
defragged++, d->ht_table[0] = newtable;
if (d->ht_table[1]) {
newtable = activeDefragAlloc(d->ht_table[1]);
if (newtable)
defragged++, d->ht_table[1] = newtable;
}
return defragged;
}
void zslUpdateNode(zskiplist *zsl, zskiplistNode *oldnode, zskiplistNode *newnode, zskiplistNode **update) {
int i;
for (i = 0; i < zsl->level; i++) {
if (update[i]->level[i].forward == oldnode)
update[i]->level[i].forward = newnode;
}
serverAssert(zsl->header!=oldnode);
if (newnode->level[0].forward) {
serverAssert(newnode->level[0].forward->backward==oldnode);
newnode->level[0].forward->backward = newnode;
} else {
serverAssert(zsl->tail==oldnode);
zsl->tail = newnode;
}
}
double *zslDefrag(zskiplist *zsl, double score, sds oldele, sds newele) {
zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x, *newx;
int i;
sds ele = newele? newele: oldele;
x = zsl->header;
for (i = zsl->level-1; i >= 0; i--) {
while (x->level[i].forward &&
x->level[i].forward->ele != oldele &&
(x->level[i].forward->score < score ||
(x->level[i].forward->score == score &&
sdscmp(x->level[i].forward->ele,ele) < 0)))
x = x->level[i].forward;
update[i] = x;
}
x = x->level[0].forward;
serverAssert(x && score == x->score && x->ele==oldele);
if (newele)
x->ele = newele;
newx = activeDefragAlloc(x);
if (newx) {
zslUpdateNode(zsl, x, newx, update);
return &newx->score;
}
return NULL;
}
long activeDefragZsetEntry(zset *zs, dictEntry *de) {
sds newsds;
double* newscore;
long defragged = 0;
sds sdsele = dictGetKey(de);
if ((newsds = activeDefragSds(sdsele)))
defragged++, de->key = newsds;
newscore = zslDefrag(zs->zsl, *(double*)dictGetVal(de), sdsele, newsds);
if (newscore) {
dictSetVal(zs->dict, de, newscore);
defragged++;
}
return defragged;
}
#define DEFRAG_SDS_DICT_NO_VAL 0
#define DEFRAG_SDS_DICT_VAL_IS_SDS 1
#define DEFRAG_SDS_DICT_VAL_IS_STROB 2
#define DEFRAG_SDS_DICT_VAL_VOID_PTR 3
long activeDefragSdsDict(dict* d, int val_type) {
dictIterator *di;
dictEntry *de;
long defragged = 0;
di = dictGetIterator(d);
while((de = dictNext(di)) != NULL) {
sds sdsele = dictGetKey(de), newsds;
if ((newsds = activeDefragSds(sdsele)))
de->key = newsds, defragged++;
if (val_type == DEFRAG_SDS_DICT_VAL_IS_SDS) {
sdsele = dictGetVal(de);
if ((newsds = activeDefragSds(sdsele)))
de->v.val = newsds, defragged++;
} else if (val_type == DEFRAG_SDS_DICT_VAL_IS_STROB) {
robj *newele, *ele = dictGetVal(de);
if ((newele = activeDefragStringOb(ele, &defragged)))
de->v.val = newele;
} else if (val_type == DEFRAG_SDS_DICT_VAL_VOID_PTR) {
void *newptr, *ptr = dictGetVal(de);
if ((newptr = activeDefragAlloc(ptr)))
de->v.val = newptr, defragged++;
}
defragged += dictIterDefragEntry(di);
}
dictReleaseIterator(di);
return defragged;
}
long activeDefragList(list *l, int val_type) {
long defragged = 0;
listNode *ln, *newln;
for (ln = l->head; ln; ln = ln->next) {
if ((newln = activeDefragAlloc(ln))) {
if (newln->prev)
newln->prev->next = newln;
else
l->head = newln;
if (newln->next)
newln->next->prev = newln;
else
l->tail = newln;
ln = newln;
defragged++;
}
if (val_type == DEFRAG_SDS_DICT_VAL_IS_SDS) {
sds newsds, sdsele = ln->value;
if ((newsds = activeDefragSds(sdsele)))
ln->value = newsds, defragged++;
} else if (val_type == DEFRAG_SDS_DICT_VAL_IS_STROB) {
robj *newele, *ele = ln->value;
if ((newele = activeDefragStringOb(ele, &defragged)))
ln->value = newele;
} else if (val_type == DEFRAG_SDS_DICT_VAL_VOID_PTR) {
void *newptr, *ptr = ln->value;
if ((newptr = activeDefragAlloc(ptr)))
ln->value = newptr, defragged++;
}
}
return defragged;
}
long activeDefragSdsListAndDict(list *l, dict *d, int dict_val_type) {
long defragged = 0;
sds newsds, sdsele;
listNode *ln, *newln;
dictIterator *di;
dictEntry *de;
for (ln = l->head; ln; ln = ln->next) {
if ((newln = activeDefragAlloc(ln))) {
if (newln->prev)
newln->prev->next = newln;
else
l->head = newln;
if (newln->next)
newln->next->prev = newln;
else
l->tail = newln;
ln = newln;
defragged++;
}
sdsele = ln->value;
if ((newsds = activeDefragSds(sdsele))) {
uint64_t hash = dictGetHash(d, newsds);
dictEntry **deref = dictFindEntryRefByPtrAndHash(d, sdsele, hash);
if (deref)
(*deref)->key = newsds;
ln->value = newsds;
defragged++;
}
}
di = dictGetIterator(d);
while((de = dictNext(di)) != NULL) {
if (dict_val_type == DEFRAG_SDS_DICT_VAL_IS_SDS) {
sds newsds, sdsele = dictGetVal(de);
if ((newsds = activeDefragSds(sdsele)))
de->v.val = newsds, defragged++;
} else if (dict_val_type == DEFRAG_SDS_DICT_VAL_IS_STROB) {
robj *newele, *ele = dictGetVal(de);
if ((newele = activeDefragStringOb(ele, &defragged)))
de->v.val = newele;
} else if (dict_val_type == DEFRAG_SDS_DICT_VAL_VOID_PTR) {
void *newptr, *ptr = dictGetVal(de);
if ((newptr = activeDefragAlloc(ptr)))
de->v.val = newptr, defragged++;
}
defragged += dictIterDefragEntry(di);
}
dictReleaseIterator(di);
return defragged;
}
dictEntry* replaceSatelliteDictKeyPtrAndOrDefragDictEntry(dict *d, sds oldkey, sds newkey, uint64_t hash, long *defragged) {
dictEntry **deref = dictFindEntryRefByPtrAndHash(d, oldkey, hash);
if (deref) {
dictEntry *de = *deref;
dictEntry *newde = activeDefragAlloc(de);
if (newde) {
de = *deref = newde;
(*defragged)++;
}
if (newkey)
de->key = newkey;
return de;
}
return NULL;
}
long activeDefragQuickListNode(quicklist *ql, quicklistNode **node_ref) {
quicklistNode *newnode, *node = *node_ref;
long defragged = 0;
unsigned char *newzl;
if ((newnode = activeDefragAlloc(node))) {
if (newnode->prev)
newnode->prev->next = newnode;
else
ql->head = newnode;
if (newnode->next)
newnode->next->prev = newnode;
else
ql->tail = newnode;
*node_ref = node = newnode;
defragged++;
}
if ((newzl = activeDefragAlloc(node->entry)))
defragged++, node->entry = newzl;
return defragged;
}
long activeDefragQuickListNodes(quicklist *ql) {
quicklistNode *node = ql->head;
long defragged = 0;
while (node) {
defragged += activeDefragQuickListNode(ql, &node);
node = node->next;
}
return defragged;
}
void defragLater(redisDb *db, dictEntry *kde) {
sds key = sdsdup(dictGetKey(kde));
listAddNodeTail(db->defrag_later, key);
}
long scanLaterList(robj *ob, unsigned long *cursor, long long endtime, long long *defragged) {
quicklist *ql = ob->ptr;
quicklistNode *node;
long iterations = 0;
int bookmark_failed = 0;
if (ob->type != OBJ_LIST || ob->encoding != OBJ_ENCODING_QUICKLIST)
return 0;
if (*cursor == 0) {
node = ql->head;
} else {
node = quicklistBookmarkFind(ql, "_AD");
if (!node) {
*cursor = 0;
return 0;
}
node = node->next;
}
(*cursor)++;
while (node) {
(*defragged) += activeDefragQuickListNode(ql, &node);
server.stat_active_defrag_scanned++;
if (++iterations > 128 && !bookmark_failed) {
if (ustime() > endtime) {
if (!quicklistBookmarkCreate(&ql, "_AD", node)) {
bookmark_failed = 1;
} else {
ob->ptr = ql;
return 1;
}
}
iterations = 0;
}
node = node->next;
}
quicklistBookmarkDelete(ql, "_AD");
*cursor = 0;
return bookmark_failed? 1: 0;
}
typedef struct {
zset *zs;
long defragged;
} scanLaterZsetData;
void scanLaterZsetCallback(void *privdata, const dictEntry *_de) {
dictEntry *de = (dictEntry*)_de;
scanLaterZsetData *data = privdata;
data->defragged += activeDefragZsetEntry(data->zs, de);
server.stat_active_defrag_scanned++;
}
long scanLaterZset(robj *ob, unsigned long *cursor) {
if (ob->type != OBJ_ZSET || ob->encoding != OBJ_ENCODING_SKIPLIST)
return 0;
zset *zs = (zset*)ob->ptr;
dict *d = zs->dict;
scanLaterZsetData data = {zs, 0};
*cursor = dictScan(d, *cursor, scanLaterZsetCallback, defragDictBucketCallback, &data);
return data.defragged;
}
void scanLaterSetCallback(void *privdata, const dictEntry *_de) {
dictEntry *de = (dictEntry*)_de;
long *defragged = privdata;
sds sdsele = dictGetKey(de), newsds;
if ((newsds = activeDefragSds(sdsele)))
(*defragged)++, de->key = newsds;
server.stat_active_defrag_scanned++;
}
long scanLaterSet(robj *ob, unsigned long *cursor) {
long defragged = 0;
if (ob->type != OBJ_SET || ob->encoding != OBJ_ENCODING_HT)
return 0;
dict *d = ob->ptr;
*cursor = dictScan(d, *cursor, scanLaterSetCallback, defragDictBucketCallback, &defragged);
return defragged;
}
void scanLaterHashCallback(void *privdata, const dictEntry *_de) {
dictEntry *de = (dictEntry*)_de;
long *defragged = privdata;
sds sdsele = dictGetKey(de), newsds;
if ((newsds = activeDefragSds(sdsele)))
(*defragged)++, de->key = newsds;
sdsele = dictGetVal(de);
if ((newsds = activeDefragSds(sdsele)))
(*defragged)++, de->v.val = newsds;
server.stat_active_defrag_scanned++;
}
long scanLaterHash(robj *ob, unsigned long *cursor) {
long defragged = 0;
if (ob->type != OBJ_HASH || ob->encoding != OBJ_ENCODING_HT)
return 0;
dict *d = ob->ptr;
*cursor = dictScan(d, *cursor, scanLaterHashCallback, defragDictBucketCallback, &defragged);
return defragged;
}
long defragQuicklist(redisDb *db, dictEntry *kde) {
robj *ob = dictGetVal(kde);
long defragged = 0;
quicklist *ql = ob->ptr, *newql;
serverAssert(ob->type == OBJ_LIST && ob->encoding == OBJ_ENCODING_QUICKLIST);
if ((newql = activeDefragAlloc(ql)))
defragged++, ob->ptr = ql = newql;
if (ql->len > server.active_defrag_max_scan_fields)
defragLater(db, kde);
else
defragged += activeDefragQuickListNodes(ql);
return defragged;
}
long defragZsetSkiplist(redisDb *db, dictEntry *kde) {
robj *ob = dictGetVal(kde);
long defragged = 0;
zset *zs = (zset*)ob->ptr;
zset *newzs;
zskiplist *newzsl;
dict *newdict;
dictEntry *de;
struct zskiplistNode *newheader;
serverAssert(ob->type == OBJ_ZSET && ob->encoding == OBJ_ENCODING_SKIPLIST);
if ((newzs = activeDefragAlloc(zs)))
defragged++, ob->ptr = zs = newzs;
if ((newzsl = activeDefragAlloc(zs->zsl)))
defragged++, zs->zsl = newzsl;
if ((newheader = activeDefragAlloc(zs->zsl->header)))
defragged++, zs->zsl->header = newheader;
if (dictSize(zs->dict) > server.active_defrag_max_scan_fields)
defragLater(db, kde);
else {
dictIterator *di = dictGetIterator(zs->dict);
while((de = dictNext(di)) != NULL) {
defragged += activeDefragZsetEntry(zs, de);
}
dictReleaseIterator(di);
}
if ((newdict = activeDefragAlloc(zs->dict)))
defragged++, zs->dict = newdict;
defragged += dictDefragTables(zs->dict);
return defragged;
}
long defragHash(redisDb *db, dictEntry *kde) {
long defragged = 0;
robj *ob = dictGetVal(kde);
dict *d, *newd;
serverAssert(ob->type == OBJ_HASH && ob->encoding == OBJ_ENCODING_HT);
d = ob->ptr;
if (dictSize(d) > server.active_defrag_max_scan_fields)
defragLater(db, kde);
else
defragged += activeDefragSdsDict(d, DEFRAG_SDS_DICT_VAL_IS_SDS);
if ((newd = activeDefragAlloc(ob->ptr)))
defragged++, ob->ptr = newd;
defragged += dictDefragTables(ob->ptr);
return defragged;
}
long defragSet(redisDb *db, dictEntry *kde) {
long defragged = 0;
robj *ob = dictGetVal(kde);
dict *d, *newd;
serverAssert(ob->type == OBJ_SET && ob->encoding == OBJ_ENCODING_HT);
d = ob->ptr;
if (dictSize(d) > server.active_defrag_max_scan_fields)
defragLater(db, kde);
else
defragged += activeDefragSdsDict(d, DEFRAG_SDS_DICT_NO_VAL);
if ((newd = activeDefragAlloc(ob->ptr)))
defragged++, ob->ptr = newd;
defragged += dictDefragTables(ob->ptr);
return defragged;
}
int defragRaxNode(raxNode **noderef) {
raxNode *newnode = activeDefragAlloc(*noderef);
if (newnode) {
*noderef = newnode;
return 1;
}
return 0;
}
int scanLaterStreamListpacks(robj *ob, unsigned long *cursor, long long endtime, long long *defragged) {
static unsigned char last[sizeof(streamID)];
raxIterator ri;
long iterations = 0;
if (ob->type != OBJ_STREAM || ob->encoding != OBJ_ENCODING_STREAM) {
*cursor = 0;
return 0;
}
stream *s = ob->ptr;
raxStart(&ri,s->rax);
if (*cursor == 0) {
defragRaxNode(&s->rax->head);
ri.node_cb = defragRaxNode;
raxSeek(&ri,"^",NULL,0);
} else {
if (!raxSeek(&ri,">", last, sizeof(last))) {
*cursor = 0;
raxStop(&ri);
return 0;
}
ri.node_cb = defragRaxNode;
}
(*cursor)++;
while (raxNext(&ri)) {
void *newdata = activeDefragAlloc(ri.data);
if (newdata)
raxSetData(ri.node, ri.data=newdata), (*defragged)++;
server.stat_active_defrag_scanned++;
if (++iterations > 128) {
if (ustime() > endtime) {
serverAssert(ri.key_len==sizeof(last));
memcpy(last,ri.key,ri.key_len);
raxStop(&ri);
return 1;
}
iterations = 0;
}
}
raxStop(&ri);
*cursor = 0;
return 0;
}
typedef void *(raxDefragFunction)(raxIterator *ri, void *privdata, long *defragged);
long defragRadixTree(rax **raxref, int defrag_data, raxDefragFunction *element_cb, void *element_cb_data) {
long defragged = 0;
raxIterator ri;
rax* rax;
if ((rax = activeDefragAlloc(*raxref)))
defragged++, *raxref = rax;
rax = *raxref;
raxStart(&ri,rax);
ri.node_cb = defragRaxNode;
defragRaxNode(&rax->head);
raxSeek(&ri,"^",NULL,0);
while (raxNext(&ri)) {
void *newdata = NULL;
if (element_cb)
newdata = element_cb(&ri, element_cb_data, &defragged);
if (defrag_data && !newdata)
newdata = activeDefragAlloc(ri.data);
if (newdata)
raxSetData(ri.node, ri.data=newdata), defragged++;
}
raxStop(&ri);
return defragged;
}
typedef struct {
streamCG *cg;
streamConsumer *c;
} PendingEntryContext;
void* defragStreamConsumerPendingEntry(raxIterator *ri, void *privdata, long *defragged) {
UNUSED(defragged);
PendingEntryContext *ctx = privdata;
streamNACK *nack = ri->data, *newnack;
nack->consumer = ctx->c;
newnack = activeDefragAlloc(nack);
if (newnack) {
void *prev;
raxInsert(ctx->cg->pel, ri->key, ri->key_len, newnack, &prev);
serverAssert(prev==nack);
}
return newnack;
}
void* defragStreamConsumer(raxIterator *ri, void *privdata, long *defragged) {
streamConsumer *c = ri->data;
streamCG *cg = privdata;
void *newc = activeDefragAlloc(c);
if (newc) {
c = newc;
}
sds newsds = activeDefragSds(c->name);
if (newsds)
(*defragged)++, c->name = newsds;
if (c->pel) {
PendingEntryContext pel_ctx = {cg, c};
*defragged += defragRadixTree(&c->pel, 0, defragStreamConsumerPendingEntry, &pel_ctx);
}
return newc;
}
void* defragStreamConsumerGroup(raxIterator *ri, void *privdata, long *defragged) {
streamCG *cg = ri->data;
UNUSED(privdata);
if (cg->consumers)
*defragged += defragRadixTree(&cg->consumers, 0, defragStreamConsumer, cg);
if (cg->pel)
*defragged += defragRadixTree(&cg->pel, 0, NULL, NULL);
return NULL;
}
long defragStream(redisDb *db, dictEntry *kde) {
long defragged = 0;
robj *ob = dictGetVal(kde);
serverAssert(ob->type == OBJ_STREAM && ob->encoding == OBJ_ENCODING_STREAM);
stream *s = ob->ptr, *news;
if ((news = activeDefragAlloc(s)))
defragged++, ob->ptr = s = news;
if (raxSize(s->rax) > server.active_defrag_max_scan_fields) {
rax *newrax = activeDefragAlloc(s->rax);
if (newrax)
defragged++, s->rax = newrax;
defragLater(db, kde);
} else
defragged += defragRadixTree(&s->rax, 1, NULL, NULL);
if (s->cgroups)
defragged += defragRadixTree(&s->cgroups, 1, defragStreamConsumerGroup, NULL);
return defragged;
}
long defragModule(redisDb *db, dictEntry *kde) {
robj *obj = dictGetVal(kde);
serverAssert(obj->type == OBJ_MODULE);
long defragged = 0;
if (!moduleDefragValue(dictGetKey(kde), obj, &defragged, db->id))
defragLater(db, kde);
return defragged;
}
long defragKey(redisDb *db, dictEntry *de) {
sds keysds = dictGetKey(de);
robj *newob, *ob;
unsigned char *newzl;
long defragged = 0;
sds newsds;
newsds = activeDefragSds(keysds);
if (newsds)
defragged++, de->key = newsds;
if (dictSize(db->expires)) {
uint64_t hash = dictGetHash(db->dict, de->key);
replaceSatelliteDictKeyPtrAndOrDefragDictEntry(db->expires, keysds, newsds, hash, &defragged);
}
ob = dictGetVal(de);
if ((newob = activeDefragStringOb(ob, &defragged))) {
de->v.val = newob;
ob = newob;
}
if (ob->type == OBJ_STRING) {
} else if (ob->type == OBJ_LIST) {
if (ob->encoding == OBJ_ENCODING_QUICKLIST) {
defragged += defragQuicklist(db, de);
} else {
serverPanic("Unknown list encoding");
}
} else if (ob->type == OBJ_SET) {
if (ob->encoding == OBJ_ENCODING_HT) {
defragged += defragSet(db, de);
} else if (ob->encoding == OBJ_ENCODING_INTSET) {
intset *newis, *is = ob->ptr;
if ((newis = activeDefragAlloc(is)))
defragged++, ob->ptr = newis;
} else {
serverPanic("Unknown set encoding");
}
} else if (ob->type == OBJ_ZSET) {
if (ob->encoding == OBJ_ENCODING_LISTPACK) {
if ((newzl = activeDefragAlloc(ob->ptr)))
defragged++, ob->ptr = newzl;
} else if (ob->encoding == OBJ_ENCODING_SKIPLIST) {
defragged += defragZsetSkiplist(db, de);
} else {
serverPanic("Unknown sorted set encoding");
}
} else if (ob->type == OBJ_HASH) {
if (ob->encoding == OBJ_ENCODING_LISTPACK) {
if ((newzl = activeDefragAlloc(ob->ptr)))
defragged++, ob->ptr = newzl;
} else if (ob->encoding == OBJ_ENCODING_HT) {
defragged += defragHash(db, de);
} else {
serverPanic("Unknown hash encoding");
}
} else if (ob->type == OBJ_STREAM) {
defragged += defragStream(db, de);
} else if (ob->type == OBJ_MODULE) {
defragged += defragModule(db, de);
} else {
serverPanic("Unknown object type");
}
return defragged;
}
void defragScanCallback(void *privdata, const dictEntry *de) {
long defragged = defragKey((redisDb*)privdata, (dictEntry*)de);
server.stat_active_defrag_hits += defragged;
if(defragged)
server.stat_active_defrag_key_hits++;
else
server.stat_active_defrag_key_misses++;
server.stat_active_defrag_scanned++;
}
void defragDictBucketCallback(dict *d, dictEntry **bucketref) {
while(*bucketref) {
dictEntry *de = *bucketref, *newde;
if ((newde = activeDefragAlloc(de))) {
*bucketref = newde;
if (server.cluster_enabled && d == server.db[0].dict) {
slotToKeyReplaceEntry(newde, server.db);
}
}
bucketref = &(*bucketref)->next;
}
}
float getAllocatorFragmentation(size_t *out_frag_bytes) {
size_t resident, active, allocated;
zmalloc_get_allocator_info(&allocated, &active, &resident);
float frag_pct = ((float)active / allocated)*100 - 100;
size_t frag_bytes = active - allocated;
float rss_pct = ((float)resident / allocated)*100 - 100;
size_t rss_bytes = resident - allocated;
if(out_frag_bytes)
*out_frag_bytes = frag_bytes;
serverLog(LL_DEBUG,
"allocated=%zu, active=%zu, resident=%zu, frag=%.0f%% (%.0f%% rss), frag_bytes=%zu (%zu rss)",
allocated, active, resident, frag_pct, rss_pct, frag_bytes, rss_bytes);
return frag_pct;
}
long defragOtherGlobals() {
long defragged = 0;
defragged += activeDefragSdsDict(evalScriptsDict(), DEFRAG_SDS_DICT_VAL_IS_STROB);
defragged += moduleDefragGlobals();
return defragged;
}
int defragLaterItem(dictEntry *de, unsigned long *cursor, long long endtime, int dbid) {
if (de) {
robj *ob = dictGetVal(de);
if (ob->type == OBJ_LIST) {
return scanLaterList(ob, cursor, endtime, &server.stat_active_defrag_hits);
} else if (ob->type == OBJ_SET) {
server.stat_active_defrag_hits += scanLaterSet(ob, cursor);
} else if (ob->type == OBJ_ZSET) {
server.stat_active_defrag_hits += scanLaterZset(ob, cursor);
} else if (ob->type == OBJ_HASH) {
server.stat_active_defrag_hits += scanLaterHash(ob, cursor);
} else if (ob->type == OBJ_STREAM) {
return scanLaterStreamListpacks(ob, cursor, endtime, &server.stat_active_defrag_hits);
} else if (ob->type == OBJ_MODULE) {
return moduleLateDefrag(dictGetKey(de), ob, cursor, endtime, &server.stat_active_defrag_hits, dbid);
} else {
*cursor = 0;
}
} else {
*cursor = 0;
}
return 0;
}
static sds defrag_later_current_key = NULL;
static unsigned long defrag_later_cursor = 0;
int defragLaterStep(redisDb *db, long long endtime) {
unsigned int iterations = 0;
unsigned long long prev_defragged = server.stat_active_defrag_hits;
unsigned long long prev_scanned = server.stat_active_defrag_scanned;
long long key_defragged;
do {
if (!defrag_later_cursor) {
listNode *head = listFirst(db->defrag_later);
if (defrag_later_current_key) {
serverAssert(defrag_later_current_key == head->value);
listDelNode(db->defrag_later, head);
defrag_later_cursor = 0;
defrag_later_current_key = NULL;
}
head = listFirst(db->defrag_later);
if (!head)
return 0;
defrag_later_current_key = head->value;
defrag_later_cursor = 0;
}
dictEntry *de = dictFind(db->dict, defrag_later_current_key);
key_defragged = server.stat_active_defrag_hits;
do {
int quit = 0;
if (defragLaterItem(de, &defrag_later_cursor, endtime,db->id))
quit = 1;
if (quit || (++iterations > 16 ||
server.stat_active_defrag_hits - prev_defragged > 512 ||
server.stat_active_defrag_scanned - prev_scanned > 64)) {
if (quit || ustime() > endtime) {
if(key_defragged != server.stat_active_defrag_hits)
server.stat_active_defrag_key_hits++;
else
server.stat_active_defrag_key_misses++;
return 1;
}
iterations = 0;
prev_defragged = server.stat_active_defrag_hits;
prev_scanned = server.stat_active_defrag_scanned;
}
} while(defrag_later_cursor);
if(key_defragged != server.stat_active_defrag_hits)
server.stat_active_defrag_key_hits++;
else
server.stat_active_defrag_key_misses++;
} while(1);
}
#define INTERPOLATE(x, x1, x2, y1, y2) ( (y1) + ((x)-(x1)) * ((y2)-(y1)) / ((x2)-(x1)) )
#define LIMIT(y, min, max) ((y)<(min)? min: ((y)>(max)? max: (y)))
void computeDefragCycles() {
size_t frag_bytes;
float frag_pct = getAllocatorFragmentation(&frag_bytes);
if (!server.active_defrag_running) {
if(frag_pct < server.active_defrag_threshold_lower || frag_bytes < server.active_defrag_ignore_bytes)
return;
}
int cpu_pct = INTERPOLATE(frag_pct,
server.active_defrag_threshold_lower,
server.active_defrag_threshold_upper,
server.active_defrag_cycle_min,
server.active_defrag_cycle_max);
cpu_pct = LIMIT(cpu_pct,
server.active_defrag_cycle_min,
server.active_defrag_cycle_max);
if (cpu_pct > server.active_defrag_running) {
server.active_defrag_running = cpu_pct;
serverLog(LL_VERBOSE,
"Starting active defrag, frag=%.0f%%, frag_bytes=%zu, cpu=%d%%",
frag_pct, frag_bytes, cpu_pct);
}
}
void activeDefragCycle(void) {
static int current_db = -1;
static unsigned long cursor = 0;
static redisDb *db = NULL;
static long long start_scan, start_stat;
unsigned int iterations = 0;
unsigned long long prev_defragged = server.stat_active_defrag_hits;
unsigned long long prev_scanned = server.stat_active_defrag_scanned;
long long start, timelimit, endtime;
mstime_t latency;
int quit = 0;
if (!server.active_defrag_enabled) {
if (server.active_defrag_running) {
server.active_defrag_running = 0;
if (db)
listEmpty(db->defrag_later);
defrag_later_current_key = NULL;
defrag_later_cursor = 0;
current_db = -1;
cursor = 0;
db = NULL;
goto update_metrics;
}
return;
}
if (hasActiveChildProcess())
return;
run_with_period(1000) {
computeDefragCycles();
}
if (!server.active_defrag_running)
return;
start = ustime();
timelimit = 1000000*server.active_defrag_running/server.hz/100;
if (timelimit <= 0) timelimit = 1;
endtime = start + timelimit;
latencyStartMonitor(latency);
do {
if (!cursor) {
if (db && defragLaterStep(db, endtime)) {
quit = 1;
break;
}
if (++current_db >= server.dbnum) {
defragOtherGlobals();
long long now = ustime();
size_t frag_bytes;
float frag_pct = getAllocatorFragmentation(&frag_bytes);
serverLog(LL_VERBOSE,
"Active defrag done in %dms, reallocated=%d, frag=%.0f%%, frag_bytes=%zu",
(int)((now - start_scan)/1000), (int)(server.stat_active_defrag_hits - start_stat), frag_pct, frag_bytes);
start_scan = now;
current_db = -1;
cursor = 0;
db = NULL;
server.active_defrag_running = 0;
computeDefragCycles();
if (server.active_defrag_running != 0 && ustime() < endtime)
continue;
break;
}
else if (current_db==0) {
start_scan = ustime();
start_stat = server.stat_active_defrag_hits;
}
db = &server.db[current_db];
cursor = 0;
}
do {
if (defragLaterStep(db, endtime)) {
quit = 1;
break;
}
cursor = dictScan(db->dict, cursor, defragScanCallback, defragDictBucketCallback, db);
if (!cursor || (++iterations > 16 ||
server.stat_active_defrag_hits - prev_defragged > 512 ||
server.stat_active_defrag_scanned - prev_scanned > 64)) {
if (!cursor || ustime() > endtime) {
quit = 1;
break;
}
iterations = 0;
prev_defragged = server.stat_active_defrag_hits;
prev_scanned = server.stat_active_defrag_scanned;
}
} while(cursor && !quit);
} while(!quit);
latencyEndMonitor(latency);
latencyAddSampleIfNeeded("active-defrag-cycle",latency);
update_metrics:
if (server.active_defrag_running > 0) {
if (server.stat_last_active_defrag_time == 0)
elapsedStart(&server.stat_last_active_defrag_time);
} else if (server.stat_last_active_defrag_time != 0) {
server.stat_total_active_defrag_time += elapsedUs(server.stat_last_active_defrag_time);
server.stat_last_active_defrag_time = 0;
}
}
#else
void activeDefragCycle(void) {
}
void *activeDefragAlloc(void *ptr) {
UNUSED(ptr);
return NULL;
}
robj *activeDefragStringOb(robj *ob, long *defragged) {
UNUSED(ob);
UNUSED(defragged);
return NULL;
}
#endif
