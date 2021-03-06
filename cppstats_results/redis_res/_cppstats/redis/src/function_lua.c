#include "functions.h"
#include "script_lua.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#define LUA_ENGINE_NAME "LUA"
#define REGISTRY_ENGINE_CTX_NAME "__ENGINE_CTX__"
#define REGISTRY_ERROR_HANDLER_NAME "__ERROR_HANDLER__"
#define REGISTRY_LOAD_CTX_NAME "__LIBRARY_CTX__"
#define LIBRARY_API_NAME "__LIBRARY_API__"
#define LOAD_TIMEOUT_MS 500
typedef struct luaEngineCtx {
lua_State *lua;
} luaEngineCtx;
typedef struct luaFunctionCtx {
int lua_function_ref;
} luaFunctionCtx;
typedef struct loadCtx {
functionLibInfo *li;
monotime start_time;
} loadCtx;
typedef struct registerFunctionArgs {
sds name;
sds desc;
luaFunctionCtx *lua_f_ctx;
uint64_t f_flags;
} registerFunctionArgs;
static void luaEngineLoadHook(lua_State *lua, lua_Debug *ar) {
UNUSED(ar);
loadCtx *load_ctx = luaGetFromRegistry(lua, REGISTRY_LOAD_CTX_NAME);
uint64_t duration = elapsedMs(load_ctx->start_time);
if (duration > LOAD_TIMEOUT_MS) {
lua_sethook(lua, luaEngineLoadHook, LUA_MASKLINE, 0);
lua_pushstring(lua,"FUNCTION LOAD timeout");
lua_error(lua);
}
}
static int luaEngineCreate(void *engine_ctx, functionLibInfo *li, sds blob, sds *err) {
luaEngineCtx *lua_engine_ctx = engine_ctx;
lua_State *lua = lua_engine_ctx->lua;
lua_newtable(lua);
lua_pushstring(lua, REDIS_API_NAME);
lua_pushstring(lua, LIBRARY_API_NAME);
lua_gettable(lua, LUA_REGISTRYINDEX);
lua_settable(lua, -3);
luaSetGlobalProtection(lua_engine_ctx->lua);
if (luaL_loadbuffer(lua, blob, sdslen(blob), "@user_function")) {
*err = sdscatprintf(sdsempty(), "Error compiling function: %s", lua_tostring(lua, -1));
lua_pop(lua, 2);
return C_ERR;
}
serverAssert(lua_isfunction(lua, -1));
loadCtx load_ctx = {
.li = li,
.start_time = getMonotonicUs(),
};
luaSaveOnRegistry(lua, REGISTRY_LOAD_CTX_NAME, &load_ctx);
lua_pushvalue(lua, -2);
lua_setfenv(lua, -2);
lua_sethook(lua,luaEngineLoadHook,LUA_MASKCOUNT,100000);
if (lua_pcall(lua,0,0,0)) {
*err = sdscatprintf(sdsempty(), "Error registering functions: %s", lua_tostring(lua, -1));
lua_pop(lua, 2);
lua_sethook(lua,NULL,0,0);
luaSaveOnRegistry(lua, REGISTRY_LOAD_CTX_NAME, NULL);
return C_ERR;
}
lua_sethook(lua,NULL,0,0);
luaSaveOnRegistry(lua, REGISTRY_LOAD_CTX_NAME, NULL);
lua_pushstring(lua, REDIS_API_NAME);
lua_pushnil(lua);
lua_settable(lua, -3);
lua_newtable(lua);
lua_pushstring(lua, "__index");
lua_pushvalue(lua, LUA_GLOBALSINDEX);
lua_settable(lua, -3);
lua_pushstring(lua, "__newindex");
lua_pushvalue(lua, LUA_GLOBALSINDEX);
lua_settable(lua, -3);
lua_setmetatable(lua, -2);
lua_pop(lua, 1);
return C_OK;
}
static void luaEngineCall(scriptRunCtx *run_ctx,
void *engine_ctx,
void *compiled_function,
robj **keys,
size_t nkeys,
robj **args,
size_t nargs)
{
luaEngineCtx *lua_engine_ctx = engine_ctx;
lua_State *lua = lua_engine_ctx->lua;
luaFunctionCtx *f_ctx = compiled_function;
lua_pushstring(lua, REGISTRY_ERROR_HANDLER_NAME);
lua_gettable(lua, LUA_REGISTRYINDEX);
lua_rawgeti(lua, LUA_REGISTRYINDEX, f_ctx->lua_function_ref);
serverAssert(lua_isfunction(lua, -1));
luaCallFunction(run_ctx, lua, keys, nkeys, args, nargs, 0);
lua_pop(lua, 1);
}
static size_t luaEngineGetUsedMemoy(void *engine_ctx) {
luaEngineCtx *lua_engine_ctx = engine_ctx;
return luaMemory(lua_engine_ctx->lua);
}
static size_t luaEngineFunctionMemoryOverhead(void *compiled_function) {
return zmalloc_size(compiled_function);
}
static size_t luaEngineMemoryOverhead(void *engine_ctx) {
luaEngineCtx *lua_engine_ctx = engine_ctx;
return zmalloc_size(lua_engine_ctx);
}
static void luaEngineFreeFunction(void *engine_ctx, void *compiled_function) {
luaEngineCtx *lua_engine_ctx = engine_ctx;
lua_State *lua = lua_engine_ctx->lua;
luaFunctionCtx *f_ctx = compiled_function;
lua_unref(lua, f_ctx->lua_function_ref);
zfree(f_ctx);
}
static void luaRegisterFunctionArgsInitialize(registerFunctionArgs *register_f_args,
sds name,
sds desc,
luaFunctionCtx *lua_f_ctx,
uint64_t flags)
{
*register_f_args = (registerFunctionArgs){
.name = name,
.desc = desc,
.lua_f_ctx = lua_f_ctx,
.f_flags = flags,
};
}
static void luaRegisterFunctionArgsDispose(lua_State *lua, registerFunctionArgs *register_f_args) {
sdsfree(register_f_args->name);
if (register_f_args->desc) sdsfree(register_f_args->desc);
lua_unref(lua, register_f_args->lua_f_ctx->lua_function_ref);
zfree(register_f_args->lua_f_ctx);
}
static int luaRegisterFunctionReadFlags(lua_State *lua, uint64_t *flags) {
int j = 1;
int ret = C_ERR;
int f_flags = 0;
while(1) {
lua_pushnumber(lua,j++);
lua_gettable(lua,-2);
int t = lua_type(lua,-1);
if (t == LUA_TNIL) {
lua_pop(lua,1);
break;
}
if (!lua_isstring(lua, -1)) {
lua_pop(lua,1);
goto done;
}
const char *flag_str = lua_tostring(lua, -1);
int found = 0;
for (scriptFlag *flag = scripts_flags_def; flag->str ; ++flag) {
if (!strcasecmp(flag->str, flag_str)) {
f_flags |= flag->flag;
found = 1;
break;
}
}
lua_pop(lua,1);
if (!found) {
goto done;
}
}
*flags = f_flags;
ret = C_OK;
done:
return ret;
}
static int luaRegisterFunctionReadNamedArgs(lua_State *lua, registerFunctionArgs *register_f_args) {
char *err = NULL;
sds name = NULL;
sds desc = NULL;
luaFunctionCtx *lua_f_ctx = NULL;
uint64_t flags = 0;
if (!lua_istable(lua, 1)) {
err = "calling redis.register_function with a single argument is only applicable to Lua table (representing named arguments).";
goto error;
}
lua_pushnil(lua);
while (lua_next(lua, -2)) {
if (!lua_isstring(lua, -2)) {
err = "named argument key given to redis.register_function is not a string";
goto error;
}
const char *key = lua_tostring(lua, -2);
if (!strcasecmp(key, "function_name")) {
if (!(name = luaGetStringSds(lua, -1))) {
err = "function_name argument given to redis.register_function must be a string";
goto error;
}
} else if (!strcasecmp(key, "description")) {
if (!(desc = luaGetStringSds(lua, -1))) {
err = "description argument given to redis.register_function must be a string";
goto error;
}
} else if (!strcasecmp(key, "callback")) {
if (!lua_isfunction(lua, -1)) {
err = "callback argument given to redis.register_function must be a function";
goto error;
}
int lua_function_ref = luaL_ref(lua, LUA_REGISTRYINDEX);
lua_f_ctx = zmalloc(sizeof(*lua_f_ctx));
lua_f_ctx->lua_function_ref = lua_function_ref;
continue;
} else if (!strcasecmp(key, "flags")) {
if (!lua_istable(lua, -1)) {
err = "flags argument to redis.register_function must be a table representing function flags";
goto error;
}
if (luaRegisterFunctionReadFlags(lua, &flags) != C_OK) {
err = "unknown flag given";
goto error;
}
} else {
err = "unknown argument given to redis.register_function";
goto error;
}
lua_pop(lua, 1);
}
if (!name) {
err = "redis.register_function must get a function name argument";
goto error;
}
if (!lua_f_ctx) {
err = "redis.register_function must get a callback argument";
goto error;
}
luaRegisterFunctionArgsInitialize(register_f_args, name, desc, lua_f_ctx, flags);
return C_OK;
error:
if (name) sdsfree(name);
if (desc) sdsfree(desc);
if (lua_f_ctx) {
lua_unref(lua, lua_f_ctx->lua_function_ref);
zfree(lua_f_ctx);
}
luaPushError(lua, err);
return C_ERR;
}
static int luaRegisterFunctionReadPositionalArgs(lua_State *lua, registerFunctionArgs *register_f_args) {
char *err = NULL;
sds name = NULL;
sds desc = NULL;
luaFunctionCtx *lua_f_ctx = NULL;
if (!(name = luaGetStringSds(lua, 1))) {
err = "first argument to redis.register_function must be a string";
goto error;
}
if (!lua_isfunction(lua, 2)) {
err = "second argument to redis.register_function must be a function";
goto error;
}
int lua_function_ref = luaL_ref(lua, LUA_REGISTRYINDEX);
lua_f_ctx = zmalloc(sizeof(*lua_f_ctx));
lua_f_ctx->lua_function_ref = lua_function_ref;
luaRegisterFunctionArgsInitialize(register_f_args, name, NULL, lua_f_ctx, 0);
return C_OK;
error:
if (name) sdsfree(name);
if (desc) sdsfree(desc);
luaPushError(lua, err);
return C_ERR;
}
static int luaRegisterFunctionReadArgs(lua_State *lua, registerFunctionArgs *register_f_args) {
int argc = lua_gettop(lua);
if (argc < 1 || argc > 2) {
luaPushError(lua, "wrong number of arguments to redis.register_function");
return C_ERR;
}
if (argc == 1) {
return luaRegisterFunctionReadNamedArgs(lua, register_f_args);
} else {
return luaRegisterFunctionReadPositionalArgs(lua, register_f_args);
}
}
static int luaRegisterFunction(lua_State *lua) {
registerFunctionArgs register_f_args = {0};
loadCtx *load_ctx = luaGetFromRegistry(lua, REGISTRY_LOAD_CTX_NAME);
if (!load_ctx) {
luaPushError(lua, "redis.register_function can only be called on FUNCTION LOAD command");
return luaRaiseError(lua);
}
if (luaRegisterFunctionReadArgs(lua, &register_f_args) != C_OK) {
return luaRaiseError(lua);
}
sds err = NULL;
if (functionLibCreateFunction(register_f_args.name, register_f_args.lua_f_ctx, load_ctx->li, register_f_args.desc, register_f_args.f_flags, &err) != C_OK) {
luaRegisterFunctionArgsDispose(lua, &register_f_args);
luaPushError(lua, err);
sdsfree(err);
return luaRaiseError(lua);
}
return 0;
}
int luaEngineInitEngine() {
luaEngineCtx *lua_engine_ctx = zmalloc(sizeof(*lua_engine_ctx));
lua_engine_ctx->lua = lua_open();
luaRegisterRedisAPI(lua_engine_ctx->lua);
lua_pushstring(lua_engine_ctx->lua, LIBRARY_API_NAME);
lua_newtable(lua_engine_ctx->lua);
lua_pushstring(lua_engine_ctx->lua, "register_function");
lua_pushcfunction(lua_engine_ctx->lua, luaRegisterFunction);
lua_settable(lua_engine_ctx->lua, -3);
luaRegisterLogFunction(lua_engine_ctx->lua);
luaRegisterVersion(lua_engine_ctx->lua);
lua_settable(lua_engine_ctx->lua, LUA_REGISTRYINDEX);
lua_pushstring(lua_engine_ctx->lua, REGISTRY_ERROR_HANDLER_NAME);
char *errh_func = "local dbg = debug\n"
"local error_handler = function (err)\n"
" local i = dbg.getinfo(2,'nSl')\n"
" if i and i.what == 'C' then\n"
" i = dbg.getinfo(3,'nSl')\n"
" end\n"
" if i then\n"
" return i.source .. ':' .. i.currentline .. ': ' .. err\n"
" else\n"
" return err\n"
" end\n"
"end\n"
"return error_handler";
luaL_loadbuffer(lua_engine_ctx->lua, errh_func, strlen(errh_func), "@err_handler_def");
lua_pcall(lua_engine_ctx->lua,0,1,0);
lua_settable(lua_engine_ctx->lua, LUA_REGISTRYINDEX);
luaRegisterGlobalProtectionFunction(lua_engine_ctx->lua);
lua_pushvalue(lua_engine_ctx->lua, LUA_GLOBALSINDEX);
luaSetGlobalProtection(lua_engine_ctx->lua);
lua_pop(lua_engine_ctx->lua, 1);
luaSaveOnRegistry(lua_engine_ctx->lua, REGISTRY_ENGINE_CTX_NAME, lua_engine_ctx);
engine *lua_engine = zmalloc(sizeof(*lua_engine));
*lua_engine = (engine) {
.engine_ctx = lua_engine_ctx,
.create = luaEngineCreate,
.call = luaEngineCall,
.get_used_memory = luaEngineGetUsedMemoy,
.get_function_memory_overhead = luaEngineFunctionMemoryOverhead,
.get_engine_memory_overhead = luaEngineMemoryOverhead,
.free_function = luaEngineFreeFunction,
};
return functionsRegisterEngine(LUA_ENGINE_NAME, lua_engine);
}
