#include "server.h"
#include "cluster.h"
#include "atomicvar.h"
#include "latency.h"
#include "script.h"
#include "functions.h"
#include <signal.h>
#include <ctype.h>
int expireIfNeeded(redisDb *db, robj *key, int force_delete_expired);
int keyIsExpired(redisDb *db, robj *key);
void updateLFU(robj *val) {
unsigned long counter = LFUDecrAndReturn(val);
counter = LFULogIncr(counter);
val->lru = (LFUGetTimeInMinutes()<<8) | counter;
}
robj *lookupKey(redisDb *db, robj *key, int flags) {
dictEntry *de = dictFind(db->dict,key->ptr);
robj *val = NULL;
if (de) {
val = dictGetVal(de);
int force_delete_expired = flags & LOOKUP_WRITE;
if (force_delete_expired) {
client *c = server.in_script ? scriptGetClient() : server.current_client;
serverAssert(!c || !c->cmd || (c->cmd->flags & (CMD_WRITE|CMD_MODULE)));
}
if (expireIfNeeded(db, key, force_delete_expired)) {
val = NULL;
}
}
if (val) {
if (!hasActiveChildProcess() && !(flags & LOOKUP_NOTOUCH)){
if (server.maxmemory_policy & MAXMEMORY_FLAG_LFU) {
updateLFU(val);
} else {
val->lru = LRU_CLOCK();
}
}
if (!(flags & (LOOKUP_NOSTATS | LOOKUP_WRITE)))
server.stat_keyspace_hits++;
} else {
if (!(flags & (LOOKUP_NONOTIFY | LOOKUP_WRITE)))
notifyKeyspaceEvent(NOTIFY_KEY_MISS, "keymiss", key, db->id);
if (!(flags & (LOOKUP_NOSTATS | LOOKUP_WRITE)))
server.stat_keyspace_misses++;
}
return val;
}
robj *lookupKeyReadWithFlags(redisDb *db, robj *key, int flags) {
serverAssert(!(flags & LOOKUP_WRITE));
return lookupKey(db, key, flags);
}
robj *lookupKeyRead(redisDb *db, robj *key) {
return lookupKeyReadWithFlags(db,key,LOOKUP_NONE);
}
robj *lookupKeyWriteWithFlags(redisDb *db, robj *key, int flags) {
return lookupKey(db, key, flags | LOOKUP_WRITE);
}
robj *lookupKeyWrite(redisDb *db, robj *key) {
return lookupKeyWriteWithFlags(db, key, LOOKUP_NONE);
}
robj *lookupKeyReadOrReply(client *c, robj *key, robj *reply) {
robj *o = lookupKeyRead(c->db, key);
if (!o) addReplyOrErrorObject(c, reply);
return o;
}
robj *lookupKeyWriteOrReply(client *c, robj *key, robj *reply) {
robj *o = lookupKeyWrite(c->db, key);
if (!o) addReplyOrErrorObject(c, reply);
return o;
}
void dbAdd(redisDb *db, robj *key, robj *val) {
sds copy = sdsdup(key->ptr);
dictEntry *de = dictAddRaw(db->dict, copy, NULL);
serverAssertWithInfo(NULL, key, de != NULL);
dictSetVal(db->dict, de, val);
signalKeyAsReady(db, key, val->type);
if (server.cluster_enabled) slotToKeyAddEntry(de, db);
}
int dbAddRDBLoad(redisDb *db, sds key, robj *val) {
dictEntry *de = dictAddRaw(db->dict, key, NULL);
if (de == NULL) return 0;
dictSetVal(db->dict, de, val);
if (server.cluster_enabled) slotToKeyAddEntry(de, db);
return 1;
}
void dbOverwrite(redisDb *db, robj *key, robj *val) {
dictEntry *de = dictFind(db->dict,key->ptr);
serverAssertWithInfo(NULL,key,de != NULL);
dictEntry auxentry = *de;
robj *old = dictGetVal(de);
if (server.maxmemory_policy & MAXMEMORY_FLAG_LFU) {
val->lru = old->lru;
}
moduleNotifyKeyUnlink(key,old,db->id);
dictSetVal(db->dict, de, val);
if (server.lazyfree_lazy_server_del) {
freeObjAsync(key,old,db->id);
dictSetVal(db->dict, &auxentry, NULL);
}
dictFreeVal(db->dict, &auxentry);
}
void setKey(client *c, redisDb *db, robj *key, robj *val, int flags) {
int keyfound = 0;
if (flags & SETKEY_ALREADY_EXIST)
keyfound = 1;
else if (!(flags & SETKEY_DOESNT_EXIST))
keyfound = (lookupKeyWrite(db,key) != NULL);
if (!keyfound) {
dbAdd(db,key,val);
} else {
dbOverwrite(db,key,val);
}
incrRefCount(val);
if (!(flags & SETKEY_KEEPTTL)) removeExpire(db,key);
if (!(flags & SETKEY_NO_SIGNAL)) signalModifiedKey(c,db,key);
}
robj *dbRandomKey(redisDb *db) {
dictEntry *de;
int maxtries = 100;
int allvolatile = dictSize(db->dict) == dictSize(db->expires);
while(1) {
sds key;
robj *keyobj;
de = dictGetFairRandomKey(db->dict);
if (de == NULL) return NULL;
key = dictGetKey(de);
keyobj = createStringObject(key,sdslen(key));
if (dictFind(db->expires,key)) {
if (allvolatile && server.masterhost && --maxtries == 0) {
return keyobj;
}
if (expireIfNeeded(db,keyobj,0)) {
decrRefCount(keyobj);
continue;
}
}
return keyobj;
}
}
static int dbGenericDelete(redisDb *db, robj *key, int async) {
if (dictSize(db->expires) > 0) dictDelete(db->expires,key->ptr);
dictEntry *de = dictUnlink(db->dict,key->ptr);
if (de) {
robj *val = dictGetVal(de);
moduleNotifyKeyUnlink(key,val,db->id);
if (async) {
freeObjAsync(key, val, db->id);
dictSetVal(db->dict, de, NULL);
}
if (server.cluster_enabled) slotToKeyDelEntry(de, db);
dictFreeUnlinkedEntry(db->dict,de);
return 1;
} else {
return 0;
}
}
int dbSyncDelete(redisDb *db, robj *key) {
return dbGenericDelete(db, key, 0);
}
int dbAsyncDelete(redisDb *db, robj *key) {
return dbGenericDelete(db, key, 1);
}
int dbDelete(redisDb *db, robj *key) {
return dbGenericDelete(db, key, server.lazyfree_lazy_server_del);
}
robj *dbUnshareStringValue(redisDb *db, robj *key, robj *o) {
serverAssert(o->type == OBJ_STRING);
if (o->refcount != 1 || o->encoding != OBJ_ENCODING_RAW) {
robj *decoded = getDecodedObject(o);
o = createRawStringObject(decoded->ptr, sdslen(decoded->ptr));
decrRefCount(decoded);
dbOverwrite(db,key,o);
}
return o;
}
long long emptyDbStructure(redisDb *dbarray, int dbnum, int async,
void(callback)(dict*))
{
long long removed = 0;
int startdb, enddb;
if (dbnum == -1) {
startdb = 0;
enddb = server.dbnum-1;
} else {
startdb = enddb = dbnum;
}
for (int j = startdb; j <= enddb; j++) {
removed += dictSize(dbarray[j].dict);
if (async) {
emptyDbAsync(&dbarray[j]);
} else {
dictEmpty(dbarray[j].dict,callback);
dictEmpty(dbarray[j].expires,callback);
}
dbarray[j].avg_ttl = 0;
dbarray[j].expires_cursor = 0;
}
return removed;
}
long long emptyData(int dbnum, int flags, void(callback)(dict*)) {
int async = (flags & EMPTYDB_ASYNC);
int with_functions = !(flags & EMPTYDB_NOFUNCTIONS);
RedisModuleFlushInfoV1 fi = {REDISMODULE_FLUSHINFO_VERSION,!async,dbnum};
long long removed = 0;
if (dbnum < -1 || dbnum >= server.dbnum) {
errno = EINVAL;
return -1;
}
moduleFireServerEvent(REDISMODULE_EVENT_FLUSHDB,
REDISMODULE_SUBEVENT_FLUSHDB_START,
&fi);
signalFlushedDb(dbnum, async);
removed = emptyDbStructure(server.db, dbnum, async, callback);
if (server.cluster_enabled) slotToKeyFlush(server.db);
if (dbnum == -1) flushSlaveKeysWithExpireList();
if (with_functions) {
serverAssert(dbnum == -1);
functionsLibCtxClearCurrent(async);
}
moduleFireServerEvent(REDISMODULE_EVENT_FLUSHDB,
REDISMODULE_SUBEVENT_FLUSHDB_END,
&fi);
return removed;
}
redisDb *initTempDb(void) {
redisDb *tempDb = zcalloc(sizeof(redisDb)*server.dbnum);
for (int i=0; i<server.dbnum; i++) {
tempDb[i].dict = dictCreate(&dbDictType);
tempDb[i].expires = dictCreate(&dbExpiresDictType);
tempDb[i].slots_to_keys = NULL;
}
if (server.cluster_enabled) {
slotToKeyInit(tempDb);
}
return tempDb;
}
void discardTempDb(redisDb *tempDb, void(callback)(dict*)) {
int async = 1;
emptyDbStructure(tempDb, -1, async, callback);
for (int i=0; i<server.dbnum; i++) {
dictRelease(tempDb[i].dict);
dictRelease(tempDb[i].expires);
}
if (server.cluster_enabled) {
slotToKeyDestroy(tempDb);
}
zfree(tempDb);
}
int selectDb(client *c, int id) {
if (id < 0 || id >= server.dbnum)
return C_ERR;
c->db = &server.db[id];
return C_OK;
}
long long dbTotalServerKeyCount() {
long long total = 0;
int j;
for (j = 0; j < server.dbnum; j++) {
total += dictSize(server.db[j].dict);
}
return total;
}
void signalModifiedKey(client *c, redisDb *db, robj *key) {
touchWatchedKey(db,key);
trackingInvalidateKey(c,key,1);
}
void signalFlushedDb(int dbid, int async) {
int startdb, enddb;
if (dbid == -1) {
startdb = 0;
enddb = server.dbnum-1;
} else {
startdb = enddb = dbid;
}
for (int j = startdb; j <= enddb; j++) {
touchAllWatchedKeysInDb(&server.db[j], NULL);
}
trackingInvalidateKeysOnFlush(async);
}
int getFlushCommandFlags(client *c, int *flags) {
if (c->argc == 2 && !strcasecmp(c->argv[1]->ptr,"sync")) {
*flags = EMPTYDB_NO_FLAGS;
} else if (c->argc == 2 && !strcasecmp(c->argv[1]->ptr,"async")) {
*flags = EMPTYDB_ASYNC;
} else if (c->argc == 1) {
*flags = server.lazyfree_lazy_user_flush ? EMPTYDB_ASYNC : EMPTYDB_NO_FLAGS;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return C_ERR;
}
return C_OK;
}
void flushAllDataAndResetRDB(int flags) {
server.dirty += emptyData(-1,flags,NULL);
if (server.child_type == CHILD_TYPE_RDB) killRDBChild();
if (server.saveparamslen > 0) {
int saved_dirty = server.dirty;
rdbSaveInfo rsi, *rsiptr;
rsiptr = rdbPopulateSaveInfo(&rsi);
rdbSave(SLAVE_REQ_NONE,server.rdb_filename,rsiptr);
server.dirty = saved_dirty;
}
server.dirty++;
#if defined(USE_JEMALLOC)
if (!(flags & EMPTYDB_ASYNC))
jemalloc_purge();
#endif
}
void flushdbCommand(client *c) {
int flags;
if (getFlushCommandFlags(c,&flags) == C_ERR) return;
server.dirty += emptyData(c->db->id,flags | EMPTYDB_NOFUNCTIONS,NULL);
addReply(c,shared.ok);
#if defined(USE_JEMALLOC)
if (!(flags & EMPTYDB_ASYNC))
jemalloc_purge();
#endif
}
void flushallCommand(client *c) {
int flags;
if (getFlushCommandFlags(c,&flags) == C_ERR) return;
flushAllDataAndResetRDB(flags | EMPTYDB_NOFUNCTIONS);
addReply(c,shared.ok);
}
void delGenericCommand(client *c, int lazy) {
int numdel = 0, j;
for (j = 1; j < c->argc; j++) {
expireIfNeeded(c->db,c->argv[j],0);
int deleted = lazy ? dbAsyncDelete(c->db,c->argv[j]) :
dbSyncDelete(c->db,c->argv[j]);
if (deleted) {
signalModifiedKey(c,c->db,c->argv[j]);
notifyKeyspaceEvent(NOTIFY_GENERIC,
"del",c->argv[j],c->db->id);
server.dirty++;
numdel++;
}
}
addReplyLongLong(c,numdel);
}
void delCommand(client *c) {
delGenericCommand(c,server.lazyfree_lazy_user_del);
}
void unlinkCommand(client *c) {
delGenericCommand(c,1);
}
void existsCommand(client *c) {
long long count = 0;
int j;
for (j = 1; j < c->argc; j++) {
if (lookupKeyReadWithFlags(c->db,c->argv[j],LOOKUP_NOTOUCH)) count++;
}
addReplyLongLong(c,count);
}
void selectCommand(client *c) {
int id;
if (getIntFromObjectOrReply(c, c->argv[1], &id, NULL) != C_OK)
return;
if (server.cluster_enabled && id != 0) {
addReplyError(c,"SELECT is not allowed in cluster mode");
return;
}
if (selectDb(c,id) == C_ERR) {
addReplyError(c,"DB index is out of range");
} else {
addReply(c,shared.ok);
}
}
void randomkeyCommand(client *c) {
robj *key;
if ((key = dbRandomKey(c->db)) == NULL) {
addReplyNull(c);
return;
}
addReplyBulk(c,key);
decrRefCount(key);
}
void keysCommand(client *c) {
dictIterator *di;
dictEntry *de;
sds pattern = c->argv[1]->ptr;
int plen = sdslen(pattern), allkeys;
unsigned long numkeys = 0;
void *replylen = addReplyDeferredLen(c);
di = dictGetSafeIterator(c->db->dict);
allkeys = (pattern[0] == '*' && plen == 1);
while((de = dictNext(di)) != NULL) {
sds key = dictGetKey(de);
robj *keyobj;
if (allkeys || stringmatchlen(pattern,plen,key,sdslen(key),0)) {
keyobj = createStringObject(key,sdslen(key));
if (!keyIsExpired(c->db,keyobj)) {
addReplyBulk(c,keyobj);
numkeys++;
}
decrRefCount(keyobj);
}
}
dictReleaseIterator(di);
setDeferredArrayLen(c,replylen,numkeys);
}
void scanCallback(void *privdata, const dictEntry *de) {
void **pd = (void**) privdata;
list *keys = pd[0];
robj *o = pd[1];
robj *key, *val = NULL;
if (o == NULL) {
sds sdskey = dictGetKey(de);
key = createStringObject(sdskey, sdslen(sdskey));
} else if (o->type == OBJ_SET) {
sds keysds = dictGetKey(de);
key = createStringObject(keysds,sdslen(keysds));
} else if (o->type == OBJ_HASH) {
sds sdskey = dictGetKey(de);
sds sdsval = dictGetVal(de);
key = createStringObject(sdskey,sdslen(sdskey));
val = createStringObject(sdsval,sdslen(sdsval));
} else if (o->type == OBJ_ZSET) {
sds sdskey = dictGetKey(de);
key = createStringObject(sdskey,sdslen(sdskey));
val = createStringObjectFromLongDouble(*(double*)dictGetVal(de),0);
} else {
serverPanic("Type not handled in SCAN callback.");
}
listAddNodeTail(keys, key);
if (val) listAddNodeTail(keys, val);
}
int parseScanCursorOrReply(client *c, robj *o, unsigned long *cursor) {
char *eptr;
errno = 0;
*cursor = strtoul(o->ptr, &eptr, 10);
if (isspace(((char*)o->ptr)[0]) || eptr[0] != '\0' || errno == ERANGE)
{
addReplyError(c, "invalid cursor");
return C_ERR;
}
return C_OK;
}
void scanGenericCommand(client *c, robj *o, unsigned long cursor) {
int i, j;
list *keys = listCreate();
listNode *node, *nextnode;
long count = 10;
sds pat = NULL;
sds typename = NULL;
int patlen = 0, use_pattern = 0;
dict *ht;
serverAssert(o == NULL || o->type == OBJ_SET || o->type == OBJ_HASH ||
o->type == OBJ_ZSET);
i = (o == NULL) ? 2 : 3;
while (i < c->argc) {
j = c->argc - i;
if (!strcasecmp(c->argv[i]->ptr, "count") && j >= 2) {
if (getLongFromObjectOrReply(c, c->argv[i+1], &count, NULL)
!= C_OK)
{
goto cleanup;
}
if (count < 1) {
addReplyErrorObject(c,shared.syntaxerr);
goto cleanup;
}
i += 2;
} else if (!strcasecmp(c->argv[i]->ptr, "match") && j >= 2) {
pat = c->argv[i+1]->ptr;
patlen = sdslen(pat);
use_pattern = !(patlen == 1 && pat[0] == '*');
i += 2;
} else if (!strcasecmp(c->argv[i]->ptr, "type") && o == NULL && j >= 2) {
typename = c->argv[i+1]->ptr;
i+= 2;
} else {
addReplyErrorObject(c,shared.syntaxerr);
goto cleanup;
}
}
ht = NULL;
if (o == NULL) {
ht = c->db->dict;
} else if (o->type == OBJ_SET && o->encoding == OBJ_ENCODING_HT) {
ht = o->ptr;
} else if (o->type == OBJ_HASH && o->encoding == OBJ_ENCODING_HT) {
ht = o->ptr;
count *= 2;
} else if (o->type == OBJ_ZSET && o->encoding == OBJ_ENCODING_SKIPLIST) {
zset *zs = o->ptr;
ht = zs->dict;
count *= 2;
}
if (ht) {
void *privdata[2];
long maxiterations = count*10;
privdata[0] = keys;
privdata[1] = o;
do {
cursor = dictScan(ht, cursor, scanCallback, NULL, privdata);
} while (cursor &&
maxiterations-- &&
listLength(keys) < (unsigned long)count);
} else if (o->type == OBJ_SET) {
int pos = 0;
int64_t ll;
while(intsetGet(o->ptr,pos++,&ll))
listAddNodeTail(keys,createStringObjectFromLongLong(ll));
cursor = 0;
} else if (o->type == OBJ_HASH || o->type == OBJ_ZSET) {
unsigned char *p = lpFirst(o->ptr);
unsigned char *vstr;
int64_t vlen;
unsigned char intbuf[LP_INTBUF_SIZE];
while(p) {
vstr = lpGet(p,&vlen,intbuf);
listAddNodeTail(keys, createStringObject((char*)vstr,vlen));
p = lpNext(o->ptr,p);
}
cursor = 0;
} else {
serverPanic("Not handled encoding in SCAN.");
}
node = listFirst(keys);
while (node) {
robj *kobj = listNodeValue(node);
nextnode = listNextNode(node);
int filter = 0;
if (use_pattern) {
if (sdsEncodedObject(kobj)) {
if (!stringmatchlen(pat, patlen, kobj->ptr, sdslen(kobj->ptr), 0))
filter = 1;
} else {
char buf[LONG_STR_SIZE];
int len;
serverAssert(kobj->encoding == OBJ_ENCODING_INT);
len = ll2string(buf,sizeof(buf),(long)kobj->ptr);
if (!stringmatchlen(pat, patlen, buf, len, 0)) filter = 1;
}
}
if (!filter && o == NULL && typename){
robj* typecheck = lookupKeyReadWithFlags(c->db, kobj, LOOKUP_NOTOUCH);
char* type = getObjectTypeName(typecheck);
if (strcasecmp((char*) typename, type)) filter = 1;
}
if (!filter && o == NULL && expireIfNeeded(c->db, kobj, 0)) filter = 1;
if (filter) {
decrRefCount(kobj);
listDelNode(keys, node);
}
if (o && (o->type == OBJ_ZSET || o->type == OBJ_HASH)) {
node = nextnode;
serverAssert(node);
nextnode = listNextNode(node);
if (filter) {
kobj = listNodeValue(node);
decrRefCount(kobj);
listDelNode(keys, node);
}
}
node = nextnode;
}
addReplyArrayLen(c, 2);
addReplyBulkLongLong(c,cursor);
addReplyArrayLen(c, listLength(keys));
while ((node = listFirst(keys)) != NULL) {
robj *kobj = listNodeValue(node);
addReplyBulk(c, kobj);
decrRefCount(kobj);
listDelNode(keys, node);
}
cleanup:
listSetFreeMethod(keys,decrRefCountVoid);
listRelease(keys);
}
void scanCommand(client *c) {
unsigned long cursor;
if (parseScanCursorOrReply(c,c->argv[1],&cursor) == C_ERR) return;
scanGenericCommand(c,NULL,cursor);
}
void dbsizeCommand(client *c) {
addReplyLongLong(c,dictSize(c->db->dict));
}
void lastsaveCommand(client *c) {
addReplyLongLong(c,server.lastsave);
}
char* getObjectTypeName(robj *o) {
char* type;
if (o == NULL) {
type = "none";
} else {
switch(o->type) {
case OBJ_STRING: type = "string"; break;
case OBJ_LIST: type = "list"; break;
case OBJ_SET: type = "set"; break;
case OBJ_ZSET: type = "zset"; break;
case OBJ_HASH: type = "hash"; break;
case OBJ_STREAM: type = "stream"; break;
case OBJ_MODULE: {
moduleValue *mv = o->ptr;
type = mv->type->name;
}; break;
default: type = "unknown"; break;
}
}
return type;
}
void typeCommand(client *c) {
robj *o;
o = lookupKeyReadWithFlags(c->db,c->argv[1],LOOKUP_NOTOUCH);
addReplyStatus(c, getObjectTypeName(o));
}
void shutdownCommand(client *c) {
int flags = SHUTDOWN_NOFLAGS;
int abort = 0;
for (int i = 1; i < c->argc; i++) {
if (!strcasecmp(c->argv[i]->ptr,"nosave")) {
flags |= SHUTDOWN_NOSAVE;
} else if (!strcasecmp(c->argv[i]->ptr,"save")) {
flags |= SHUTDOWN_SAVE;
} else if (!strcasecmp(c->argv[i]->ptr, "now")) {
flags |= SHUTDOWN_NOW;
} else if (!strcasecmp(c->argv[i]->ptr, "force")) {
flags |= SHUTDOWN_FORCE;
} else if (!strcasecmp(c->argv[i]->ptr, "abort")) {
abort = 1;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}
if ((abort && flags != SHUTDOWN_NOFLAGS) ||
(flags & SHUTDOWN_NOSAVE && flags & SHUTDOWN_SAVE))
{
addReplyErrorObject(c,shared.syntaxerr);
return;
}
if (abort) {
if (abortShutdown() == C_OK)
addReply(c, shared.ok);
else
addReplyError(c, "No shutdown in progress.");
return;
}
if (!(flags & SHUTDOWN_NOW) && c->flags & CLIENT_DENY_BLOCKING) {
addReplyError(c, "SHUTDOWN without NOW or ABORT isn't allowed for DENY BLOCKING client");
return;
}
if (!(flags & SHUTDOWN_NOSAVE) && scriptIsTimedout()) {
if (server.busy_module_yield_flags && server.busy_module_yield_reply) {
addReplyErrorFormat(c, "-BUSY %s", server.busy_module_yield_reply);
} else if (server.busy_module_yield_flags) {
addReplyErrorObject(c, shared.slowmoduleerr);
} else if (scriptIsEval()) {
addReplyErrorObject(c, shared.slowevalerr);
} else {
addReplyErrorObject(c, shared.slowscripterr);
}
return;
}
blockClient(c, BLOCKED_SHUTDOWN);
if (prepareForShutdown(flags) == C_OK) exit(0);
}
void renameGenericCommand(client *c, int nx) {
robj *o;
long long expire;
int samekey = 0;
if (sdscmp(c->argv[1]->ptr,c->argv[2]->ptr) == 0) samekey = 1;
if ((o = lookupKeyWriteOrReply(c,c->argv[1],shared.nokeyerr)) == NULL)
return;
if (samekey) {
addReply(c,nx ? shared.czero : shared.ok);
return;
}
incrRefCount(o);
expire = getExpire(c->db,c->argv[1]);
if (lookupKeyWrite(c->db,c->argv[2]) != NULL) {
if (nx) {
decrRefCount(o);
addReply(c,shared.czero);
return;
}
dbDelete(c->db,c->argv[2]);
}
dbAdd(c->db,c->argv[2],o);
if (expire != -1) setExpire(c,c->db,c->argv[2],expire);
dbDelete(c->db,c->argv[1]);
signalModifiedKey(c,c->db,c->argv[1]);
signalModifiedKey(c,c->db,c->argv[2]);
notifyKeyspaceEvent(NOTIFY_GENERIC,"rename_from",
c->argv[1],c->db->id);
notifyKeyspaceEvent(NOTIFY_GENERIC,"rename_to",
c->argv[2],c->db->id);
server.dirty++;
addReply(c,nx ? shared.cone : shared.ok);
}
void renameCommand(client *c) {
renameGenericCommand(c,0);
}
void renamenxCommand(client *c) {
renameGenericCommand(c,1);
}
void moveCommand(client *c) {
robj *o;
redisDb *src, *dst;
int srcid, dbid;
long long expire;
if (server.cluster_enabled) {
addReplyError(c,"MOVE is not allowed in cluster mode");
return;
}
src = c->db;
srcid = c->db->id;
if (getIntFromObjectOrReply(c, c->argv[2], &dbid, NULL) != C_OK)
return;
if (selectDb(c,dbid) == C_ERR) {
addReplyError(c,"DB index is out of range");
return;
}
dst = c->db;
selectDb(c,srcid);
if (src == dst) {
addReplyErrorObject(c,shared.sameobjecterr);
return;
}
o = lookupKeyWrite(c->db,c->argv[1]);
if (!o) {
addReply(c,shared.czero);
return;
}
expire = getExpire(c->db,c->argv[1]);
if (lookupKeyWrite(dst,c->argv[1]) != NULL) {
addReply(c,shared.czero);
return;
}
dbAdd(dst,c->argv[1],o);
if (expire != -1) setExpire(c,dst,c->argv[1],expire);
incrRefCount(o);
dbDelete(src,c->argv[1]);
signalModifiedKey(c,src,c->argv[1]);
signalModifiedKey(c,dst,c->argv[1]);
notifyKeyspaceEvent(NOTIFY_GENERIC,
"move_from",c->argv[1],src->id);
notifyKeyspaceEvent(NOTIFY_GENERIC,
"move_to",c->argv[1],dst->id);
server.dirty++;
addReply(c,shared.cone);
}
void copyCommand(client *c) {
robj *o;
redisDb *src, *dst;
int srcid, dbid;
long long expire;
int j, replace = 0, delete = 0;
src = c->db;
dst = c->db;
srcid = c->db->id;
dbid = c->db->id;
for (j = 3; j < c->argc; j++) {
int additional = c->argc - j - 1;
if (!strcasecmp(c->argv[j]->ptr,"replace")) {
replace = 1;
} else if (!strcasecmp(c->argv[j]->ptr, "db") && additional >= 1) {
if (getIntFromObjectOrReply(c, c->argv[j+1], &dbid, NULL) != C_OK)
return;
if (selectDb(c, dbid) == C_ERR) {
addReplyError(c,"DB index is out of range");
return;
}
dst = c->db;
selectDb(c,srcid);
j++;
} else {
addReplyErrorObject(c,shared.syntaxerr);
return;
}
}
if ((server.cluster_enabled == 1) && (srcid != 0 || dbid != 0)) {
addReplyError(c,"Copying to another database is not allowed in cluster mode");
return;
}
robj *key = c->argv[1];
robj *newkey = c->argv[2];
if (src == dst && (sdscmp(key->ptr, newkey->ptr) == 0)) {
addReplyErrorObject(c,shared.sameobjecterr);
return;
}
o = lookupKeyRead(c->db, key);
if (!o) {
addReply(c,shared.czero);
return;
}
expire = getExpire(c->db,key);
if (lookupKeyWrite(dst,newkey) != NULL) {
if (replace) {
delete = 1;
} else {
addReply(c,shared.czero);
return;
}
}
robj *newobj;
switch(o->type) {
case OBJ_STRING: newobj = dupStringObject(o); break;
case OBJ_LIST: newobj = listTypeDup(o); break;
case OBJ_SET: newobj = setTypeDup(o); break;
case OBJ_ZSET: newobj = zsetDup(o); break;
case OBJ_HASH: newobj = hashTypeDup(o); break;
case OBJ_STREAM: newobj = streamDup(o); break;
case OBJ_MODULE:
newobj = moduleTypeDupOrReply(c, key, newkey, dst->id, o);
if (!newobj) return;
break;
default:
addReplyError(c, "unknown type object");
return;
}
if (delete) {
dbDelete(dst,newkey);
}
dbAdd(dst,newkey,newobj);
if (expire != -1) setExpire(c, dst, newkey, expire);
signalModifiedKey(c,dst,c->argv[2]);
notifyKeyspaceEvent(NOTIFY_GENERIC,"copy_to",c->argv[2],dst->id);
server.dirty++;
addReply(c,shared.cone);
}
void scanDatabaseForReadyLists(redisDb *db) {
dictEntry *de;
dictIterator *di = dictGetSafeIterator(db->blocking_keys);
while((de = dictNext(di)) != NULL) {
robj *key = dictGetKey(de);
dictEntry *kde = dictFind(db->dict,key->ptr);
if (kde) {
robj *value = dictGetVal(kde);
signalKeyAsReady(db, key, value->type);
}
}
dictReleaseIterator(di);
}
int dbSwapDatabases(int id1, int id2) {
if (id1 < 0 || id1 >= server.dbnum ||
id2 < 0 || id2 >= server.dbnum) return C_ERR;
if (id1 == id2) return C_OK;
redisDb aux = server.db[id1];
redisDb *db1 = &server.db[id1], *db2 = &server.db[id2];
db1->dict = db2->dict;
db1->expires = db2->expires;
db1->avg_ttl = db2->avg_ttl;
db1->expires_cursor = db2->expires_cursor;
db2->dict = aux.dict;
db2->expires = aux.expires;
db2->avg_ttl = aux.avg_ttl;
db2->expires_cursor = aux.expires_cursor;
scanDatabaseForReadyLists(db1);
touchAllWatchedKeysInDb(db1, db2);
scanDatabaseForReadyLists(db2);
touchAllWatchedKeysInDb(db2, db1);
return C_OK;
}
void swapMainDbWithTempDb(redisDb *tempDb) {
if (server.cluster_enabled) {
clusterSlotToKeyMapping *aux = server.db->slots_to_keys;
server.db->slots_to_keys = tempDb->slots_to_keys;
tempDb->slots_to_keys = aux;
}
for (int i=0; i<server.dbnum; i++) {
redisDb aux = server.db[i];
redisDb *activedb = &server.db[i], *newdb = &tempDb[i];
activedb->dict = newdb->dict;
activedb->expires = newdb->expires;
activedb->avg_ttl = newdb->avg_ttl;
activedb->expires_cursor = newdb->expires_cursor;
newdb->dict = aux.dict;
newdb->expires = aux.expires;
newdb->avg_ttl = aux.avg_ttl;
newdb->expires_cursor = aux.expires_cursor;
scanDatabaseForReadyLists(activedb);
touchAllWatchedKeysInDb(activedb, newdb);
}
trackingInvalidateKeysOnFlush(1);
flushSlaveKeysWithExpireList();
}
void swapdbCommand(client *c) {
int id1, id2;
if (server.cluster_enabled) {
addReplyError(c,"SWAPDB is not allowed in cluster mode");
return;
}
if (getIntFromObjectOrReply(c, c->argv[1], &id1,
"invalid first DB index") != C_OK)
return;
if (getIntFromObjectOrReply(c, c->argv[2], &id2,
"invalid second DB index") != C_OK)
return;
if (dbSwapDatabases(id1,id2) == C_ERR) {
addReplyError(c,"DB index is out of range");
return;
} else {
RedisModuleSwapDbInfo si = {REDISMODULE_SWAPDBINFO_VERSION,id1,id2};
moduleFireServerEvent(REDISMODULE_EVENT_SWAPDB,0,&si);
server.dirty++;
addReply(c,shared.ok);
}
}
int removeExpire(redisDb *db, robj *key) {
serverAssertWithInfo(NULL,key,dictFind(db->dict,key->ptr) != NULL);
return dictDelete(db->expires,key->ptr) == DICT_OK;
}
void setExpire(client *c, redisDb *db, robj *key, long long when) {
dictEntry *kde, *de;
kde = dictFind(db->dict,key->ptr);
serverAssertWithInfo(NULL,key,kde != NULL);
de = dictAddOrFind(db->expires,dictGetKey(kde));
dictSetSignedIntegerVal(de,when);
int writable_slave = server.masterhost && server.repl_slave_ro == 0;
if (c && writable_slave && !(c->flags & CLIENT_MASTER))
rememberSlaveKeyWithExpire(db,key);
}
long long getExpire(redisDb *db, robj *key) {
dictEntry *de;
if (dictSize(db->expires) == 0 ||
(de = dictFind(db->expires,key->ptr)) == NULL) return -1;
serverAssertWithInfo(NULL,key,dictFind(db->dict,key->ptr) != NULL);
return dictGetSignedIntegerVal(de);
}
void deleteExpiredKeyAndPropagate(redisDb *db, robj *keyobj) {
mstime_t expire_latency;
latencyStartMonitor(expire_latency);
if (server.lazyfree_lazy_expire)
dbAsyncDelete(db,keyobj);
else
dbSyncDelete(db,keyobj);
latencyEndMonitor(expire_latency);
latencyAddSampleIfNeeded("expire-del",expire_latency);
notifyKeyspaceEvent(NOTIFY_EXPIRED,"expired",keyobj,db->id);
signalModifiedKey(NULL, db, keyobj);
propagateDeletion(db,keyobj,server.lazyfree_lazy_expire);
server.stat_expiredkeys++;
}
void propagateDeletion(redisDb *db, robj *key, int lazy) {
robj *argv[2];
argv[0] = lazy ? shared.unlink : shared.del;
argv[1] = key;
incrRefCount(argv[0]);
incrRefCount(argv[1]);
int prev_replication_allowed = server.replication_allowed;
server.replication_allowed = 1;
alsoPropagate(db->id,argv,2,PROPAGATE_AOF|PROPAGATE_REPL);
server.replication_allowed = prev_replication_allowed;
decrRefCount(argv[0]);
decrRefCount(argv[1]);
}
int keyIsExpired(redisDb *db, robj *key) {
mstime_t when = getExpire(db,key);
mstime_t now;
if (when < 0) return 0;
if (server.loading) return 0;
if (server.script_caller) {
now = scriptTimeSnapshot();
}
else if (server.fixed_time_expire > 0) {
now = server.mstime;
}
else {
now = mstime();
}
return now > when;
}
int expireIfNeeded(redisDb *db, robj *key, int force_delete_expired) {
if (!keyIsExpired(db,key)) return 0;
if (server.masterhost != NULL) {
if (server.current_client == server.master) return 0;
if (!force_delete_expired) return 1;
}
if (checkClientPauseTimeoutAndReturnIfPaused()) return 1;
deleteExpiredKeyAndPropagate(db,key);
return 1;
}
keyReference *getKeysPrepareResult(getKeysResult *result, int numkeys) {
if (!result->keys) {
serverAssert(!result->numkeys);
result->keys = result->keysbuf;
}
if (numkeys > result->size) {
if (result->keys != result->keysbuf) {
result->keys = zrealloc(result->keys, numkeys * sizeof(keyReference));
} else {
result->keys = zmalloc(numkeys * sizeof(keyReference));
if (result->numkeys)
memcpy(result->keys, result->keysbuf, result->numkeys * sizeof(keyReference));
}
result->size = numkeys;
}
return result->keys;
}
int64_t getAllKeySpecsFlags(struct redisCommand *cmd, int inv) {
int64_t flags = 0;
for (int j = 0; j < cmd->key_specs_num; j++) {
keySpec *spec = cmd->key_specs + j;
flags |= inv? ~spec->flags : spec->flags;
}
return flags;
}
int getKeysUsingKeySpecs(struct redisCommand *cmd, robj **argv, int argc, int search_flags, getKeysResult *result) {
int j, i, k = 0, last, first, step;
keyReference *keys;
for (j = 0; j < cmd->key_specs_num; j++) {
keySpec *spec = cmd->key_specs + j;
serverAssert(spec->begin_search_type != KSPEC_BS_INVALID);
if (spec->flags & (CMD_KEY_CHANNEL) && !(search_flags & GET_KEYSPEC_INCLUDE_CHANNELS)) {
continue;
}
first = 0;
if (spec->begin_search_type == KSPEC_BS_INDEX) {
first = spec->bs.index.pos;
} else if (spec->begin_search_type == KSPEC_BS_KEYWORD) {
int start_index = spec->bs.keyword.startfrom > 0 ? spec->bs.keyword.startfrom : argc+spec->bs.keyword.startfrom;
int end_index = spec->bs.keyword.startfrom > 0 ? argc-1: 1;
for (i = start_index; i != end_index; i = start_index <= end_index ? i + 1 : i - 1) {
if (i >= argc || i < 1)
break;
if (!strcasecmp((char*)argv[i]->ptr,spec->bs.keyword.keyword)) {
first = i+1;
break;
}
}
if (!first) {
continue;
}
} else {
goto invalid_spec;
}
if (spec->find_keys_type == KSPEC_FK_RANGE) {
step = spec->fk.range.keystep;
if (spec->fk.range.lastkey >= 0) {
last = first + spec->fk.range.lastkey;
} else {
if (!spec->fk.range.limit) {
last = argc + spec->fk.range.lastkey;
} else {
serverAssert(spec->fk.range.lastkey == -1);
last = first + ((argc-first)/spec->fk.range.limit + spec->fk.range.lastkey);
}
}
} else if (spec->find_keys_type == KSPEC_FK_KEYNUM) {
step = spec->fk.keynum.keystep;
long long numkeys;
if (spec->fk.keynum.keynumidx >= argc)
goto invalid_spec;
sds keynum_str = argv[first + spec->fk.keynum.keynumidx]->ptr;
if (!string2ll(keynum_str,sdslen(keynum_str),&numkeys) || numkeys < 0) {
goto invalid_spec;
}
first += spec->fk.keynum.firstkey;
last = first + (int)numkeys-1;
} else {
goto invalid_spec;
}
int count = ((last - first)+1);
keys = getKeysPrepareResult(result, count);
if (last >= argc || last < first || first >= argc) {
goto invalid_spec;
}
for (i = first; i <= last; i += step) {
if (i >= argc || i < first) {
if (cmd->flags & CMD_MODULE || cmd->arity < 0) {
continue;
} else {
serverPanic("Redis built-in command declared keys positions not matching the arity requirements.");
}
}
keys[k].pos = i;
keys[k++].flags = spec->flags;
}
continue;
invalid_spec:
if (search_flags & GET_KEYSPEC_RETURN_PARTIAL) {
continue;
} else {
result->numkeys = 0;
return -1;
}
}
result->numkeys = k;
return k;
}
int getKeysFromCommandWithSpecs(struct redisCommand *cmd, robj **argv, int argc, int search_flags, getKeysResult *result) {
if (cmd->flags & CMD_MODULE_GETKEYS) {
return moduleGetCommandKeysViaAPI(cmd,argv,argc,result);
} else {
if (!(getAllKeySpecsFlags(cmd, 0) & CMD_KEY_INCOMPLETE)) {
int ret = getKeysUsingKeySpecs(cmd,argv,argc,search_flags,result);
if (ret >= 0)
return ret;
}
if (!(cmd->flags & CMD_MODULE) && cmd->getkeys_proc)
return cmd->getkeys_proc(cmd,argv,argc,result);
return 0;
}
}
int doesCommandHaveKeys(struct redisCommand *cmd) {
return (!(cmd->flags & CMD_MODULE) && cmd->getkeys_proc) ||
(cmd->flags & CMD_MODULE_GETKEYS) ||
(getAllKeySpecsFlags(cmd, 1) & CMD_KEY_CHANNEL);
}
int getKeysUsingLegacyRangeSpec(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
int j, i = 0, last, first, step;
keyReference *keys;
UNUSED(argv);
if (cmd->legacy_range_key_spec.begin_search_type == KSPEC_BS_INVALID) {
result->numkeys = 0;
return 0;
}
first = cmd->legacy_range_key_spec.bs.index.pos;
last = cmd->legacy_range_key_spec.fk.range.lastkey;
if (last >= 0)
last += first;
step = cmd->legacy_range_key_spec.fk.range.keystep;
if (last < 0) last = argc+last;
int count = ((last - first)+1);
keys = getKeysPrepareResult(result, count);
for (j = first; j <= last; j += step) {
if (j >= argc || j < first) {
if (cmd->flags & CMD_MODULE || cmd->arity < 0) {
result->numkeys = 0;
return 0;
} else {
serverPanic("Redis built-in command declared keys positions not matching the arity requirements.");
}
}
keys[i].pos = j;
keys[i++].flags = 0;
}
result->numkeys = i;
return i;
}
int getKeysFromCommand(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
if (cmd->flags & CMD_MODULE_GETKEYS) {
return moduleGetCommandKeysViaAPI(cmd,argv,argc,result);
} else if (!(cmd->flags & CMD_MODULE) && cmd->getkeys_proc) {
return cmd->getkeys_proc(cmd,argv,argc,result);
} else {
return getKeysUsingLegacyRangeSpec(cmd,argv,argc,result);
}
}
void getKeysFreeResult(getKeysResult *result) {
if (result && result->keys != result->keysbuf)
zfree(result->keys);
}
int genericGetKeys(int storeKeyOfs, int keyCountOfs, int firstKeyOfs, int keyStep,
robj **argv, int argc, getKeysResult *result) {
int i, num;
keyReference *keys;
num = atoi(argv[keyCountOfs]->ptr);
if (num < 1 || num > (argc - firstKeyOfs)/keyStep) {
result->numkeys = 0;
return 0;
}
int numkeys = storeKeyOfs ? num + 1 : num;
keys = getKeysPrepareResult(result, numkeys);
result->numkeys = numkeys;
for (i = 0; i < num; i++) {
keys[i].pos = firstKeyOfs+(i*keyStep);
keys[i].flags = 0;
}
if (storeKeyOfs) {
keys[num].pos = storeKeyOfs;
keys[num].flags = 0;
}
return result->numkeys;
}
int sintercardGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 1, 2, 1, argv, argc, result);
}
int zunionInterDiffStoreGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(1, 2, 3, 1, argv, argc, result);
}
int zunionInterDiffGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 1, 2, 1, argv, argc, result);
}
int evalGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 2, 3, 1, argv, argc, result);
}
int functionGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 2, 3, 1, argv, argc, result);
}
int lmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 1, 2, 1, argv, argc, result);
}
int blmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 2, 3, 1, argv, argc, result);
}
int zmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 1, 2, 1, argv, argc, result);
}
int bzmpopGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
UNUSED(cmd);
return genericGetKeys(0, 2, 3, 1, argv, argc, result);
}
int sortROGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
keyReference *keys;
UNUSED(cmd);
UNUSED(argv);
UNUSED(argc);
keys = getKeysPrepareResult(result, 1);
keys[0].pos = 1;
keys[0].flags = CMD_KEY_RO | CMD_KEY_ACCESS;
return 1;
}
int sortGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
int i, j, num, found_store = 0;
keyReference *keys;
UNUSED(cmd);
num = 0;
keys = getKeysPrepareResult(result, 2);
keys[num].pos = 1;
keys[num++].flags = CMD_KEY_RO | CMD_KEY_ACCESS;
struct {
char *name;
int skip;
} skiplist[] = {
{"limit", 2},
{"get", 1},
{"by", 1},
{NULL, 0}
};
for (i = 2; i < argc; i++) {
for (j = 0; skiplist[j].name != NULL; j++) {
if (!strcasecmp(argv[i]->ptr,skiplist[j].name)) {
i += skiplist[j].skip;
break;
} else if (!strcasecmp(argv[i]->ptr,"store") && i+1 < argc) {
found_store = 1;
keys[num].pos = i+1;
keys[num].flags = CMD_KEY_OW | CMD_KEY_UPDATE;
break;
}
}
}
result->numkeys = num + found_store;
return result->numkeys;
}
int migrateGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
int i, num, first;
keyReference *keys;
UNUSED(cmd);
first = 3;
num = 1;
if (argc > 6) {
for (i = 6; i < argc; i++) {
if (!strcasecmp(argv[i]->ptr,"keys") &&
sdslen(argv[3]->ptr) == 0)
{
first = i+1;
num = argc-first;
break;
}
}
}
keys = getKeysPrepareResult(result, num);
for (i = 0; i < num; i++) {
keys[i].pos = first+i;
keys[i].flags = CMD_KEY_RW | CMD_KEY_ACCESS | CMD_KEY_DELETE;
}
result->numkeys = num;
return num;
}
int georadiusGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
int i, num;
keyReference *keys;
UNUSED(cmd);
int stored_key = -1;
for (i = 5; i < argc; i++) {
char *arg = argv[i]->ptr;
if ((!strcasecmp(arg, "store") || !strcasecmp(arg, "storedist")) && ((i+1) < argc)) {
stored_key = i+1;
i++;
}
}
num = 1 + (stored_key == -1 ? 0 : 1);
keys = getKeysPrepareResult(result, num);
keys[0].pos = 1;
keys[0].flags = 0;
if(num > 1) {
keys[1].pos = stored_key;
keys[1].flags = 0;
}
result->numkeys = num;
return num;
}
int xreadGetKeys(struct redisCommand *cmd, robj **argv, int argc, getKeysResult *result) {
int i, num = 0;
keyReference *keys;
UNUSED(cmd);
int streams_pos = -1;
for (i = 1; i < argc; i++) {
char *arg = argv[i]->ptr;
if (!strcasecmp(arg, "block")) {
i++;
} else if (!strcasecmp(arg, "count")) {
i++;
} else if (!strcasecmp(arg, "group")) {
i += 2;
} else if (!strcasecmp(arg, "noack")) {
} else if (!strcasecmp(arg, "streams")) {
streams_pos = i;
break;
} else {
break;
}
}
if (streams_pos != -1) num = argc - streams_pos - 1;
if (streams_pos == -1 || num == 0 || num % 2 != 0) {
result->numkeys = 0;
return 0;
}
num /= 2;
keys = getKeysPrepareResult(result, num);
for (i = streams_pos+1; i < argc-num; i++) {
keys[i-streams_pos-1].pos = i;
keys[i-streams_pos-1].flags = 0;
}
result->numkeys = num;
return num;
}
