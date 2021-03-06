#include "redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
static RedisModuleType *MemAllocType;
#define MAX_DB 16
RedisModuleDict *mem_pool[MAX_DB];
typedef struct MemAllocObject {
long long size;
long long used;
uint64_t mask;
} MemAllocObject;
MemAllocObject *createMemAllocObject(void) {
MemAllocObject *o = RedisModule_Calloc(1, sizeof(*o));
return o;
}
#define BLOCK_SIZE 4096
struct MemBlock {
char block[BLOCK_SIZE];
struct MemBlock *next;
};
void MemBlockFree(struct MemBlock *head) {
if (head) {
struct MemBlock *block = head->next, *next;
RedisModule_Free(head);
while (block) {
next = block->next;
RedisModule_Free(block);
block = next;
}
}
}
struct MemBlock *MemBlockCreate(long long num) {
if (num <= 0) {
return NULL;
}
struct MemBlock *head = RedisModule_Calloc(1, sizeof(struct MemBlock));
struct MemBlock *block = head;
while (--num) {
block->next = RedisModule_Calloc(1, sizeof(struct MemBlock));
block = block->next;
}
return head;
}
long long MemBlockNum(const struct MemBlock *head) {
long long num = 0;
const struct MemBlock *block = head;
while (block) {
num++;
block = block->next;
}
return num;
}
size_t MemBlockWrite(struct MemBlock *head, long long block_index, const char *data, size_t size) {
size_t w_size = 0;
struct MemBlock *block = head;
while (block_index-- && block) {
block = block->next;
}
if (block) {
size = size > BLOCK_SIZE ? BLOCK_SIZE:size;
memcpy(block->block, data, size);
w_size += size;
}
return w_size;
}
int MemBlockRead(struct MemBlock *head, long long block_index, char *data, size_t size) {
size_t r_size = 0;
struct MemBlock *block = head;
while (block_index-- && block) {
block = block->next;
}
if (block) {
size = size > BLOCK_SIZE ? BLOCK_SIZE:size;
memcpy(data, block->block, size);
r_size += size;
}
return r_size;
}
void MemPoolFreeDb(RedisModuleCtx *ctx, int dbid) {
RedisModuleString *key;
void *tdata;
RedisModuleDictIter *iter = RedisModule_DictIteratorStartC(mem_pool[dbid], "^", NULL, 0);
while((key = RedisModule_DictNext(ctx, iter, &tdata)) != NULL) {
MemBlockFree((struct MemBlock *)tdata);
}
RedisModule_DictIteratorStop(iter);
RedisModule_FreeDict(NULL, mem_pool[dbid]);
mem_pool[dbid] = RedisModule_CreateDict(NULL);
}
struct MemBlock *MemBlockClone(const struct MemBlock *head) {
struct MemBlock *newhead = NULL;
if (head) {
newhead = RedisModule_Calloc(1, sizeof(struct MemBlock));
memcpy(newhead->block, head->block, BLOCK_SIZE);
struct MemBlock *newblock = newhead;
const struct MemBlock *oldblock = head->next;
while (oldblock) {
newblock->next = RedisModule_Calloc(1, sizeof(struct MemBlock));
newblock = newblock->next;
memcpy(newblock->block, oldblock->block, BLOCK_SIZE);
oldblock = oldblock->next;
}
}
return newhead;
}
void swapDbCallback(RedisModuleCtx *ctx, RedisModuleEvent e, uint64_t sub, void *data) {
REDISMODULE_NOT_USED(ctx);
REDISMODULE_NOT_USED(e);
REDISMODULE_NOT_USED(sub);
RedisModuleSwapDbInfo *ei = data;
RedisModuleDict *tmp = mem_pool[ei->dbnum_first];
mem_pool[ei->dbnum_first] = mem_pool[ei->dbnum_second];
mem_pool[ei->dbnum_second] = tmp;
}
void flushdbCallback(RedisModuleCtx *ctx, RedisModuleEvent e, uint64_t sub, void *data) {
REDISMODULE_NOT_USED(ctx);
REDISMODULE_NOT_USED(e);
int i;
RedisModuleFlushInfo *fi = data;
RedisModule_AutoMemory(ctx);
if (sub == REDISMODULE_SUBEVENT_FLUSHDB_START) {
if (fi->dbnum != -1) {
MemPoolFreeDb(ctx, fi->dbnum);
} else {
for (i = 0; i < MAX_DB; i++) {
MemPoolFreeDb(ctx, i);
}
}
}
}
int MemAlloc_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
if (argc != 3) {
return RedisModule_WrongArity(ctx);
}
long long block_num;
if ((RedisModule_StringToLongLong(argv[2], &block_num) != REDISMODULE_OK) || block_num <= 0) {
return RedisModule_ReplyWithError(ctx, "ERR invalid block_num: must be a value greater than 0");
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != MemAllocType) {
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
}
MemAllocObject *o;
if (type == REDISMODULE_KEYTYPE_EMPTY) {
o = createMemAllocObject();
RedisModule_ModuleTypeSetValue(key, MemAllocType, o);
} else {
o = RedisModule_ModuleTypeGetValue(key);
}
struct MemBlock *mem = MemBlockCreate(block_num);
RedisModule_Assert(mem != NULL);
RedisModule_DictSet(mem_pool[RedisModule_GetSelectedDb(ctx)], argv[1], mem);
o->size = block_num;
o->used = 0;
o->mask = 0;
RedisModule_ReplyWithLongLong(ctx, block_num);
RedisModule_ReplicateVerbatim(ctx);
return REDISMODULE_OK;
}
int MemFree_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
if (argc != 2) {
return RedisModule_WrongArity(ctx);
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != MemAllocType) {
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
}
int ret = 0;
MemAllocObject *o;
if (type == REDISMODULE_KEYTYPE_EMPTY) {
RedisModule_ReplyWithLongLong(ctx, ret);
return REDISMODULE_OK;
} else {
o = RedisModule_ModuleTypeGetValue(key);
}
int nokey;
struct MemBlock *mem = (struct MemBlock *)RedisModule_DictGet(mem_pool[RedisModule_GetSelectedDb(ctx)], argv[1], &nokey);
if (!nokey && mem) {
RedisModule_DictDel(mem_pool[RedisModule_GetSelectedDb(ctx)], argv[1], NULL);
MemBlockFree(mem);
o->used = 0;
o->size = 0;
o->mask = 0;
ret = 1;
}
RedisModule_ReplyWithLongLong(ctx, ret);
RedisModule_ReplicateVerbatim(ctx);
return REDISMODULE_OK;
}
int MemWrite_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
if (argc != 4) {
return RedisModule_WrongArity(ctx);
}
long long block_index;
if ((RedisModule_StringToLongLong(argv[2], &block_index) != REDISMODULE_OK) || block_index < 0) {
return RedisModule_ReplyWithError(ctx, "ERR invalid block_index: must be a value greater than 0");
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != MemAllocType) {
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
}
MemAllocObject *o;
if (type == REDISMODULE_KEYTYPE_EMPTY) {
return RedisModule_ReplyWithError(ctx, "ERR Memory has not been allocated");
} else {
o = RedisModule_ModuleTypeGetValue(key);
}
if (o->mask & (1UL << block_index)) {
return RedisModule_ReplyWithError(ctx, "ERR block is busy");
}
int ret = 0;
int nokey;
struct MemBlock *mem = (struct MemBlock *)RedisModule_DictGet(mem_pool[RedisModule_GetSelectedDb(ctx)], argv[1], &nokey);
if (!nokey && mem) {
size_t len;
const char *buf = RedisModule_StringPtrLen(argv[3], &len);
ret = MemBlockWrite(mem, block_index, buf, len);
o->mask |= (1UL << block_index);
o->used++;
}
RedisModule_ReplyWithLongLong(ctx, ret);
RedisModule_ReplicateVerbatim(ctx);
return REDISMODULE_OK;
}
int MemRead_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
if (argc != 3) {
return RedisModule_WrongArity(ctx);
}
long long block_index;
if ((RedisModule_StringToLongLong(argv[2], &block_index) != REDISMODULE_OK) || block_index < 0) {
return RedisModule_ReplyWithError(ctx, "ERR invalid block_index: must be a value greater than 0");
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != MemAllocType) {
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
}
MemAllocObject *o;
if (type == REDISMODULE_KEYTYPE_EMPTY) {
return RedisModule_ReplyWithError(ctx, "ERR Memory has not been allocated");
} else {
o = RedisModule_ModuleTypeGetValue(key);
}
if (!(o->mask & (1UL << block_index))) {
return RedisModule_ReplyWithNull(ctx);
}
int nokey;
struct MemBlock *mem = (struct MemBlock *)RedisModule_DictGet(mem_pool[RedisModule_GetSelectedDb(ctx)], argv[1], &nokey);
RedisModule_Assert(nokey == 0 && mem != NULL);
char buf[BLOCK_SIZE];
MemBlockRead(mem, block_index, buf, sizeof(buf));
RedisModule_ReplyWithStringBuffer(ctx, buf, strlen(buf));
return REDISMODULE_OK;
}
int MemUsage_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
if (argc != 2) {
return RedisModule_WrongArity(ctx);
}
long long dbid;
if ((RedisModule_StringToLongLong(argv[1], (long long *)&dbid) != REDISMODULE_OK)) {
return RedisModule_ReplyWithError(ctx, "ERR invalid value: must be a integer");
}
if (dbid < 0 || dbid >= MAX_DB) {
return RedisModule_ReplyWithError(ctx, "ERR dbid out of range");
}
long long size = 0, used = 0;
void *data;
RedisModuleString *key;
RedisModuleDictIter *iter = RedisModule_DictIteratorStartC(mem_pool[dbid], "^", NULL, 0);
while((key = RedisModule_DictNext(ctx, iter, &data)) != NULL) {
int dbbackup = RedisModule_GetSelectedDb(ctx);
RedisModule_SelectDb(ctx, dbid);
RedisModuleKey *openkey = RedisModule_OpenKey(ctx, key, REDISMODULE_READ);
int type = RedisModule_KeyType(openkey);
RedisModule_Assert(type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(openkey) == MemAllocType);
MemAllocObject *o = RedisModule_ModuleTypeGetValue(openkey);
used += o->used;
size += o->size;
RedisModule_CloseKey(openkey);
RedisModule_SelectDb(ctx, dbbackup);
}
RedisModule_DictIteratorStop(iter);
RedisModule_ReplyWithArray(ctx, 4);
RedisModule_ReplyWithSimpleString(ctx, "total");
RedisModule_ReplyWithLongLong(ctx, size);
RedisModule_ReplyWithSimpleString(ctx, "used");
RedisModule_ReplyWithLongLong(ctx, used);
return REDISMODULE_OK;
}
int MemAllocAndWrite_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
if (argc < 3) {
return RedisModule_WrongArity(ctx);
}
long long block_num;
if ((RedisModule_StringToLongLong(argv[2], &block_num) != REDISMODULE_OK) || block_num <= 0) {
return RedisModule_ReplyWithError(ctx, "ERR invalid block_num: must be a value greater than 0");
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != MemAllocType) {
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
}
MemAllocObject *o;
if (type == REDISMODULE_KEYTYPE_EMPTY) {
o = createMemAllocObject();
RedisModule_ModuleTypeSetValue(key, MemAllocType, o);
} else {
o = RedisModule_ModuleTypeGetValue(key);
}
struct MemBlock *mem = MemBlockCreate(block_num);
RedisModule_Assert(mem != NULL);
RedisModule_DictSet(mem_pool[RedisModule_GetSelectedDb(ctx)], argv[1], mem);
o->used = 0;
o->mask = 0;
o->size = block_num;
int i = 3;
long long block_index;
for (; i < argc; i++) {
RedisModule_StringToLongLong(argv[i], &block_index);
size_t len;
const char * buf = RedisModule_StringPtrLen(argv[i + 1], &len);
MemBlockWrite(mem, block_index, buf, len);
o->used++;
o->mask |= (1UL << block_index);
}
RedisModule_ReplyWithSimpleString(ctx, "OK");
RedisModule_ReplicateVerbatim(ctx);
return REDISMODULE_OK;
}
void *MemAllocRdbLoad(RedisModuleIO *rdb, int encver) {
if (encver != 0) {
return NULL;
}
MemAllocObject *o = createMemAllocObject();
o->size = RedisModule_LoadSigned(rdb);
o->used = RedisModule_LoadSigned(rdb);
o->mask = RedisModule_LoadUnsigned(rdb);
const RedisModuleString *key = RedisModule_GetKeyNameFromIO(rdb);
int dbid = RedisModule_GetDbIdFromIO(rdb);
if (o->size) {
size_t size;
char *tmpbuf;
long long num = o->size;
struct MemBlock *head = RedisModule_Calloc(1, sizeof(struct MemBlock));
tmpbuf = RedisModule_LoadStringBuffer(rdb, &size);
memcpy(head->block, tmpbuf, size > BLOCK_SIZE ? BLOCK_SIZE:size);
RedisModule_Free(tmpbuf);
struct MemBlock *block = head;
while (--num) {
block->next = RedisModule_Calloc(1, sizeof(struct MemBlock));
block = block->next;
tmpbuf = RedisModule_LoadStringBuffer(rdb, &size);
memcpy(block->block, tmpbuf, size > BLOCK_SIZE ? BLOCK_SIZE:size);
RedisModule_Free(tmpbuf);
}
RedisModule_DictSet(mem_pool[dbid], (RedisModuleString *)key, head);
}
return o;
}
void MemAllocRdbSave(RedisModuleIO *rdb, void *value) {
MemAllocObject *o = value;
RedisModule_SaveSigned(rdb, o->size);
RedisModule_SaveSigned(rdb, o->used);
RedisModule_SaveUnsigned(rdb, o->mask);
const RedisModuleString *key = RedisModule_GetKeyNameFromIO(rdb);
int dbid = RedisModule_GetDbIdFromIO(rdb);
if (o->size) {
int nokey;
struct MemBlock *mem = (struct MemBlock *)RedisModule_DictGet(mem_pool[dbid], (RedisModuleString *)key, &nokey);
RedisModule_Assert(nokey == 0 && mem != NULL);
struct MemBlock *block = mem;
while (block) {
RedisModule_SaveStringBuffer(rdb, block->block, BLOCK_SIZE);
block = block->next;
}
}
}
void MemAllocAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
MemAllocObject *o = (MemAllocObject *)value;
if (o->size) {
int dbid = RedisModule_GetDbIdFromIO(aof);
int nokey;
size_t i = 0, j = 0;
struct MemBlock *mem = (struct MemBlock *)RedisModule_DictGet(mem_pool[dbid], (RedisModuleString *)key, &nokey);
RedisModule_Assert(nokey == 0 && mem != NULL);
size_t array_size = o->size * 2;
RedisModuleString ** string_array = RedisModule_Calloc(array_size, sizeof(RedisModuleString *));
while (mem) {
string_array[i] = RedisModule_CreateStringFromLongLong(NULL, j);
string_array[i + 1] = RedisModule_CreateString(NULL, mem->block, BLOCK_SIZE);
mem = mem->next;
i += 2;
j++;
}
RedisModule_EmitAOF(aof, "mem.allocandwrite", "slv", key, o->size, string_array, array_size);
for (i = 0; i < array_size; i++) {
RedisModule_FreeString(NULL, string_array[i]);
}
RedisModule_Free(string_array);
} else {
RedisModule_EmitAOF(aof, "mem.allocandwrite", "sl", key, o->size);
}
}
void MemAllocFree(void *value) {
RedisModule_Free(value);
}
void MemAllocUnlink(RedisModuleString *key, const void *value) {
REDISMODULE_NOT_USED(key);
REDISMODULE_NOT_USED(value);
RedisModule_Assert(0);
}
void MemAllocUnlink2(RedisModuleKeyOptCtx *ctx, const void *value) {
MemAllocObject *o = (MemAllocObject *)value;
const RedisModuleString *key = RedisModule_GetKeyNameFromOptCtx(ctx);
int dbid = RedisModule_GetDbIdFromOptCtx(ctx);
if (o->size) {
void *oldval;
RedisModule_DictDel(mem_pool[dbid], (RedisModuleString *)key, &oldval);
RedisModule_Assert(oldval != NULL);
MemBlockFree((struct MemBlock *)oldval);
}
}
void MemAllocDigest(RedisModuleDigest *md, void *value) {
MemAllocObject *o = (MemAllocObject *)value;
RedisModule_DigestAddLongLong(md, o->size);
RedisModule_DigestAddLongLong(md, o->used);
RedisModule_DigestAddLongLong(md, o->mask);
int dbid = RedisModule_GetDbIdFromDigest(md);
const RedisModuleString *key = RedisModule_GetKeyNameFromDigest(md);
if (o->size) {
int nokey;
struct MemBlock *mem = (struct MemBlock *)RedisModule_DictGet(mem_pool[dbid], (RedisModuleString *)key, &nokey);
RedisModule_Assert(nokey == 0 && mem != NULL);
struct MemBlock *block = mem;
while (block) {
RedisModule_DigestAddStringBuffer(md, (const char *)block->block, BLOCK_SIZE);
block = block->next;
}
}
}
void *MemAllocCopy2(RedisModuleKeyOptCtx *ctx, const void *value) {
const MemAllocObject *old = value;
MemAllocObject *new = createMemAllocObject();
new->size = old->size;
new->used = old->used;
new->mask = old->mask;
int from_dbid = RedisModule_GetDbIdFromOptCtx(ctx);
int to_dbid = RedisModule_GetToDbIdFromOptCtx(ctx);
const RedisModuleString *fromkey = RedisModule_GetKeyNameFromOptCtx(ctx);
const RedisModuleString *tokey = RedisModule_GetToKeyNameFromOptCtx(ctx);
if (old->size) {
int nokey;
struct MemBlock *oldmem = (struct MemBlock *)RedisModule_DictGet(mem_pool[from_dbid], (RedisModuleString *)fromkey, &nokey);
RedisModule_Assert(nokey == 0 && oldmem != NULL);
struct MemBlock *newmem = MemBlockClone(oldmem);
RedisModule_Assert(newmem != NULL);
RedisModule_DictSet(mem_pool[to_dbid], (RedisModuleString *)tokey, newmem);
}
return new;
}
size_t MemAllocMemUsage2(RedisModuleKeyOptCtx *ctx, const void *value, size_t sample_size) {
REDISMODULE_NOT_USED(ctx);
REDISMODULE_NOT_USED(sample_size);
uint64_t size = 0;
MemAllocObject *o = (MemAllocObject *)value;
size += sizeof(*o);
size += o->size * sizeof(struct MemBlock);
return size;
}
size_t MemAllocMemFreeEffort2(RedisModuleKeyOptCtx *ctx, const void *value) {
REDISMODULE_NOT_USED(ctx);
MemAllocObject *o = (MemAllocObject *)value;
return o->size;
}
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
if (RedisModule_Init(ctx, "datatype2", 1,REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
return REDISMODULE_ERR;
}
RedisModuleTypeMethods tm = {
.version = REDISMODULE_TYPE_METHOD_VERSION,
.rdb_load = MemAllocRdbLoad,
.rdb_save = MemAllocRdbSave,
.aof_rewrite = MemAllocAofRewrite,
.free = MemAllocFree,
.digest = MemAllocDigest,
.unlink = MemAllocUnlink,
.unlink2 = MemAllocUnlink2,
.copy2 = MemAllocCopy2,
.mem_usage2 = MemAllocMemUsage2,
.free_effort2 = MemAllocMemFreeEffort2,
};
MemAllocType = RedisModule_CreateDataType(ctx, "mem_alloc", 0, &tm);
if (MemAllocType == NULL) {
return REDISMODULE_ERR;
}
if (RedisModule_CreateCommand(ctx, "mem.alloc", MemAlloc_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR) {
return REDISMODULE_ERR;
}
if (RedisModule_CreateCommand(ctx, "mem.free", MemFree_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR) {
return REDISMODULE_ERR;
}
if (RedisModule_CreateCommand(ctx, "mem.write", MemWrite_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR) {
return REDISMODULE_ERR;
}
if (RedisModule_CreateCommand(ctx, "mem.read", MemRead_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
return REDISMODULE_ERR;
}
if (RedisModule_CreateCommand(ctx, "mem.usage", MemUsage_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
return REDISMODULE_ERR;
}
if (RedisModule_CreateCommand(ctx, "mem.allocandwrite", MemAllocAndWrite_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR) {
return REDISMODULE_ERR;
}
for(int i = 0; i < MAX_DB; i++){
mem_pool[i] = RedisModule_CreateDict(NULL);
}
RedisModule_SubscribeToServerEvent(ctx, RedisModuleEvent_FlushDB, flushdbCallback);
RedisModule_SubscribeToServerEvent(ctx, RedisModuleEvent_SwapDB, swapDbCallback);
return REDISMODULE_OK;
}
