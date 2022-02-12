#define REDISMODULE_EXPERIMENTAL_API
#include "../redismodule.h"
#include <pthread.h>
#include <unistd.h>
static RedisModuleUser *global;
static uint64_t global_auth_client_id = 0;
int RevokeCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
if (global_auth_client_id) {
RedisModule_DeauthenticateAndCloseClient(ctx, global_auth_client_id);
return RedisModule_ReplyWithSimpleString(ctx, "OK");
} else {
return RedisModule_ReplyWithError(ctx, "Global user currently not used");
}
}
int ResetCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
RedisModule_FreeModuleUser(global);
global = RedisModule_CreateModuleUser("global");
RedisModule_SetModuleUserACL(global, "allcommands");
RedisModule_SetModuleUserACL(global, "allkeys");
RedisModule_SetModuleUserACL(global, "on");
return RedisModule_ReplyWithSimpleString(ctx, "OK");
}
void HelloACL_UserChanged(uint64_t client_id, void *privdata) {
REDISMODULE_NOT_USED(privdata);
REDISMODULE_NOT_USED(client_id);
global_auth_client_id = 0;
}
int AuthGlobalCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
if (global_auth_client_id) {
return RedisModule_ReplyWithError(ctx, "Global user currently used");
}
RedisModule_AuthenticateClientWithUser(ctx, global, HelloACL_UserChanged, NULL, &global_auth_client_id);
return RedisModule_ReplyWithSimpleString(ctx, "OK");
}
#define TIMEOUT_TIME 1000
int HelloACL_Reply(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
size_t length;
RedisModuleString *user_string = RedisModule_GetBlockedClientPrivateData(ctx);
const char *name = RedisModule_StringPtrLen(user_string, &length);
if (RedisModule_AuthenticateClientWithACLUser(ctx, name, length, NULL, NULL, NULL) ==
REDISMODULE_ERR) {
return RedisModule_ReplyWithError(ctx, "Invalid Username or password");
}
return RedisModule_ReplyWithSimpleString(ctx, "OK");
}
int HelloACL_Timeout(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
return RedisModule_ReplyWithSimpleString(ctx, "Request timedout");
}
void HelloACL_FreeData(RedisModuleCtx *ctx, void *privdata) {
REDISMODULE_NOT_USED(ctx);
RedisModule_FreeString(NULL, privdata);
}
void *HelloACL_ThreadMain(void *args) {
void **targs = args;
RedisModuleBlockedClient *bc = targs[0];
RedisModuleString *user = targs[1];
RedisModule_Free(targs);
RedisModule_UnblockClient(bc,user);
return NULL;
}
int AuthAsyncCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 2) return RedisModule_WrongArity(ctx);
pthread_t tid;
RedisModuleBlockedClient *bc = RedisModule_BlockClient(ctx, HelloACL_Reply, HelloACL_Timeout, HelloACL_FreeData, TIMEOUT_TIME);
void **targs = RedisModule_Alloc(sizeof(void*)*2);
targs[0] = bc;
targs[1] = RedisModule_CreateStringFromString(NULL, argv[1]);
if (pthread_create(&tid, NULL, HelloACL_ThreadMain, targs) != 0) {
RedisModule_AbortBlock(bc);
return RedisModule_ReplyWithError(ctx, "-ERR Can't start thread");
}
return REDISMODULE_OK;
}
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
if (RedisModule_Init(ctx,"helloacl",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"helloacl.reset",
ResetCommand_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"helloacl.revoke",
RevokeCommand_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"helloacl.authglobal",
AuthGlobalCommand_RedisCommand,"no-auth",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"helloacl.authasync",
AuthAsyncCommand_RedisCommand,"no-auth",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
global = RedisModule_CreateModuleUser("global");
RedisModule_SetModuleUserACL(global, "allcommands");
RedisModule_SetModuleUserACL(global, "allkeys");
RedisModule_SetModuleUserACL(global, "on");
global_auth_client_id = 0;
return REDISMODULE_OK;
}
