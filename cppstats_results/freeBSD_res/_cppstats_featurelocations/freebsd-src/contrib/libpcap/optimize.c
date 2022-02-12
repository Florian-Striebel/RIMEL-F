






















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap-types.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <setjmp.h>
#include <string.h>

#include <errno.h>

#include "pcap-int.h"

#include "gencode.h"
#include "optimize.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

#if defined(BDEBUG)






static int pcap_optimizer_debug;










PCAP_API void pcap_set_optimizer_debug(int value);

PCAP_API_DEF void
pcap_set_optimizer_debug(int value)
{
pcap_optimizer_debug = value;
}







static int pcap_print_dot_graph;










PCAP_API void pcap_set_print_dot_graph(int value);

PCAP_API_DEF void
pcap_set_print_dot_graph(int value)
{
pcap_print_dot_graph = value;
}

#endif















#if PCAP_IS_AT_LEAST_GNUC_VERSION(3,4)



#define lowest_set_bit(mask) __builtin_ctz(mask)
#elif defined(_MSC_VER)




#include <intrin.h>

#if !defined(__clang__)
#pragma intrinsic(_BitScanForward)
#endif

static __forceinline int
lowest_set_bit(int mask)
{
unsigned long bit;





if (_BitScanForward(&bit, (unsigned int)mask) == 0)
return -1;
return (int)bit;
}
#elif defined(MSDOS) && defined(__DJGPP__)




#define lowest_set_bit(mask) (ffs((mask)) - 1)
#elif (defined(MSDOS) && defined(__WATCOMC__)) || defined(STRINGS_H_DECLARES_FFS)





#include <strings.h>
#define lowest_set_bit(mask) (ffs((mask)) - 1)
#else




static int
lowest_set_bit(int mask)
{
unsigned int v = (unsigned int)mask;

static const int MultiplyDeBruijnBitPosition[32] = {
0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};












return (MultiplyDeBruijnBitPosition[((v & -v) * 0x077CB531U) >> 27]);
}
#endif




#define NOP -1







#define A_ATOM BPF_MEMWORDS
#define X_ATOM (BPF_MEMWORDS+1)






#define AX_ATOM N_ATOMS







struct valnode {
int code;
int v0, v1;
int val;
struct valnode *next;
};


#define K(i) F(opt_state, BPF_LD|BPF_IMM|BPF_W, i, 0L)

struct vmapinfo {
int is_const;
bpf_int32 const_val;
};

typedef struct {



jmp_buf top_ctx;




char *errbuf;






int done;

int n_blocks;
struct block **blocks;
int n_edges;
struct edge **edges;





int nodewords;
int edgewords;
struct block **levels;
bpf_u_int32 *space;

#define BITS_PER_WORD (8*sizeof(bpf_u_int32))



#define SET_MEMBER(p, a) ((p)[(unsigned)(a) / BITS_PER_WORD] & ((bpf_u_int32)1 << ((unsigned)(a) % BITS_PER_WORD)))





#define SET_INSERT(p, a) (p)[(unsigned)(a) / BITS_PER_WORD] |= ((bpf_u_int32)1 << ((unsigned)(a) % BITS_PER_WORD))





#define SET_DELETE(p, a) (p)[(unsigned)(a) / BITS_PER_WORD] &= ~((bpf_u_int32)1 << ((unsigned)(a) % BITS_PER_WORD))





#define SET_INTERSECT(a, b, n){register bpf_u_int32 *_x = a, *_y = b;register int _n = n;while (--_n >= 0) *_x++ &= *_y++;}









#define SET_SUBTRACT(a, b, n){register bpf_u_int32 *_x = a, *_y = b;register int _n = n;while (--_n >= 0) *_x++ &=~ *_y++;}









#define SET_UNION(a, b, n){register bpf_u_int32 *_x = a, *_y = b;register int _n = n;while (--_n >= 0) *_x++ |= *_y++;}






uset all_dom_sets;
uset all_closure_sets;
uset all_edge_sets;

#define MODULUS 213
struct valnode *hashtbl[MODULUS];
int curval;
int maxval;

struct vmapinfo *vmap;
struct valnode *vnode_base;
struct valnode *next_vnode;
} opt_state_t;

typedef struct {



jmp_buf top_ctx;




char *errbuf;







struct bpf_insn *fstart;
struct bpf_insn *ftail;
} conv_state_t;

static void opt_init(opt_state_t *, struct icode *);
static void opt_cleanup(opt_state_t *);
static void PCAP_NORETURN opt_error(opt_state_t *, const char *, ...)
PCAP_PRINTFLIKE(2, 3);

static void intern_blocks(opt_state_t *, struct icode *);

static void find_inedges(opt_state_t *, struct block *);
#if defined(BDEBUG)
static void opt_dump(opt_state_t *, struct icode *);
#endif

#if !defined(MAX)
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

static void
find_levels_r(opt_state_t *opt_state, struct icode *ic, struct block *b)
{
int level;

if (isMarked(ic, b))
return;

Mark(ic, b);
b->link = 0;

if (JT(b)) {
find_levels_r(opt_state, ic, JT(b));
find_levels_r(opt_state, ic, JF(b));
level = MAX(JT(b)->level, JF(b)->level) + 1;
} else
level = 0;
b->level = level;
b->link = opt_state->levels[level];
opt_state->levels[level] = b;
}







static void
find_levels(opt_state_t *opt_state, struct icode *ic)
{
memset((char *)opt_state->levels, 0, opt_state->n_blocks * sizeof(*opt_state->levels));
unMarkAll(ic);
find_levels_r(opt_state, ic, ic->root);
}





static void
find_dom(opt_state_t *opt_state, struct block *root)
{
int i;
struct block *b;
bpf_u_int32 *x;




x = opt_state->all_dom_sets;
i = opt_state->n_blocks * opt_state->nodewords;
while (--i >= 0)
*x++ = 0xFFFFFFFFU;

for (i = opt_state->nodewords; --i >= 0;)
root->dom[i] = 0;


for (i = root->level; i >= 0; --i) {
for (b = opt_state->levels[i]; b; b = b->link) {
SET_INSERT(b->dom, b->id);
if (JT(b) == 0)
continue;
SET_INTERSECT(JT(b)->dom, b->dom, opt_state->nodewords);
SET_INTERSECT(JF(b)->dom, b->dom, opt_state->nodewords);
}
}
}

static void
propedom(opt_state_t *opt_state, struct edge *ep)
{
SET_INSERT(ep->edom, ep->id);
if (ep->succ) {
SET_INTERSECT(ep->succ->et.edom, ep->edom, opt_state->edgewords);
SET_INTERSECT(ep->succ->ef.edom, ep->edom, opt_state->edgewords);
}
}





static void
find_edom(opt_state_t *opt_state, struct block *root)
{
int i;
uset x;
struct block *b;

x = opt_state->all_edge_sets;
for (i = opt_state->n_edges * opt_state->edgewords; --i >= 0; )
x[i] = 0xFFFFFFFFU;


memset(root->et.edom, 0, opt_state->edgewords * sizeof(*(uset)0));
memset(root->ef.edom, 0, opt_state->edgewords * sizeof(*(uset)0));
for (i = root->level; i >= 0; --i) {
for (b = opt_state->levels[i]; b != 0; b = b->link) {
propedom(opt_state, &b->et);
propedom(opt_state, &b->ef);
}
}
}








static void
find_closure(opt_state_t *opt_state, struct block *root)
{
int i;
struct block *b;




memset((char *)opt_state->all_closure_sets, 0,
opt_state->n_blocks * opt_state->nodewords * sizeof(*opt_state->all_closure_sets));


for (i = root->level; i >= 0; --i) {
for (b = opt_state->levels[i]; b; b = b->link) {
SET_INSERT(b->closure, b->id);
if (JT(b) == 0)
continue;
SET_UNION(JT(b)->closure, b->closure, opt_state->nodewords);
SET_UNION(JF(b)->closure, b->closure, opt_state->nodewords);
}
}
}







static int
atomuse(struct stmt *s)
{
register int c = s->code;

if (c == NOP)
return -1;

switch (BPF_CLASS(c)) {

case BPF_RET:
return (BPF_RVAL(c) == BPF_A) ? A_ATOM :
(BPF_RVAL(c) == BPF_X) ? X_ATOM : -1;

case BPF_LD:
case BPF_LDX:
return (BPF_MODE(c) == BPF_IND) ? X_ATOM :
(BPF_MODE(c) == BPF_MEM) ? s->k : -1;

case BPF_ST:
return A_ATOM;

case BPF_STX:
return X_ATOM;

case BPF_JMP:
case BPF_ALU:
if (BPF_SRC(c) == BPF_X)
return AX_ATOM;
return A_ATOM;

case BPF_MISC:
return BPF_MISCOP(c) == BPF_TXA ? X_ATOM : A_ATOM;
}
abort();

}








static int
atomdef(struct stmt *s)
{
if (s->code == NOP)
return -1;

switch (BPF_CLASS(s->code)) {

case BPF_LD:
case BPF_ALU:
return A_ATOM;

case BPF_LDX:
return X_ATOM;

case BPF_ST:
case BPF_STX:
return s->k;

case BPF_MISC:
return BPF_MISCOP(s->code) == BPF_TAX ? X_ATOM : A_ATOM;
}
return -1;
}












static void
compute_local_ud(struct block *b)
{
struct slist *s;
atomset def = 0, use = 0, killed = 0;
int atom;

for (s = b->stmts; s; s = s->next) {
if (s->s.code == NOP)
continue;
atom = atomuse(&s->s);
if (atom >= 0) {
if (atom == AX_ATOM) {
if (!ATOMELEM(def, X_ATOM))
use |= ATOMMASK(X_ATOM);
if (!ATOMELEM(def, A_ATOM))
use |= ATOMMASK(A_ATOM);
}
else if (atom < N_ATOMS) {
if (!ATOMELEM(def, atom))
use |= ATOMMASK(atom);
}
else
abort();
}
atom = atomdef(&s->s);
if (atom >= 0) {
if (!ATOMELEM(use, atom))
killed |= ATOMMASK(atom);
def |= ATOMMASK(atom);
}
}
if (BPF_CLASS(b->s.code) == BPF_JMP) {



atom = atomuse(&b->s);
if (atom >= 0) {
if (atom == AX_ATOM) {
if (!ATOMELEM(def, X_ATOM))
use |= ATOMMASK(X_ATOM);
if (!ATOMELEM(def, A_ATOM))
use |= ATOMMASK(A_ATOM);
}
else if (atom < N_ATOMS) {
if (!ATOMELEM(def, atom))
use |= ATOMMASK(atom);
}
else
abort();
}
}

b->def = def;
b->kill = killed;
b->in_use = use;
}




static void
find_ud(opt_state_t *opt_state, struct block *root)
{
int i, maxlevel;
struct block *p;





maxlevel = root->level;
for (i = maxlevel; i >= 0; --i)
for (p = opt_state->levels[i]; p; p = p->link) {
compute_local_ud(p);
p->out_use = 0;
}

for (i = 1; i <= maxlevel; ++i) {
for (p = opt_state->levels[i]; p; p = p->link) {
p->out_use |= JT(p)->in_use | JF(p)->in_use;
p->in_use |= p->out_use &~ p->kill;
}
}
}
static void
init_val(opt_state_t *opt_state)
{
opt_state->curval = 0;
opt_state->next_vnode = opt_state->vnode_base;
memset((char *)opt_state->vmap, 0, opt_state->maxval * sizeof(*opt_state->vmap));
memset((char *)opt_state->hashtbl, 0, sizeof opt_state->hashtbl);
}


static int
F(opt_state_t *opt_state, int code, int v0, int v1)
{
u_int hash;
int val;
struct valnode *p;

hash = (u_int)code ^ ((u_int)v0 << 4) ^ ((u_int)v1 << 8);
hash %= MODULUS;

for (p = opt_state->hashtbl[hash]; p; p = p->next)
if (p->code == code && p->v0 == v0 && p->v1 == v1)
return p->val;

val = ++opt_state->curval;
if (BPF_MODE(code) == BPF_IMM &&
(BPF_CLASS(code) == BPF_LD || BPF_CLASS(code) == BPF_LDX)) {
opt_state->vmap[val].const_val = v0;
opt_state->vmap[val].is_const = 1;
}
p = opt_state->next_vnode++;
p->val = val;
p->code = code;
p->v0 = v0;
p->v1 = v1;
p->next = opt_state->hashtbl[hash];
opt_state->hashtbl[hash] = p;

return val;
}

static inline void
vstore(struct stmt *s, int *valp, int newval, int alter)
{
if (alter && newval != VAL_UNKNOWN && *valp == newval)
s->code = NOP;
else
*valp = newval;
}





static void
fold_op(opt_state_t *opt_state, struct stmt *s, int v0, int v1)
{
bpf_u_int32 a, b;

a = opt_state->vmap[v0].const_val;
b = opt_state->vmap[v1].const_val;

switch (BPF_OP(s->code)) {
case BPF_ADD:
a += b;
break;

case BPF_SUB:
a -= b;
break;

case BPF_MUL:
a *= b;
break;

case BPF_DIV:
if (b == 0)
opt_error(opt_state, "division by zero");
a /= b;
break;

case BPF_MOD:
if (b == 0)
opt_error(opt_state, "modulus by zero");
a %= b;
break;

case BPF_AND:
a &= b;
break;

case BPF_OR:
a |= b;
break;

case BPF_XOR:
a ^= b;
break;

case BPF_LSH:











if (b < 32)
a <<= b;
else
a = 0;
break;

case BPF_RSH:











if (b < 32)
a >>= b;
else
a = 0;
break;

default:
abort();
}
s->k = a;
s->code = BPF_LD|BPF_IMM;
opt_state->done = 0;
}

static inline struct slist *
this_op(struct slist *s)
{
while (s != 0 && s->s.code == NOP)
s = s->next;
return s;
}

static void
opt_not(struct block *b)
{
struct block *tmp = JT(b);

JT(b) = JF(b);
JF(b) = tmp;
}

static void
opt_peep(opt_state_t *opt_state, struct block *b)
{
struct slist *s;
struct slist *next, *last;
int val;

s = b->stmts;
if (s == 0)
return;

last = s;
for (; ; s = next) {



s = this_op(s);
if (s == 0)
break;





next = this_op(s->next);
if (next == 0)
break;
last = next;





if (s->s.code == BPF_ST &&
next->s.code == (BPF_LDX|BPF_MEM) &&
s->s.k == next->s.k) {
opt_state->done = 0;
next->s.code = BPF_MISC|BPF_TAX;
}




if (s->s.code == (BPF_LD|BPF_IMM) &&
next->s.code == (BPF_MISC|BPF_TAX)) {
s->s.code = BPF_LDX|BPF_IMM;
next->s.code = BPF_MISC|BPF_TXA;
opt_state->done = 0;
}




if (s->s.code == (BPF_LD|BPF_IMM)) {
struct slist *add, *tax, *ild;







if (ATOMELEM(b->out_use, X_ATOM))
continue;







if (next->s.code != (BPF_LDX|BPF_MSH|BPF_B))
add = next;
else
add = this_op(next->next);
if (add == 0 || add->s.code != (BPF_ALU|BPF_ADD|BPF_X))
continue;





tax = this_op(add->next);
if (tax == 0 || tax->s.code != (BPF_MISC|BPF_TAX))
continue;





ild = this_op(tax->next);
if (ild == 0 || BPF_CLASS(ild->s.code) != BPF_LD ||
BPF_MODE(ild->s.code) != BPF_IND)
continue;




























ild->s.k += s->s.k;
s->s.code = NOP;
add->s.code = NOP;
tax->s.code = NOP;
opt_state->done = 0;
}
}







if (b->s.code == (BPF_JMP|BPF_JEQ|BPF_K) &&
!ATOMELEM(b->out_use, A_ATOM)) {




if (last->s.code == (BPF_ALU|BPF_SUB|BPF_X)) {
val = b->val[X_ATOM];
if (opt_state->vmap[val].is_const) {









b->s.k += opt_state->vmap[val].const_val;
last->s.code = NOP;
opt_state->done = 0;
} else if (b->s.k == 0) {









last->s.code = NOP;
b->s.code = BPF_JMP|BPF_JEQ|BPF_X;
opt_state->done = 0;
}
}






else if (last->s.code == (BPF_ALU|BPF_SUB|BPF_K)) {
last->s.code = NOP;
b->s.k += last->s.k;
opt_state->done = 0;
}







else if (last->s.code == (BPF_ALU|BPF_AND|BPF_K) &&
b->s.k == 0) {
b->s.k = last->s.k;
b->s.code = BPF_JMP|BPF_K|BPF_JSET;
last->s.code = NOP;
opt_state->done = 0;
opt_not(b);
}
}




if (b->s.code == (BPF_JMP|BPF_K|BPF_JSET)) {
if (b->s.k == 0)
JT(b) = JF(b);
if ((u_int)b->s.k == 0xffffffffU)
JF(b) = JT(b);
}





val = b->val[X_ATOM];
if (opt_state->vmap[val].is_const && BPF_SRC(b->s.code) == BPF_X) {
bpf_int32 v = opt_state->vmap[val].const_val;
b->s.code &= ~BPF_X;
b->s.k = v;
}




val = b->val[A_ATOM];
if (opt_state->vmap[val].is_const && BPF_SRC(b->s.code) == BPF_K) {
bpf_int32 v = opt_state->vmap[val].const_val;
switch (BPF_OP(b->s.code)) {

case BPF_JEQ:
v = v == b->s.k;
break;

case BPF_JGT:
v = (unsigned)v > (unsigned)b->s.k;
break;

case BPF_JGE:
v = (unsigned)v >= (unsigned)b->s.k;
break;

case BPF_JSET:
v &= b->s.k;
break;

default:
abort();
}
if (JF(b) != JT(b))
opt_state->done = 0;
if (v)
JF(b) = JT(b);
else
JT(b) = JF(b);
}
}







static void
opt_stmt(opt_state_t *opt_state, struct stmt *s, int val[], int alter)
{
int op;
int v;

switch (s->code) {

case BPF_LD|BPF_ABS|BPF_W:
case BPF_LD|BPF_ABS|BPF_H:
case BPF_LD|BPF_ABS|BPF_B:
v = F(opt_state, s->code, s->k, 0L);
vstore(s, &val[A_ATOM], v, alter);
break;

case BPF_LD|BPF_IND|BPF_W:
case BPF_LD|BPF_IND|BPF_H:
case BPF_LD|BPF_IND|BPF_B:
v = val[X_ATOM];
if (alter && opt_state->vmap[v].is_const) {
s->code = BPF_LD|BPF_ABS|BPF_SIZE(s->code);
s->k += opt_state->vmap[v].const_val;
v = F(opt_state, s->code, s->k, 0L);
opt_state->done = 0;
}
else
v = F(opt_state, s->code, s->k, v);
vstore(s, &val[A_ATOM], v, alter);
break;

case BPF_LD|BPF_LEN:
v = F(opt_state, s->code, 0L, 0L);
vstore(s, &val[A_ATOM], v, alter);
break;

case BPF_LD|BPF_IMM:
v = K(s->k);
vstore(s, &val[A_ATOM], v, alter);
break;

case BPF_LDX|BPF_IMM:
v = K(s->k);
vstore(s, &val[X_ATOM], v, alter);
break;

case BPF_LDX|BPF_MSH|BPF_B:
v = F(opt_state, s->code, s->k, 0L);
vstore(s, &val[X_ATOM], v, alter);
break;

case BPF_ALU|BPF_NEG:
if (alter && opt_state->vmap[val[A_ATOM]].is_const) {
s->code = BPF_LD|BPF_IMM;
















s->k = 0U - (bpf_u_int32)(opt_state->vmap[val[A_ATOM]].const_val);
val[A_ATOM] = K(s->k);
}
else
val[A_ATOM] = F(opt_state, s->code, val[A_ATOM], 0L);
break;

case BPF_ALU|BPF_ADD|BPF_K:
case BPF_ALU|BPF_SUB|BPF_K:
case BPF_ALU|BPF_MUL|BPF_K:
case BPF_ALU|BPF_DIV|BPF_K:
case BPF_ALU|BPF_MOD|BPF_K:
case BPF_ALU|BPF_AND|BPF_K:
case BPF_ALU|BPF_OR|BPF_K:
case BPF_ALU|BPF_XOR|BPF_K:
case BPF_ALU|BPF_LSH|BPF_K:
case BPF_ALU|BPF_RSH|BPF_K:
op = BPF_OP(s->code);
if (alter) {
if (s->k == 0) {











if (op == BPF_ADD ||
op == BPF_LSH || op == BPF_RSH ||
op == BPF_OR || op == BPF_XOR) {
s->code = NOP;
break;
}
if (op == BPF_MUL || op == BPF_AND) {
s->code = BPF_LD|BPF_IMM;
val[A_ATOM] = K(s->k);
break;
}
if (op == BPF_DIV)
opt_error(opt_state,
"division by zero");
if (op == BPF_MOD)
opt_error(opt_state,
"modulus by zero");
}
if (opt_state->vmap[val[A_ATOM]].is_const) {
fold_op(opt_state, s, val[A_ATOM], K(s->k));
val[A_ATOM] = K(s->k);
break;
}
}
val[A_ATOM] = F(opt_state, s->code, val[A_ATOM], K(s->k));
break;

case BPF_ALU|BPF_ADD|BPF_X:
case BPF_ALU|BPF_SUB|BPF_X:
case BPF_ALU|BPF_MUL|BPF_X:
case BPF_ALU|BPF_DIV|BPF_X:
case BPF_ALU|BPF_MOD|BPF_X:
case BPF_ALU|BPF_AND|BPF_X:
case BPF_ALU|BPF_OR|BPF_X:
case BPF_ALU|BPF_XOR|BPF_X:
case BPF_ALU|BPF_LSH|BPF_X:
case BPF_ALU|BPF_RSH|BPF_X:
op = BPF_OP(s->code);
if (alter && opt_state->vmap[val[X_ATOM]].is_const) {
if (opt_state->vmap[val[A_ATOM]].is_const) {
fold_op(opt_state, s, val[A_ATOM], val[X_ATOM]);
val[A_ATOM] = K(s->k);
}
else {
s->code = BPF_ALU|BPF_K|op;
s->k = opt_state->vmap[val[X_ATOM]].const_val;






if ((op == BPF_LSH || op == BPF_RSH) &&
(s->k < 0 || s->k > 31))
opt_error(opt_state,
"shift by more than 31 bits");
opt_state->done = 0;
val[A_ATOM] =
F(opt_state, s->code, val[A_ATOM], K(s->k));
}
break;
}







if (alter && opt_state->vmap[val[A_ATOM]].is_const
&& opt_state->vmap[val[A_ATOM]].const_val == 0) {
if (op == BPF_ADD || op == BPF_OR || op == BPF_XOR) {
s->code = BPF_MISC|BPF_TXA;
vstore(s, &val[A_ATOM], val[X_ATOM], alter);
break;
}
else if (op == BPF_MUL || op == BPF_DIV || op == BPF_MOD ||
op == BPF_AND || op == BPF_LSH || op == BPF_RSH) {
s->code = BPF_LD|BPF_IMM;
s->k = 0;
vstore(s, &val[A_ATOM], K(s->k), alter);
break;
}
else if (op == BPF_NEG) {
s->code = NOP;
break;
}
}
val[A_ATOM] = F(opt_state, s->code, val[A_ATOM], val[X_ATOM]);
break;

case BPF_MISC|BPF_TXA:
vstore(s, &val[A_ATOM], val[X_ATOM], alter);
break;

case BPF_LD|BPF_MEM:
v = val[s->k];
if (alter && opt_state->vmap[v].is_const) {
s->code = BPF_LD|BPF_IMM;
s->k = opt_state->vmap[v].const_val;
opt_state->done = 0;
}
vstore(s, &val[A_ATOM], v, alter);
break;

case BPF_MISC|BPF_TAX:
vstore(s, &val[X_ATOM], val[A_ATOM], alter);
break;

case BPF_LDX|BPF_MEM:
v = val[s->k];
if (alter && opt_state->vmap[v].is_const) {
s->code = BPF_LDX|BPF_IMM;
s->k = opt_state->vmap[v].const_val;
opt_state->done = 0;
}
vstore(s, &val[X_ATOM], v, alter);
break;

case BPF_ST:
vstore(s, &val[s->k], val[A_ATOM], alter);
break;

case BPF_STX:
vstore(s, &val[s->k], val[X_ATOM], alter);
break;
}
}

static void
deadstmt(opt_state_t *opt_state, register struct stmt *s, register struct stmt *last[])
{
register int atom;

atom = atomuse(s);
if (atom >= 0) {
if (atom == AX_ATOM) {
last[X_ATOM] = 0;
last[A_ATOM] = 0;
}
else
last[atom] = 0;
}
atom = atomdef(s);
if (atom >= 0) {
if (last[atom]) {
opt_state->done = 0;
last[atom]->code = NOP;
}
last[atom] = s;
}
}

static void
opt_deadstores(opt_state_t *opt_state, register struct block *b)
{
register struct slist *s;
register int atom;
struct stmt *last[N_ATOMS];

memset((char *)last, 0, sizeof last);

for (s = b->stmts; s != 0; s = s->next)
deadstmt(opt_state, &s->s, last);
deadstmt(opt_state, &b->s, last);

for (atom = 0; atom < N_ATOMS; ++atom)
if (last[atom] && !ATOMELEM(b->out_use, atom)) {
last[atom]->code = NOP;
opt_state->done = 0;
}
}

static void
opt_blk(opt_state_t *opt_state, struct block *b, int do_stmts)
{
struct slist *s;
struct edge *p;
int i;
bpf_int32 aval, xval;

#if 0
for (s = b->stmts; s && s->next; s = s->next)
if (BPF_CLASS(s->s.code) == BPF_JMP) {
do_stmts = 0;
break;
}
#endif




p = b->in_edges;
if (p == 0) {




memset((char *)b->val, 0, sizeof(b->val));
} else {






memcpy((char *)b->val, (char *)p->pred->val, sizeof(b->val));








while ((p = p->next) != NULL) {
for (i = 0; i < N_ATOMS; ++i)
if (b->val[i] != p->pred->val[i])
b->val[i] = 0;
}
}
aval = b->val[A_ATOM];
xval = b->val[X_ATOM];
for (s = b->stmts; s; s = s->next)
opt_stmt(opt_state, &s->s, b->val, do_stmts);
























if (do_stmts &&
((b->out_use == 0 &&
aval != VAL_UNKNOWN && b->val[A_ATOM] == aval &&
xval != VAL_UNKNOWN && b->val[X_ATOM] == xval) ||
BPF_CLASS(b->s.code) == BPF_RET)) {
if (b->stmts != 0) {
b->stmts = 0;
opt_state->done = 0;
}
} else {
opt_peep(opt_state, b);
opt_deadstores(opt_state, b);
}



if (BPF_SRC(b->s.code) == BPF_K)
b->oval = K(b->s.k);
else
b->oval = b->val[X_ATOM];
b->et.code = b->s.code;
b->ef.code = -b->s.code;
}






static int
use_conflict(struct block *b, struct block *succ)
{
int atom;
atomset use = succ->out_use;

if (use == 0)
return 0;

for (atom = 0; atom < N_ATOMS; ++atom)
if (ATOMELEM(use, atom))
if (b->val[atom] != succ->val[atom])
return 1;
return 0;
}

static struct block *
fold_edge(struct block *child, struct edge *ep)
{
int sense;
int aval0, aval1, oval0, oval1;
int code = ep->code;

if (code < 0) {
code = -code;
sense = 0;
} else
sense = 1;

if (child->s.code != code)
return 0;

aval0 = child->val[A_ATOM];
oval0 = child->oval;
aval1 = ep->pred->val[A_ATOM];
oval1 = ep->pred->oval;

if (aval0 != aval1)
return 0;

if (oval0 == oval1)





return sense ? JT(child) : JF(child);

if (sense && code == (BPF_JMP|BPF_JEQ|BPF_K))














return JF(child);

return 0;
}

static void
opt_j(opt_state_t *opt_state, struct edge *ep)
{
register int i, k;
register struct block *target;

if (JT(ep->succ) == 0)
return;

if (JT(ep->succ) == JF(ep->succ)) {




if (!use_conflict(ep->pred, ep->succ->et.succ)) {
opt_state->done = 0;
ep->succ = JT(ep->succ);
}
}







top:
for (i = 0; i < opt_state->edgewords; ++i) {
register bpf_u_int32 x = ep->edom[i];

while (x != 0) {
k = lowest_set_bit(x);
x &=~ ((bpf_u_int32)1 << k);
k += i * BITS_PER_WORD;

target = fold_edge(ep->succ, opt_state->edges[k]);




if (target != 0 && !use_conflict(ep->pred, target)) {
opt_state->done = 0;
ep->succ = target;
if (JT(target) != 0)



goto top;
return;
}
}
}
}


static void
or_pullup(opt_state_t *opt_state, struct block *b)
{
int val, at_top;
struct block *pull;
struct block **diffp, **samep;
struct edge *ep;

ep = b->in_edges;
if (ep == 0)
return;





val = ep->pred->val[A_ATOM];
for (ep = ep->next; ep != 0; ep = ep->next)
if (val != ep->pred->val[A_ATOM])
return;

if (JT(b->in_edges->pred) == b)
diffp = &JT(b->in_edges->pred);
else
diffp = &JF(b->in_edges->pred);

at_top = 1;
for (;;) {
if (*diffp == 0)
return;

if (JT(*diffp) != JT(b))
return;

if (!SET_MEMBER((*diffp)->dom, b->id))
return;

if ((*diffp)->val[A_ATOM] != val)
break;

diffp = &JF(*diffp);
at_top = 0;
}
samep = &JF(*diffp);
for (;;) {
if (*samep == 0)
return;

if (JT(*samep) != JT(b))
return;

if (!SET_MEMBER((*samep)->dom, b->id))
return;

if ((*samep)->val[A_ATOM] == val)
break;




samep = &JF(*samep);
}
#if defined(notdef)

for (i = 0; i < N_ATOMS; ++i)
if ((*samep)->val[i] != pred->val[i])
return;
#endif

pull = *samep;
*samep = JF(pull);
JF(pull) = *diffp;






if (at_top) {
for (ep = b->in_edges; ep != 0; ep = ep->next) {
if (JT(ep->pred) == b)
JT(ep->pred) = pull;
else
JF(ep->pred) = pull;
}
}
else
*diffp = pull;

opt_state->done = 0;
}

static void
and_pullup(opt_state_t *opt_state, struct block *b)
{
int val, at_top;
struct block *pull;
struct block **diffp, **samep;
struct edge *ep;

ep = b->in_edges;
if (ep == 0)
return;




val = ep->pred->val[A_ATOM];
for (ep = ep->next; ep != 0; ep = ep->next)
if (val != ep->pred->val[A_ATOM])
return;

if (JT(b->in_edges->pred) == b)
diffp = &JT(b->in_edges->pred);
else
diffp = &JF(b->in_edges->pred);

at_top = 1;
for (;;) {
if (*diffp == 0)
return;

if (JF(*diffp) != JF(b))
return;

if (!SET_MEMBER((*diffp)->dom, b->id))
return;

if ((*diffp)->val[A_ATOM] != val)
break;

diffp = &JT(*diffp);
at_top = 0;
}
samep = &JT(*diffp);
for (;;) {
if (*samep == 0)
return;

if (JF(*samep) != JF(b))
return;

if (!SET_MEMBER((*samep)->dom, b->id))
return;

if ((*samep)->val[A_ATOM] == val)
break;




samep = &JT(*samep);
}
#if defined(notdef)

for (i = 0; i < N_ATOMS; ++i)
if ((*samep)->val[i] != pred->val[i])
return;
#endif

pull = *samep;
*samep = JT(pull);
JT(pull) = *diffp;






if (at_top) {
for (ep = b->in_edges; ep != 0; ep = ep->next) {
if (JT(ep->pred) == b)
JT(ep->pred) = pull;
else
JF(ep->pred) = pull;
}
}
else
*diffp = pull;

opt_state->done = 0;
}

static void
opt_blks(opt_state_t *opt_state, struct icode *ic, int do_stmts)
{
int i, maxlevel;
struct block *p;

init_val(opt_state);
maxlevel = ic->root->level;

find_inedges(opt_state, ic->root);
for (i = maxlevel; i >= 0; --i)
for (p = opt_state->levels[i]; p; p = p->link)
opt_blk(opt_state, p, do_stmts);

if (do_stmts)




return;

for (i = 1; i <= maxlevel; ++i) {
for (p = opt_state->levels[i]; p; p = p->link) {
opt_j(opt_state, &p->et);
opt_j(opt_state, &p->ef);
}
}

find_inedges(opt_state, ic->root);
for (i = 1; i <= maxlevel; ++i) {
for (p = opt_state->levels[i]; p; p = p->link) {
or_pullup(opt_state, p);
and_pullup(opt_state, p);
}
}
}

static inline void
link_inedge(struct edge *parent, struct block *child)
{
parent->next = child->in_edges;
child->in_edges = parent;
}

static void
find_inedges(opt_state_t *opt_state, struct block *root)
{
int i;
struct block *b;

for (i = 0; i < opt_state->n_blocks; ++i)
opt_state->blocks[i]->in_edges = 0;





for (i = root->level; i > 0; --i) {
for (b = opt_state->levels[i]; b != 0; b = b->link) {
link_inedge(&b->et, JT(b));
link_inedge(&b->ef, JF(b));
}
}
}

static void
opt_root(struct block **b)
{
struct slist *tmp, *s;

s = (*b)->stmts;
(*b)->stmts = 0;
while (BPF_CLASS((*b)->s.code) == BPF_JMP && JT(*b) == JF(*b))
*b = JT(*b);

tmp = (*b)->stmts;
if (tmp != 0)
sappend(s, tmp);
(*b)->stmts = s;






if (BPF_CLASS((*b)->s.code) == BPF_RET)
(*b)->stmts = 0;
}

static void
opt_loop(opt_state_t *opt_state, struct icode *ic, int do_stmts)
{

#if defined(BDEBUG)
if (pcap_optimizer_debug > 1 || pcap_print_dot_graph) {
printf("opt_loop(root, %d) begin\n", do_stmts);
opt_dump(opt_state, ic);
}
#endif
do {
opt_state->done = 1;
find_levels(opt_state, ic);
find_dom(opt_state, ic->root);
find_closure(opt_state, ic->root);
find_ud(opt_state, ic->root);
find_edom(opt_state, ic->root);
opt_blks(opt_state, ic, do_stmts);
#if defined(BDEBUG)
if (pcap_optimizer_debug > 1 || pcap_print_dot_graph) {
printf("opt_loop(root, %d) bottom, done=%d\n", do_stmts, opt_state->done);
opt_dump(opt_state, ic);
}
#endif
} while (!opt_state->done);
}





int
bpf_optimize(struct icode *ic, char *errbuf)
{
opt_state_t opt_state;

memset(&opt_state, 0, sizeof(opt_state));
opt_state.errbuf = errbuf;
if (setjmp(opt_state.top_ctx)) {
opt_cleanup(&opt_state);
return -1;
}
opt_init(&opt_state, ic);
opt_loop(&opt_state, ic, 0);
opt_loop(&opt_state, ic, 1);
intern_blocks(&opt_state, ic);
#if defined(BDEBUG)
if (pcap_optimizer_debug > 1 || pcap_print_dot_graph) {
printf("after intern_blocks()\n");
opt_dump(&opt_state, ic);
}
#endif
opt_root(&ic->root);
#if defined(BDEBUG)
if (pcap_optimizer_debug > 1 || pcap_print_dot_graph) {
printf("after opt_root()\n");
opt_dump(&opt_state, ic);
}
#endif
opt_cleanup(&opt_state);
return 0;
}

static void
make_marks(struct icode *ic, struct block *p)
{
if (!isMarked(ic, p)) {
Mark(ic, p);
if (BPF_CLASS(p->s.code) != BPF_RET) {
make_marks(ic, JT(p));
make_marks(ic, JF(p));
}
}
}





static void
mark_code(struct icode *ic)
{
ic->cur_mark += 1;
make_marks(ic, ic->root);
}





static int
eq_slist(struct slist *x, struct slist *y)
{
for (;;) {
while (x && x->s.code == NOP)
x = x->next;
while (y && y->s.code == NOP)
y = y->next;
if (x == 0)
return y == 0;
if (y == 0)
return x == 0;
if (x->s.code != y->s.code || x->s.k != y->s.k)
return 0;
x = x->next;
y = y->next;
}
}

static inline int
eq_blk(struct block *b0, struct block *b1)
{
if (b0->s.code == b1->s.code &&
b0->s.k == b1->s.k &&
b0->et.succ == b1->et.succ &&
b0->ef.succ == b1->ef.succ)
return eq_slist(b0->stmts, b1->stmts);
return 0;
}

static void
intern_blocks(opt_state_t *opt_state, struct icode *ic)
{
struct block *p;
int i, j;
int done1;
top:
done1 = 1;
for (i = 0; i < opt_state->n_blocks; ++i)
opt_state->blocks[i]->link = 0;

mark_code(ic);

for (i = opt_state->n_blocks - 1; --i >= 0; ) {
if (!isMarked(ic, opt_state->blocks[i]))
continue;
for (j = i + 1; j < opt_state->n_blocks; ++j) {
if (!isMarked(ic, opt_state->blocks[j]))
continue;
if (eq_blk(opt_state->blocks[i], opt_state->blocks[j])) {
opt_state->blocks[i]->link = opt_state->blocks[j]->link ?
opt_state->blocks[j]->link : opt_state->blocks[j];
break;
}
}
}
for (i = 0; i < opt_state->n_blocks; ++i) {
p = opt_state->blocks[i];
if (JT(p) == 0)
continue;
if (JT(p)->link) {
done1 = 0;
JT(p) = JT(p)->link;
}
if (JF(p)->link) {
done1 = 0;
JF(p) = JF(p)->link;
}
}
if (!done1)
goto top;
}

static void
opt_cleanup(opt_state_t *opt_state)
{
free((void *)opt_state->vnode_base);
free((void *)opt_state->vmap);
free((void *)opt_state->edges);
free((void *)opt_state->space);
free((void *)opt_state->levels);
free((void *)opt_state->blocks);
}




static void PCAP_NORETURN
opt_error(opt_state_t *opt_state, const char *fmt, ...)
{
va_list ap;

if (opt_state->errbuf != NULL) {
va_start(ap, fmt);
(void)pcap_vsnprintf(opt_state->errbuf,
PCAP_ERRBUF_SIZE, fmt, ap);
va_end(ap);
}
longjmp(opt_state->top_ctx, 1);

}




static u_int
slength(struct slist *s)
{
u_int n = 0;

for (; s; s = s->next)
if (s->s.code != NOP)
++n;
return n;
}





static int
count_blocks(struct icode *ic, struct block *p)
{
if (p == 0 || isMarked(ic, p))
return 0;
Mark(ic, p);
return count_blocks(ic, JT(p)) + count_blocks(ic, JF(p)) + 1;
}





static void
number_blks_r(opt_state_t *opt_state, struct icode *ic, struct block *p)
{
int n;

if (p == 0 || isMarked(ic, p))
return;

Mark(ic, p);
n = opt_state->n_blocks++;
p->id = n;
opt_state->blocks[n] = p;

number_blks_r(opt_state, ic, JT(p));
number_blks_r(opt_state, ic, JF(p));
}



















static u_int
count_stmts(struct icode *ic, struct block *p)
{
u_int n;

if (p == 0 || isMarked(ic, p))
return 0;
Mark(ic, p);
n = count_stmts(ic, JT(p)) + count_stmts(ic, JF(p));
return slength(p->stmts) + n + 1 + p->longjt + p->longjf;
}






static void
opt_init(opt_state_t *opt_state, struct icode *ic)
{
bpf_u_int32 *p;
int i, n, max_stmts;





unMarkAll(ic);
n = count_blocks(ic, ic->root);
opt_state->blocks = (struct block **)calloc(n, sizeof(*opt_state->blocks));
if (opt_state->blocks == NULL)
opt_error(opt_state, "malloc");
unMarkAll(ic);
opt_state->n_blocks = 0;
number_blks_r(opt_state, ic, ic->root);

opt_state->n_edges = 2 * opt_state->n_blocks;
opt_state->edges = (struct edge **)calloc(opt_state->n_edges, sizeof(*opt_state->edges));
if (opt_state->edges == NULL) {
opt_error(opt_state, "malloc");
}




opt_state->levels = (struct block **)calloc(opt_state->n_blocks, sizeof(*opt_state->levels));
if (opt_state->levels == NULL) {
opt_error(opt_state, "malloc");
}

opt_state->edgewords = opt_state->n_edges / (8 * sizeof(bpf_u_int32)) + 1;
opt_state->nodewords = opt_state->n_blocks / (8 * sizeof(bpf_u_int32)) + 1;


opt_state->space = (bpf_u_int32 *)malloc(2 * opt_state->n_blocks * opt_state->nodewords * sizeof(*opt_state->space)
+ opt_state->n_edges * opt_state->edgewords * sizeof(*opt_state->space));
if (opt_state->space == NULL) {
opt_error(opt_state, "malloc");
}
p = opt_state->space;
opt_state->all_dom_sets = p;
for (i = 0; i < n; ++i) {
opt_state->blocks[i]->dom = p;
p += opt_state->nodewords;
}
opt_state->all_closure_sets = p;
for (i = 0; i < n; ++i) {
opt_state->blocks[i]->closure = p;
p += opt_state->nodewords;
}
opt_state->all_edge_sets = p;
for (i = 0; i < n; ++i) {
register struct block *b = opt_state->blocks[i];

b->et.edom = p;
p += opt_state->edgewords;
b->ef.edom = p;
p += opt_state->edgewords;
b->et.id = i;
opt_state->edges[i] = &b->et;
b->ef.id = opt_state->n_blocks + i;
opt_state->edges[opt_state->n_blocks + i] = &b->ef;
b->et.pred = b;
b->ef.pred = b;
}
max_stmts = 0;
for (i = 0; i < n; ++i)
max_stmts += slength(opt_state->blocks[i]->stmts) + 1;





opt_state->maxval = 3 * max_stmts;
opt_state->vmap = (struct vmapinfo *)calloc(opt_state->maxval, sizeof(*opt_state->vmap));
if (opt_state->vmap == NULL) {
opt_error(opt_state, "malloc");
}
opt_state->vnode_base = (struct valnode *)calloc(opt_state->maxval, sizeof(*opt_state->vnode_base));
if (opt_state->vnode_base == NULL) {
opt_error(opt_state, "malloc");
}
}






#if defined(BDEBUG)
int bids[NBIDS];
#endif

static void PCAP_NORETURN conv_error(conv_state_t *, const char *, ...)
PCAP_PRINTFLIKE(2, 3);







static int
convert_code_r(conv_state_t *conv_state, struct icode *ic, struct block *p)
{
struct bpf_insn *dst;
struct slist *src;
u_int slen;
u_int off;
u_int extrajmps;
struct slist **offset = NULL;

if (p == 0 || isMarked(ic, p))
return (1);
Mark(ic, p);

if (convert_code_r(conv_state, ic, JF(p)) == 0)
return (0);
if (convert_code_r(conv_state, ic, JT(p)) == 0)
return (0);

slen = slength(p->stmts);
dst = conv_state->ftail -= (slen + 1 + p->longjt + p->longjf);


p->offset = (int)(dst - conv_state->fstart);


if (slen) {
offset = (struct slist **)calloc(slen, sizeof(struct slist *));
if (!offset) {
conv_error(conv_state, "not enough core");

}
}
src = p->stmts;
for (off = 0; off < slen && src; off++) {
#if 0
printf("off=%d src=%x\n", off, src);
#endif
offset[off] = src;
src = src->next;
}

off = 0;
for (src = p->stmts; src; src = src->next) {
if (src->s.code == NOP)
continue;
dst->code = (u_short)src->s.code;
dst->k = src->s.k;


if (BPF_CLASS(src->s.code) != BPF_JMP || src->s.code == (BPF_JMP|BPF_JA)) {
#if 0
if (src->s.jt || src->s.jf) {
free(offset);
conv_error(conv_state, "illegal jmp destination");

}
#endif
goto filled;
}
if (off == slen - 2)
goto filled;

{
u_int i;
int jt, jf;
const char ljerr[] = "%s for block-local relative jump: off=%d";

#if 0
printf("code=%x off=%d %x %x\n", src->s.code,
off, src->s.jt, src->s.jf);
#endif

if (!src->s.jt || !src->s.jf) {
free(offset);
conv_error(conv_state, ljerr, "no jmp destination", off);

}

jt = jf = 0;
for (i = 0; i < slen; i++) {
if (offset[i] == src->s.jt) {
if (jt) {
free(offset);
conv_error(conv_state, ljerr, "multiple matches", off);

}

if (i - off - 1 >= 256) {
free(offset);
conv_error(conv_state, ljerr, "out-of-range jump", off);

}
dst->jt = (u_char)(i - off - 1);
jt++;
}
if (offset[i] == src->s.jf) {
if (jf) {
free(offset);
conv_error(conv_state, ljerr, "multiple matches", off);

}
if (i - off - 1 >= 256) {
free(offset);
conv_error(conv_state, ljerr, "out-of-range jump", off);

}
dst->jf = (u_char)(i - off - 1);
jf++;
}
}
if (!jt || !jf) {
free(offset);
conv_error(conv_state, ljerr, "no destination found", off);

}
}
filled:
++dst;
++off;
}
if (offset)
free(offset);

#if defined(BDEBUG)
if (dst - conv_state->fstart < NBIDS)
bids[dst - conv_state->fstart] = p->id + 1;
#endif
dst->code = (u_short)p->s.code;
dst->k = p->s.k;
if (JT(p)) {
extrajmps = 0;
off = JT(p)->offset - (p->offset + slen) - 1;
if (off >= 256) {

if (p->longjt == 0) {

p->longjt++;
return(0);
}

if (extrajmps >= 256) {
conv_error(conv_state, "too many extra jumps");

}
dst->jt = (u_char)extrajmps;
extrajmps++;
dst[extrajmps].code = BPF_JMP|BPF_JA;
dst[extrajmps].k = off - extrajmps;
}
else
dst->jt = (u_char)off;
off = JF(p)->offset - (p->offset + slen) - 1;
if (off >= 256) {

if (p->longjf == 0) {

p->longjf++;
return(0);
}


if (extrajmps >= 256) {
conv_error(conv_state, "too many extra jumps");

}
dst->jf = (u_char)extrajmps;
extrajmps++;
dst[extrajmps].code = BPF_JMP|BPF_JA;
dst[extrajmps].k = off - extrajmps;
}
else
dst->jf = (u_char)off;
}
return (1);
}




















struct bpf_insn *
icode_to_fcode(struct icode *ic, struct block *root, u_int *lenp,
char *errbuf)
{
u_int n;
struct bpf_insn *fp;
conv_state_t conv_state;

conv_state.fstart = NULL;
conv_state.errbuf = errbuf;
if (setjmp(conv_state.top_ctx) != 0) {
free(conv_state.fstart);
return NULL;
}





for (;;) {
unMarkAll(ic);
n = *lenp = count_stmts(ic, root);

fp = (struct bpf_insn *)malloc(sizeof(*fp) * n);
if (fp == NULL) {
(void)pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"malloc");
free(fp);
return NULL;
}
memset((char *)fp, 0, sizeof(*fp) * n);
conv_state.fstart = fp;
conv_state.ftail = fp + n;

unMarkAll(ic);
if (convert_code_r(&conv_state, ic, root))
break;
free(fp);
}

return fp;
}




static void PCAP_NORETURN
conv_error(conv_state_t *conv_state, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
(void)pcap_vsnprintf(conv_state->errbuf,
PCAP_ERRBUF_SIZE, fmt, ap);
va_end(ap);
longjmp(conv_state->top_ctx, 1);

}









int
install_bpf_program(pcap_t *p, struct bpf_program *fp)
{
size_t prog_size;




if (!bpf_validate(fp->bf_insns, fp->bf_len)) {
pcap_snprintf(p->errbuf, sizeof(p->errbuf),
"BPF program is not valid");
return (-1);
}




pcap_freecode(&p->fcode);

prog_size = sizeof(*fp->bf_insns) * fp->bf_len;
p->fcode.bf_len = fp->bf_len;
p->fcode.bf_insns = (struct bpf_insn *)malloc(prog_size);
if (p->fcode.bf_insns == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "malloc");
return (-1);
}
memcpy(p->fcode.bf_insns, fp->bf_insns, prog_size);
return (0);
}

#if defined(BDEBUG)
static void
dot_dump_node(struct icode *ic, struct block *block, struct bpf_program *prog,
FILE *out)
{
int icount, noffset;
int i;

if (block == NULL || isMarked(ic, block))
return;
Mark(ic, block);

icount = slength(block->stmts) + 1 + block->longjt + block->longjf;
noffset = min(block->offset + icount, (int)prog->bf_len);

fprintf(out, "\tblock%d [shape=ellipse, id=\"block-%d\" label=\"BLOCK%d\\n", block->id, block->id, block->id);
for (i = block->offset; i < noffset; i++) {
fprintf(out, "\\n%s", bpf_image(prog->bf_insns + i, i));
}
fprintf(out, "\" tooltip=\"");
for (i = 0; i < BPF_MEMWORDS; i++)
if (block->val[i] != VAL_UNKNOWN)
fprintf(out, "val[%d]=%d ", i, block->val[i]);
fprintf(out, "val[A]=%d ", block->val[A_ATOM]);
fprintf(out, "val[X]=%d", block->val[X_ATOM]);
fprintf(out, "\"");
if (JT(block) == NULL)
fprintf(out, ", peripheries=2");
fprintf(out, "];\n");

dot_dump_node(ic, JT(block), prog, out);
dot_dump_node(ic, JF(block), prog, out);
}

static void
dot_dump_edge(struct icode *ic, struct block *block, FILE *out)
{
if (block == NULL || isMarked(ic, block))
return;
Mark(ic, block);

if (JT(block)) {
fprintf(out, "\t\"block%d\":se -> \"block%d\":n [label=\"T\"]; \n",
block->id, JT(block)->id);
fprintf(out, "\t\"block%d\":sw -> \"block%d\":n [label=\"F\"]; \n",
block->id, JF(block)->id);
}
dot_dump_edge(ic, JT(block), out);
dot_dump_edge(ic, JF(block), out);
}




















static int
dot_dump(struct icode *ic, char *errbuf)
{
struct bpf_program f;
FILE *out = stdout;

memset(bids, 0, sizeof bids);
f.bf_insns = icode_to_fcode(ic, ic->root, &f.bf_len, errbuf);
if (f.bf_insns == NULL)
return -1;

fprintf(out, "digraph BPF {\n");
unMarkAll(ic);
dot_dump_node(ic, ic->root, &f, out);
unMarkAll(ic);
dot_dump_edge(ic, ic->root, out);
fprintf(out, "}\n");

free((char *)f.bf_insns);
return 0;
}

static int
plain_dump(struct icode *ic, char *errbuf)
{
struct bpf_program f;

memset(bids, 0, sizeof bids);
f.bf_insns = icode_to_fcode(ic, ic->root, &f.bf_len, errbuf);
if (f.bf_insns == NULL)
return -1;
bpf_dump(&f, 1);
putchar('\n');
free((char *)f.bf_insns);
return 0;
}

static void
opt_dump(opt_state_t *opt_state, struct icode *ic)
{
int status;
char errbuf[PCAP_ERRBUF_SIZE];





if (pcap_print_dot_graph)
status = dot_dump(ic, errbuf);
else
status = plain_dump(ic, errbuf);
if (status == -1)
opt_error(opt_state, "opt_dump: icode_to_fcode failed: %s", errbuf);
}
#endif
