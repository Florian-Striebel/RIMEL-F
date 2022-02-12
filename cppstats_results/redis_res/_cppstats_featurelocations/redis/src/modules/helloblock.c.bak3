
































#define REDISMODULE_EXPERIMENTAL_API
#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


int HelloBlock_Reply(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
int *myint = RedisModule_GetBlockedClientPrivateData(ctx);
return RedisModule_ReplyWithLongLong(ctx,*myint);
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



void *HelloBlock_ThreadMain(void *arg) {
void **targ = arg;
RedisModuleBlockedClient *bc = targ[0];
long long delay = (unsigned long)targ[1];
RedisModule_Free(targ);

sleep(delay);
int *r = RedisModule_Alloc(sizeof(int));
*r = rand();
RedisModule_UnblockClient(bc,r);
return NULL;
}









void HelloBlock_Disconnected(RedisModuleCtx *ctx, RedisModuleBlockedClient *bc) {
RedisModule_Log(ctx,"warning","Blocked client %p disconnected!",
(void*)bc);




}




int HelloBlock_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 3) return RedisModule_WrongArity(ctx);
long long delay;
long long timeout;

if (RedisModule_StringToLongLong(argv[1],&delay) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx,"ERR invalid count");
}

if (RedisModule_StringToLongLong(argv[2],&timeout) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx,"ERR invalid count");
}

pthread_t tid;
RedisModuleBlockedClient *bc = RedisModule_BlockClient(ctx,HelloBlock_Reply,HelloBlock_Timeout,HelloBlock_FreeData,timeout);




RedisModule_SetDisconnectCallback(bc,HelloBlock_Disconnected);




void **targ = RedisModule_Alloc(sizeof(void*)*2);
targ[0] = bc;
targ[1] = (void*)(unsigned long) delay;

if (pthread_create(&tid,NULL,HelloBlock_ThreadMain,targ) != 0) {
RedisModule_AbortBlock(bc);
return RedisModule_ReplyWithError(ctx,"-ERR Can't start thread");
}
return REDISMODULE_OK;
}








void *HelloKeys_ThreadMain(void *arg) {
RedisModuleBlockedClient *bc = arg;
RedisModuleCtx *ctx = RedisModule_GetThreadSafeContext(bc);
long long cursor = 0;
size_t replylen = 0;

RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_LEN);
do {
RedisModule_ThreadSafeContextLock(ctx);
RedisModuleCallReply *reply = RedisModule_Call(ctx,
"SCAN","l",(long long)cursor);
RedisModule_ThreadSafeContextUnlock(ctx);

RedisModuleCallReply *cr_cursor =
RedisModule_CallReplyArrayElement(reply,0);
RedisModuleCallReply *cr_keys =
RedisModule_CallReplyArrayElement(reply,1);

RedisModuleString *s = RedisModule_CreateStringFromCallReply(cr_cursor);
RedisModule_StringToLongLong(s,&cursor);
RedisModule_FreeString(ctx,s);

size_t items = RedisModule_CallReplyLength(cr_keys);
for (size_t j = 0; j < items; j++) {
RedisModuleCallReply *ele =
RedisModule_CallReplyArrayElement(cr_keys,j);
RedisModule_ReplyWithCallReply(ctx,ele);
replylen++;
}
RedisModule_FreeCallReply(reply);
} while (cursor != 0);
RedisModule_ReplySetArrayLength(ctx,replylen);

RedisModule_FreeThreadSafeContext(ctx);
RedisModule_UnblockClient(bc,NULL);
return NULL;
}





int HelloKeys_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
if (argc != 1) return RedisModule_WrongArity(ctx);

pthread_t tid;




RedisModuleBlockedClient *bc = RedisModule_BlockClient(ctx,NULL,NULL,NULL,0);




if (pthread_create(&tid,NULL,HelloKeys_ThreadMain,bc) != 0) {
RedisModule_AbortBlock(bc);
return RedisModule_ReplyWithError(ctx,"-ERR Can't start thread");
}
return REDISMODULE_OK;
}



int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);

if (RedisModule_Init(ctx,"helloblock",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.block",
HelloBlock_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"hello.keys",
HelloKeys_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;

return REDISMODULE_OK;
}
