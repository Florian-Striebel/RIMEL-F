#include "script_lua.h"
#include "server.h"
#include "sha1.h"
#include "rand.h"
#include "cluster.h"
#include "monotonic.h"
#include "resp_parser.h"
#include "version.h"
#include <lauxlib.h>
#include <lualib.h>
#include <ctype.h>
#include <math.h>
static int redis_math_random (lua_State *L);
static int redis_math_randomseed (lua_State *L);
static void redisProtocolToLuaType_Int(void *ctx, long long val, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_BulkString(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_NullBulkString(void *ctx, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_NullArray(void *ctx, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_Status(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_Error(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_Array(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
static void redisProtocolToLuaType_Map(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
static void redisProtocolToLuaType_Set(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
static void redisProtocolToLuaType_Null(void *ctx, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_Bool(void *ctx, int val, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_Double(void *ctx, double d, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_BigNumber(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_VerbatimString(void *ctx, const char *format, const char *str, size_t len, const char *proto, size_t proto_len);
static void redisProtocolToLuaType_Attribute(struct ReplyParser *parser, void *ctx, size_t len, const char *proto);
static void luaReplyToRedisReply(client *c, client* script_client, lua_State *lua);
void luaSaveOnRegistry(lua_State* lua, const char* name, void* ptr) {
lua_pushstring(lua, name);
if (ptr) {
lua_pushlightuserdata(lua, ptr);
} else {
lua_pushnil(lua);
}
lua_settable(lua, LUA_REGISTRYINDEX);
}
void* luaGetFromRegistry(lua_State* lua, const char* name) {
lua_pushstring(lua, name);
lua_gettable(lua, LUA_REGISTRYINDEX);
if (lua_isnil(lua, -1)) {
return NULL;
}
serverAssert(lua_islightuserdata(lua, -1));
void* ptr = (void*) lua_topointer(lua, -1);
serverAssert(ptr);
lua_pop(lua, 1);
return ptr;
}
static const ReplyParserCallbacks DefaultLuaTypeParserCallbacks = {
.null_array_callback = redisProtocolToLuaType_NullArray,
.bulk_string_callback = redisProtocolToLuaType_BulkString,
.null_bulk_string_callback = redisProtocolToLuaType_NullBulkString,
.error_callback = redisProtocolToLuaType_Error,
.simple_str_callback = redisProtocolToLuaType_Status,
.long_callback = redisProtocolToLuaType_Int,
.array_callback = redisProtocolToLuaType_Array,
.set_callback = redisProtocolToLuaType_Set,
.map_callback = redisProtocolToLuaType_Map,
.bool_callback = redisProtocolToLuaType_Bool,
.double_callback = redisProtocolToLuaType_Double,
.null_callback = redisProtocolToLuaType_Null,
.big_number_callback = redisProtocolToLuaType_BigNumber,
.verbatim_string_callback = redisProtocolToLuaType_VerbatimString,
.attribute_callback = redisProtocolToLuaType_Attribute,
.error = NULL,
};
static void redisProtocolToLuaType(lua_State *lua, char* reply) {
ReplyParser parser = {.curr_location = reply, .callbacks = DefaultLuaTypeParserCallbacks};
parseReply(&parser, lua);
}
static void redisProtocolToLuaType_Int(void *ctx, long long val, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 1)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_pushnumber(lua,(lua_Number)val);
}
static void redisProtocolToLuaType_NullBulkString(void *ctx, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 1)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_pushboolean(lua,0);
}
static void redisProtocolToLuaType_NullArray(void *ctx, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 1)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_pushboolean(lua,0);
}
static void redisProtocolToLuaType_BulkString(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 1)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_pushlstring(lua,str,len);
}
static void redisProtocolToLuaType_Status(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 3)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
lua_pushstring(lua,"ok");
lua_pushlstring(lua,str,len);
lua_settable(lua,-3);
}
static void redisProtocolToLuaType_Error(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 3)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
lua_pushstring(lua,"err");
lua_pushlstring(lua,str,len);
lua_settable(lua,-3);
}
static void redisProtocolToLuaType_Map(struct ReplyParser *parser, void *ctx, size_t len, const char *proto) {
UNUSED(proto);
lua_State *lua = ctx;
if (lua) {
if (!lua_checkstack(lua, 3)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
lua_pushstring(lua, "map");
lua_newtable(lua);
}
for (size_t j = 0; j < len; j++) {
parseReply(parser,lua);
parseReply(parser,lua);
if (lua) lua_settable(lua,-3);
}
if (lua) lua_settable(lua,-3);
}
static void redisProtocolToLuaType_Set(struct ReplyParser *parser, void *ctx, size_t len, const char *proto) {
UNUSED(proto);
lua_State *lua = ctx;
if (lua) {
if (!lua_checkstack(lua, 3)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
lua_pushstring(lua, "set");
lua_newtable(lua);
}
for (size_t j = 0; j < len; j++) {
parseReply(parser,lua);
if (lua) {
if (!lua_checkstack(lua, 1)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_pushboolean(lua,1);
lua_settable(lua,-3);
}
}
if (lua) lua_settable(lua,-3);
}
static void redisProtocolToLuaType_Array(struct ReplyParser *parser, void *ctx, size_t len, const char *proto) {
UNUSED(proto);
lua_State *lua = ctx;
if (lua){
if (!lua_checkstack(lua, 2)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
}
for (size_t j = 0; j < len; j++) {
if (lua) lua_pushnumber(lua,j+1);
parseReply(parser,lua);
if (lua) lua_settable(lua,-3);
}
}
static void redisProtocolToLuaType_Attribute(struct ReplyParser *parser, void *ctx, size_t len, const char *proto) {
UNUSED(proto);
for (size_t j = 0; j < len; j++) {
parseReply(parser,NULL);
parseReply(parser,NULL);
}
parseReply(parser,ctx);
}
static void redisProtocolToLuaType_VerbatimString(void *ctx, const char *format, const char *str, size_t len, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 5)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
lua_pushstring(lua,"verbatim_string");
lua_newtable(lua);
lua_pushstring(lua,"string");
lua_pushlstring(lua,str,len);
lua_settable(lua,-3);
lua_pushstring(lua,"format");
lua_pushlstring(lua,format,3);
lua_settable(lua,-3);
lua_settable(lua,-3);
}
static void redisProtocolToLuaType_BigNumber(void *ctx, const char *str, size_t len, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 3)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
lua_pushstring(lua,"big_number");
lua_pushlstring(lua,str,len);
lua_settable(lua,-3);
}
static void redisProtocolToLuaType_Null(void *ctx, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 1)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_pushnil(lua);
}
static void redisProtocolToLuaType_Bool(void *ctx, int val, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 1)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_pushboolean(lua,val);
}
static void redisProtocolToLuaType_Double(void *ctx, double d, const char *proto, size_t proto_len) {
UNUSED(proto);
UNUSED(proto_len);
if (!ctx) {
return;
}
lua_State *lua = ctx;
if (!lua_checkstack(lua, 3)) {
serverPanic("lua stack limit reach when parsing redis.call reply");
}
lua_newtable(lua);
lua_pushstring(lua,"double");
lua_pushnumber(lua,d);
lua_settable(lua,-3);
}
void luaPushError(lua_State *lua, char *error) {
lua_Debug dbg;
if (ldbIsEnabled()) {
ldbLog(sdscatprintf(sdsempty(),"<error> %s",error));
}
lua_newtable(lua);
lua_pushstring(lua,"err");
if(lua_getstack(lua, 1, &dbg) && lua_getinfo(lua, "nSl", &dbg)) {
sds msg = sdscatprintf(sdsempty(), "%s: %d: %s",
dbg.source, dbg.currentline, error);
lua_pushstring(lua, msg);
sdsfree(msg);
} else {
lua_pushstring(lua, error);
}
lua_settable(lua,-3);
}
int luaRaiseError(lua_State *lua) {
lua_pushstring(lua,"err");
lua_gettable(lua,-2);
return lua_error(lua);
}
static void luaReplyToRedisReply(client *c, client* script_client, lua_State *lua) {
int t = lua_type(lua,-1);
if (!lua_checkstack(lua, 4)) {
addReplyErrorFormat(c, "reached lua stack limit");
lua_pop(lua,1);
return;
}
switch(t) {
case LUA_TSTRING:
addReplyBulkCBuffer(c,(char*)lua_tostring(lua,-1),lua_strlen(lua,-1));
break;
case LUA_TBOOLEAN:
if (script_client->resp == 2)
addReply(c,lua_toboolean(lua,-1) ? shared.cone :
shared.null[c->resp]);
else
addReplyBool(c,lua_toboolean(lua,-1));
break;
case LUA_TNUMBER:
addReplyLongLong(c,(long long)lua_tonumber(lua,-1));
break;
case LUA_TTABLE:
lua_pushstring(lua,"err");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TSTRING) {
addReplyErrorFormat(c,"-%s",lua_tostring(lua,-1));
lua_pop(lua,2);
return;
}
lua_pop(lua,1);
lua_pushstring(lua,"ok");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TSTRING) {
sds ok = sdsnew(lua_tostring(lua,-1));
sdsmapchars(ok,"\r\n"," ",2);
addReplySds(c,sdscatprintf(sdsempty(),"+%s\r\n",ok));
sdsfree(ok);
lua_pop(lua,2);
return;
}
lua_pop(lua,1);
lua_pushstring(lua,"double");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TNUMBER) {
addReplyDouble(c,lua_tonumber(lua,-1));
lua_pop(lua,2);
return;
}
lua_pop(lua,1);
lua_pushstring(lua,"big_number");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TSTRING) {
sds big_num = sdsnewlen(lua_tostring(lua,-1), lua_strlen(lua,-1));
sdsmapchars(big_num,"\r\n"," ",2);
addReplyBigNum(c,big_num,sdslen(big_num));
sdsfree(big_num);
lua_pop(lua,2);
return;
}
lua_pop(lua,1);
lua_pushstring(lua,"verbatim_string");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TTABLE) {
lua_pushstring(lua,"format");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TSTRING){
char* format = (char*)lua_tostring(lua,-1);
lua_pushstring(lua,"string");
lua_gettable(lua,-3);
t = lua_type(lua,-1);
if (t == LUA_TSTRING){
size_t len;
char* str = (char*)lua_tolstring(lua,-1,&len);
addReplyVerbatim(c, str, len, format);
lua_pop(lua,4);
return;
}
lua_pop(lua,1);
}
lua_pop(lua,1);
}
lua_pop(lua,1);
lua_pushstring(lua,"map");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TTABLE) {
int maplen = 0;
void *replylen = addReplyDeferredLen(c);
lua_pushnil(lua);
while (lua_next(lua,-2)) {
lua_pushvalue(lua,-2);
luaReplyToRedisReply(c, script_client, lua);
luaReplyToRedisReply(c, script_client, lua);
maplen++;
}
setDeferredMapLen(c,replylen,maplen);
lua_pop(lua,2);
return;
}
lua_pop(lua,1);
lua_pushstring(lua,"set");
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TTABLE) {
int setlen = 0;
void *replylen = addReplyDeferredLen(c);
lua_pushnil(lua);
while (lua_next(lua,-2)) {
lua_pop(lua,1);
lua_pushvalue(lua,-1);
luaReplyToRedisReply(c, script_client, lua);
setlen++;
}
setDeferredSetLen(c,replylen,setlen);
lua_pop(lua,2);
return;
}
lua_pop(lua,1);
void *replylen = addReplyDeferredLen(c);
int j = 1, mbulklen = 0;
while(1) {
lua_pushnumber(lua,j++);
lua_gettable(lua,-2);
t = lua_type(lua,-1);
if (t == LUA_TNIL) {
lua_pop(lua,1);
break;
}
luaReplyToRedisReply(c, script_client, lua);
mbulklen++;
}
setDeferredArrayLen(c,replylen,mbulklen);
break;
default:
addReplyNull(c);
}
lua_pop(lua,1);
}
#define LUA_CMD_OBJCACHE_SIZE 32
#define LUA_CMD_OBJCACHE_MAX_LEN 64
static int luaRedisGenericCommand(lua_State *lua, int raise_error) {
int j, argc = lua_gettop(lua);
scriptRunCtx* rctx = luaGetFromRegistry(lua, REGISTRY_RUN_CTX_NAME);
if (!rctx) {
luaPushError(lua, "redis.call/pcall can only be called inside a script invocation");
return luaRaiseError(lua);
}
sds err = NULL;
client* c = rctx->c;
sds reply;
static robj **argv = NULL;
static int argv_size = 0;
static robj *cached_objects[LUA_CMD_OBJCACHE_SIZE];
static size_t cached_objects_len[LUA_CMD_OBJCACHE_SIZE];
static int inuse = 0;
if (inuse) {
char *recursion_warning =
"luaRedisGenericCommand() recursive call detected. "
"Are you doing funny stuff with Lua debug hooks?";
serverLog(LL_WARNING,"%s",recursion_warning);
luaPushError(lua,recursion_warning);
return 1;
}
inuse++;
if (argc == 0) {
luaPushError(lua,
"Please specify at least one argument for redis.call()");
inuse--;
return raise_error ? luaRaiseError(lua) : 1;
}
if (argv_size < argc) {
argv = zrealloc(argv,sizeof(robj*)*argc);
argv_size = argc;
}
for (j = 0; j < argc; j++) {
char *obj_s;
size_t obj_len;
char dbuf[64];
if (lua_type(lua,j+1) == LUA_TNUMBER) {
lua_Number num = lua_tonumber(lua,j+1);
obj_len = snprintf(dbuf,sizeof(dbuf),"%.17g",(double)num);
obj_s = dbuf;
} else {
obj_s = (char*)lua_tolstring(lua,j+1,&obj_len);
if (obj_s == NULL) break;
}
if (j < LUA_CMD_OBJCACHE_SIZE && cached_objects[j] &&
cached_objects_len[j] >= obj_len)
{
sds s = cached_objects[j]->ptr;
argv[j] = cached_objects[j];
cached_objects[j] = NULL;
memcpy(s,obj_s,obj_len+1);
sdssetlen(s, obj_len);
} else {
argv[j] = createStringObject(obj_s, obj_len);
}
}
if (j != argc) {
j--;
while (j >= 0) {
decrRefCount(argv[j]);
j--;
}
luaPushError(lua,
"Lua redis() command arguments must be strings or integers");
inuse--;
return raise_error ? luaRaiseError(lua) : 1;
}
lua_pop(lua, argc);
if (ldbIsEnabled()) {
sds cmdlog = sdsnew("<redis>");
for (j = 0; j < c->argc; j++) {
if (j == 10) {
cmdlog = sdscatprintf(cmdlog," ... (%d more)",
c->argc-j-1);
break;
} else {
cmdlog = sdscatlen(cmdlog," ",1);
cmdlog = sdscatsds(cmdlog,c->argv[j]->ptr);
}
}
ldbLog(cmdlog);
}
scriptCall(rctx, argv, argc, &err);
if (err) {
luaPushError(lua, err);
sdsfree(err);
goto cleanup;
}
if (listLength(c->reply) == 0 && (size_t)c->bufpos < c->buf_usable_size) {
c->buf[c->bufpos] = '\0';
reply = c->buf;
c->bufpos = 0;
} else {
reply = sdsnewlen(c->buf,c->bufpos);
c->bufpos = 0;
while(listLength(c->reply)) {
clientReplyBlock *o = listNodeValue(listFirst(c->reply));
reply = sdscatlen(reply,o->buf,o->used);
listDelNode(c->reply,listFirst(c->reply));
}
}
if (raise_error && reply[0] != '-') raise_error = 0;
redisProtocolToLuaType(lua,reply);
if (ldbIsEnabled())
ldbLogRedisReply(reply);
if (reply != c->buf) sdsfree(reply);
c->reply_bytes = 0;
cleanup:
for (j = 0; j < c->argc; j++) {
robj *o = c->argv[j];
if (j < LUA_CMD_OBJCACHE_SIZE &&
o->refcount == 1 &&
(o->encoding == OBJ_ENCODING_RAW ||
o->encoding == OBJ_ENCODING_EMBSTR) &&
sdslen(o->ptr) <= LUA_CMD_OBJCACHE_MAX_LEN)
{
sds s = o->ptr;
if (cached_objects[j]) decrRefCount(cached_objects[j]);
cached_objects[j] = o;
cached_objects_len[j] = sdsalloc(s);
} else {
decrRefCount(o);
}
}
if (c->argv != argv) {
zfree(c->argv);
argv = NULL;
argv_size = 0;
}
c->user = NULL;
c->argv = NULL;
c->argc = 0;
if (raise_error) {
inuse--;
return luaRaiseError(lua);
}
inuse--;
return 1;
}
static int luaRedisCallCommand(lua_State *lua) {
return luaRedisGenericCommand(lua,1);
}
static int luaRedisPCallCommand(lua_State *lua) {
return luaRedisGenericCommand(lua,0);
}
static int luaRedisSha1hexCommand(lua_State *lua) {
int argc = lua_gettop(lua);
char digest[41];
size_t len;
char *s;
if (argc != 1) {
lua_pushstring(lua, "wrong number of arguments");
return lua_error(lua);
}
s = (char*)lua_tolstring(lua,1,&len);
sha1hex(digest,s,len);
lua_pushstring(lua,digest);
return 1;
}
static int luaRedisReturnSingleFieldTable(lua_State *lua, char *field) {
if (lua_gettop(lua) != 1 || lua_type(lua,-1) != LUA_TSTRING) {
luaPushError(lua, "wrong number or type of arguments");
return 1;
}
lua_newtable(lua);
lua_pushstring(lua, field);
lua_pushvalue(lua, -3);
lua_settable(lua, -3);
return 1;
}
static int luaRedisErrorReplyCommand(lua_State *lua) {
return luaRedisReturnSingleFieldTable(lua,"err");
}
static int luaRedisStatusReplyCommand(lua_State *lua) {
return luaRedisReturnSingleFieldTable(lua,"ok");
}
static int luaRedisSetReplCommand(lua_State *lua) {
int flags, argc = lua_gettop(lua);
scriptRunCtx* rctx = luaGetFromRegistry(lua, REGISTRY_RUN_CTX_NAME);
if (!rctx) {
lua_pushstring(lua, "redis.set_repl can only be called inside a script invocation");
return lua_error(lua);
}
if (argc != 1) {
lua_pushstring(lua, "redis.set_repl() requires two arguments.");
return lua_error(lua);
}
flags = lua_tonumber(lua,-1);
if ((flags & ~(PROPAGATE_AOF|PROPAGATE_REPL)) != 0) {
lua_pushstring(lua, "Invalid replication flags. Use REPL_AOF, REPL_REPLICA, REPL_ALL or REPL_NONE.");
return lua_error(lua);
}
scriptSetRepl(rctx, flags);
return 0;
}
static int luaLogCommand(lua_State *lua) {
int j, argc = lua_gettop(lua);
int level;
sds log;
if (argc < 2) {
lua_pushstring(lua, "redis.log() requires two arguments or more.");
return lua_error(lua);
} else if (!lua_isnumber(lua,-argc)) {
lua_pushstring(lua, "First argument must be a number (log level).");
return lua_error(lua);
}
level = lua_tonumber(lua,-argc);
if (level < LL_DEBUG || level > LL_WARNING) {
lua_pushstring(lua, "Invalid debug level.");
return lua_error(lua);
}
if (level < server.verbosity) return 0;
log = sdsempty();
for (j = 1; j < argc; j++) {
size_t len;
char *s;
s = (char*)lua_tolstring(lua,(-argc)+j,&len);
if (s) {
if (j != 1) log = sdscatlen(log," ",1);
log = sdscatlen(log,s,len);
}
}
serverLogRaw(level,log);
sdsfree(log);
return 0;
}
static int luaSetResp(lua_State *lua) {
scriptRunCtx* rctx = luaGetFromRegistry(lua, REGISTRY_RUN_CTX_NAME);
if (!rctx) {
lua_pushstring(lua, "redis.setresp can only be called inside a script invocation");
return lua_error(lua);
}
int argc = lua_gettop(lua);
if (argc != 1) {
lua_pushstring(lua, "redis.setresp() requires one argument.");
return lua_error(lua);
}
int resp = lua_tonumber(lua,-argc);
if (resp != 2 && resp != 3) {
lua_pushstring(lua, "RESP version must be 2 or 3.");
return lua_error(lua);
}
scriptSetResp(rctx, resp);
return 0;
}
static void luaLoadLib(lua_State *lua, const char *libname, lua_CFunction luafunc) {
lua_pushcfunction(lua, luafunc);
lua_pushstring(lua, libname);
lua_call(lua, 1, 0);
}
LUALIB_API int (luaopen_cjson) (lua_State *L);
LUALIB_API int (luaopen_struct) (lua_State *L);
LUALIB_API int (luaopen_cmsgpack) (lua_State *L);
LUALIB_API int (luaopen_bit) (lua_State *L);
static void luaLoadLibraries(lua_State *lua) {
luaLoadLib(lua, "", luaopen_base);
luaLoadLib(lua, LUA_TABLIBNAME, luaopen_table);
luaLoadLib(lua, LUA_STRLIBNAME, luaopen_string);
luaLoadLib(lua, LUA_MATHLIBNAME, luaopen_math);
luaLoadLib(lua, LUA_DBLIBNAME, luaopen_debug);
luaLoadLib(lua, "cjson", luaopen_cjson);
luaLoadLib(lua, "struct", luaopen_struct);
luaLoadLib(lua, "cmsgpack", luaopen_cmsgpack);
luaLoadLib(lua, "bit", luaopen_bit);
#if 0
luaLoadLib(lua, LUA_LOADLIBNAME, luaopen_package);
luaLoadLib(lua, LUA_OSLIBNAME, luaopen_os);
#endif
}
static void luaRemoveUnsupportedFunctions(lua_State *lua) {
lua_pushnil(lua);
lua_setglobal(lua,"loadfile");
lua_pushnil(lua);
lua_setglobal(lua,"dofile");
}
sds luaGetStringSds(lua_State *lua, int index) {
if (!lua_isstring(lua, index)) {
return NULL;
}
size_t len;
const char *str = lua_tolstring(lua, index, &len);
sds str_sds = sdsnewlen(str, len);
return str_sds;
}
void luaEnableGlobalsProtection(lua_State *lua, int is_eval) {
char *s[32];
sds code = sdsempty();
int j = 0;
s[j++]="local dbg=debug\n";
s[j++]="local mt = {}\n";
s[j++]="setmetatable(_G, mt)\n";
s[j++]="mt.__newindex = function (t, n, v)\n";
s[j++]=" if dbg.getinfo(2) then\n";
s[j++]=" local w = dbg.getinfo(2, \"S\").what\n";
s[j++]= is_eval ? " if w ~= \"main\" and w ~= \"C\" then\n" : " if w ~= \"C\" then\n";
s[j++]=" error(\"Script attempted to create global variable '\"..tostring(n)..\"'\", 2)\n";
s[j++]=" end\n";
s[j++]=" end\n";
s[j++]=" rawset(t, n, v)\n";
s[j++]="end\n";
s[j++]="mt.__index = function (t, n)\n";
s[j++]=" if dbg.getinfo(2) and dbg.getinfo(2, \"S\").what ~= \"C\" then\n";
s[j++]=" error(\"Script attempted to access nonexistent global variable '\"..tostring(n)..\"'\", 2)\n";
s[j++]=" end\n";
s[j++]=" return rawget(t, n)\n";
s[j++]="end\n";
s[j++]="debug = nil\n";
s[j++]=NULL;
for (j = 0; s[j] != NULL; j++) code = sdscatlen(code,s[j],strlen(s[j]));
luaL_loadbuffer(lua,code,sdslen(code),"@enable_strict_lua");
lua_pcall(lua,0,0,0);
sdsfree(code);
}
void luaRegisterGlobalProtectionFunction(lua_State *lua) {
lua_pushstring(lua, REGISTRY_SET_GLOBALS_PROTECTION_NAME);
char *global_protection_func = "local dbg = debug\n"
"local globals_protection = function (t)\n"
" local mt = {}\n"
" setmetatable(t, mt)\n"
" mt.__newindex = function (t, n, v)\n"
" if dbg.getinfo(2) then\n"
" local w = dbg.getinfo(2, \"S\").what\n"
" if w ~= \"C\" then\n"
" error(\"Script attempted to create global variable '\"..tostring(n)..\"'\", 2)\n"
" end"
" end"
" rawset(t, n, v)\n"
" end\n"
" mt.__index = function (t, n)\n"
" if dbg.getinfo(2) and dbg.getinfo(2, \"S\").what ~= \"C\" then\n"
" error(\"Script attempted to access nonexistent global variable '\"..tostring(n)..\"'\", 2)\n"
" end\n"
" return rawget(t, n)\n"
" end\n"
"end\n"
"return globals_protection";
int res = luaL_loadbuffer(lua, global_protection_func, strlen(global_protection_func), "@global_protection_def");
serverAssert(res == 0);
res = lua_pcall(lua,0,1,0);
serverAssert(res == 0);
lua_settable(lua, LUA_REGISTRYINDEX);
}
void luaSetGlobalProtection(lua_State *lua) {
lua_pushstring(lua, REGISTRY_SET_GLOBALS_PROTECTION_NAME);
lua_gettable(lua, LUA_REGISTRYINDEX);
lua_pushvalue(lua, -2);
int res = lua_pcall(lua, 1, 0, 0);
serverAssert(res == 0);
}
void luaRegisterVersion(lua_State* lua) {
lua_pushstring(lua,"REDIS_VERSION_NUM");
lua_pushnumber(lua,REDIS_VERSION_NUM);
lua_settable(lua,-3);
lua_pushstring(lua,"REDIS_VERSION");
lua_pushstring(lua,REDIS_VERSION);
lua_settable(lua,-3);
}
void luaRegisterLogFunction(lua_State* lua) {
lua_pushstring(lua,"log");
lua_pushcfunction(lua,luaLogCommand);
lua_settable(lua,-3);
lua_pushstring(lua,"LOG_DEBUG");
lua_pushnumber(lua,LL_DEBUG);
lua_settable(lua,-3);
lua_pushstring(lua,"LOG_VERBOSE");
lua_pushnumber(lua,LL_VERBOSE);
lua_settable(lua,-3);
lua_pushstring(lua,"LOG_NOTICE");
lua_pushnumber(lua,LL_NOTICE);
lua_settable(lua,-3);
lua_pushstring(lua,"LOG_WARNING");
lua_pushnumber(lua,LL_WARNING);
lua_settable(lua,-3);
}
void luaRegisterRedisAPI(lua_State* lua) {
luaLoadLibraries(lua);
luaRemoveUnsupportedFunctions(lua);
lua_newtable(lua);
lua_pushstring(lua,"call");
lua_pushcfunction(lua,luaRedisCallCommand);
lua_settable(lua,-3);
lua_pushstring(lua,"pcall");
lua_pushcfunction(lua,luaRedisPCallCommand);
lua_settable(lua,-3);
luaRegisterLogFunction(lua);
luaRegisterVersion(lua);
lua_pushstring(lua,"setresp");
lua_pushcfunction(lua,luaSetResp);
lua_settable(lua,-3);
lua_pushstring(lua, "sha1hex");
lua_pushcfunction(lua, luaRedisSha1hexCommand);
lua_settable(lua, -3);
lua_pushstring(lua, "error_reply");
lua_pushcfunction(lua, luaRedisErrorReplyCommand);
lua_settable(lua, -3);
lua_pushstring(lua, "status_reply");
lua_pushcfunction(lua, luaRedisStatusReplyCommand);
lua_settable(lua, -3);
lua_pushstring(lua,"set_repl");
lua_pushcfunction(lua,luaRedisSetReplCommand);
lua_settable(lua,-3);
lua_pushstring(lua,"REPL_NONE");
lua_pushnumber(lua,PROPAGATE_NONE);
lua_settable(lua,-3);
lua_pushstring(lua,"REPL_AOF");
lua_pushnumber(lua,PROPAGATE_AOF);
lua_settable(lua,-3);
lua_pushstring(lua,"REPL_SLAVE");
lua_pushnumber(lua,PROPAGATE_REPL);
lua_settable(lua,-3);
lua_pushstring(lua,"REPL_REPLICA");
lua_pushnumber(lua,PROPAGATE_REPL);
lua_settable(lua,-3);
lua_pushstring(lua,"REPL_ALL");
lua_pushnumber(lua,PROPAGATE_AOF|PROPAGATE_REPL);
lua_settable(lua,-3);
lua_setglobal(lua,REDIS_API_NAME);
lua_getglobal(lua,"math");
lua_pushstring(lua,"random");
lua_pushcfunction(lua,redis_math_random);
lua_settable(lua,-3);
lua_pushstring(lua,"randomseed");
lua_pushcfunction(lua,redis_math_randomseed);
lua_settable(lua,-3);
lua_setglobal(lua,"math");
}
static void luaCreateArray(lua_State *lua, robj **elev, int elec) {
int j;
lua_newtable(lua);
for (j = 0; j < elec; j++) {
lua_pushlstring(lua,(char*)elev[j]->ptr,sdslen(elev[j]->ptr));
lua_rawseti(lua,-2,j+1);
}
}
static int redis_math_random (lua_State *L) {
scriptRunCtx* rctx = luaGetFromRegistry(L, REGISTRY_RUN_CTX_NAME);
if (!rctx) {
return luaL_error(L, "math.random can only be called inside a script invocation");
}
lua_Number r = (lua_Number)(redisLrand48()%REDIS_LRAND48_MAX) /
(lua_Number)REDIS_LRAND48_MAX;
switch (lua_gettop(L)) {
case 0: {
lua_pushnumber(L, r);
break;
}
case 1: {
int u = luaL_checkint(L, 1);
luaL_argcheck(L, 1<=u, 1, "interval is empty");
lua_pushnumber(L, floor(r*u)+1);
break;
}
case 2: {
int l = luaL_checkint(L, 1);
int u = luaL_checkint(L, 2);
luaL_argcheck(L, l<=u, 2, "interval is empty");
lua_pushnumber(L, floor(r*(u-l+1))+l);
break;
}
default: return luaL_error(L, "wrong number of arguments");
}
return 1;
}
static int redis_math_randomseed (lua_State *L) {
scriptRunCtx* rctx = luaGetFromRegistry(L, REGISTRY_RUN_CTX_NAME);
if (!rctx) {
return luaL_error(L, "math.randomseed can only be called inside a script invocation");
}
redisSrand48(luaL_checkint(L, 1));
return 0;
}
static void luaMaskCountHook(lua_State *lua, lua_Debug *ar) {
UNUSED(ar);
scriptRunCtx* rctx = luaGetFromRegistry(lua, REGISTRY_RUN_CTX_NAME);
if (scriptInterrupt(rctx) == SCRIPT_KILL) {
serverLog(LL_WARNING,"Lua script killed by user with SCRIPT KILL.");
lua_sethook(lua, luaMaskCountHook, LUA_MASKLINE, 0);
lua_pushstring(lua,"Script killed by user with SCRIPT KILL...");
lua_error(lua);
}
}
void luaCallFunction(scriptRunCtx* run_ctx, lua_State *lua, robj** keys, size_t nkeys, robj** args, size_t nargs, int debug_enabled) {
client* c = run_ctx->original_client;
int delhook = 0;
luaSaveOnRegistry(lua, REGISTRY_RUN_CTX_NAME, run_ctx);
if (server.busy_reply_threshold > 0 && !debug_enabled) {
lua_sethook(lua,luaMaskCountHook,LUA_MASKCOUNT,100000);
delhook = 1;
} else if (debug_enabled) {
lua_sethook(lua,luaLdbLineHook,LUA_MASKLINE|LUA_MASKCOUNT,100000);
delhook = 1;
}
luaCreateArray(lua,keys,nkeys);
if (run_ctx->flags & SCRIPT_EVAL_MODE) lua_setglobal(lua,"KEYS");
luaCreateArray(lua,args,nargs);
if (run_ctx->flags & SCRIPT_EVAL_MODE) lua_setglobal(lua,"ARGV");
int err;
if (run_ctx->flags & SCRIPT_EVAL_MODE) {
err = lua_pcall(lua,0,1,-2);
} else {
err = lua_pcall(lua,2,1,-4);
}
#define LUA_GC_CYCLE_PERIOD 50
{
static long gc_count = 0;
gc_count++;
if (gc_count == LUA_GC_CYCLE_PERIOD) {
lua_gc(lua,LUA_GCSTEP,LUA_GC_CYCLE_PERIOD);
gc_count = 0;
}
}
if (err) {
addReplyErrorFormat(c,"Error running script (call to %s): %s\n",
run_ctx->funcname, lua_tostring(lua,-1));
lua_pop(lua,1);
} else {
luaReplyToRedisReply(c, run_ctx->c, lua);
}
if (delhook) lua_sethook(lua,NULL,0,0);
luaSaveOnRegistry(lua, REGISTRY_RUN_CTX_NAME, NULL);
}
unsigned long luaMemory(lua_State *lua) {
return lua_gc(lua, LUA_GCCOUNT, 0) * 1024LL;
}
