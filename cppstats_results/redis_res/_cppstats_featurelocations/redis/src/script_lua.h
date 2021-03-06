




























#if !defined(__SCRIPT_LUA_H_)
#define __SCRIPT_LUA_H_



















#include "server.h"
#include "script.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define REGISTRY_RUN_CTX_NAME "__RUN_CTX__"
#define REGISTRY_SET_GLOBALS_PROTECTION_NAME "__GLOBAL_PROTECTION__"
#define REDIS_API_NAME "redis"

void luaRegisterRedisAPI(lua_State* lua);
sds luaGetStringSds(lua_State *lua, int index);
void luaEnableGlobalsProtection(lua_State *lua, int is_eval);
void luaRegisterGlobalProtectionFunction(lua_State *lua);
void luaSetGlobalProtection(lua_State *lua);
void luaRegisterLogFunction(lua_State* lua);
void luaRegisterVersion(lua_State* lua);
void luaPushError(lua_State *lua, char *error);
int luaRaiseError(lua_State *lua);
void luaSaveOnRegistry(lua_State* lua, const char* name, void* ptr);
void* luaGetFromRegistry(lua_State* lua, const char* name);
void luaCallFunction(scriptRunCtx* r_ctx, lua_State *lua, robj** keys, size_t nkeys, robj** args, size_t nargs, int debug_enabled);
unsigned long luaMemory(lua_State *lua);


#endif
