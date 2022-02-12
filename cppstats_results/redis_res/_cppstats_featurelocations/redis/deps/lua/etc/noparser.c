













#define LUA_CORE

#include "llex.h"
#include "lparser.h"
#include "lzio.h"

LUAI_FUNC void luaX_init (lua_State *L) {
UNUSED(L);
}

LUAI_FUNC Proto *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff, const char *name) {
UNUSED(z);
UNUSED(buff);
UNUSED(name);
lua_pushliteral(L,"parser not loaded");
lua_error(L);
return NULL;
}

#if defined(NODUMP)
#include "lundump.h"

LUAI_FUNC int luaU_dump (lua_State* L, const Proto* f, lua_Writer w, void* data, int strip) {
UNUSED(f);
UNUSED(w);
UNUSED(data);
UNUSED(strip);
#if 1
UNUSED(L);
return 0;
#else
lua_pushliteral(L,"dumper not loaded");
lua_error(L);
#endif
}
#endif
