#define REDISMODULE_EXPERIMENTAL_API
#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
static RedisModuleDict *Keyspace;
int cmd_SET(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 3) return RedisModule_WrongArity(ctx);
RedisModule_DictSet(Keyspace,argv[1],argv[2]);
RedisModule_RetainString(NULL,argv[2]);
return RedisModule_ReplyWithSimpleString(ctx, "OK");
}
int cmd_GET(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 2) return RedisModule_WrongArity(ctx);
RedisModuleString *val = RedisModule_DictGet(Keyspace,argv[1],NULL);
if (val == NULL) {
return RedisModule_ReplyWithNull(ctx);
} else {
return RedisModule_ReplyWithString(ctx, val);
}
}
int cmd_KEYRANGE(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 4) return RedisModule_WrongArity(ctx);
long long count;
if (RedisModule_StringToLongLong(argv[3],&count) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx,"ERR invalid count");
}
RedisModuleDictIter *iter = RedisModule_DictIteratorStart(
Keyspace, ">=", argv[1]);
char *key;
size_t keylen;
long long replylen = 0;
RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_LEN);
while((key = RedisModule_DictNextC(iter,&keylen,NULL)) != NULL) {
if (replylen >= count) break;
if (RedisModule_DictCompare(iter,"<=",argv[2]) == REDISMODULE_ERR)
break;
RedisModule_ReplyWithStringBuffer(ctx,key,keylen);
replylen++;
}
RedisModule_ReplySetArrayLength(ctx,replylen);
RedisModule_DictIteratorStop(iter);
return REDISMODULE_OK;
}
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
if (RedisModule_Init(ctx,"hellodict",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"hellodict.set",
cmd_SET,"write deny-oom",1,1,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"hellodict.get",
cmd_GET,"readonly",1,1,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"hellodict.keyrange",
cmd_KEYRANGE,"readonly",1,1,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
Keyspace = RedisModule_CreateDict(NULL);
return REDISMODULE_OK;
}
