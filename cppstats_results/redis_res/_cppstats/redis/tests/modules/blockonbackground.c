#define _XOPEN_SOURCE 700
#include "redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define UNUSED(x) (void)(x)
int HelloBlock_Reply(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
UNUSED(argv);
UNUSED(argc);
int *myint = RedisModule_GetBlockedClientPrivateData(ctx);
return RedisModule_ReplyWithLongLong(ctx,*myint);
}
int HelloBlock_Timeout(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
UNUSED(argv);
UNUSED(argc);
RedisModuleBlockedClient *bc = RedisModule_GetBlockedClientHandle(ctx);
RedisModule_BlockedClientMeasureTimeEnd(bc);
return RedisModule_ReplyWithSimpleString(ctx,"Request timedout");
}
void HelloBlock_FreeData(RedisModuleCtx *ctx, void *privdata) {
UNUSED(ctx);
RedisModule_Free(privdata);
}
void HelloBlock_FreeStringData(RedisModuleCtx *ctx, void *privdata) {
RedisModule_FreeString(ctx, (RedisModuleString*)privdata);
}
void *BlockDebug_ThreadMain(void *arg) {
void **targ = arg;
RedisModuleBlockedClient *bc = targ[0];
long long delay = (unsigned long)targ[1];
long long enable_time_track = (unsigned long)targ[2];
if (enable_time_track)
RedisModule_BlockedClientMeasureTimeStart(bc);
RedisModule_Free(targ);
struct timespec ts;
ts.tv_sec = delay / 1000;
ts.tv_nsec = (delay % 1000) * 1000000;
nanosleep(&ts, NULL);
int *r = RedisModule_Alloc(sizeof(int));
*r = rand();
if (enable_time_track)
RedisModule_BlockedClientMeasureTimeEnd(bc);
RedisModule_UnblockClient(bc,r);
return NULL;
}
void *DoubleBlock_ThreadMain(void *arg) {
void **targ = arg;
RedisModuleBlockedClient *bc = targ[0];
long long delay = (unsigned long)targ[1];
RedisModule_BlockedClientMeasureTimeStart(bc);
RedisModule_Free(targ);
struct timespec ts;
ts.tv_sec = delay / 1000;
ts.tv_nsec = (delay % 1000) * 1000000;
nanosleep(&ts, NULL);
int *r = RedisModule_Alloc(sizeof(int));
*r = rand();
RedisModule_BlockedClientMeasureTimeEnd(bc);
RedisModule_BlockedClientMeasureTimeStart(bc);
nanosleep(&ts, NULL);
RedisModule_BlockedClientMeasureTimeEnd(bc);
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
void **targ = RedisModule_Alloc(sizeof(void*)*3);
targ[0] = bc;
targ[1] = (void*)(unsigned long) delay;
targ[2] = (void*)(unsigned long) 1;
if (pthread_create(&tid,NULL,BlockDebug_ThreadMain,targ) != 0) {
RedisModule_AbortBlock(bc);
return RedisModule_ReplyWithError(ctx,"-ERR Can't start thread");
}
return REDISMODULE_OK;
}
int HelloBlockNoTracking_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
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
void **targ = RedisModule_Alloc(sizeof(void*)*3);
targ[0] = bc;
targ[1] = (void*)(unsigned long) delay;
targ[2] = (void*)(unsigned long) 0;
if (pthread_create(&tid,NULL,BlockDebug_ThreadMain,targ) != 0) {
RedisModule_AbortBlock(bc);
return RedisModule_ReplyWithError(ctx,"-ERR Can't start thread");
}
return REDISMODULE_OK;
}
int HelloDoubleBlock_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 2) return RedisModule_WrongArity(ctx);
long long delay;
if (RedisModule_StringToLongLong(argv[1],&delay) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx,"ERR invalid count");
}
pthread_t tid;
RedisModuleBlockedClient *bc = RedisModule_BlockClient(ctx,HelloBlock_Reply,HelloBlock_Timeout,HelloBlock_FreeData,0);
void **targ = RedisModule_Alloc(sizeof(void*)*2);
targ[0] = bc;
targ[1] = (void*)(unsigned long) delay;
if (pthread_create(&tid,NULL,DoubleBlock_ThreadMain,targ) != 0) {
RedisModule_AbortBlock(bc);
return RedisModule_ReplyWithError(ctx,"-ERR Can't start thread");
}
return REDISMODULE_OK;
}
RedisModuleBlockedClient *blocked_client = NULL;
int Block_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (RedisModule_IsBlockedReplyRequest(ctx)) {
RedisModuleString *r = RedisModule_GetBlockedClientPrivateData(ctx);
return RedisModule_ReplyWithString(ctx, r);
} else if (RedisModule_IsBlockedTimeoutRequest(ctx)) {
RedisModule_UnblockClient(blocked_client, NULL);
blocked_client = NULL;
return RedisModule_ReplyWithSimpleString(ctx, "Timed out");
}
if (argc != 2) return RedisModule_WrongArity(ctx);
long long timeout;
if (RedisModule_StringToLongLong(argv[1], &timeout) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx, "ERR invalid timeout");
}
if (blocked_client) {
return RedisModule_ReplyWithError(ctx, "ERR another client already blocked");
}
blocked_client = RedisModule_BlockClient(ctx, Block_RedisCommand,
timeout > 0 ? Block_RedisCommand : NULL, HelloBlock_FreeStringData, timeout);
return REDISMODULE_OK;
}
int IsBlocked_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
UNUSED(argv);
UNUSED(argc);
RedisModule_ReplyWithLongLong(ctx, blocked_client ? 1 : 0);
return REDISMODULE_OK;
}
int Release_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 2) return RedisModule_WrongArity(ctx);
if (!blocked_client) {
return RedisModule_ReplyWithError(ctx, "ERR No blocked client");
}
RedisModuleString *replystr = argv[1];
RedisModule_RetainString(ctx, replystr);
RedisModule_UnblockClient(blocked_client, replystr);
blocked_client = NULL;
RedisModule_ReplyWithSimpleString(ctx, "OK");
return REDISMODULE_OK;
}
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
UNUSED(argv);
UNUSED(argc);
if (RedisModule_Init(ctx,"block",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"block.debug",
HelloBlock_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"block.double_debug",
HelloDoubleBlock_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"block.debug_no_track",
HelloBlockNoTracking_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx, "block.block",
Block_RedisCommand, "", 0, 0, 0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"block.is_blocked",
IsBlocked_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"block.release",
Release_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
return REDISMODULE_OK;
}
