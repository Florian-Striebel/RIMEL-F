





#if !defined(ldo_h)
#define ldo_h


#include "lobject.h"
#include "lstate.h"
#include "lzio.h"


#define luaD_checkstack(L,n) if ((char *)L->stack_last - (char *)L->top <= (n)*(int)sizeof(TValue)) luaD_growstack(L, n); else condhardstacktests(luaD_reallocstack(L, L->stacksize - EXTRA_STACK - 1));





#define incr_top(L) {luaD_checkstack(L,1); L->top++;}

#define savestack(L,p) ((char *)(p) - (char *)L->stack)
#define restorestack(L,n) ((TValue *)((char *)L->stack + (n)))

#define saveci(L,p) ((char *)(p) - (char *)L->base_ci)
#define restoreci(L,n) ((CallInfo *)((char *)L->base_ci + (n)))



#define PCRLUA 0
#define PCRC 1
#define PCRYIELD 2



typedef void (*Pfunc) (lua_State *L, void *ud);

LUAI_FUNC int luaD_protectedparser (lua_State *L, ZIO *z, const char *name);
LUAI_FUNC void luaD_callhook (lua_State *L, int event, int line);
LUAI_FUNC int luaD_precall (lua_State *L, StkId func, int nresults);
LUAI_FUNC void luaD_call (lua_State *L, StkId func, int nResults);
LUAI_FUNC int luaD_pcall (lua_State *L, Pfunc func, void *u,
ptrdiff_t oldtop, ptrdiff_t ef);
LUAI_FUNC int luaD_poscall (lua_State *L, StkId firstResult);
LUAI_FUNC void luaD_reallocCI (lua_State *L, int newsize);
LUAI_FUNC void luaD_reallocstack (lua_State *L, int newsize);
LUAI_FUNC void luaD_growstack (lua_State *L, int n);

LUAI_FUNC void luaD_throw (lua_State *L, int errcode);
LUAI_FUNC int luaD_rawrunprotected (lua_State *L, Pfunc f, void *ud);

LUAI_FUNC void luaD_seterrorobj (lua_State *L, int errcode, StkId oldtop);

#endif

