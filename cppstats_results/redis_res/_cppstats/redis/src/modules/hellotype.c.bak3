




































#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

static RedisModuleType *HelloType;







struct HelloTypeNode {
int64_t value;
struct HelloTypeNode *next;
};

struct HelloTypeObject {
struct HelloTypeNode *head;
size_t len;
};

struct HelloTypeObject *createHelloTypeObject(void) {
struct HelloTypeObject *o;
o = RedisModule_Alloc(sizeof(*o));
o->head = NULL;
o->len = 0;
return o;
}

void HelloTypeInsert(struct HelloTypeObject *o, int64_t ele) {
struct HelloTypeNode *next = o->head, *newnode, *prev = NULL;

while(next && next->value < ele) {
prev = next;
next = next->next;
}
newnode = RedisModule_Alloc(sizeof(*newnode));
newnode->value = ele;
newnode->next = next;
if (prev) {
prev->next = newnode;
} else {
o->head = newnode;
}
o->len++;
}

void HelloTypeReleaseObject(struct HelloTypeObject *o) {
struct HelloTypeNode *cur, *next;
cur = o->head;
while(cur) {
next = cur->next;
RedisModule_Free(cur);
cur = next;
}
RedisModule_Free(o);
}




int HelloTypeInsert_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);

if (argc != 3) return RedisModule_WrongArity(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY &&
RedisModule_ModuleTypeGetType(key) != HelloType)
{
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

long long value;
if ((RedisModule_StringToLongLong(argv[2],&value) != REDISMODULE_OK)) {
return RedisModule_ReplyWithError(ctx,"ERR invalid value: must be a signed 64 bit integer");
}


struct HelloTypeObject *hto;
if (type == REDISMODULE_KEYTYPE_EMPTY) {
hto = createHelloTypeObject();
RedisModule_ModuleTypeSetValue(key,HelloType,hto);
} else {
hto = RedisModule_ModuleTypeGetValue(key);
}


HelloTypeInsert(hto,value);
RedisModule_SignalKeyAsReady(ctx,argv[1]);

RedisModule_ReplyWithLongLong(ctx,hto->len);
RedisModule_ReplicateVerbatim(ctx);
return REDISMODULE_OK;
}


int HelloTypeRange_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);

if (argc != 4) return RedisModule_WrongArity(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY &&
RedisModule_ModuleTypeGetType(key) != HelloType)
{
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

long long first, count;
if (RedisModule_StringToLongLong(argv[2],&first) != REDISMODULE_OK ||
RedisModule_StringToLongLong(argv[3],&count) != REDISMODULE_OK ||
first < 0 || count < 0)
{
return RedisModule_ReplyWithError(ctx,
"ERR invalid first or count parameters");
}

struct HelloTypeObject *hto = RedisModule_ModuleTypeGetValue(key);
struct HelloTypeNode *node = hto ? hto->head : NULL;
RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_LEN);
long long arraylen = 0;
while(node && count--) {
RedisModule_ReplyWithLongLong(ctx,node->value);
arraylen++;
node = node->next;
}
RedisModule_ReplySetArrayLength(ctx,arraylen);
return REDISMODULE_OK;
}


int HelloTypeLen_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);

if (argc != 2) return RedisModule_WrongArity(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY &&
RedisModule_ModuleTypeGetType(key) != HelloType)
{
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

struct HelloTypeObject *hto = RedisModule_ModuleTypeGetValue(key);
RedisModule_ReplyWithLongLong(ctx,hto ? hto->len : 0);
return REDISMODULE_OK;
}






int HelloBlock_Reply(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);

RedisModuleString *keyname = RedisModule_GetBlockedClientReadyKey(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx,keyname,REDISMODULE_READ);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_MODULE ||
RedisModule_ModuleTypeGetType(key) != HelloType)
{
RedisModule_CloseKey(key);
return REDISMODULE_ERR;
}



RedisModule_CloseKey(key);
return HelloTypeRange_RedisCommand(ctx,argv,argc-1);
}


int HelloBlock_Timeout(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
return RedisModule_ReplyWithSimpleString(ctx,"Request timedout");
}


void HelloBlock_FreeData(RedisModuleCtx *ctx, void *privdata) {
REDISMODULE_NOT_USED(ctx);
RedisModule_Free(privdata);
}




int HelloTypeBRange_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 5) return RedisModule_WrongArity(ctx);
RedisModule_AutoMemory(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY &&
RedisModule_ModuleTypeGetType(key) != HelloType)
{
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}



long long timeout;
if (RedisModule_StringToLongLong(argv[4],&timeout) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx,
"ERR invalid timeout parameter");
}


if (type != REDISMODULE_KEYTYPE_EMPTY) {
return HelloTypeRange_RedisCommand(ctx,argv,argc-1);
}


void *privdata = RedisModule_Alloc(100);
RedisModule_BlockClientOnKeys(ctx,HelloBlock_Reply,HelloBlock_Timeout,HelloBlock_FreeData,timeout,argv+1,1,privdata);
return REDISMODULE_OK;
}



void *HelloTypeRdbLoad(RedisModuleIO *rdb, int encver) {
if (encver != 0) {

return NULL;
}
uint64_t elements = RedisModule_LoadUnsigned(rdb);
struct HelloTypeObject *hto = createHelloTypeObject();
while(elements--) {
int64_t ele = RedisModule_LoadSigned(rdb);
HelloTypeInsert(hto,ele);
}
return hto;
}

void HelloTypeRdbSave(RedisModuleIO *rdb, void *value) {
struct HelloTypeObject *hto = value;
struct HelloTypeNode *node = hto->head;
RedisModule_SaveUnsigned(rdb,hto->len);
while(node) {
RedisModule_SaveSigned(rdb,node->value);
node = node->next;
}
}

void HelloTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
struct HelloTypeObject *hto = value;
struct HelloTypeNode *node = hto->head;
while(node) {
RedisModule_EmitAOF(aof,"HELLOTYPE.INSERT","sl",key,node->value);
node = node->next;
}
}



size_t HelloTypeMemUsage(const void *value) {
const struct HelloTypeObject *hto = value;
struct HelloTypeNode *node = hto->head;
return sizeof(*hto) + sizeof(*node)*hto->len;
}

void HelloTypeFree(void *value) {
HelloTypeReleaseObject(value);
}

void HelloTypeDigest(RedisModuleDigest *md, void *value) {
struct HelloTypeObject *hto = value;
struct HelloTypeNode *node = hto->head;
while(node) {
RedisModule_DigestAddLongLong(md,node->value);
node = node->next;
}
RedisModule_DigestEndSequence(md);
}



int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);

if (RedisModule_Init(ctx,"hellotype",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;

RedisModuleTypeMethods tm = {
.version = REDISMODULE_TYPE_METHOD_VERSION,
.rdb_load = HelloTypeRdbLoad,
.rdb_save = HelloTypeRdbSave,
.aof_rewrite = HelloTypeAofRewrite,
.mem_usage = HelloTypeMemUsage,
.free = HelloTypeFree,
.digest = HelloTypeDigest
};

HelloType = RedisModule_CreateDataType(ctx,"hellotype",0,&tm);
if (HelloType == NULL) return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hellotype.insert",
HelloTypeInsert_RedisCommand,"write deny-oom",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hellotype.range",
HelloTypeRange_RedisCommand,"readonly",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hellotype.len",
HelloTypeLen_RedisCommand,"readonly",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hellotype.brange",
HelloTypeBRange_RedisCommand,"readonly",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

return REDISMODULE_OK;
}
