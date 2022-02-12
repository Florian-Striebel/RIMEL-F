#include "redismodule.h"
#include <assert.h>
#include <errno.h>
#include <strings.h>
int list_getall(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc < 2 || argc > 3) return RedisModule_WrongArity(ctx);
int reverse = (argc == 3 &&
!strcasecmp(RedisModule_StringPtrLen(argv[2], NULL),
"REVERSE"));
RedisModule_AutoMemory(ctx);
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ);
if (RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_LIST) {
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
}
long n = RedisModule_ValueLength(key);
RedisModule_ReplyWithArray(ctx, n);
if (!reverse) {
for (long i = 0; i < n; i++) {
RedisModuleString *elem = RedisModule_ListGet(key, i);
RedisModule_ReplyWithString(ctx, elem);
RedisModule_FreeString(ctx, elem);
}
} else {
for (long i = -1; i >= -n; i--) {
RedisModuleString *elem = RedisModule_ListGet(key, i);
RedisModule_ReplyWithString(ctx, elem);
RedisModule_FreeString(ctx, elem);
}
}
assert(RedisModule_ListGet(key, n) == NULL);
assert(errno == EDOM);
return REDISMODULE_OK;
}
int list_edit(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc < 3) return RedisModule_WrongArity(ctx);
RedisModule_AutoMemory(ctx);
int argpos = 1;
int keymode = REDISMODULE_READ | REDISMODULE_WRITE;
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[argpos++], keymode);
if (RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_LIST) {
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
}
int reverse = 0;
if (argc >= 4 &&
!strcasecmp(RedisModule_StringPtrLen(argv[argpos], NULL), "REVERSE")) {
reverse = 1;
argpos++;
}
size_t cmdstr_len;
const char *cmdstr = RedisModule_StringPtrLen(argv[argpos++], &cmdstr_len);
long num_req_args = 0;
long min_list_length = 0;
for (size_t cmdpos = 0; cmdpos < cmdstr_len; cmdpos++) {
char c = cmdstr[cmdpos];
if (c == 'i' || c == 'r') num_req_args++;
if (c == 'd' || c == 'r' || c == 'k') min_list_length++;
}
if (argc < argpos + num_req_args) {
return RedisModule_ReplyWithError(ctx, "ERR too few args");
}
if ((long)RedisModule_ValueLength(key) < min_list_length) {
return RedisModule_ReplyWithError(ctx, "ERR list too short");
}
long long num_edits = 0;
long index = reverse ? -1 : 0;
RedisModuleString *value;
for (size_t cmdpos = 0; cmdpos < cmdstr_len; cmdpos++) {
switch (cmdstr[cmdpos]) {
case 'i':
value = argv[argpos++];
assert(RedisModule_ListInsert(key, index, value) == REDISMODULE_OK);
index += reverse ? -1 : 1;
num_edits++;
break;
case 'd':
assert(RedisModule_ListDelete(key, index) == REDISMODULE_OK);
num_edits++;
break;
case 'r':
value = argv[argpos++];
assert(RedisModule_ListSet(key, index, value) == REDISMODULE_OK);
index += reverse ? -1 : 1;
num_edits++;
break;
case 'k':
index += reverse ? -1 : 1;
break;
}
}
RedisModule_ReplyWithLongLong(ctx, num_edits);
RedisModule_CloseKey(key);
return REDISMODULE_OK;
}
static int replyByErrno(RedisModuleCtx *ctx) {
switch (errno) {
case EDOM:
return RedisModule_ReplyWithError(ctx, "ERR index out of bounds");
case ENOTSUP:
return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
default: assert(0);
}
}
int list_get(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 3) return RedisModule_WrongArity(ctx);
long long index;
if (RedisModule_StringToLongLong(argv[2], &index) != REDISMODULE_OK) {
return RedisModule_ReplyWithError(ctx, "ERR index must be a number");
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ);
RedisModuleString *value = RedisModule_ListGet(key, index);
if (value) {
RedisModule_ReplyWithString(ctx, value);
RedisModule_FreeString(ctx, value);
} else {
replyByErrno(ctx);
}
RedisModule_CloseKey(key);
return REDISMODULE_OK;
}
int list_set(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 4) return RedisModule_WrongArity(ctx);
long long index;
if (RedisModule_StringToLongLong(argv[2], &index) != REDISMODULE_OK) {
RedisModule_ReplyWithError(ctx, "ERR index must be a number");
return REDISMODULE_OK;
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_WRITE);
if (RedisModule_ListSet(key, index, argv[3]) == REDISMODULE_OK) {
RedisModule_ReplyWithSimpleString(ctx, "OK");
} else {
replyByErrno(ctx);
}
RedisModule_CloseKey(key);
return REDISMODULE_OK;
}
int list_insert(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 4) return RedisModule_WrongArity(ctx);
long long index;
if (RedisModule_StringToLongLong(argv[2], &index) != REDISMODULE_OK) {
RedisModule_ReplyWithError(ctx, "ERR index must be a number");
return REDISMODULE_OK;
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_WRITE);
if (RedisModule_ListInsert(key, index, argv[3]) == REDISMODULE_OK) {
RedisModule_ReplyWithSimpleString(ctx, "OK");
} else {
replyByErrno(ctx);
}
RedisModule_CloseKey(key);
return REDISMODULE_OK;
}
int list_delete(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
if (argc != 3) return RedisModule_WrongArity(ctx);
long long index;
if (RedisModule_StringToLongLong(argv[2], &index) != REDISMODULE_OK) {
RedisModule_ReplyWithError(ctx, "ERR index must be a number");
return REDISMODULE_OK;
}
RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_WRITE);
if (RedisModule_ListDelete(key, index) == REDISMODULE_OK) {
RedisModule_ReplyWithSimpleString(ctx, "OK");
} else {
replyByErrno(ctx);
}
RedisModule_CloseKey(key);
return REDISMODULE_OK;
}
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
REDISMODULE_NOT_USED(argv);
REDISMODULE_NOT_USED(argc);
if (RedisModule_Init(ctx, "list", 1, REDISMODULE_APIVER_1) == REDISMODULE_OK &&
RedisModule_CreateCommand(ctx, "list.getall", list_getall, "",
1, 1, 1) == REDISMODULE_OK &&
RedisModule_CreateCommand(ctx, "list.edit", list_edit, "write",
1, 1, 1) == REDISMODULE_OK &&
RedisModule_CreateCommand(ctx, "list.get", list_get, "write",
1, 1, 1) == REDISMODULE_OK &&
RedisModule_CreateCommand(ctx, "list.set", list_set, "write",
1, 1, 1) == REDISMODULE_OK &&
RedisModule_CreateCommand(ctx, "list.insert", list_insert, "write",
1, 1, 1) == REDISMODULE_OK &&
RedisModule_CreateCommand(ctx, "list.delete", list_delete, "write",
1, 1, 1) == REDISMODULE_OK) {
return REDISMODULE_OK;
} else {
return REDISMODULE_ERR;
}
}
