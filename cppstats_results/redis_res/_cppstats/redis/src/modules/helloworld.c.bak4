



































#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>






int HelloSimple_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
RedisModule_ReplyWithLongLong(ctx,RedisModule_GetSelectedDb(ctx));
return REDISMODULE_OK;
}







int HelloPushNative_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
if (argc != 3) return RedisModule_WrongArity(ctx);

RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);

RedisModule_ListPush(key,REDISMODULE_LIST_TAIL,argv[2]);
size_t newlen = RedisModule_ValueLength(key);
RedisModule_CloseKey(key);
RedisModule_ReplyWithLongLong(ctx,newlen);
return REDISMODULE_OK;
}






int HelloPushCall_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
if (argc != 3) return RedisModule_WrongArity(ctx);

RedisModuleCallReply *reply;

reply = RedisModule_Call(ctx,"RPUSH","ss",argv[1],argv[2]);
long long len = RedisModule_CallReplyInteger(reply);
RedisModule_FreeCallReply(reply);
RedisModule_ReplyWithLongLong(ctx,len);
return REDISMODULE_OK;
}




int HelloPushCall2_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
if (argc != 3) return RedisModule_WrongArity(ctx);

RedisModuleCallReply *reply;

reply = RedisModule_Call(ctx,"RPUSH","ss",argv[1],argv[2]);
RedisModule_ReplyWithCallReply(ctx,reply);
RedisModule_FreeCallReply(reply);
return REDISMODULE_OK;
}




int HelloListSumLen_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
if (argc != 2) return RedisModule_WrongArity(ctx);

RedisModuleCallReply *reply;

reply = RedisModule_Call(ctx,"LRANGE","sll",argv[1],(long long)0,(long long)-1);
size_t strlen = 0;
size_t items = RedisModule_CallReplyLength(reply);
size_t j;
for (j = 0; j < items; j++) {
RedisModuleCallReply *ele = RedisModule_CallReplyArrayElement(reply,j);
strlen += RedisModule_CallReplyLength(ele);
}
RedisModule_FreeCallReply(reply);
RedisModule_ReplyWithLongLong(ctx,strlen);
return REDISMODULE_OK;
}





int HelloListSplice_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 4) return RedisModule_WrongArity(ctx);

RedisModuleKey *srckey = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
RedisModuleKey *dstkey = RedisModule_OpenKey(ctx,argv[2],
REDISMODULE_READ|REDISMODULE_WRITE);


if ((RedisModule_KeyType(srckey) != REDISMODULE_KEYTYPE_LIST &&
RedisModule_KeyType(srckey) != REDISMODULE_KEYTYPE_EMPTY) ||
(RedisModule_KeyType(dstkey) != REDISMODULE_KEYTYPE_LIST &&
RedisModule_KeyType(dstkey) != REDISMODULE_KEYTYPE_EMPTY))
{
RedisModule_CloseKey(srckey);
RedisModule_CloseKey(dstkey);
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

long long count;
if ((RedisModule_StringToLongLong(argv[3],&count) != REDISMODULE_OK) ||
(count < 0)) {
RedisModule_CloseKey(srckey);
RedisModule_CloseKey(dstkey);
return RedisModule_ReplyWithError(ctx,"ERR invalid count");
}

while(count-- > 0) {
RedisModuleString *ele;

ele = RedisModule_ListPop(srckey,REDISMODULE_LIST_TAIL);
if (ele == NULL) break;
RedisModule_ListPush(dstkey,REDISMODULE_LIST_HEAD,ele);
RedisModule_FreeString(ctx,ele);
}

size_t len = RedisModule_ValueLength(srckey);
RedisModule_CloseKey(srckey);
RedisModule_CloseKey(dstkey);
RedisModule_ReplyWithLongLong(ctx,len);
return REDISMODULE_OK;
}



int HelloListSpliceAuto_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 4) return RedisModule_WrongArity(ctx);

RedisModule_AutoMemory(ctx);

RedisModuleKey *srckey = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
RedisModuleKey *dstkey = RedisModule_OpenKey(ctx,argv[2],
REDISMODULE_READ|REDISMODULE_WRITE);


if ((RedisModule_KeyType(srckey) != REDISMODULE_KEYTYPE_LIST &&
RedisModule_KeyType(srckey) != REDISMODULE_KEYTYPE_EMPTY) ||
(RedisModule_KeyType(dstkey) != REDISMODULE_KEYTYPE_LIST &&
RedisModule_KeyType(dstkey) != REDISMODULE_KEYTYPE_EMPTY))
{
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

long long count;
if ((RedisModule_StringToLongLong(argv[3],&count) != REDISMODULE_OK) ||
(count < 0))
{
return RedisModule_ReplyWithError(ctx,"ERR invalid count");
}

while(count-- > 0) {
RedisModuleString *ele;

ele = RedisModule_ListPop(srckey,REDISMODULE_LIST_TAIL);
if (ele == NULL) break;
RedisModule_ListPush(dstkey,REDISMODULE_LIST_HEAD,ele);
}

size_t len = RedisModule_ValueLength(srckey);
RedisModule_ReplyWithLongLong(ctx,len);
return REDISMODULE_OK;
}




int HelloRandArray_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 2) return RedisModule_WrongArity(ctx);
long long count;
if (RedisModule_StringToLongLong(argv[1],&count) != REDISMODULE_OK ||
count < 0)
return RedisModule_ReplyWithError(ctx,"ERR invalid count");




RedisModule_ReplyWithArray(ctx,count);
while(count--) RedisModule_ReplyWithLongLong(ctx,rand());
return REDISMODULE_OK;
}





int HelloRepl1_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
RedisModule_AutoMemory(ctx);











RedisModule_Replicate(ctx,"ECHO","c","foo");



RedisModule_Call(ctx,"INCR","c!","foo");
RedisModule_Call(ctx,"INCR","c!","bar");

RedisModule_ReplyWithLongLong(ctx,0);

return REDISMODULE_OK;
}











int HelloRepl2_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 2) return RedisModule_WrongArity(ctx);

RedisModule_AutoMemory(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);

if (RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_LIST)
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);

size_t listlen = RedisModule_ValueLength(key);
long long sum = 0;


while(listlen--) {
RedisModuleString *ele = RedisModule_ListPop(key,REDISMODULE_LIST_TAIL);
long long val;
if (RedisModule_StringToLongLong(ele,&val) != REDISMODULE_OK) val = 0;
val++;
sum += val;
RedisModuleString *newele = RedisModule_CreateStringFromLongLong(ctx,val);
RedisModule_ListPush(key,REDISMODULE_LIST_HEAD,newele);
}
RedisModule_ReplyWithLongLong(ctx,sum);
RedisModule_ReplicateVerbatim(ctx);
return REDISMODULE_OK;
}









int HelloToggleCase_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 2) return RedisModule_WrongArity(ctx);

RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);

int keytype = RedisModule_KeyType(key);
if (keytype != REDISMODULE_KEYTYPE_STRING &&
keytype != REDISMODULE_KEYTYPE_EMPTY)
{
RedisModule_CloseKey(key);
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

if (keytype == REDISMODULE_KEYTYPE_STRING) {
size_t len, j;
char *s = RedisModule_StringDMA(key,&len,REDISMODULE_WRITE);
for (j = 0; j < len; j++) {
if (isupper(s[j])) {
s[j] = tolower(s[j]);
} else {
s[j] = toupper(s[j]);
}
}
}

RedisModule_CloseKey(key);
RedisModule_ReplyWithSimpleString(ctx,"OK");
RedisModule_ReplicateVerbatim(ctx);
return REDISMODULE_OK;
}





int HelloMoreExpire_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
if (argc != 3) return RedisModule_WrongArity(ctx);

mstime_t addms, expire;

if (RedisModule_StringToLongLong(argv[2],&addms) != REDISMODULE_OK)
return RedisModule_ReplyWithError(ctx,"ERR invalid expire time");

RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
expire = RedisModule_GetExpire(key);
if (expire != REDISMODULE_NO_EXPIRE) {
expire += addms;
RedisModule_SetExpire(key,expire);
}
return RedisModule_ReplyWithSimpleString(ctx,"OK");
}







int HelloZsumRange_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
double score_start, score_end;
if (argc != 4) return RedisModule_WrongArity(ctx);

if (RedisModule_StringToDouble(argv[2],&score_start) != REDISMODULE_OK ||
RedisModule_StringToDouble(argv[3],&score_end) != REDISMODULE_OK)
{
return RedisModule_ReplyWithError(ctx,"ERR invalid range");
}

RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
if (RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_ZSET) {
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

double scoresum_a = 0;
double scoresum_b = 0;

RedisModule_ZsetFirstInScoreRange(key,score_start,score_end,0,0);
while(!RedisModule_ZsetRangeEndReached(key)) {
double score;
RedisModuleString *ele = RedisModule_ZsetRangeCurrentElement(key,&score);
RedisModule_FreeString(ctx,ele);
scoresum_a += score;
RedisModule_ZsetRangeNext(key);
}
RedisModule_ZsetRangeStop(key);

RedisModule_ZsetLastInScoreRange(key,score_start,score_end,0,0);
while(!RedisModule_ZsetRangeEndReached(key)) {
double score;
RedisModuleString *ele = RedisModule_ZsetRangeCurrentElement(key,&score);
RedisModule_FreeString(ctx,ele);
scoresum_b += score;
RedisModule_ZsetRangePrev(key);
}

RedisModule_ZsetRangeStop(key);

RedisModule_CloseKey(key);

RedisModule_ReplyWithArray(ctx,2);
RedisModule_ReplyWithDouble(ctx,scoresum_a);
RedisModule_ReplyWithDouble(ctx,scoresum_b);
return REDISMODULE_OK;
}








int HelloLexRange_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);

if (argc != 6) return RedisModule_WrongArity(ctx);

RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
if (RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_ZSET) {
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}

if (RedisModule_ZsetFirstInLexRange(key,argv[2],argv[3]) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx,"invalid range");
}

int arraylen = 0;
RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_LEN);
while(!RedisModule_ZsetRangeEndReached(key)) {
double score;
RedisModuleString *ele = RedisModule_ZsetRangeCurrentElement(key,&score);
RedisModule_ReplyWithString(ctx,ele);
RedisModule_FreeString(ctx,ele);
RedisModule_ZsetRangeNext(key);
arraylen++;
}
RedisModule_ZsetRangeStop(key);
RedisModule_ReplySetArrayLength(ctx,arraylen);
RedisModule_CloseKey(key);
return REDISMODULE_OK;
}








int HelloHCopy_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);

if (argc != 4) return RedisModule_WrongArity(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
REDISMODULE_READ|REDISMODULE_WRITE);
int type = RedisModule_KeyType(key);
if (type != REDISMODULE_KEYTYPE_HASH &&
type != REDISMODULE_KEYTYPE_EMPTY)
{
return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
}


RedisModuleString *oldval;
RedisModule_HashGet(key,REDISMODULE_HASH_NONE,argv[2],&oldval,NULL);
if (oldval) {
RedisModule_HashSet(key,REDISMODULE_HASH_NONE,argv[3],oldval,NULL);
}
RedisModule_ReplyWithLongLong(ctx,oldval != NULL);
return REDISMODULE_OK;
}



















int HelloLeftPad_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
RedisModule_AutoMemory(ctx);
long long padlen;

if (argc != 4) return RedisModule_WrongArity(ctx);

if ((RedisModule_StringToLongLong(argv[2],&padlen) != REDISMODULE_OK) ||
(padlen< 0)) {
return RedisModule_ReplyWithError(ctx,"ERR invalid padding length");
}
size_t strlen, chlen;
const char *str = RedisModule_StringPtrLen(argv[1], &strlen);
const char *ch = RedisModule_StringPtrLen(argv[3], &chlen);



if (strlen >= (size_t)padlen)
return RedisModule_ReplyWithString(ctx,argv[1]);


if (chlen != 1)
return RedisModule_ReplyWithError(ctx,
"ERR padding must be a single char");


padlen -= strlen;
char *buf = RedisModule_PoolAlloc(ctx,padlen+strlen);
for (long long j = 0; j < padlen; j++) buf[j] = *ch;
memcpy(buf+padlen,str,strlen);

RedisModule_ReplyWithStringBuffer(ctx,buf,padlen+strlen);
return REDISMODULE_OK;
}



int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (RedisModule_Init(ctx,"helloworld",1,REDISMODULE_APIVER_1)
== REDISMODULE_ERR) return REDISMODULE_ERR;


for (int j = 0; j < argc; j++) {
const char *s = RedisModule_StringPtrLen(argv[j],NULL);
printf("Module loaded with ARGV[%d] = %s\n", j, s);
}

if (RedisModule_CreateCommand(ctx,"hello.simple",
HelloSimple_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.push.native",
HelloPushNative_RedisCommand,"write deny-oom",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.push.call",
HelloPushCall_RedisCommand,"write deny-oom",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.push.call2",
HelloPushCall2_RedisCommand,"write deny-oom",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.list.sum.len",
HelloListSumLen_RedisCommand,"readonly",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.list.splice",
HelloListSplice_RedisCommand,"write deny-oom",1,2,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.list.splice.auto",
HelloListSpliceAuto_RedisCommand,
"write deny-oom",1,2,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.rand.array",
HelloRandArray_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.repl1",
HelloRepl1_RedisCommand,"write",0,0,0) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.repl2",
HelloRepl2_RedisCommand,"write",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.toggle.case",
HelloToggleCase_RedisCommand,"write",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.more.expire",
HelloMoreExpire_RedisCommand,"write",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.zsumrange",
HelloZsumRange_RedisCommand,"readonly",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.lexrange",
HelloLexRange_RedisCommand,"readonly",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.hcopy",
HelloHCopy_RedisCommand,"write deny-oom",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

if (RedisModule_CreateCommand(ctx,"hello.leftpad",
HelloLeftPad_RedisCommand,"",1,1,1) == REDISMODULE_ERR)
return REDISMODULE_ERR;

return REDISMODULE_OK;
}
