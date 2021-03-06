#include <string.h>
#define lparser_c
#define LUA_CORE
#include "lua.h"
#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#define hasmultret(k) ((k) == VCALL || (k) == VVARARG)
#define getlocvar(fs, i) ((fs)->f->locvars[(fs)->actvar[i]])
#define luaY_checklimit(fs,v,l,m) if ((v)>(l)) errorlimit(fs,l,m)
typedef struct BlockCnt {
struct BlockCnt *previous;
int breaklist;
lu_byte nactvar;
lu_byte upval;
lu_byte isbreakable;
} BlockCnt;
static void chunk (LexState *ls);
static void expr (LexState *ls, expdesc *v);
static void anchor_token (LexState *ls) {
if (ls->t.token == TK_NAME || ls->t.token == TK_STRING) {
TString *ts = ls->t.seminfo.ts;
luaX_newstring(ls, getstr(ts), ts->tsv.len);
}
}
static void error_expected (LexState *ls, int token) {
luaX_syntaxerror(ls,
luaO_pushfstring(ls->L, LUA_QS " expected", luaX_token2str(ls, token)));
}
static void errorlimit (FuncState *fs, int limit, const char *what) {
const char *msg = (fs->f->linedefined == 0) ?
luaO_pushfstring(fs->L, "main function has more than %d %s", limit, what) :
luaO_pushfstring(fs->L, "function at line %d has more than %d %s",
fs->f->linedefined, limit, what);
luaX_lexerror(fs->ls, msg, 0);
}
static int testnext (LexState *ls, int c) {
if (ls->t.token == c) {
luaX_next(ls);
return 1;
}
else return 0;
}
static void check (LexState *ls, int c) {
if (ls->t.token != c)
error_expected(ls, c);
}
static void checknext (LexState *ls, int c) {
check(ls, c);
luaX_next(ls);
}
#define check_condition(ls,c,msg) { if (!(c)) luaX_syntaxerror(ls, msg); }
static void check_match (LexState *ls, int what, int who, int where) {
if (!testnext(ls, what)) {
if (where == ls->linenumber)
error_expected(ls, what);
else {
luaX_syntaxerror(ls, luaO_pushfstring(ls->L,
LUA_QS " expected (to close " LUA_QS " at line %d)",
luaX_token2str(ls, what), luaX_token2str(ls, who), where));
}
}
}
static TString *str_checkname (LexState *ls) {
TString *ts;
check(ls, TK_NAME);
ts = ls->t.seminfo.ts;
luaX_next(ls);
return ts;
}
static void init_exp (expdesc *e, expkind k, int i) {
e->f = e->t = NO_JUMP;
e->k = k;
e->u.s.info = i;
}
static void codestring (LexState *ls, expdesc *e, TString *s) {
init_exp(e, VK, luaK_stringK(ls->fs, s));
}
static void checkname(LexState *ls, expdesc *e) {
codestring(ls, e, str_checkname(ls));
}
static int registerlocalvar (LexState *ls, TString *varname) {
FuncState *fs = ls->fs;
Proto *f = fs->f;
int oldsize = f->sizelocvars;
luaM_growvector(ls->L, f->locvars, fs->nlocvars, f->sizelocvars,
LocVar, SHRT_MAX, "too many local variables");
while (oldsize < f->sizelocvars) f->locvars[oldsize++].varname = NULL;
f->locvars[fs->nlocvars].varname = varname;
luaC_objbarrier(ls->L, f, varname);
return fs->nlocvars++;
}
#define new_localvarliteral(ls,v,n) new_localvar(ls, luaX_newstring(ls, "" v, (sizeof(v)/sizeof(char))-1), n)
static void new_localvar (LexState *ls, TString *name, int n) {
FuncState *fs = ls->fs;
luaY_checklimit(fs, fs->nactvar+n+1, LUAI_MAXVARS, "local variables");
fs->actvar[fs->nactvar+n] = cast(unsigned short, registerlocalvar(ls, name));
}
static void adjustlocalvars (LexState *ls, int nvars) {
FuncState *fs = ls->fs;
fs->nactvar = cast_byte(fs->nactvar + nvars);
for (; nvars; nvars--) {
getlocvar(fs, fs->nactvar - nvars).startpc = fs->pc;
}
}
static void removevars (LexState *ls, int tolevel) {
FuncState *fs = ls->fs;
while (fs->nactvar > tolevel)
getlocvar(fs, --fs->nactvar).endpc = fs->pc;
}
static int indexupvalue (FuncState *fs, TString *name, expdesc *v) {
int i;
Proto *f = fs->f;
int oldsize = f->sizeupvalues;
for (i=0; i<f->nups; i++) {
if (fs->upvalues[i].k == v->k && fs->upvalues[i].info == v->u.s.info) {
lua_assert(f->upvalues[i] == name);
return i;
}
}
luaY_checklimit(fs, f->nups + 1, LUAI_MAXUPVALUES, "upvalues");
luaM_growvector(fs->L, f->upvalues, f->nups, f->sizeupvalues,
TString *, MAX_INT, "");
while (oldsize < f->sizeupvalues) f->upvalues[oldsize++] = NULL;
f->upvalues[f->nups] = name;
luaC_objbarrier(fs->L, f, name);
lua_assert(v->k == VLOCAL || v->k == VUPVAL);
fs->upvalues[f->nups].k = cast_byte(v->k);
fs->upvalues[f->nups].info = cast_byte(v->u.s.info);
return f->nups++;
}
static int searchvar (FuncState *fs, TString *n) {
int i;
for (i=fs->nactvar-1; i >= 0; i--) {
if (n == getlocvar(fs, i).varname)
return i;
}
return -1;
}
static void markupval (FuncState *fs, int level) {
BlockCnt *bl = fs->bl;
while (bl && bl->nactvar > level) bl = bl->previous;
if (bl) bl->upval = 1;
}
static int singlevaraux (FuncState *fs, TString *n, expdesc *var, int base) {
if (fs == NULL) {
init_exp(var, VGLOBAL, NO_REG);
return VGLOBAL;
}
else {
int v = searchvar(fs, n);
if (v >= 0) {
init_exp(var, VLOCAL, v);
if (!base)
markupval(fs, v);
return VLOCAL;
}
else {
if (singlevaraux(fs->prev, n, var, 0) == VGLOBAL)
return VGLOBAL;
var->u.s.info = indexupvalue(fs, n, var);
var->k = VUPVAL;
return VUPVAL;
}
}
}
static void singlevar (LexState *ls, expdesc *var) {
TString *varname = str_checkname(ls);
FuncState *fs = ls->fs;
if (singlevaraux(fs, varname, var, 1) == VGLOBAL)
var->u.s.info = luaK_stringK(fs, varname);
}
static void adjust_assign (LexState *ls, int nvars, int nexps, expdesc *e) {
FuncState *fs = ls->fs;
int extra = nvars - nexps;
if (hasmultret(e->k)) {
extra++;
if (extra < 0) extra = 0;
luaK_setreturns(fs, e, extra);
if (extra > 1) luaK_reserveregs(fs, extra-1);
}
else {
if (e->k != VVOID) luaK_exp2nextreg(fs, e);
if (extra > 0) {
int reg = fs->freereg;
luaK_reserveregs(fs, extra);
luaK_nil(fs, reg, extra);
}
}
}
static void enterlevel (LexState *ls) {
if (++ls->L->nCcalls > LUAI_MAXCCALLS)
luaX_lexerror(ls, "chunk has too many syntax levels", 0);
}
#define leavelevel(ls) ((ls)->L->nCcalls--)
static void enterblock (FuncState *fs, BlockCnt *bl, lu_byte isbreakable) {
bl->breaklist = NO_JUMP;
bl->isbreakable = isbreakable;
bl->nactvar = fs->nactvar;
bl->upval = 0;
bl->previous = fs->bl;
fs->bl = bl;
lua_assert(fs->freereg == fs->nactvar);
}
static void leaveblock (FuncState *fs) {
BlockCnt *bl = fs->bl;
fs->bl = bl->previous;
removevars(fs->ls, bl->nactvar);
if (bl->upval)
luaK_codeABC(fs, OP_CLOSE, bl->nactvar, 0, 0);
lua_assert(!bl->isbreakable || !bl->upval);
lua_assert(bl->nactvar == fs->nactvar);
fs->freereg = fs->nactvar;
luaK_patchtohere(fs, bl->breaklist);
}
static void pushclosure (LexState *ls, FuncState *func, expdesc *v) {
FuncState *fs = ls->fs;
Proto *f = fs->f;
int oldsize = f->sizep;
int i;
luaM_growvector(ls->L, f->p, fs->np, f->sizep, Proto *,
MAXARG_Bx, "constant table overflow");
while (oldsize < f->sizep) f->p[oldsize++] = NULL;
f->p[fs->np++] = func->f;
luaC_objbarrier(ls->L, f, func->f);
init_exp(v, VRELOCABLE, luaK_codeABx(fs, OP_CLOSURE, 0, fs->np-1));
for (i=0; i<func->f->nups; i++) {
OpCode o = (func->upvalues[i].k == VLOCAL) ? OP_MOVE : OP_GETUPVAL;
luaK_codeABC(fs, o, 0, func->upvalues[i].info, 0);
}
}
static void open_func (LexState *ls, FuncState *fs) {
lua_State *L = ls->L;
Proto *f = luaF_newproto(L);
fs->f = f;
fs->prev = ls->fs;
fs->ls = ls;
fs->L = L;
ls->fs = fs;
fs->pc = 0;
fs->lasttarget = -1;
fs->jpc = NO_JUMP;
fs->freereg = 0;
fs->nk = 0;
fs->np = 0;
fs->nlocvars = 0;
fs->nactvar = 0;
fs->bl = NULL;
f->source = ls->source;
f->maxstacksize = 2;
fs->h = luaH_new(L, 0, 0);
sethvalue2s(L, L->top, fs->h);
incr_top(L);
setptvalue2s(L, L->top, f);
incr_top(L);
}
static void close_func (LexState *ls) {
lua_State *L = ls->L;
FuncState *fs = ls->fs;
Proto *f = fs->f;
removevars(ls, 0);
luaK_ret(fs, 0, 0);
luaM_reallocvector(L, f->code, f->sizecode, fs->pc, Instruction);
f->sizecode = fs->pc;
luaM_reallocvector(L, f->lineinfo, f->sizelineinfo, fs->pc, int);
f->sizelineinfo = fs->pc;
luaM_reallocvector(L, f->k, f->sizek, fs->nk, TValue);
f->sizek = fs->nk;
luaM_reallocvector(L, f->p, f->sizep, fs->np, Proto *);
f->sizep = fs->np;
luaM_reallocvector(L, f->locvars, f->sizelocvars, fs->nlocvars, LocVar);
f->sizelocvars = fs->nlocvars;
luaM_reallocvector(L, f->upvalues, f->sizeupvalues, f->nups, TString *);
f->sizeupvalues = f->nups;
lua_assert(luaG_checkcode(f));
lua_assert(fs->bl == NULL);
ls->fs = fs->prev;
if (fs) anchor_token(ls);
L->top -= 2;
}
Proto *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff, const char *name) {
struct LexState lexstate;
struct FuncState funcstate;
lexstate.buff = buff;
luaX_setinput(L, &lexstate, z, luaS_new(L, name));
open_func(&lexstate, &funcstate);
funcstate.f->is_vararg = VARARG_ISVARARG;
luaX_next(&lexstate);
chunk(&lexstate);
check(&lexstate, TK_EOS);
close_func(&lexstate);
lua_assert(funcstate.prev == NULL);
lua_assert(funcstate.f->nups == 0);
lua_assert(lexstate.fs == NULL);
return funcstate.f;
}
static void field (LexState *ls, expdesc *v) {
FuncState *fs = ls->fs;
expdesc key;
luaK_exp2anyreg(fs, v);
luaX_next(ls);
checkname(ls, &key);
luaK_indexed(fs, v, &key);
}
static void yindex (LexState *ls, expdesc *v) {
luaX_next(ls);
expr(ls, v);
luaK_exp2val(ls->fs, v);
checknext(ls, ']');
}
struct ConsControl {
expdesc v;
expdesc *t;
int nh;
int na;
int tostore;
};
static void recfield (LexState *ls, struct ConsControl *cc) {
FuncState *fs = ls->fs;
int reg = ls->fs->freereg;
expdesc key, val;
int rkkey;
if (ls->t.token == TK_NAME) {
luaY_checklimit(fs, cc->nh, MAX_INT, "items in a constructor");
checkname(ls, &key);
}
else
yindex(ls, &key);
cc->nh++;
checknext(ls, '=');
rkkey = luaK_exp2RK(fs, &key);
expr(ls, &val);
luaK_codeABC(fs, OP_SETTABLE, cc->t->u.s.info, rkkey, luaK_exp2RK(fs, &val));
fs->freereg = reg;
}
static void closelistfield (FuncState *fs, struct ConsControl *cc) {
if (cc->v.k == VVOID) return;
luaK_exp2nextreg(fs, &cc->v);
cc->v.k = VVOID;
if (cc->tostore == LFIELDS_PER_FLUSH) {
luaK_setlist(fs, cc->t->u.s.info, cc->na, cc->tostore);
cc->tostore = 0;
}
}
static void lastlistfield (FuncState *fs, struct ConsControl *cc) {
if (cc->tostore == 0) return;
if (hasmultret(cc->v.k)) {
luaK_setmultret(fs, &cc->v);
luaK_setlist(fs, cc->t->u.s.info, cc->na, LUA_MULTRET);
cc->na--;
}
else {
if (cc->v.k != VVOID)
luaK_exp2nextreg(fs, &cc->v);
luaK_setlist(fs, cc->t->u.s.info, cc->na, cc->tostore);
}
}
static void listfield (LexState *ls, struct ConsControl *cc) {
expr(ls, &cc->v);
luaY_checklimit(ls->fs, cc->na, MAX_INT, "items in a constructor");
cc->na++;
cc->tostore++;
}
static void constructor (LexState *ls, expdesc *t) {
FuncState *fs = ls->fs;
int line = ls->linenumber;
int pc = luaK_codeABC(fs, OP_NEWTABLE, 0, 0, 0);
struct ConsControl cc;
cc.na = cc.nh = cc.tostore = 0;
cc.t = t;
init_exp(t, VRELOCABLE, pc);
init_exp(&cc.v, VVOID, 0);
luaK_exp2nextreg(ls->fs, t);
checknext(ls, '{');
do {
lua_assert(cc.v.k == VVOID || cc.tostore > 0);
if (ls->t.token == '}') break;
closelistfield(fs, &cc);
switch(ls->t.token) {
case TK_NAME: {
luaX_lookahead(ls);
if (ls->lookahead.token != '=')
listfield(ls, &cc);
else
recfield(ls, &cc);
break;
}
case '[': {
recfield(ls, &cc);
break;
}
default: {
listfield(ls, &cc);
break;
}
}
} while (testnext(ls, ',') || testnext(ls, ';'));
check_match(ls, '}', '{', line);
lastlistfield(fs, &cc);
SETARG_B(fs->f->code[pc], luaO_int2fb(cc.na));
SETARG_C(fs->f->code[pc], luaO_int2fb(cc.nh));
}
static void parlist (LexState *ls) {
FuncState *fs = ls->fs;
Proto *f = fs->f;
int nparams = 0;
f->is_vararg = 0;
if (ls->t.token != ')') {
do {
switch (ls->t.token) {
case TK_NAME: {
new_localvar(ls, str_checkname(ls), nparams++);
break;
}
case TK_DOTS: {
luaX_next(ls);
#if defined(LUA_COMPAT_VARARG)
new_localvarliteral(ls, "arg", nparams++);
f->is_vararg = VARARG_HASARG | VARARG_NEEDSARG;
#endif
f->is_vararg |= VARARG_ISVARARG;
break;
}
default: luaX_syntaxerror(ls, "<name> or " LUA_QL("...") " expected");
}
} while (!f->is_vararg && testnext(ls, ','));
}
adjustlocalvars(ls, nparams);
f->numparams = cast_byte(fs->nactvar - (f->is_vararg & VARARG_HASARG));
luaK_reserveregs(fs, fs->nactvar);
}
static void body (LexState *ls, expdesc *e, int needself, int line) {
FuncState new_fs;
open_func(ls, &new_fs);
new_fs.f->linedefined = line;
checknext(ls, '(');
if (needself) {
new_localvarliteral(ls, "self", 0);
adjustlocalvars(ls, 1);
}
parlist(ls);
checknext(ls, ')');
chunk(ls);
new_fs.f->lastlinedefined = ls->linenumber;
check_match(ls, TK_END, TK_FUNCTION, line);
close_func(ls);
pushclosure(ls, &new_fs, e);
}
static int explist1 (LexState *ls, expdesc *v) {
int n = 1;
expr(ls, v);
while (testnext(ls, ',')) {
luaK_exp2nextreg(ls->fs, v);
expr(ls, v);
n++;
}
return n;
}
static void funcargs (LexState *ls, expdesc *f) {
FuncState *fs = ls->fs;
expdesc args;
int base, nparams;
int line = ls->linenumber;
switch (ls->t.token) {
case '(': {
if (line != ls->lastline)
luaX_syntaxerror(ls,"ambiguous syntax (function call x new statement)");
luaX_next(ls);
if (ls->t.token == ')')
args.k = VVOID;
else {
explist1(ls, &args);
luaK_setmultret(fs, &args);
}
check_match(ls, ')', '(', line);
break;
}
case '{': {
constructor(ls, &args);
break;
}
case TK_STRING: {
codestring(ls, &args, ls->t.seminfo.ts);
luaX_next(ls);
break;
}
default: {
luaX_syntaxerror(ls, "function arguments expected");
return;
}
}
lua_assert(f->k == VNONRELOC);
base = f->u.s.info;
if (hasmultret(args.k))
nparams = LUA_MULTRET;
else {
if (args.k != VVOID)
luaK_exp2nextreg(fs, &args);
nparams = fs->freereg - (base+1);
}
init_exp(f, VCALL, luaK_codeABC(fs, OP_CALL, base, nparams+1, 2));
luaK_fixline(fs, line);
fs->freereg = base+1;
}
static void prefixexp (LexState *ls, expdesc *v) {
switch (ls->t.token) {
case '(': {
int line = ls->linenumber;
luaX_next(ls);
expr(ls, v);
check_match(ls, ')', '(', line);
luaK_dischargevars(ls->fs, v);
return;
}
case TK_NAME: {
singlevar(ls, v);
return;
}
default: {
luaX_syntaxerror(ls, "unexpected symbol");
return;
}
}
}
static void primaryexp (LexState *ls, expdesc *v) {
FuncState *fs = ls->fs;
prefixexp(ls, v);
for (;;) {
switch (ls->t.token) {
case '.': {
field(ls, v);
break;
}
case '[': {
expdesc key;
luaK_exp2anyreg(fs, v);
yindex(ls, &key);
luaK_indexed(fs, v, &key);
break;
}
case ':': {
expdesc key;
luaX_next(ls);
checkname(ls, &key);
luaK_self(fs, v, &key);
funcargs(ls, v);
break;
}
case '(': case TK_STRING: case '{': {
luaK_exp2nextreg(fs, v);
funcargs(ls, v);
break;
}
default: return;
}
}
}
static void simpleexp (LexState *ls, expdesc *v) {
switch (ls->t.token) {
case TK_NUMBER: {
init_exp(v, VKNUM, 0);
v->u.nval = ls->t.seminfo.r;
break;
}
case TK_STRING: {
codestring(ls, v, ls->t.seminfo.ts);
break;
}
case TK_NIL: {
init_exp(v, VNIL, 0);
break;
}
case TK_TRUE: {
init_exp(v, VTRUE, 0);
break;
}
case TK_FALSE: {
init_exp(v, VFALSE, 0);
break;
}
case TK_DOTS: {
FuncState *fs = ls->fs;
check_condition(ls, fs->f->is_vararg,
"cannot use " LUA_QL("...") " outside a vararg function");
fs->f->is_vararg &= ~VARARG_NEEDSARG;
init_exp(v, VVARARG, luaK_codeABC(fs, OP_VARARG, 0, 1, 0));
break;
}
case '{': {
constructor(ls, v);
return;
}
case TK_FUNCTION: {
luaX_next(ls);
body(ls, v, 0, ls->linenumber);
return;
}
default: {
primaryexp(ls, v);
return;
}
}
luaX_next(ls);
}
static UnOpr getunopr (int op) {
switch (op) {
case TK_NOT: return OPR_NOT;
case '-': return OPR_MINUS;
case '#': return OPR_LEN;
default: return OPR_NOUNOPR;
}
}
static BinOpr getbinopr (int op) {
switch (op) {
case '+': return OPR_ADD;
case '-': return OPR_SUB;
case '*': return OPR_MUL;
case '/': return OPR_DIV;
case '%': return OPR_MOD;
case '^': return OPR_POW;
case TK_CONCAT: return OPR_CONCAT;
case TK_NE: return OPR_NE;
case TK_EQ: return OPR_EQ;
case '<': return OPR_LT;
case TK_LE: return OPR_LE;
case '>': return OPR_GT;
case TK_GE: return OPR_GE;
case TK_AND: return OPR_AND;
case TK_OR: return OPR_OR;
default: return OPR_NOBINOPR;
}
}
static const struct {
lu_byte left;
lu_byte right;
} priority[] = {
{6, 6}, {6, 6}, {7, 7}, {7, 7}, {7, 7},
{10, 9}, {5, 4},
{3, 3}, {3, 3},
{3, 3}, {3, 3}, {3, 3}, {3, 3},
{2, 2}, {1, 1}
};
#define UNARY_PRIORITY 8
static BinOpr subexpr (LexState *ls, expdesc *v, unsigned int limit) {
BinOpr op;
UnOpr uop;
enterlevel(ls);
uop = getunopr(ls->t.token);
if (uop != OPR_NOUNOPR) {
luaX_next(ls);
subexpr(ls, v, UNARY_PRIORITY);
luaK_prefix(ls->fs, uop, v);
}
else simpleexp(ls, v);
op = getbinopr(ls->t.token);
while (op != OPR_NOBINOPR && priority[op].left > limit) {
expdesc v2;
BinOpr nextop;
luaX_next(ls);
luaK_infix(ls->fs, op, v);
nextop = subexpr(ls, &v2, priority[op].right);
luaK_posfix(ls->fs, op, v, &v2);
op = nextop;
}
leavelevel(ls);
return op;
}
static void expr (LexState *ls, expdesc *v) {
subexpr(ls, v, 0);
}
static int block_follow (int token) {
switch (token) {
case TK_ELSE: case TK_ELSEIF: case TK_END:
case TK_UNTIL: case TK_EOS:
return 1;
default: return 0;
}
}
static void block (LexState *ls) {
FuncState *fs = ls->fs;
BlockCnt bl;
enterblock(fs, &bl, 0);
chunk(ls);
lua_assert(bl.breaklist == NO_JUMP);
leaveblock(fs);
}
struct LHS_assign {
struct LHS_assign *prev;
expdesc v;
};
static void check_conflict (LexState *ls, struct LHS_assign *lh, expdesc *v) {
FuncState *fs = ls->fs;
int extra = fs->freereg;
int conflict = 0;
for (; lh; lh = lh->prev) {
if (lh->v.k == VINDEXED) {
if (lh->v.u.s.info == v->u.s.info) {
conflict = 1;
lh->v.u.s.info = extra;
}
if (lh->v.u.s.aux == v->u.s.info) {
conflict = 1;
lh->v.u.s.aux = extra;
}
}
}
if (conflict) {
luaK_codeABC(fs, OP_MOVE, fs->freereg, v->u.s.info, 0);
luaK_reserveregs(fs, 1);
}
}
static void assignment (LexState *ls, struct LHS_assign *lh, int nvars) {
expdesc e;
check_condition(ls, VLOCAL <= lh->v.k && lh->v.k <= VINDEXED,
"syntax error");
if (testnext(ls, ',')) {
struct LHS_assign nv;
nv.prev = lh;
primaryexp(ls, &nv.v);
if (nv.v.k == VLOCAL)
check_conflict(ls, lh, &nv.v);
luaY_checklimit(ls->fs, nvars, LUAI_MAXCCALLS - ls->L->nCcalls,
"variables in assignment");
assignment(ls, &nv, nvars+1);
}
else {
int nexps;
checknext(ls, '=');
nexps = explist1(ls, &e);
if (nexps != nvars) {
adjust_assign(ls, nvars, nexps, &e);
if (nexps > nvars)
ls->fs->freereg -= nexps - nvars;
}
else {
luaK_setoneret(ls->fs, &e);
luaK_storevar(ls->fs, &lh->v, &e);
return;
}
}
init_exp(&e, VNONRELOC, ls->fs->freereg-1);
luaK_storevar(ls->fs, &lh->v, &e);
}
static int cond (LexState *ls) {
expdesc v;
expr(ls, &v);
if (v.k == VNIL) v.k = VFALSE;
luaK_goiftrue(ls->fs, &v);
return v.f;
}
static void breakstat (LexState *ls) {
FuncState *fs = ls->fs;
BlockCnt *bl = fs->bl;
int upval = 0;
while (bl && !bl->isbreakable) {
upval |= bl->upval;
bl = bl->previous;
}
if (!bl)
luaX_syntaxerror(ls, "no loop to break");
if (upval)
luaK_codeABC(fs, OP_CLOSE, bl->nactvar, 0, 0);
luaK_concat(fs, &bl->breaklist, luaK_jump(fs));
}
static void whilestat (LexState *ls, int line) {
FuncState *fs = ls->fs;
int whileinit;
int condexit;
BlockCnt bl;
luaX_next(ls);
whileinit = luaK_getlabel(fs);
condexit = cond(ls);
enterblock(fs, &bl, 1);
checknext(ls, TK_DO);
block(ls);
luaK_patchlist(fs, luaK_jump(fs), whileinit);
check_match(ls, TK_END, TK_WHILE, line);
leaveblock(fs);
luaK_patchtohere(fs, condexit);
}
static void repeatstat (LexState *ls, int line) {
int condexit;
FuncState *fs = ls->fs;
int repeat_init = luaK_getlabel(fs);
BlockCnt bl1, bl2;
enterblock(fs, &bl1, 1);
enterblock(fs, &bl2, 0);
luaX_next(ls);
chunk(ls);
check_match(ls, TK_UNTIL, TK_REPEAT, line);
condexit = cond(ls);
if (!bl2.upval) {
leaveblock(fs);
luaK_patchlist(ls->fs, condexit, repeat_init);
}
else {
breakstat(ls);
luaK_patchtohere(ls->fs, condexit);
leaveblock(fs);
luaK_patchlist(ls->fs, luaK_jump(fs), repeat_init);
}
leaveblock(fs);
}
static int exp1 (LexState *ls) {
expdesc e;
int k;
expr(ls, &e);
k = e.k;
luaK_exp2nextreg(ls->fs, &e);
return k;
}
static void forbody (LexState *ls, int base, int line, int nvars, int isnum) {
BlockCnt bl;
FuncState *fs = ls->fs;
int prep, endfor;
adjustlocalvars(ls, 3);
checknext(ls, TK_DO);
prep = isnum ? luaK_codeAsBx(fs, OP_FORPREP, base, NO_JUMP) : luaK_jump(fs);
enterblock(fs, &bl, 0);
adjustlocalvars(ls, nvars);
luaK_reserveregs(fs, nvars);
block(ls);
leaveblock(fs);
luaK_patchtohere(fs, prep);
endfor = (isnum) ? luaK_codeAsBx(fs, OP_FORLOOP, base, NO_JUMP) :
luaK_codeABC(fs, OP_TFORLOOP, base, 0, nvars);
luaK_fixline(fs, line);
luaK_patchlist(fs, (isnum ? endfor : luaK_jump(fs)), prep + 1);
}
static void fornum (LexState *ls, TString *varname, int line) {
FuncState *fs = ls->fs;
int base = fs->freereg;
new_localvarliteral(ls, "(for index)", 0);
new_localvarliteral(ls, "(for limit)", 1);
new_localvarliteral(ls, "(for step)", 2);
new_localvar(ls, varname, 3);
checknext(ls, '=');
exp1(ls);
checknext(ls, ',');
exp1(ls);
if (testnext(ls, ','))
exp1(ls);
else {
luaK_codeABx(fs, OP_LOADK, fs->freereg, luaK_numberK(fs, 1));
luaK_reserveregs(fs, 1);
}
forbody(ls, base, line, 1, 1);
}
static void forlist (LexState *ls, TString *indexname) {
FuncState *fs = ls->fs;
expdesc e;
int nvars = 0;
int line;
int base = fs->freereg;
new_localvarliteral(ls, "(for generator)", nvars++);
new_localvarliteral(ls, "(for state)", nvars++);
new_localvarliteral(ls, "(for control)", nvars++);
new_localvar(ls, indexname, nvars++);
while (testnext(ls, ','))
new_localvar(ls, str_checkname(ls), nvars++);
checknext(ls, TK_IN);
line = ls->linenumber;
adjust_assign(ls, 3, explist1(ls, &e), &e);
luaK_checkstack(fs, 3);
forbody(ls, base, line, nvars - 3, 0);
}
static void forstat (LexState *ls, int line) {
FuncState *fs = ls->fs;
TString *varname;
BlockCnt bl;
enterblock(fs, &bl, 1);
luaX_next(ls);
varname = str_checkname(ls);
switch (ls->t.token) {
case '=': fornum(ls, varname, line); break;
case ',': case TK_IN: forlist(ls, varname); break;
default: luaX_syntaxerror(ls, LUA_QL("=") " or " LUA_QL("in") " expected");
}
check_match(ls, TK_END, TK_FOR, line);
leaveblock(fs);
}
static int test_then_block (LexState *ls) {
int condexit;
luaX_next(ls);
condexit = cond(ls);
checknext(ls, TK_THEN);
block(ls);
return condexit;
}
static void ifstat (LexState *ls, int line) {
FuncState *fs = ls->fs;
int flist;
int escapelist = NO_JUMP;
flist = test_then_block(ls);
while (ls->t.token == TK_ELSEIF) {
luaK_concat(fs, &escapelist, luaK_jump(fs));
luaK_patchtohere(fs, flist);
flist = test_then_block(ls);
}
if (ls->t.token == TK_ELSE) {
luaK_concat(fs, &escapelist, luaK_jump(fs));
luaK_patchtohere(fs, flist);
luaX_next(ls);
block(ls);
}
else
luaK_concat(fs, &escapelist, flist);
luaK_patchtohere(fs, escapelist);
check_match(ls, TK_END, TK_IF, line);
}
static void localfunc (LexState *ls) {
expdesc v, b;
FuncState *fs = ls->fs;
new_localvar(ls, str_checkname(ls), 0);
init_exp(&v, VLOCAL, fs->freereg);
luaK_reserveregs(fs, 1);
adjustlocalvars(ls, 1);
body(ls, &b, 0, ls->linenumber);
luaK_storevar(fs, &v, &b);
getlocvar(fs, fs->nactvar - 1).startpc = fs->pc;
}
static void localstat (LexState *ls) {
int nvars = 0;
int nexps;
expdesc e;
do {
new_localvar(ls, str_checkname(ls), nvars++);
} while (testnext(ls, ','));
if (testnext(ls, '='))
nexps = explist1(ls, &e);
else {
e.k = VVOID;
nexps = 0;
}
adjust_assign(ls, nvars, nexps, &e);
adjustlocalvars(ls, nvars);
}
static int funcname (LexState *ls, expdesc *v) {
int needself = 0;
singlevar(ls, v);
while (ls->t.token == '.')
field(ls, v);
if (ls->t.token == ':') {
needself = 1;
field(ls, v);
}
return needself;
}
static void funcstat (LexState *ls, int line) {
int needself;
expdesc v, b;
luaX_next(ls);
needself = funcname(ls, &v);
body(ls, &b, needself, line);
luaK_storevar(ls->fs, &v, &b);
luaK_fixline(ls->fs, line);
}
static void exprstat (LexState *ls) {
FuncState *fs = ls->fs;
struct LHS_assign v;
primaryexp(ls, &v.v);
if (v.v.k == VCALL)
SETARG_C(getcode(fs, &v.v), 1);
else {
v.prev = NULL;
assignment(ls, &v, 1);
}
}
static void retstat (LexState *ls) {
FuncState *fs = ls->fs;
expdesc e;
int first, nret;
luaX_next(ls);
if (block_follow(ls->t.token) || ls->t.token == ';')
first = nret = 0;
else {
nret = explist1(ls, &e);
if (hasmultret(e.k)) {
luaK_setmultret(fs, &e);
if (e.k == VCALL && nret == 1) {
SET_OPCODE(getcode(fs,&e), OP_TAILCALL);
lua_assert(GETARG_A(getcode(fs,&e)) == fs->nactvar);
}
first = fs->nactvar;
nret = LUA_MULTRET;
}
else {
if (nret == 1)
first = luaK_exp2anyreg(fs, &e);
else {
luaK_exp2nextreg(fs, &e);
first = fs->nactvar;
lua_assert(nret == fs->freereg - first);
}
}
}
luaK_ret(fs, first, nret);
}
static int statement (LexState *ls) {
int line = ls->linenumber;
switch (ls->t.token) {
case TK_IF: {
ifstat(ls, line);
return 0;
}
case TK_WHILE: {
whilestat(ls, line);
return 0;
}
case TK_DO: {
luaX_next(ls);
block(ls);
check_match(ls, TK_END, TK_DO, line);
return 0;
}
case TK_FOR: {
forstat(ls, line);
return 0;
}
case TK_REPEAT: {
repeatstat(ls, line);
return 0;
}
case TK_FUNCTION: {
funcstat(ls, line);
return 0;
}
case TK_LOCAL: {
luaX_next(ls);
if (testnext(ls, TK_FUNCTION))
localfunc(ls);
else
localstat(ls);
return 0;
}
case TK_RETURN: {
retstat(ls);
return 1;
}
case TK_BREAK: {
luaX_next(ls);
breakstat(ls);
return 1;
}
default: {
exprstat(ls);
return 0;
}
}
}
static void chunk (LexState *ls) {
int islast = 0;
enterlevel(ls);
while (!islast && !block_follow(ls->t.token)) {
islast = statement(ls);
testnext(ls, ';');
lua_assert(ls->fs->f->maxstacksize >= ls->fs->freereg &&
ls->fs->freereg >= ls->fs->nactvar);
ls->fs->freereg = ls->fs->nactvar;
}
leavelevel(ls);
}
