





















#if !defined(LUA_UCL_H_)
#define LUA_UCL_H_

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "ucl.h"




struct ucl_lua_funcdata {
lua_State *L;
int idx;
char *ret;
};




UCL_EXTERN int luaopen_ucl (lua_State *L);







UCL_EXTERN ucl_object_t* ucl_object_lua_import (lua_State *L, int idx);







UCL_EXTERN ucl_object_t* ucl_object_lua_import_escape (lua_State *L, int idx);







UCL_EXTERN int ucl_object_push_lua (lua_State *L,
const ucl_object_t *obj, bool allow_array);






UCL_EXTERN int ucl_object_push_lua_filter_nil (lua_State *L,
const ucl_object_t *obj,
bool allow_array);

UCL_EXTERN struct ucl_lua_funcdata* ucl_object_toclosure (const ucl_object_t *obj);

#endif
