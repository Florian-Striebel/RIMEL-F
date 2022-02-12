































#define REDISMODULE_EXPERIMENTAL_API
#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MSGTYPE_PING 1
#define MSGTYPE_PONG 2


int PingallCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);

RedisModule_SendClusterMessage(ctx,NULL,MSGTYPE_PING,"Hey",3);
return RedisModule_ReplyWithSimpleString(ctx, "OK");
}


int ListCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);

size_t numnodes;
char **ids = RedisModule_GetClusterNodesList(ctx,&numnodes);
if (ids == NULL) {
return RedisModule_ReplyWithError(ctx,"Cluster not enabled");
}

RedisModule_ReplyWithArray(ctx,numnodes);
for (size_t j = 0; j < numnodes; j++) {
int port;
RedisModule_GetClusterNodeInfo(ctx,ids[j],NULL,NULL,&port,NULL);
RedisModule_ReplyWithArray(ctx,2);
RedisModule_ReplyWithStringBuffer(ctx,ids[j],REDISMODULE_NODE_ID_LEN);
RedisModule_ReplyWithLongLong(ctx,port);
}
RedisModule_FreeClusterNodesList(ids);
return REDISMODULE_OK;
}


void PingReceiver(RedisModuleCtx *ctx, const char *sender_id, uint8_t type, const unsigned char *payload, uint32_t len) {
RedisModule_Log(ctx,"notice","PING (type %d) RECEIVED from %.*s: '%.*s'",
type,REDISMODULE_NODE_ID_LEN,sender_id,(int)len, payload);
RedisModule_SendClusterMessage(ctx,NULL,MSGTYPE_PONG,(unsigned char*)"Ohi!",4);
RedisModuleCallReply *reply = RedisModule_Call(ctx, "INCR", "c", "pings_received");
RedisModule_FreeCallReply(reply);
}


void PongReceiver(RedisModuleCtx *ctx, const char *sender_id, uint8_t type, const unsigned char *payload, uint32_t len) {
RedisModule_Log(ctx,"notice","PONG (type %d) RECEIVED from %.*s: '%.*s'",
type,REDISMODULE_NODE_ID_LEN,sender_id,(int)len, payload);
}



int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);

if (RedisModule_Init(ctx,"hellocluster",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hellocluster.pingall",
PingallCommand_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hellocluster.list",
ListCommand_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;







RedisModule_SetClusterFlags(ctx,REDISMODULE_CLUSTER_FLAG_NO_REDIRECTION);


RedisModule_RegisterClusterMessageReceiver(ctx,MSGTYPE_PING,PingReceiver);
RedisModule_RegisterClusterMessageReceiver(ctx,MSGTYPE_PONG,PongReceiver);
return REDISMODULE_OK;
}
