#include "server.h"
#include "bio.h"
#include "atomicvar.h"
#include "functions.h"

static redisAtomic size_t lazyfree_objects = 0;
static redisAtomic size_t lazyfreed_objects = 0;



void lazyfreeFreeObject(void *args[]) {
robj *o = (robj *) args[0];
decrRefCount(o);
atomicDecr(lazyfree_objects,1);
atomicIncr(lazyfreed_objects,1);
}




void lazyfreeFreeDatabase(void *args[]) {
dict *ht1 = (dict *) args[0];
dict *ht2 = (dict *) args[1];

size_t numkeys = dictSize(ht1);
dictRelease(ht1);
dictRelease(ht2);
atomicDecr(lazyfree_objects,numkeys);
atomicIncr(lazyfreed_objects,numkeys);
}


void lazyFreeTrackingTable(void *args[]) {
rax *rt = args[0];
size_t len = rt->numele;
freeTrackingRadixTree(rt);
atomicDecr(lazyfree_objects,len);
atomicIncr(lazyfreed_objects,len);
}


void lazyFreeLuaScripts(void *args[]) {
dict *lua_scripts = args[0];
long long len = dictSize(lua_scripts);
dictRelease(lua_scripts);
atomicDecr(lazyfree_objects,len);
atomicIncr(lazyfreed_objects,len);
}


void lazyFreeFunctionsCtx(void *args[]) {
functionsLibCtx *functions_lib_ctx = args[0];
size_t len = functionsLibCtxfunctionsLen(functions_lib_ctx);
functionsLibCtxFree(functions_lib_ctx);
atomicDecr(lazyfree_objects,len);
atomicIncr(lazyfreed_objects,len);
}


void lazyFreeReplicationBacklogRefMem(void *args[]) {
list *blocks = args[0];
rax *index = args[1];
long long len = listLength(blocks);
len += raxSize(index);
listRelease(blocks);
raxFree(index);
atomicDecr(lazyfree_objects,len);
atomicIncr(lazyfreed_objects,len);
}


size_t lazyfreeGetPendingObjectsCount(void) {
size_t aux;
atomicGet(lazyfree_objects,aux);
return aux;
}


size_t lazyfreeGetFreedObjectsCount(void) {
size_t aux;
atomicGet(lazyfreed_objects,aux);
return aux;
}

void lazyfreeResetStats() {
atomicSet(lazyfreed_objects,0);
}
















size_t lazyfreeGetFreeEffort(robj *key, robj *obj, int dbid) {
if (obj->type == OBJ_LIST) {
quicklist *ql = obj->ptr;
return ql->len;
} else if (obj->type == OBJ_SET && obj->encoding == OBJ_ENCODING_HT) {
dict *ht = obj->ptr;
return dictSize(ht);
} else if (obj->type == OBJ_ZSET && obj->encoding == OBJ_ENCODING_SKIPLIST){
zset *zs = obj->ptr;
return zs->zsl->length;
} else if (obj->type == OBJ_HASH && obj->encoding == OBJ_ENCODING_HT) {
dict *ht = obj->ptr;
return dictSize(ht);
} else if (obj->type == OBJ_STREAM) {
size_t effort = 0;
stream *s = obj->ptr;



effort += s->rax->numnodes;




if (s->cgroups && raxSize(s->cgroups)) {
raxIterator ri;
streamCG *cg;
raxStart(&ri,s->cgroups);
raxSeek(&ri,"^",NULL,0);


serverAssert(raxNext(&ri));
cg = ri.data;
effort += raxSize(s->cgroups)*(1+raxSize(cg->pel));
raxStop(&ri);
}
return effort;
} else if (obj->type == OBJ_MODULE) {
size_t effort = moduleGetFreeEffort(key, obj, dbid);


return effort == 0 ? ULONG_MAX : effort;
} else {
return 1;
}
}






#define LAZYFREE_THRESHOLD 64


void freeObjAsync(robj *key, robj *obj, int dbid) {
size_t free_effort = lazyfreeGetFreeEffort(key,obj,dbid);




if (free_effort > LAZYFREE_THRESHOLD && obj->refcount == 1) {
atomicIncr(lazyfree_objects,1);
bioCreateLazyFreeJob(lazyfreeFreeObject,1,obj);
} else {
decrRefCount(obj);
}
}




void emptyDbAsync(redisDb *db) {
dict *oldht1 = db->dict, *oldht2 = db->expires;
db->dict = dictCreate(&dbDictType);
db->expires = dictCreate(&dbExpiresDictType);
atomicIncr(lazyfree_objects,dictSize(oldht1));
bioCreateLazyFreeJob(lazyfreeFreeDatabase,2,oldht1,oldht2);
}



void freeTrackingRadixTreeAsync(rax *tracking) {

if (tracking->numnodes > LAZYFREE_THRESHOLD) {
atomicIncr(lazyfree_objects,tracking->numele);
bioCreateLazyFreeJob(lazyFreeTrackingTable,1,tracking);
} else {
freeTrackingRadixTree(tracking);
}
}


void freeLuaScriptsAsync(dict *lua_scripts) {
if (dictSize(lua_scripts) > LAZYFREE_THRESHOLD) {
atomicIncr(lazyfree_objects,dictSize(lua_scripts));
bioCreateLazyFreeJob(lazyFreeLuaScripts,1,lua_scripts);
} else {
dictRelease(lua_scripts);
}
}


void freeFunctionsAsync(functionsLibCtx *functions_lib_ctx) {
if (functionsLibCtxfunctionsLen(functions_lib_ctx) > LAZYFREE_THRESHOLD) {
atomicIncr(lazyfree_objects,functionsLibCtxfunctionsLen(functions_lib_ctx));
bioCreateLazyFreeJob(lazyFreeFunctionsCtx,1,functions_lib_ctx);
} else {
functionsLibCtxFree(functions_lib_ctx);
}
}


void freeReplicationBacklogRefMemAsync(list *blocks, rax *index) {
if (listLength(blocks) > LAZYFREE_THRESHOLD ||
raxSize(index) > LAZYFREE_THRESHOLD)
{
atomicIncr(lazyfree_objects,listLength(blocks)+raxSize(index));
bioCreateLazyFreeJob(lazyFreeReplicationBacklogRefMem,2,blocks,index);
} else {
listRelease(blocks);
raxFree(index);
}
}
