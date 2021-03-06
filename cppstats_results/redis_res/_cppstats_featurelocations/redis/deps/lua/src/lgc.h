





#if !defined(lgc_h)
#define lgc_h


#include "lobject.h"





#define GCSpause 0
#define GCSpropagate 1
#define GCSsweepstring 2
#define GCSsweep 3
#define GCSfinalize 4





#define resetbits(x,m) ((x) &= cast(lu_byte, ~(m)))
#define setbits(x,m) ((x) |= (m))
#define testbits(x,m) ((x) & (m))
#define bitmask(b) (1<<(b))
#define bit2mask(b1,b2) (bitmask(b1) | bitmask(b2))
#define l_setbit(x,b) setbits(x, bitmask(b))
#define resetbit(x,b) resetbits(x, bitmask(b))
#define testbit(x,b) testbits(x, bitmask(b))
#define set2bits(x,b1,b2) setbits(x, (bit2mask(b1, b2)))
#define reset2bits(x,b1,b2) resetbits(x, (bit2mask(b1, b2)))
#define test2bits(x,b1,b2) testbits(x, (bit2mask(b1, b2)))
















#define WHITE0BIT 0
#define WHITE1BIT 1
#define BLACKBIT 2
#define FINALIZEDBIT 3
#define KEYWEAKBIT 3
#define VALUEWEAKBIT 4
#define FIXEDBIT 5
#define SFIXEDBIT 6
#define WHITEBITS bit2mask(WHITE0BIT, WHITE1BIT)


#define iswhite(x) test2bits((x)->gch.marked, WHITE0BIT, WHITE1BIT)
#define isblack(x) testbit((x)->gch.marked, BLACKBIT)
#define isgray(x) (!isblack(x) && !iswhite(x))

#define otherwhite(g) (g->currentwhite ^ WHITEBITS)
#define isdead(g,v) ((v)->gch.marked & otherwhite(g) & WHITEBITS)

#define changewhite(x) ((x)->gch.marked ^= WHITEBITS)
#define gray2black(x) l_setbit((x)->gch.marked, BLACKBIT)

#define valiswhite(x) (iscollectable(x) && iswhite(gcvalue(x)))

#define luaC_white(g) cast(lu_byte, (g)->currentwhite & WHITEBITS)


#define luaC_checkGC(L) { condhardstacktests(luaD_reallocstack(L, L->stacksize - EXTRA_STACK - 1)); if (G(L)->totalbytes >= G(L)->GCthreshold) luaC_step(L); }





#define luaC_barrier(L,p,v) { if (valiswhite(v) && isblack(obj2gco(p))) luaC_barrierf(L,obj2gco(p),gcvalue(v)); }


#define luaC_barriert(L,t,v) { if (valiswhite(v) && isblack(obj2gco(t))) luaC_barrierback(L,t); }


#define luaC_objbarrier(L,p,o) { if (iswhite(obj2gco(o)) && isblack(obj2gco(p))) luaC_barrierf(L,obj2gco(p),obj2gco(o)); }



#define luaC_objbarriert(L,t,o) { if (iswhite(obj2gco(o)) && isblack(obj2gco(t))) luaC_barrierback(L,t); }


LUAI_FUNC size_t luaC_separateudata (lua_State *L, int all);
LUAI_FUNC void luaC_callGCTM (lua_State *L);
LUAI_FUNC void luaC_freeall (lua_State *L);
LUAI_FUNC void luaC_step (lua_State *L);
LUAI_FUNC void luaC_fullgc (lua_State *L);
LUAI_FUNC void luaC_link (lua_State *L, GCObject *o, lu_byte tt);
LUAI_FUNC void luaC_linkupval (lua_State *L, UpVal *uv);
LUAI_FUNC void luaC_barrierf (lua_State *L, GCObject *o, GCObject *v);
LUAI_FUNC void luaC_barrierback (lua_State *L, Table *t);


#endif
