





#if !defined(lundump_h)
#define lundump_h

#include "lobject.h"
#include "lzio.h"


LUAI_FUNC Proto* luaU_undump (lua_State* L, ZIO* Z, Mbuffer* buff, const char* name);


LUAI_FUNC void luaU_header (char* h);


LUAI_FUNC int luaU_dump (lua_State* L, const Proto* f, lua_Writer w, void* data, int strip);

#if defined(luac_c)

LUAI_FUNC void luaU_print (const Proto* f, int full);
#endif


#define LUAC_VERSION 0x51


#define LUAC_FORMAT 0


#define LUAC_HEADERSIZE 12

#endif
