#define REDISMODULE_EXPERIMENTAL_API
#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
void timerHandler(RedisModuleCtx *ctx, void *data) {
REDISMODULE_NOT_USED(ctx);
printf("Fired %s!\n", (char *)data);
RedisModule_Free(data);
}
int TimerCommand_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
for (int j = 0; j < 10; j++) {
int delay = rand() % 5000;
char *buf = RedisModule_Alloc(256);
snprintf(buf,256,"After %d", delay);
RedisModuleTimerID tid = RedisModule_CreateTimer(ctx,delay,timerHandler,buf);
REDISMODULE_NOT_USED(tid);
}
return RedisModule_ReplyWithSimpleString(ctx, "OK");
}
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
if (RedisModule_Init(ctx,"hellotimer",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;
if (RedisModule_CreateCommand(ctx,"hellotimer.timer",
TimerCommand_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;
return REDISMODULE_OK;
}
