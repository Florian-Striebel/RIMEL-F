#include <string.h>
#define lgc_c
#define LUA_CORE
#include "lua.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#define GCSTEPSIZE 1024u
#define GCSWEEPMAX 40
#define GCSWEEPCOST 10
#define GCFINALIZECOST 100
#define maskmarks cast_byte(~(bitmask(BLACKBIT)|WHITEBITS))
#define makewhite(g,x) ((x)->gch.marked = cast_byte(((x)->gch.marked & maskmarks) | luaC_white(g)))
#define white2gray(x) reset2bits((x)->gch.marked, WHITE0BIT, WHITE1BIT)
#define black2gray(x) resetbit((x)->gch.marked, BLACKBIT)
#define stringmark(s) reset2bits((s)->tsv.marked, WHITE0BIT, WHITE1BIT)
#define isfinalized(u) testbit((u)->marked, FINALIZEDBIT)
#define markfinalized(u) l_setbit((u)->marked, FINALIZEDBIT)
#define KEYWEAK bitmask(KEYWEAKBIT)
#define VALUEWEAK bitmask(VALUEWEAKBIT)
#define markvalue(g,o) { checkconsistency(o); if (iscollectable(o) && iswhite(gcvalue(o))) reallymarkobject(g,gcvalue(o)); }
#define markobject(g,t) { if (iswhite(obj2gco(t))) reallymarkobject(g, obj2gco(t)); }
#define setthreshold(g) (g->GCthreshold = (g->estimate/100) * g->gcpause)
static void removeentry (Node *n) {
lua_assert(ttisnil(gval(n)));
if (iscollectable(gkey(n)))
setttype(gkey(n), LUA_TDEADKEY);
}
static void reallymarkobject (global_State *g, GCObject *o) {
lua_assert(iswhite(o) && !isdead(g, o));
white2gray(o);
switch (o->gch.tt) {
case LUA_TSTRING: {
return;
}
case LUA_TUSERDATA: {
Table *mt = gco2u(o)->metatable;
gray2black(o);
if (mt) markobject(g, mt);
markobject(g, gco2u(o)->env);
return;
}
case LUA_TUPVAL: {
UpVal *uv = gco2uv(o);
markvalue(g, uv->v);
if (uv->v == &uv->u.value)
gray2black(o);
return;
}
case LUA_TFUNCTION: {
gco2cl(o)->c.gclist = g->gray;
g->gray = o;
break;
}
case LUA_TTABLE: {
gco2h(o)->gclist = g->gray;
g->gray = o;
break;
}
case LUA_TTHREAD: {
gco2th(o)->gclist = g->gray;
g->gray = o;
break;
}
case LUA_TPROTO: {
gco2p(o)->gclist = g->gray;
g->gray = o;
break;
}
default: lua_assert(0);
}
}
static void marktmu (global_State *g) {
GCObject *u = g->tmudata;
if (u) {
do {
u = u->gch.next;
makewhite(g, u);
reallymarkobject(g, u);
} while (u != g->tmudata);
}
}
size_t luaC_separateudata (lua_State *L, int all) {
global_State *g = G(L);
size_t deadmem = 0;
GCObject **p = &g->mainthread->next;
GCObject *curr;
while ((curr = *p) != NULL) {
if (!(iswhite(curr) || all) || isfinalized(gco2u(curr)))
p = &curr->gch.next;
else if (fasttm(L, gco2u(curr)->metatable, TM_GC) == NULL) {
markfinalized(gco2u(curr));
p = &curr->gch.next;
}
else {
deadmem += sizeudata(gco2u(curr));
markfinalized(gco2u(curr));
*p = curr->gch.next;
if (g->tmudata == NULL)
g->tmudata = curr->gch.next = curr;
else {
curr->gch.next = g->tmudata->gch.next;
g->tmudata->gch.next = curr;
g->tmudata = curr;
}
}
}
return deadmem;
}
static int traversetable (global_State *g, Table *h) {
int i;
int weakkey = 0;
int weakvalue = 0;
const TValue *mode;
if (h->metatable)
markobject(g, h->metatable);
mode = gfasttm(g, h->metatable, TM_MODE);
if (mode && ttisstring(mode)) {
weakkey = (strchr(svalue(mode), 'k') != NULL);
weakvalue = (strchr(svalue(mode), 'v') != NULL);
if (weakkey || weakvalue) {
h->marked &= ~(KEYWEAK | VALUEWEAK);
h->marked |= cast_byte((weakkey << KEYWEAKBIT) |
(weakvalue << VALUEWEAKBIT));
h->gclist = g->weak;
g->weak = obj2gco(h);
}
}
if (weakkey && weakvalue) return 1;
if (!weakvalue) {
i = h->sizearray;
while (i--)
markvalue(g, &h->array[i]);
}
i = sizenode(h);
while (i--) {
Node *n = gnode(h, i);
lua_assert(ttype(gkey(n)) != LUA_TDEADKEY || ttisnil(gval(n)));
if (ttisnil(gval(n)))
removeentry(n);
else {
lua_assert(!ttisnil(gkey(n)));
if (!weakkey) markvalue(g, gkey(n));
if (!weakvalue) markvalue(g, gval(n));
}
}
return weakkey || weakvalue;
}
static void traverseproto (global_State *g, Proto *f) {
int i;
if (f->source) stringmark(f->source);
for (i=0; i<f->sizek; i++)
markvalue(g, &f->k[i]);
for (i=0; i<f->sizeupvalues; i++) {
if (f->upvalues[i])
stringmark(f->upvalues[i]);
}
for (i=0; i<f->sizep; i++) {
if (f->p[i])
markobject(g, f->p[i]);
}
for (i=0; i<f->sizelocvars; i++) {
if (f->locvars[i].varname)
stringmark(f->locvars[i].varname);
}
}
static void traverseclosure (global_State *g, Closure *cl) {
markobject(g, cl->c.env);
if (cl->c.isC) {
int i;
for (i=0; i<cl->c.nupvalues; i++)
markvalue(g, &cl->c.upvalue[i]);
}
else {
int i;
lua_assert(cl->l.nupvalues == cl->l.p->nups);
markobject(g, cl->l.p);
for (i=0; i<cl->l.nupvalues; i++)
markobject(g, cl->l.upvals[i]);
}
}
static void checkstacksizes (lua_State *L, StkId max) {
int ci_used = cast_int(L->ci - L->base_ci);
int s_used = cast_int(max - L->stack);
if (L->size_ci > LUAI_MAXCALLS)
return;
if (4*ci_used < L->size_ci && 2*BASIC_CI_SIZE < L->size_ci)
luaD_reallocCI(L, L->size_ci/2);
condhardstacktests(luaD_reallocCI(L, ci_used + 1));
if (4*s_used < L->stacksize &&
2*(BASIC_STACK_SIZE+EXTRA_STACK) < L->stacksize)
luaD_reallocstack(L, L->stacksize/2);
condhardstacktests(luaD_reallocstack(L, s_used));
}
static void traversestack (global_State *g, lua_State *l) {
StkId o, lim;
CallInfo *ci;
markvalue(g, gt(l));
lim = l->top;
for (ci = l->base_ci; ci <= l->ci; ci++) {
lua_assert(ci->top <= l->stack_last);
if (lim < ci->top) lim = ci->top;
}
for (o = l->stack; o < l->top; o++)
markvalue(g, o);
for (; o <= lim; o++)
setnilvalue(o);
checkstacksizes(l, lim);
}
static l_mem propagatemark (global_State *g) {
GCObject *o = g->gray;
lua_assert(isgray(o));
gray2black(o);
switch (o->gch.tt) {
case LUA_TTABLE: {
Table *h = gco2h(o);
g->gray = h->gclist;
if (traversetable(g, h))
black2gray(o);
return sizeof(Table) + sizeof(TValue) * h->sizearray +
sizeof(Node) * sizenode(h);
}
case LUA_TFUNCTION: {
Closure *cl = gco2cl(o);
g->gray = cl->c.gclist;
traverseclosure(g, cl);
return (cl->c.isC) ? sizeCclosure(cl->c.nupvalues) :
sizeLclosure(cl->l.nupvalues);
}
case LUA_TTHREAD: {
lua_State *th = gco2th(o);
g->gray = th->gclist;
th->gclist = g->grayagain;
g->grayagain = o;
black2gray(o);
traversestack(g, th);
return sizeof(lua_State) + sizeof(TValue) * th->stacksize +
sizeof(CallInfo) * th->size_ci;
}
case LUA_TPROTO: {
Proto *p = gco2p(o);
g->gray = p->gclist;
traverseproto(g, p);
return sizeof(Proto) + sizeof(Instruction) * p->sizecode +
sizeof(Proto *) * p->sizep +
sizeof(TValue) * p->sizek +
sizeof(int) * p->sizelineinfo +
sizeof(LocVar) * p->sizelocvars +
sizeof(TString *) * p->sizeupvalues;
}
default: lua_assert(0); return 0;
}
}
static size_t propagateall (global_State *g) {
size_t m = 0;
while (g->gray) m += propagatemark(g);
return m;
}
static int iscleared (const TValue *o, int iskey) {
if (!iscollectable(o)) return 0;
if (ttisstring(o)) {
stringmark(rawtsvalue(o));
return 0;
}
return iswhite(gcvalue(o)) ||
(ttisuserdata(o) && (!iskey && isfinalized(uvalue(o))));
}
static void cleartable (GCObject *l) {
while (l) {
Table *h = gco2h(l);
int i = h->sizearray;
lua_assert(testbit(h->marked, VALUEWEAKBIT) ||
testbit(h->marked, KEYWEAKBIT));
if (testbit(h->marked, VALUEWEAKBIT)) {
while (i--) {
TValue *o = &h->array[i];
if (iscleared(o, 0))
setnilvalue(o);
}
}
i = sizenode(h);
while (i--) {
Node *n = gnode(h, i);
if (!ttisnil(gval(n)) &&
(iscleared(key2tval(n), 1) || iscleared(gval(n), 0))) {
setnilvalue(gval(n));
removeentry(n);
}
}
l = h->gclist;
}
}
static void freeobj (lua_State *L, GCObject *o) {
switch (o->gch.tt) {
case LUA_TPROTO: luaF_freeproto(L, gco2p(o)); break;
case LUA_TFUNCTION: luaF_freeclosure(L, gco2cl(o)); break;
case LUA_TUPVAL: luaF_freeupval(L, gco2uv(o)); break;
case LUA_TTABLE: luaH_free(L, gco2h(o)); break;
case LUA_TTHREAD: {
lua_assert(gco2th(o) != L && gco2th(o) != G(L)->mainthread);
luaE_freethread(L, gco2th(o));
break;
}
case LUA_TSTRING: {
G(L)->strt.nuse--;
luaM_freemem(L, o, sizestring(gco2ts(o)));
break;
}
case LUA_TUSERDATA: {
luaM_freemem(L, o, sizeudata(gco2u(o)));
break;
}
default: lua_assert(0);
}
}
#define sweepwholelist(L,p) sweeplist(L,p,MAX_LUMEM)
static GCObject **sweeplist (lua_State *L, GCObject **p, lu_mem count) {
GCObject *curr;
global_State *g = G(L);
int deadmask = otherwhite(g);
while ((curr = *p) != NULL && count-- > 0) {
if (curr->gch.tt == LUA_TTHREAD)
sweepwholelist(L, &gco2th(curr)->openupval);
if ((curr->gch.marked ^ WHITEBITS) & deadmask) {
lua_assert(!isdead(g, curr) || testbit(curr->gch.marked, FIXEDBIT));
makewhite(g, curr);
p = &curr->gch.next;
}
else {
lua_assert(isdead(g, curr) || deadmask == bitmask(SFIXEDBIT));
*p = curr->gch.next;
if (curr == g->rootgc)
g->rootgc = curr->gch.next;
freeobj(L, curr);
}
}
return p;
}
static void checkSizes (lua_State *L) {
global_State *g = G(L);
if (g->strt.nuse < cast(lu_int32, g->strt.size/4) &&
g->strt.size > MINSTRTABSIZE*2)
luaS_resize(L, g->strt.size/2);
if (luaZ_sizebuffer(&g->buff) > LUA_MINBUFFER*2) {
size_t newsize = luaZ_sizebuffer(&g->buff) / 2;
luaZ_resizebuffer(L, &g->buff, newsize);
}
}
static void GCTM (lua_State *L) {
global_State *g = G(L);
GCObject *o = g->tmudata->gch.next;
Udata *udata = rawgco2u(o);
const TValue *tm;
if (o == g->tmudata)
g->tmudata = NULL;
else
g->tmudata->gch.next = udata->uv.next;
udata->uv.next = g->mainthread->next;
g->mainthread->next = o;
makewhite(g, o);
tm = fasttm(L, udata->uv.metatable, TM_GC);
if (tm != NULL) {
lu_byte oldah = L->allowhook;
lu_mem oldt = g->GCthreshold;
L->allowhook = 0;
g->GCthreshold = 2*g->totalbytes;
setobj2s(L, L->top, tm);
setuvalue(L, L->top+1, udata);
L->top += 2;
luaD_call(L, L->top - 2, 0);
L->allowhook = oldah;
g->GCthreshold = oldt;
}
}
void luaC_callGCTM (lua_State *L) {
while (G(L)->tmudata)
GCTM(L);
}
void luaC_freeall (lua_State *L) {
global_State *g = G(L);
int i;
g->currentwhite = WHITEBITS | bitmask(SFIXEDBIT);
sweepwholelist(L, &g->rootgc);
for (i = 0; i < g->strt.size; i++)
sweepwholelist(L, &g->strt.hash[i]);
}
static void markmt (global_State *g) {
int i;
for (i=0; i<NUM_TAGS; i++)
if (g->mt[i]) markobject(g, g->mt[i]);
}
static void markroot (lua_State *L) {
global_State *g = G(L);
g->gray = NULL;
g->grayagain = NULL;
g->weak = NULL;
markobject(g, g->mainthread);
markvalue(g, gt(g->mainthread));
markvalue(g, registry(L));
markmt(g);
g->gcstate = GCSpropagate;
}
static void remarkupvals (global_State *g) {
UpVal *uv;
for (uv = g->uvhead.u.l.next; uv != &g->uvhead; uv = uv->u.l.next) {
lua_assert(uv->u.l.next->u.l.prev == uv && uv->u.l.prev->u.l.next == uv);
if (isgray(obj2gco(uv)))
markvalue(g, uv->v);
}
}
static void atomic (lua_State *L) {
global_State *g = G(L);
size_t udsize;
remarkupvals(g);
propagateall(g);
g->gray = g->weak;
g->weak = NULL;
lua_assert(!iswhite(obj2gco(g->mainthread)));
markobject(g, L);
markmt(g);
propagateall(g);
g->gray = g->grayagain;
g->grayagain = NULL;
propagateall(g);
udsize = luaC_separateudata(L, 0);
marktmu(g);
udsize += propagateall(g);
cleartable(g->weak);
g->currentwhite = cast_byte(otherwhite(g));
g->sweepstrgc = 0;
g->sweepgc = &g->rootgc;
g->gcstate = GCSsweepstring;
g->estimate = g->totalbytes - udsize;
}
static l_mem singlestep (lua_State *L) {
global_State *g = G(L);
switch (g->gcstate) {
case GCSpause: {
markroot(L);
return 0;
}
case GCSpropagate: {
if (g->gray)
return propagatemark(g);
else {
atomic(L);
return 0;
}
}
case GCSsweepstring: {
lu_mem old = g->totalbytes;
sweepwholelist(L, &g->strt.hash[g->sweepstrgc++]);
if (g->sweepstrgc >= g->strt.size)
g->gcstate = GCSsweep;
lua_assert(old >= g->totalbytes);
g->estimate -= old - g->totalbytes;
return GCSWEEPCOST;
}
case GCSsweep: {
lu_mem old = g->totalbytes;
g->sweepgc = sweeplist(L, g->sweepgc, GCSWEEPMAX);
if (*g->sweepgc == NULL) {
checkSizes(L);
g->gcstate = GCSfinalize;
}
lua_assert(old >= g->totalbytes);
g->estimate -= old - g->totalbytes;
return GCSWEEPMAX*GCSWEEPCOST;
}
case GCSfinalize: {
if (g->tmudata) {
GCTM(L);
if (g->estimate > GCFINALIZECOST)
g->estimate -= GCFINALIZECOST;
return GCFINALIZECOST;
}
else {
g->gcstate = GCSpause;
g->gcdept = 0;
return 0;
}
}
default: lua_assert(0); return 0;
}
}
void luaC_step (lua_State *L) {
global_State *g = G(L);
l_mem lim = (GCSTEPSIZE/100) * g->gcstepmul;
if (lim == 0)
lim = (MAX_LUMEM-1)/2;
g->gcdept += g->totalbytes - g->GCthreshold;
do {
lim -= singlestep(L);
if (g->gcstate == GCSpause)
break;
} while (lim > 0);
if (g->gcstate != GCSpause) {
if (g->gcdept < GCSTEPSIZE)
g->GCthreshold = g->totalbytes + GCSTEPSIZE;
else {
g->gcdept -= GCSTEPSIZE;
g->GCthreshold = g->totalbytes;
}
}
else {
setthreshold(g);
}
}
void luaC_fullgc (lua_State *L) {
global_State *g = G(L);
if (g->gcstate <= GCSpropagate) {
g->sweepstrgc = 0;
g->sweepgc = &g->rootgc;
g->gray = NULL;
g->grayagain = NULL;
g->weak = NULL;
g->gcstate = GCSsweepstring;
}
lua_assert(g->gcstate != GCSpause && g->gcstate != GCSpropagate);
while (g->gcstate != GCSfinalize) {
lua_assert(g->gcstate == GCSsweepstring || g->gcstate == GCSsweep);
singlestep(L);
}
markroot(L);
while (g->gcstate != GCSpause) {
singlestep(L);
}
setthreshold(g);
}
void luaC_barrierf (lua_State *L, GCObject *o, GCObject *v) {
global_State *g = G(L);
lua_assert(isblack(o) && iswhite(v) && !isdead(g, v) && !isdead(g, o));
lua_assert(g->gcstate != GCSfinalize && g->gcstate != GCSpause);
lua_assert(ttype(&o->gch) != LUA_TTABLE);
if (g->gcstate == GCSpropagate)
reallymarkobject(g, v);
else
makewhite(g, o);
}
void luaC_barrierback (lua_State *L, Table *t) {
global_State *g = G(L);
GCObject *o = obj2gco(t);
lua_assert(isblack(o) && !isdead(g, o));
lua_assert(g->gcstate != GCSfinalize && g->gcstate != GCSpause);
black2gray(o);
t->gclist = g->grayagain;
g->grayagain = o;
}
void luaC_link (lua_State *L, GCObject *o, lu_byte tt) {
global_State *g = G(L);
o->gch.next = g->rootgc;
g->rootgc = o;
o->gch.marked = luaC_white(g);
o->gch.tt = tt;
}
void luaC_linkupval (lua_State *L, UpVal *uv) {
global_State *g = G(L);
GCObject *o = obj2gco(uv);
o->gch.next = g->rootgc;
g->rootgc = o;
if (isgray(o)) {
if (g->gcstate == GCSpropagate) {
gray2black(o);
luaC_barrier(L, uv, uv->v);
}
else {
makewhite(g, o);
lua_assert(g->gcstate != GCSfinalize && g->gcstate != GCSpause);
}
}
}
