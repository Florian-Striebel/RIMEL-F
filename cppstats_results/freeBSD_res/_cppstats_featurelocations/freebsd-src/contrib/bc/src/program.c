


































#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <setjmp.h>

#include <signal.h>

#include <time.h>

#include <read.h>
#include <parse.h>
#include <program.h>
#include <vm.h>







static inline void bc_program_setVecs(BcProgram *p, BcFunc *f) {
BC_SIG_ASSERT_LOCKED;
p->consts = &f->consts;
p->strs = &f->strs;
}






static inline void bc_program_type_num(BcResult *r, BcNum *n) {

#if BC_ENABLED


assert(r->t != BC_RESULT_VOID);

#endif

if (BC_ERR(!BC_PROG_NUM(r, n))) bc_err(BC_ERR_EXEC_TYPE);
}

#if BC_ENABLED






static void bc_program_type_match(BcResult *r, BcType t) {
if (BC_ERR((r->t != BC_RESULT_ARRAY) != (!t))) bc_err(BC_ERR_EXEC_TYPE);
}
#endif










static size_t bc_program_index(const char *restrict code, size_t *restrict bgn)
{
uchar amt = (uchar) code[(*bgn)++], i = 0;
size_t res = 0;

for (; i < amt; ++i, ++(*bgn)) {
size_t temp = ((size_t) ((int) (uchar) code[*bgn]) & UCHAR_MAX);
res |= (temp << (i * CHAR_BIT));
}

return res;
}







static char* bc_program_string(BcProgram *p, const BcNum *n) {
BcFunc *f = bc_vec_item(&p->fns, n->rdx);
return *((char**) bc_vec_item(&f->strs, n->scale));
}

#if BC_ENABLED







static void bc_program_prepGlobals(BcProgram *p) {

size_t i;

for (i = 0; i < BC_PROG_GLOBALS_LEN; ++i)
bc_vec_push(p->globals_v + i, p->globals + i);

#if BC_ENABLE_EXTRA_MATH
bc_rand_push(&p->rng);
#endif
}








static void bc_program_popGlobals(BcProgram *p, bool reset) {

size_t i;

BC_SIG_ASSERT_LOCKED;

for (i = 0; i < BC_PROG_GLOBALS_LEN; ++i) {
BcVec *v = p->globals_v + i;
bc_vec_npop(v, reset ? v->len - 1 : 1);
p->globals[i] = BC_PROG_GLOBAL(v);
}

#if BC_ENABLE_EXTRA_MATH
bc_rand_pop(&p->rng, reset);
#endif
}







static BcVec* bc_program_dereference(const BcProgram *p, BcVec *vec) {

BcVec *v;
size_t vidx, nidx, i = 0;


assert(vec->size == sizeof(uchar));



vidx = bc_program_index(vec->v, &i);
nidx = bc_program_index(vec->v, &i);

v = bc_vec_item(bc_vec_item(&p->arrs, vidx), nidx);


assert(v->size != sizeof(uchar));

return v;
}
#endif








static void bc_program_pushBigdig(BcProgram *p, BcBigDig dig, BcResultType type)
{
BcResult res;

res.t = type;

BC_SIG_LOCK;

bc_num_createFromBigdig(&res.d.n, dig);
bc_vec_push(&p->results, &res);

BC_SIG_UNLOCK;
}

size_t bc_program_addString(BcProgram *p, const char *str, size_t fidx) {

BcFunc *f;
char **str_ptr;
BcVec *slabs;

BC_SIG_ASSERT_LOCKED;


f = bc_vec_item(&p->fns, fidx);
str_ptr = bc_vec_pushEmpty(&f->strs);


slabs = fidx == BC_PROG_MAIN || fidx == BC_PROG_READ ?
&vm.main_slabs : &vm.other_slabs;

*str_ptr = bc_slabvec_strdup(slabs, str);

return f->strs.len - 1;
}

size_t bc_program_search(BcProgram *p, const char *id, bool var) {

BcVec *v, *map;
size_t i;

BC_SIG_ASSERT_LOCKED;


v = var ? &p->vars : &p->arrs;
map = var ? &p->var_map : &p->arr_map;





if (bc_map_insert(map, id, v->len, &i)) {
BcVec *temp = bc_vec_pushEmpty(v);
bc_array_init(temp, var);
}

return ((BcId*) bc_vec_item(map, i))->idx;
}









static inline BcVec* bc_program_vec(const BcProgram *p, size_t idx, BcType type)
{
const BcVec *v = (type == BC_TYPE_VAR) ? &p->vars : &p->arrs;
return bc_vec_item(v, idx);
}












static BcNum* bc_program_num(BcProgram *p, BcResult *r) {

BcNum *n;

#if defined(_WIN32)




n = NULL;
#endif

switch (r->t) {

case BC_RESULT_STR:
case BC_RESULT_TEMP:
case BC_RESULT_IBASE:
case BC_RESULT_SCALE:
case BC_RESULT_OBASE:
#if BC_ENABLE_EXTRA_MATH
case BC_RESULT_SEED:
#endif
{
n = &r->d.n;
break;
}

case BC_RESULT_VAR:
case BC_RESULT_ARRAY:
case BC_RESULT_ARRAY_ELEM:
{
BcVec *v;
BcType type = (r->t == BC_RESULT_VAR) ? BC_TYPE_VAR : BC_TYPE_ARRAY;


v = bc_program_vec(p, r->d.loc.loc, type);





if (r->t == BC_RESULT_ARRAY_ELEM) {

size_t idx = r->d.loc.idx;

v = bc_vec_top(v);

#if BC_ENABLED






if (v->size == sizeof(uchar)) v = bc_program_dereference(p, v);
#endif


assert(v->size == sizeof(BcNum));






if (v->len <= idx) {
BC_SIG_LOCK;
bc_array_expand(v, bc_vm_growSize(idx, 1));
BC_SIG_UNLOCK;
}

n = bc_vec_item(v, idx);
}


else n = bc_vec_top(v);

break;
}

case BC_RESULT_ZERO:
{
n = &vm.zero;
break;
}

case BC_RESULT_ONE:
{
n = &vm.one;
break;
}

#if BC_ENABLED


case BC_RESULT_VOID:
#if !defined(NDEBUG)
{
abort();
}
#endif

case BC_RESULT_LAST:
{
n = &p->last;
break;
}
#endif
}

return n;
}










static void bc_program_operand(BcProgram *p, BcResult **r,
BcNum **n, size_t idx)
{
*r = bc_vec_item_rev(&p->results, idx);

#if BC_ENABLED
if (BC_ERR((*r)->t == BC_RESULT_VOID)) bc_err(BC_ERR_EXEC_VOID_VAL);
#endif

*n = bc_program_num(p, *r);
}















static void bc_program_binPrep(BcProgram *p, BcResult **l, BcNum **ln,
BcResult **r, BcNum **rn, size_t idx)
{
BcResultType lt;

assert(p != NULL && l != NULL && ln != NULL && r != NULL && rn != NULL);

#if !defined(BC_PROG_NO_STACK_CHECK)

if (BC_IS_DC) {
if (BC_ERR(!BC_PROG_STACK(&p->results, idx + 2)))
bc_err(BC_ERR_EXEC_STACK);
}
#endif

assert(BC_PROG_STACK(&p->results, idx + 2));


bc_program_operand(p, l, ln, idx + 1);
bc_program_operand(p, r, rn, idx);

lt = (*l)->t;

#if BC_ENABLED

assert(lt != BC_RESULT_VOID && (*r)->t != BC_RESULT_VOID);
#endif




if (lt == (*r)->t && (lt == BC_RESULT_VAR || lt == BC_RESULT_ARRAY_ELEM))
*ln = bc_program_num(p, *l);

if (BC_ERR(lt == BC_RESULT_STR)) bc_err(BC_ERR_EXEC_TYPE);
}

















static void bc_program_binOpPrep(BcProgram *p, BcResult **l, BcNum **ln,
BcResult **r, BcNum **rn, size_t idx)
{
bc_program_binPrep(p, l, ln, r, rn, idx);
bc_program_type_num(*l, *ln);
bc_program_type_num(*r, *rn);
}













static void bc_program_assignPrep(BcProgram *p, BcResult **l, BcNum **ln,
BcResult **r, BcNum **rn)
{
BcResultType lt, min;


min = BC_RESULT_TEMP - ((unsigned int) (BC_IS_BC));


bc_program_binPrep(p, l, ln, r, rn, 0);

lt = (*l)->t;


if (BC_ERR(lt >= min && lt <= BC_RESULT_ONE)) bc_err(BC_ERR_EXEC_TYPE);



bool good = ((*r)->t == BC_RESULT_STR && lt <= BC_RESULT_ARRAY_ELEM);

assert(BC_PROG_STR(*rn) || (*r)->t != BC_RESULT_STR);


if (!good) bc_program_type_num(*r, *rn);
}











static void bc_program_prep(BcProgram *p, BcResult **r, BcNum **n, size_t idx) {

assert(p != NULL && r != NULL && n != NULL);

#if !defined(BC_PROG_NO_STACK_CHECK)

if (BC_IS_DC) {
if (BC_ERR(!BC_PROG_STACK(&p->results, idx + 1)))
bc_err(BC_ERR_EXEC_STACK);
}
#endif

assert(BC_PROG_STACK(&p->results, idx + 1));

bc_program_operand(p, r, n, idx);


bc_program_type_num(*r, *n);
}






static BcResult* bc_program_prepResult(BcProgram *p) {

BcResult *res = bc_vec_pushEmpty(&p->results);

bc_result_clear(res);

return res;
}










static void bc_program_const(BcProgram *p, const char *code, size_t *bgn) {



BcResult *r = bc_program_prepResult(p);
BcConst *c = bc_vec_item(p->consts, bc_program_index(code, bgn));
BcBigDig base = BC_PROG_IBASE(p);


if (c->base != base) {


if (c->num.num == NULL) {
BC_SIG_LOCK;
bc_num_init(&c->num, BC_NUM_RDX(strlen(c->val)));
BC_SIG_UNLOCK;
}


bc_num_parse(&c->num, c->val, base);

c->base = base;
}

BC_SIG_LOCK;

bc_num_createCopy(&r->d.n, &c->num);

BC_SIG_UNLOCK;
}






static void bc_program_op(BcProgram *p, uchar inst) {

BcResult *opd1, *opd2, *res;
BcNum *n1, *n2;
size_t idx = inst - BC_INST_POWER;

res = bc_program_prepResult(p);

bc_program_binOpPrep(p, &opd1, &n1, &opd2, &n2, 1);

BC_SIG_LOCK;




bc_num_init(&res->d.n, bc_program_opReqs[idx](n1, n2, BC_PROG_SCALE(p)));

BC_SIG_UNLOCK;

assert(BC_NUM_RDX_VALID(n1));
assert(BC_NUM_RDX_VALID(n2));


bc_program_ops[idx](n1, n2, &res->d.n, BC_PROG_SCALE(p));

bc_program_retire(p, 1, 2);
}





static void bc_program_read(BcProgram *p) {

BcStatus s;
BcInstPtr ip;
size_t i;
const char* file;
bool is_stdin;
BcFunc *f = bc_vec_item(&p->fns, BC_PROG_READ);



for (i = 0; i < p->stack.len; ++i) {
BcInstPtr *ip_ptr = bc_vec_item(&p->stack, i);
if (ip_ptr->func == BC_PROG_READ) bc_err(BC_ERR_EXEC_REC_READ);
}

BC_SIG_LOCK;


file = vm.file;
is_stdin = vm.is_stdin;



vm.is_stdin = false;

if (!BC_PARSE_IS_INITED(&vm.read_prs, p)) {






bc_parse_init(&vm.read_prs, p, BC_PROG_READ);



bc_vec_init(&vm.read_buf, sizeof(char), BC_DTOR_NONE);
}


else bc_parse_updateFunc(&vm.read_prs, BC_PROG_READ);

BC_SETJMP_LOCKED(exec_err);

BC_SIG_UNLOCK;


bc_lex_file(&vm.read_prs.l, bc_program_stdin_name);
bc_vec_popAll(&f->code);


if (!BC_R) s = bc_read_line(&vm.read_buf, "");
else s = bc_read_line(&vm.read_buf, BC_IS_BC ? "read> " : "?> ");


if (s == BC_STATUS_EOF) bc_err(BC_ERR_EXEC_READ_EXPR);


bc_parse_text(&vm.read_prs, vm.read_buf.v, false);
BC_SIG_LOCK;
vm.expr(&vm.read_prs, BC_PARSE_NOREAD | BC_PARSE_NEEDVAL);
BC_SIG_UNLOCK;



if (BC_ERR(vm.read_prs.l.t != BC_LEX_NLINE &&
vm.read_prs.l.t != BC_LEX_EOF))
{
bc_err(BC_ERR_EXEC_READ_EXPR);
}

#if BC_ENABLED

if (BC_G) bc_program_prepGlobals(p);
#endif


ip.func = BC_PROG_READ;
ip.idx = 0;
ip.len = p->results.len;


f = bc_vec_item(&p->fns, BC_PROG_READ);


bc_vec_pushByte(&f->code, vm.read_ret);


BC_SIG_LOCK;
bc_vec_push(&p->stack, &ip);

#if DC_ENABLED

if (BC_IS_DC) {
size_t temp = 0;
bc_vec_push(&p->tail_calls, &temp);
}
#endif

exec_err:
BC_SIG_MAYLOCK;
vm.is_stdin = is_stdin;
vm.file = file;
BC_LONGJMP_CONT;
}

#if BC_ENABLE_EXTRA_MATH





static void bc_program_rand(BcProgram *p) {

BcRand rand = bc_rand_int(&p->rng);

bc_program_pushBigdig(p, (BcBigDig) rand, BC_RESULT_TEMP);

#if !defined(NDEBUG)


{
BcResult *r = bc_vec_top(&p->results);
assert(BC_NUM_RDX_VALID_NP(r->d.n));
}
#endif
}
#endif





static void bc_program_printChars(const char *str) {

const char *nl;
size_t len = vm.nchars + strlen(str);
sig_atomic_t lock;

BC_SIG_TRYLOCK(lock);

bc_file_puts(&vm.fout, bc_flush_save, str);



nl = strrchr(str, '\n');

if (nl != NULL) len = strlen(nl + 1);

vm.nchars = len > UINT16_MAX ? UINT16_MAX : (uint16_t) len;

BC_SIG_TRYUNLOCK(lock);
}





static void bc_program_printString(const char *restrict str) {

size_t i, len = strlen(str);

#if DC_ENABLED

if (!len && BC_IS_DC) {
bc_vm_putchar('\0', bc_flush_save);
return;
}
#endif


for (i = 0; i < len; ++i) {

int c = str[i];


if (c == '\\' && i != len - 1) {

const char *ptr;


c = str[++i];
ptr = strchr(bc_program_esc_chars, c);


if (ptr != NULL) {


if (c == 'n') {
BC_SIG_LOCK;
vm.nchars = UINT16_MAX;
BC_SIG_UNLOCK;
}


c = bc_program_esc_seqs[(size_t) (ptr - bc_program_esc_chars)];
}
else {



bc_vm_putchar('\\', bc_flush_save);
}
}

bc_vm_putchar(c, bc_flush_save);
}
}







static void bc_program_print(BcProgram *p, uchar inst, size_t idx) {

BcResult *r;
char *str;
BcNum *n;
bool pop = (inst != BC_INST_PRINT);

assert(p != NULL);

#if !defined(BC_PROG_NO_STACK_CHECK)
if (BC_IS_DC) {
if (BC_ERR(!BC_PROG_STACK(&p->results, idx + 1)))
bc_err(BC_ERR_EXEC_STACK);
}
#endif

assert(BC_PROG_STACK(&p->results, idx + 1));

r = bc_vec_item_rev(&p->results, idx);

#if BC_ENABLED




if (r->t == BC_RESULT_VOID) {
if (BC_ERR(pop)) bc_err(BC_ERR_EXEC_VOID_VAL);
bc_vec_pop(&p->results);
return;
}
#endif

n = bc_program_num(p, r);


if (BC_PROG_NUM(r, n)) {

#if BC_ENABLED
assert(inst != BC_INST_PRINT_STR);
#endif


bc_num_print(n, BC_PROG_OBASE(p), !pop);

#if BC_ENABLED

if (BC_IS_BC) bc_num_copy(&p->last, n);
#endif
}
else {


bc_file_flush(&vm.fout, bc_flush_save);
str = bc_program_string(p, n);

#if BC_ENABLED
if (inst == BC_INST_PRINT_STR) bc_program_printChars(str);
else
#endif
{
bc_program_printString(str);


if (inst == BC_INST_PRINT)
bc_vm_putchar('\n', bc_flush_err);
}
}


if (BC_IS_BC || pop) bc_vec_pop(&p->results);
}

void bc_program_negate(BcResult *r, BcNum *n) {
bc_num_copy(&r->d.n, n);
if (BC_NUM_NONZERO(&r->d.n)) BC_NUM_NEG_TGL_NP(r->d.n);
}

void bc_program_not(BcResult *r, BcNum *n) {
if (!bc_num_cmpZero(n)) bc_num_one(&r->d.n);
}

#if BC_ENABLE_EXTRA_MATH
void bc_program_trunc(BcResult *r, BcNum *n) {
bc_num_copy(&r->d.n, n);
bc_num_truncate(&r->d.n, n->scale);
}
#endif






static void bc_program_unary(BcProgram *p, uchar inst) {

BcResult *res, *ptr;
BcNum *num;

res = bc_program_prepResult(p);

bc_program_prep(p, &ptr, &num, 1);

BC_SIG_LOCK;

bc_num_init(&res->d.n, num->len);

BC_SIG_UNLOCK;


bc_program_unarys[inst - BC_INST_NEG](res, num);
bc_program_retire(p, 1, 1);
}






static void bc_program_logical(BcProgram *p, uchar inst) {

BcResult *opd1, *opd2, *res;
BcNum *n1, *n2;
bool cond = 0;
ssize_t cmp;

res = bc_program_prepResult(p);



bc_program_binOpPrep(p, &opd1, &n1, &opd2, &n2, 1);



if (inst == BC_INST_BOOL_AND)
cond = (bc_num_cmpZero(n1) && bc_num_cmpZero(n2));
else if (inst == BC_INST_BOOL_OR)
cond = (bc_num_cmpZero(n1) || bc_num_cmpZero(n2));
else {


cmp = bc_num_cmp(n1, n2);

switch (inst) {

case BC_INST_REL_EQ:
{
cond = (cmp == 0);
break;
}

case BC_INST_REL_LE:
{
cond = (cmp <= 0);
break;
}

case BC_INST_REL_GE:
{
cond = (cmp >= 0);
break;
}

case BC_INST_REL_NE:
{
cond = (cmp != 0);
break;
}

case BC_INST_REL_LT:
{
cond = (cmp < 0);
break;
}

case BC_INST_REL_GT:
{
cond = (cmp > 0);
break;
}
#if !defined(NDEBUG)
default:
{

abort();
}
#endif
}
}

BC_SIG_LOCK;

bc_num_init(&res->d.n, BC_NUM_DEF_SIZE);

BC_SIG_UNLOCK;

if (cond) bc_num_one(&res->d.n);

bc_program_retire(p, 1, 2);
}










static void bc_program_assignStr(BcProgram *p, BcNum *num, BcVec *v, bool push)
{
BcNum *n;

assert(BC_PROG_STACK(&p->results, 1 + !push));
assert(num != NULL && num->num == NULL && num->cap == 0);



if (!push) bc_vec_pop(v);

bc_vec_npop(&p->results, 1 + !push);

n = bc_vec_pushEmpty(v);


memcpy(n, num, sizeof(BcNum));
}














static void bc_program_copyToVar(BcProgram *p, size_t idx, BcType t, bool last)
{
BcResult *ptr = NULL, r;
BcVec *vec;
BcNum *n = NULL;
bool var = (t == BC_TYPE_VAR);

#if DC_ENABLED

if (BC_IS_DC) {
if (BC_ERR(!BC_PROG_STACK(&p->results, 1))) bc_err(BC_ERR_EXEC_STACK);
}
#endif

assert(BC_PROG_STACK(&p->results, 1));

bc_program_operand(p, &ptr, &n, 0);

#if BC_ENABLED

if (BC_IS_BC)
{

bc_program_type_match(ptr, t);



if (!last && var)
n = bc_vec_item_rev(bc_program_vec(p, ptr->d.loc.loc, t), 1);
}
#endif

vec = bc_program_vec(p, idx, t);



if (ptr->t == BC_RESULT_STR) {

assert(BC_PROG_STR(n));

if (BC_ERR(!var)) bc_err(BC_ERR_EXEC_TYPE);

bc_program_assignStr(p, n, vec, true);

return;
}

BC_SIG_LOCK;


if (var) {
if (BC_PROG_STR(n)) memcpy(&r.d.n, n, sizeof(BcNum));
else bc_num_createCopy(&r.d.n, n);
}
else {



BcVec *v = (BcVec*) n, *rv = &r.d.v;

#if BC_ENABLED

if (BC_IS_BC) {

BcVec *parent;
bool ref, ref_size;




parent = bc_program_vec(p, ptr->d.loc.loc, t);
assert(parent != NULL);


if (!last) v = bc_vec_item_rev(parent, !last);
assert(v != NULL);


ref = (v->size == sizeof(BcNum) && t == BC_TYPE_REF);






ref_size = (v->size == sizeof(uchar));


if (ref || (ref_size && t == BC_TYPE_REF)) {


bc_vec_init(rv, sizeof(uchar), BC_DTOR_NONE);


if (ref) {

assert(parent->len >= (size_t) (!last + 1));


vec = bc_program_vec(p, idx, t);




bc_vec_pushIndex(rv, ptr->d.loc.loc);
bc_vec_pushIndex(rv, parent->len - !last - 1);
}


else bc_vec_npush(rv, v->len * sizeof(uchar), v->v);



bc_vec_push(vec, &r.d);
bc_vec_pop(&p->results);



BC_SIG_UNLOCK;
return;
}


else if (ref_size && t != BC_TYPE_REF)
v = bc_program_dereference(p, v);
}
#endif



bc_array_init(rv, true);
bc_array_copy(rv, v);
}


bc_vec_push(vec, &r.d);
bc_vec_pop(&p->results);

BC_SIG_UNLOCK;
}






static void bc_program_assign(BcProgram *p, uchar inst) {


BcResult *left, *right, res;
BcNum *l, *r;
bool ob, sc, use_val = BC_INST_USE_VAL(inst);

bc_program_assignPrep(p, &left, &l, &right, &r);


assert(left->t != BC_RESULT_STR);


if (right->t == BC_RESULT_STR) {

assert(BC_PROG_STR(r));

#if BC_ENABLED
if (inst != BC_INST_ASSIGN && inst != BC_INST_ASSIGN_NO_VAL)
bc_err(BC_ERR_EXEC_TYPE);
#endif


if (left->t == BC_RESULT_ARRAY_ELEM) {

BC_SIG_LOCK;


bc_num_free(l);

memcpy(l, r, sizeof(BcNum));


bc_vec_npop(&p->results, 2);

BC_SIG_UNLOCK;
}
else {



BcVec *v = bc_program_vec(p, left->d.loc.loc, BC_TYPE_VAR);
bc_program_assignStr(p, r, v, false);
}

#if BC_ENABLED



if (inst == BC_INST_ASSIGN) {
res.t = BC_RESULT_STR;
memcpy(&res.d.n, r, sizeof(BcNum));
bc_vec_push(&p->results, &res);
}

#endif


return;
}


if (BC_INST_IS_ASSIGN(inst)) {










if (right->t == BC_RESULT_TEMP || right->t >= BC_RESULT_IBASE) {

BC_SIG_LOCK;

bc_num_free(l);
memcpy(l, r, sizeof(BcNum));
right->t = BC_RESULT_ZERO;

BC_SIG_UNLOCK;
}

else bc_num_copy(l, r);
}
#if BC_ENABLED
else {



BcBigDig scale = BC_PROG_SCALE(p);




if (BC_PROG_STR(l)) bc_err(BC_ERR_EXEC_TYPE);



if (!use_val)
inst -= (BC_INST_ASSIGN_POWER_NO_VAL - BC_INST_ASSIGN_POWER);

assert(BC_NUM_RDX_VALID(l));
assert(BC_NUM_RDX_VALID(r));



bc_program_ops[inst - BC_INST_ASSIGN_POWER](l, r, l, scale);
}
#endif

ob = (left->t == BC_RESULT_OBASE);
sc = (left->t == BC_RESULT_SCALE);



if (ob || sc || left->t == BC_RESULT_IBASE) {

BcVec *v;
BcBigDig *ptr, *ptr_t, val, max, min;


val = bc_num_bigdig(l);


if (sc) {


min = 0;
max = vm.maxes[BC_PROG_GLOBALS_SCALE];


v = p->globals_v + BC_PROG_GLOBALS_SCALE;
ptr_t = p->globals + BC_PROG_GLOBALS_SCALE;
}
else {


min = BC_NUM_MIN_BASE;
if (BC_ENABLE_EXTRA_MATH && ob && (BC_IS_DC || !BC_IS_POSIX))
min = 0;
max = vm.maxes[ob + BC_PROG_GLOBALS_IBASE];


v = p->globals_v + BC_PROG_GLOBALS_IBASE + ob;
ptr_t = p->globals + BC_PROG_GLOBALS_IBASE + ob;
}


if (BC_ERR(val > max || val < min)) {


BcErr e = left->t - BC_RESULT_IBASE + BC_ERR_EXEC_IBASE;

bc_verr(e, min, max);
}


ptr = bc_vec_top(v);
*ptr = val;
*ptr_t = val;
}
#if BC_ENABLE_EXTRA_MATH

else if (left->t == BC_RESULT_SEED) bc_num_rng(l, &p->rng);
#endif

BC_SIG_LOCK;



if (use_val) {
bc_num_createCopy(&res.d.n, l);
res.t = BC_RESULT_TEMP;
bc_vec_npop(&p->results, 2);
bc_vec_push(&p->results, &res);
}
else bc_vec_npop(&p->results, 2);

BC_SIG_UNLOCK;
}












static void bc_program_pushVar(BcProgram *p, const char *restrict code,
size_t *restrict bgn, bool pop, bool copy)
{
BcResult r;
size_t idx = bc_program_index(code, bgn);


r.t = BC_RESULT_VAR;
r.d.loc.loc = idx;

#if DC_ENABLED


if (BC_IS_DC && (pop || copy)) {


BcVec *v = bc_program_vec(p, idx, BC_TYPE_VAR);
BcNum *num = bc_vec_top(v);


if (BC_ERR(!BC_PROG_STACK(v, 2 - copy))) {
const char *name = bc_map_name(&p->var_map, idx);
bc_verr(BC_ERR_EXEC_STACK_REGISTER, name);
}

assert(BC_PROG_STACK(v, 2 - copy));


if (!BC_PROG_STR(num)) {

BC_SIG_LOCK;


r.t = BC_RESULT_TEMP;
bc_num_createCopy(&r.d.n, num);


if (!copy) bc_vec_pop(v);

bc_vec_push(&p->results, &r);

BC_SIG_UNLOCK;

return;
}
else {


memcpy(&r.d.n, num, sizeof(BcNum));
r.t = BC_RESULT_STR;
}


if (!copy) bc_vec_pop(v);
}
#endif

bc_vec_push(&p->results, &r);
}









static void bc_program_pushArray(BcProgram *p, const char *restrict code,
size_t *restrict bgn, uchar inst)
{
BcResult r, *operand;
BcNum *num;
BcBigDig temp;


r.d.loc.loc = bc_program_index(code, bgn);


if (inst == BC_INST_ARRAY) {
r.t = BC_RESULT_ARRAY;
bc_vec_push(&p->results, &r);
return;
}


bc_program_prep(p, &operand, &num, 0);
temp = bc_num_bigdig(num);


r.t = BC_RESULT_ARRAY_ELEM;
r.d.loc.idx = (size_t) temp;

BC_SIG_LOCK;


bc_vec_pop(&p->results);
bc_vec_push(&p->results, &r);

BC_SIG_UNLOCK;
}

#if BC_ENABLED








static void bc_program_incdec(BcProgram *p, uchar inst) {

BcResult *ptr, res, copy;
BcNum *num;
uchar inst2;

bc_program_prep(p, &ptr, &num, 0);

BC_SIG_LOCK;


copy.t = BC_RESULT_TEMP;
bc_num_createCopy(&copy.d.n, num);

BC_SETJMP_LOCKED(exit);

BC_SIG_UNLOCK;


res.t = BC_RESULT_ONE;
inst2 = BC_INST_ASSIGN_PLUS_NO_VAL + (inst & 0x01);

bc_vec_push(&p->results, &res);
bc_program_assign(p, inst2);

BC_SIG_LOCK;

bc_vec_push(&p->results, &copy);

BC_UNSETJMP;

BC_SIG_UNLOCK;


return;

exit:
BC_SIG_MAYLOCK;
bc_num_free(&copy.d.n);
BC_LONGJMP_CONT;
}










static void bc_program_call(BcProgram *p, const char *restrict code,
size_t *restrict bgn)
{
BcInstPtr ip;
size_t i, nargs;
BcFunc *f;
BcVec *v;
BcAuto *a;
BcResult *arg;


nargs = bc_program_index(code, bgn);


ip.idx = 0;
ip.func = bc_program_index(code, bgn);
f = bc_vec_item(&p->fns, ip.func);


if (BC_ERR(!f->code.len)) bc_verr(BC_ERR_EXEC_UNDEF_FUNC, f->name);
if (BC_ERR(nargs != f->nparams))
bc_verr(BC_ERR_EXEC_PARAMS, f->nparams, nargs);


ip.len = p->results.len - nargs;

assert(BC_PROG_STACK(&p->results, nargs));


if (BC_G) bc_program_prepGlobals(p);


for (i = 0; i < nargs; ++i) {

size_t j;
bool last = true;

arg = bc_vec_top(&p->results);
if (BC_ERR(arg->t == BC_RESULT_VOID)) bc_err(BC_ERR_EXEC_VOID_VAL);


a = bc_vec_item(&f->autos, nargs - 1 - i);




if (arg->t == BC_RESULT_VAR || arg->t == BC_RESULT_ARRAY) {


for (j = 0; j < i && last; ++j) {

BcAuto *aptr = bc_vec_item(&f->autos, nargs - 1 - j);




last = (arg->d.loc.loc != aptr->idx ||
(!aptr->type) != (arg->t == BC_RESULT_VAR));
}
}


bc_program_copyToVar(p, a->idx, a->type, last);
}

BC_SIG_LOCK;


for (; i < f->autos.len; ++i) {


a = bc_vec_item(&f->autos, i);
v = bc_program_vec(p, a->idx, a->type);


if (a->type == BC_TYPE_VAR) {
BcNum *n = bc_vec_pushEmpty(v);
bc_num_init(n, BC_NUM_DEF_SIZE);
}
else {

BcVec *v2;

assert(a->type == BC_TYPE_ARRAY);

v2 = bc_vec_pushEmpty(v);
bc_array_init(v2, true);
}
}


bc_vec_push(&p->stack, &ip);

BC_SIG_UNLOCK;
}







static void bc_program_return(BcProgram *p, uchar inst) {

BcResult *res;
BcFunc *f;
BcInstPtr *ip;
size_t i, nresults;


ip = bc_vec_top(&p->stack);



nresults = p->results.len - ip->len;


assert(BC_PROG_STACK(&p->stack, 2));



assert(BC_PROG_STACK(&p->results, ip->len + (inst == BC_INST_RET)));


f = bc_vec_item(&p->fns, ip->func);

res = bc_program_prepResult(p);


if (inst == BC_INST_RET) {

BcNum *num;
BcResult *operand;


bc_program_operand(p, &operand, &num, 1);

if (BC_PROG_STR(num)) {




res->t = BC_RESULT_STR;

memcpy(&res->d.n, num, sizeof(BcNum));
}
else {

BC_SIG_LOCK;

bc_num_createCopy(&res->d.n, num);
}
}

else if (inst == BC_INST_RET_VOID) res->t = BC_RESULT_VOID;
else {

BC_SIG_LOCK;


bc_num_init(&res->d.n, BC_NUM_DEF_SIZE);
}

BC_SIG_MAYUNLOCK;


for (i = 0; i < f->autos.len; ++i) {

BcAuto *a = bc_vec_item(&f->autos, i);
BcVec *v = bc_program_vec(p, a->idx, a->type);

bc_vec_pop(v);
}

BC_SIG_LOCK;


bc_program_retire(p, 1, nresults);


if (BC_G) bc_program_popGlobals(p, false);


bc_vec_pop(&p->stack);

BC_SIG_UNLOCK;
}
#endif






static void bc_program_builtin(BcProgram *p, uchar inst) {

BcResult *opd, *res;
BcNum *num;
bool len = (inst == BC_INST_LENGTH);


#if BC_ENABLE_EXTRA_MATH
assert(inst >= BC_INST_LENGTH && inst <= BC_INST_IRAND);
#else
assert(inst >= BC_INST_LENGTH && inst <= BC_INST_ABS);
#endif

#if !defined(BC_PROG_NO_STACK_CHECK)

if (BC_IS_DC && BC_ERR(!BC_PROG_STACK(&p->results, 1)))
bc_err(BC_ERR_EXEC_STACK);
#endif

assert(BC_PROG_STACK(&p->results, 1));

res = bc_program_prepResult(p);

bc_program_operand(p, &opd, &num, 1);

assert(num != NULL);



if (!len && (inst != BC_INST_SCALE_FUNC || BC_IS_BC))
bc_program_type_num(opd, num);


if (inst == BC_INST_SQRT) bc_num_sqrt(num, &res->d.n, BC_PROG_SCALE(p));


else if (inst == BC_INST_ABS) {

BC_SIG_LOCK;

bc_num_createCopy(&res->d.n, num);

BC_SIG_UNLOCK;

BC_NUM_NEG_CLR_NP(res->d.n);
}
#if BC_ENABLE_EXTRA_MATH

else if (inst == BC_INST_IRAND) {

BC_SIG_LOCK;

bc_num_init(&res->d.n, num->len - BC_NUM_RDX_VAL(num));

BC_SIG_UNLOCK;

bc_num_irand(num, &res->d.n, &p->rng);
}
#endif


else {

BcBigDig val = 0;


if (len) {


if (opd->t == BC_RESULT_ARRAY) {



BcVec *v = (BcVec*) num;

#if BC_ENABLED

if (BC_IS_BC && v->size == sizeof(uchar))
v = bc_program_dereference(p, v);
#endif

assert(v->size == sizeof(BcNum));

val = (BcBigDig) v->len;
}
else {


if (!BC_PROG_NUM(opd, num)) {

char *str;


str = bc_program_string(p, num);
val = (BcBigDig) strlen(str);
}
else
{

val = (BcBigDig) bc_num_len(num);
}
}
}


else if (BC_IS_BC || BC_PROG_NUM(opd, num))
val = (BcBigDig) bc_num_scale(num);

BC_SIG_LOCK;


bc_num_createFromBigdig(&res->d.n, val);

BC_SIG_UNLOCK;
}

bc_program_retire(p, 1, 1);
}





static void bc_program_divmod(BcProgram *p) {

BcResult *opd1, *opd2, *res, *res2;
BcNum *n1, *n2;
size_t req;


bc_vec_grow(&p->results, 2);



res2 = bc_program_prepResult(p);
res = bc_program_prepResult(p);


bc_program_binOpPrep(p, &opd1, &n1, &opd2, &n2, 2);

req = bc_num_mulReq(n1, n2, BC_PROG_SCALE(p));

BC_SIG_LOCK;


bc_num_init(&res->d.n, req);
bc_num_init(&res2->d.n, req);

BC_SIG_UNLOCK;


bc_num_divmod(n1, n2, &res2->d.n, &res->d.n, BC_PROG_SCALE(p));

bc_program_retire(p, 2, 2);
}





static void bc_program_modexp(BcProgram *p) {

BcResult *r1, *r2, *r3, *res;
BcNum *n1, *n2, *n3;

#if DC_ENABLED


if (BC_IS_DC && BC_ERR(!BC_PROG_STACK(&p->results, 3)))
bc_err(BC_ERR_EXEC_STACK);

#endif

assert(BC_PROG_STACK(&p->results, 3));

res = bc_program_prepResult(p);


bc_program_operand(p, &r1, &n1, 3);
bc_program_type_num(r1, n1);


bc_program_binOpPrep(p, &r2, &n2, &r3, &n3, 1);



if (r1->t == BC_RESULT_ARRAY_ELEM && (r1->t == r2->t || r1->t == r3->t))
n1 = bc_program_num(p, r1);

BC_SIG_LOCK;

bc_num_init(&res->d.n, n3->len);

BC_SIG_UNLOCK;

bc_num_modexp(n1, n2, n3, &res->d.n);

bc_program_retire(p, 1, 3);
}






static uchar bc_program_asciifyNum(BcProgram *p, BcNum *n) {

BcNum num;
BcBigDig val;

#if !defined(NDEBUG)

val = 0;
#endif

bc_num_clear(&num);

BC_SETJMP(num_err);

BC_SIG_LOCK;

bc_num_createCopy(&num, n);

BC_SIG_UNLOCK;


bc_num_truncate(&num, num.scale);
BC_NUM_NEG_CLR_NP(num);



bc_num_mod(&num, &p->strmb, &num, 0);




val = bc_num_bigdig2(&num);

num_err:
BC_SIG_MAYLOCK;
bc_num_free(&num);
BC_LONGJMP_CONT;
return (uchar) val;
}






static void bc_program_asciify(BcProgram *p, size_t fidx) {

BcResult *r, res;
BcNum *n;
char str[2], *str2;
uchar c;
size_t idx;


if (BC_ERR(!BC_PROG_STACK(&p->results, 1))) bc_err(BC_ERR_EXEC_STACK);

assert(BC_PROG_STACK(&p->results, 1));


bc_program_operand(p, &r, &n, 0);

assert(n != NULL);


if (BC_PROG_NUM(r, n)) c = bc_program_asciifyNum(p, n);
else {


str2 = bc_program_string(p, n);
c = (uchar) str2[0];
}


str[0] = (char) c;
str[1] = '\0';


BC_SIG_LOCK;
idx = bc_program_addString(p, str, fidx);
BC_SIG_UNLOCK;


res.t = BC_RESULT_STR;
bc_num_clear(&res.d.n);
res.d.n.rdx = fidx;
res.d.n.scale = idx;


bc_vec_pop(&p->results);
bc_vec_push(&p->results, &res);
}





static void bc_program_printStream(BcProgram *p) {

BcResult *r;
BcNum *n;


if (BC_ERR(!BC_PROG_STACK(&p->results, 1))) bc_err(BC_ERR_EXEC_STACK);

assert(BC_PROG_STACK(&p->results, 1));


bc_program_operand(p, &r, &n, 0);

assert(n != NULL);


if (BC_PROG_NUM(r, n)) bc_num_stream(n);
else bc_program_printChars(bc_program_string(p, n));


bc_vec_pop(&p->results);
}

#if DC_ENABLED








static void bc_program_regStackLen(BcProgram *p, const char *restrict code,
size_t *restrict bgn)
{
size_t idx = bc_program_index(code, bgn);
BcVec *v = bc_program_vec(p, idx, BC_TYPE_VAR);

bc_program_pushBigdig(p, (BcBigDig) v->len, BC_RESULT_TEMP);
}





static void bc_program_stackLen(BcProgram *p) {
bc_program_pushBigdig(p, (BcBigDig) p->results.len, BC_RESULT_TEMP);
}








static void bc_program_nquit(BcProgram *p, uchar inst) {

BcResult *opnd;
BcNum *num;
BcBigDig val;
size_t i;


assert(p->stack.len == p->tail_calls.len);


if (inst == BC_INST_QUIT) val = 2;
else {

bc_program_prep(p, &opnd, &num, 0);
val = bc_num_bigdig(num);

bc_vec_pop(&p->results);
}


for (i = 0; val && i < p->tail_calls.len; ++i) {


size_t calls = *((size_t*) bc_vec_item_rev(&p->tail_calls, i)) + 1;


if (calls >= val) val = 0;
else val -= (BcBigDig) calls;
}


if (i == p->stack.len) {
vm.status = BC_STATUS_QUIT;
BC_JMP;
}
else {




BC_SIG_LOCK;
bc_vec_npop(&p->stack, i);
bc_vec_npop(&p->tail_calls, i);
BC_SIG_UNLOCK;
}
}





static void bc_program_execStackLen(BcProgram *p) {

size_t i, amt, len = p->tail_calls.len;

amt = len;

for (i = 0; i < len; ++i)
amt += *((size_t*) bc_vec_item(&p->tail_calls, i));

bc_program_pushBigdig(p, (BcBigDig) amt, BC_RESULT_TEMP);
}










static void bc_program_execStr(BcProgram *p, const char *restrict code,
size_t *restrict bgn, bool cond, size_t len)
{
BcResult *r;
char *str;
BcFunc *f;
BcInstPtr ip;
size_t fidx;
BcNum *n;

assert(p->stack.len == p->tail_calls.len);


if (BC_ERR(!BC_PROG_STACK(&p->results, 1))) bc_err(BC_ERR_EXEC_STACK);

assert(BC_PROG_STACK(&p->results, 1));


bc_program_operand(p, &r, &n, 0);


if (cond) {

bool exec;
size_t idx, then_idx, else_idx;


then_idx = bc_program_index(code, bgn);
else_idx = bc_program_index(code, bgn);


exec = (r->d.n.len != 0);

idx = exec ? then_idx : else_idx;

BC_SIG_LOCK;
BC_SETJMP_LOCKED(exit);





if (exec || (else_idx != SIZE_MAX))
n = bc_vec_top(bc_program_vec(p, idx, BC_TYPE_VAR));
else goto exit;

if (BC_ERR(!BC_PROG_STR(n))) bc_err(BC_ERR_EXEC_TYPE);

BC_UNSETJMP;
BC_SIG_UNLOCK;
}
else {




assert(r->t != BC_RESULT_VAR);

if (r->t != BC_RESULT_STR) return;
}

assert(BC_PROG_STR(n));


str = bc_program_string(p, n);


BC_SIG_LOCK;
fidx = bc_program_insertFunc(p, str);
BC_SIG_UNLOCK;
f = bc_vec_item(&p->fns, fidx);


if (!f->code.len) {

BC_SIG_LOCK;

if (!BC_PARSE_IS_INITED(&vm.read_prs, p)) {

bc_parse_init(&vm.read_prs, p, fidx);



bc_vec_init(&vm.read_buf, sizeof(char), BC_DTOR_NONE);
}


else bc_parse_updateFunc(&vm.read_prs, fidx);

bc_lex_file(&vm.read_prs.l, vm.file);

BC_SETJMP_LOCKED(err);

BC_SIG_UNLOCK;


bc_parse_text(&vm.read_prs, str, false);

BC_SIG_LOCK;
vm.expr(&vm.read_prs, BC_PARSE_NOCALL);

BC_UNSETJMP;



assert(vm.read_prs.l.t == BC_LEX_EOF);

BC_SIG_UNLOCK;
}


ip.idx = 0;
ip.len = p->results.len;
ip.func = fidx;

BC_SIG_LOCK;


bc_vec_pop(&p->results);




if (p->stack.len > 1 && *bgn == len - 1 && code[*bgn] == BC_INST_POP_EXEC) {

size_t *call_ptr = bc_vec_top(&p->tail_calls);


*call_ptr += 1;



bc_vec_pop(&p->stack);
}

else bc_vec_push(&p->tail_calls, &ip.idx);


bc_vec_push(&p->stack, &ip);

BC_SIG_UNLOCK;

return;

err:
BC_SIG_MAYLOCK;

f = bc_vec_item(&p->fns, fidx);


bc_vec_popAll(&f->code);

exit:
bc_vec_pop(&p->results);
BC_LONGJMP_CONT;
}





static void bc_program_printStack(BcProgram *p) {

size_t idx;

for (idx = 0; idx < p->results.len; ++idx)
bc_program_print(p, BC_INST_PRINT, idx);
}
#endif






static void bc_program_pushGlobal(BcProgram *p, uchar inst) {

BcResultType t;


assert(inst >= BC_INST_IBASE && inst <= BC_INST_SCALE);


t = inst - BC_INST_IBASE + BC_RESULT_IBASE;
bc_program_pushBigdig(p, p->globals[inst - BC_INST_IBASE], t);
}






static void bc_program_globalSetting(BcProgram *p, uchar inst) {

BcBigDig val;


assert(inst >= BC_INST_LINE_LENGTH && inst <= BC_INST_LEADING_ZERO);

if (inst == BC_INST_LINE_LENGTH) val = (BcBigDig) vm.line_len;
#if BC_ENABLED
else if (inst == BC_INST_GLOBAL_STACKS) val = (BC_G != 0);
#endif
else val = (BC_Z != 0);


bc_program_pushBigdig(p, val, BC_RESULT_TEMP);
}

#if BC_ENABLE_EXTRA_MATH





static void bc_program_pushSeed(BcProgram *p) {

BcResult *res;

res = bc_program_prepResult(p);
res->t = BC_RESULT_SEED;

BC_SIG_LOCK;


bc_num_init(&res->d.n, 2 * BC_RAND_NUM_SIZE);

BC_SIG_UNLOCK;

bc_num_createFromRNG(&res->d.n, &p->rng);
}

#endif







static void bc_program_addFunc(BcProgram *p, BcId *id_ptr) {

BcInstPtr *ip;
BcFunc *f;

BC_SIG_ASSERT_LOCKED;


f = bc_vec_pushEmpty(&p->fns);
bc_func_init(f, id_ptr->name);


if (p->stack.len) {
ip = bc_vec_top(&p->stack);
bc_program_setVecs(p, (BcFunc*) bc_vec_item(&p->fns, ip->func));
}
}

size_t bc_program_insertFunc(BcProgram *p, const char *name) {

BcId *id_ptr;
bool new;
size_t idx;

BC_SIG_ASSERT_LOCKED;

assert(p != NULL && name != NULL);


new = bc_map_insert(&p->fn_map, name, p->fns.len, &idx);
id_ptr = (BcId*) bc_vec_item(&p->fn_map, idx);
idx = id_ptr->idx;


if (new) {


bc_program_addFunc(p, id_ptr);
}
#if BC_ENABLED

else if (BC_IS_BC) {
BcFunc *func = bc_vec_item(&p->fns, idx);
bc_func_reset(func);
}
#endif

return idx;
}

#if !defined(NDEBUG)
void bc_program_free(BcProgram *p) {

size_t i;

BC_SIG_ASSERT_LOCKED;

assert(p != NULL);


for (i = 0; i < BC_PROG_GLOBALS_LEN; ++i) bc_vec_free(p->globals_v + i);

bc_vec_free(&p->fns);
bc_vec_free(&p->fn_map);
bc_vec_free(&p->vars);
bc_vec_free(&p->var_map);
bc_vec_free(&p->arrs);
bc_vec_free(&p->arr_map);
bc_vec_free(&p->results);
bc_vec_free(&p->stack);

#if BC_ENABLED
if (BC_IS_BC) bc_num_free(&p->last);
#endif

#if BC_ENABLE_EXTRA_MATH
bc_rand_free(&p->rng);
#endif

#if DC_ENABLED
if (BC_IS_DC) bc_vec_free(&p->tail_calls);
#endif
}
#endif

void bc_program_init(BcProgram *p) {

BcInstPtr ip;
size_t i;

BC_SIG_ASSERT_LOCKED;

assert(p != NULL);


memset(&ip, 0, sizeof(BcInstPtr));


for (i = 0; i < BC_PROG_GLOBALS_LEN; ++i) {

BcBigDig val = i == BC_PROG_GLOBALS_SCALE ? 0 : BC_BASE;

bc_vec_init(p->globals_v + i, sizeof(BcBigDig), BC_DTOR_NONE);
bc_vec_push(p->globals_v + i, &val);

p->globals[i] = val;
}

#if DC_ENABLED

if (BC_IS_DC) {

bc_vec_init(&p->tail_calls, sizeof(size_t), BC_DTOR_NONE);


i = 0;
bc_vec_push(&p->tail_calls, &i);
}
#endif

bc_num_setup(&p->strmb, p->strmb_num, BC_NUM_BIGDIG_LOG10);
bc_num_bigdig2num(&p->strmb, BC_NUM_STREAM_BASE);

#if BC_ENABLE_EXTRA_MATH


srand((unsigned int) time(NULL));
bc_rand_init(&p->rng);
#endif

#if BC_ENABLED
if (BC_IS_BC) bc_num_init(&p->last, BC_NUM_DEF_SIZE);
#endif

#if !defined(NDEBUG)
bc_vec_init(&p->fns, sizeof(BcFunc), BC_DTOR_FUNC);
#else
bc_vec_init(&p->fns, sizeof(BcFunc), BC_DTOR_NONE);
#endif
bc_map_init(&p->fn_map);
bc_program_insertFunc(p, bc_func_main);
bc_program_insertFunc(p, bc_func_read);

bc_vec_init(&p->vars, sizeof(BcVec), BC_DTOR_VEC);
bc_map_init(&p->var_map);

bc_vec_init(&p->arrs, sizeof(BcVec), BC_DTOR_VEC);
bc_map_init(&p->arr_map);

bc_vec_init(&p->results, sizeof(BcResult), BC_DTOR_RESULT);


bc_vec_init(&p->stack, sizeof(BcInstPtr), BC_DTOR_NONE);
bc_vec_push(&p->stack, &ip);


bc_program_setVecs(p, (BcFunc*) bc_vec_item(&p->fns, BC_PROG_MAIN));

assert(p->consts != NULL && p->strs != NULL);
}

void bc_program_reset(BcProgram *p) {

BcFunc *f;
BcInstPtr *ip;

BC_SIG_ASSERT_LOCKED;


bc_vec_npop(&p->stack, p->stack.len - 1);
bc_vec_popAll(&p->results);

#if BC_ENABLED

if (BC_G) bc_program_popGlobals(p, true);
#endif


f = bc_vec_item(&p->fns, BC_PROG_MAIN);
bc_vec_npop(&f->code, f->code.len);


ip = bc_vec_top(&p->stack);
bc_program_setVecs(p, f);
memset(ip, 0, sizeof(BcInstPtr));


if (vm.sig) {
bc_file_write(&vm.fout, bc_flush_none, bc_program_ready_msg,
bc_program_ready_msg_len);
bc_file_flush(&vm.fout, bc_flush_err);
vm.sig = 0;
}
}

void bc_program_exec(BcProgram *p) {

size_t idx;
BcResult r, *ptr;
BcInstPtr *ip;
BcFunc *func;
char *code;
bool cond = false;
uchar inst;
#if BC_ENABLED
BcNum *num;
#endif
#if !BC_HAS_COMPUTED_GOTO
#if !defined(NDEBUG)
size_t jmp_bufs_len;
#endif
#endif

#if BC_HAS_COMPUTED_GOTO
BC_PROG_LBLS;
BC_PROG_LBLS_ASSERT;



func = (BcFunc*) bc_vec_item(&p->fns, BC_PROG_MAIN);
bc_vec_pushByte(&func->code, BC_INST_INVALID);
#endif

ip = bc_vec_top(&p->stack);
func = (BcFunc*) bc_vec_item(&p->fns, ip->func);
code = func->code.v;


BC_SIG_LOCK;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

#if !BC_HAS_COMPUTED_GOTO

#if !defined(NDEBUG)
jmp_bufs_len = vm.jmp_bufs.len;
#endif



while (ip->idx < func->code.len)
#endif
{

BC_SIG_ASSERT_NOT_LOCKED;

#if BC_HAS_COMPUTED_GOTO

BC_PROG_JUMP(inst, code, ip);

#else


inst = (uchar) code[(ip->idx)++];

#endif

#if BC_DEBUG_CODE
bc_file_printf(&vm.ferr, "inst: %s\n", bc_inst_names[inst]);
bc_file_flush(&vm.ferr, bc_flush_none);
#endif

#if !BC_HAS_COMPUTED_GOTO
switch (inst)
#endif
{

#if BC_ENABLED


BC_PROG_LBL(BC_INST_JUMP_ZERO):
{
bc_program_prep(p, &ptr, &num, 0);

cond = !bc_num_cmpZero(num);
bc_vec_pop(&p->results);

BC_PROG_DIRECT_JUMP(BC_INST_JUMP)
}

BC_PROG_FALLTHROUGH

BC_PROG_LBL(BC_INST_JUMP):
{
idx = bc_program_index(code, &ip->idx);


if (inst == BC_INST_JUMP || cond) {


size_t *addr = bc_vec_item(&func->labels, idx);



assert(*addr != SIZE_MAX);


ip->idx = *addr;
}

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_CALL):
{
assert(BC_IS_BC);

bc_program_call(p, code, &ip->idx);



BC_SIG_LOCK;
ip = bc_vec_top(&p->stack);
func = bc_vec_item(&p->fns, ip->func);
code = func->code.v;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_INC):
BC_PROG_LBL(BC_INST_DEC):
{
bc_program_incdec(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_HALT):
{
vm.status = BC_STATUS_QUIT;


BC_JMP;

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_RET):
BC_PROG_LBL(BC_INST_RET0):
BC_PROG_LBL(BC_INST_RET_VOID):
{
bc_program_return(p, inst);



BC_SIG_LOCK;
ip = bc_vec_top(&p->stack);
func = bc_vec_item(&p->fns, ip->func);
code = func->code.v;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}
#endif

BC_PROG_LBL(BC_INST_BOOL_OR):
BC_PROG_LBL(BC_INST_BOOL_AND):
BC_PROG_LBL(BC_INST_REL_EQ):
BC_PROG_LBL(BC_INST_REL_LE):
BC_PROG_LBL(BC_INST_REL_GE):
BC_PROG_LBL(BC_INST_REL_NE):
BC_PROG_LBL(BC_INST_REL_LT):
BC_PROG_LBL(BC_INST_REL_GT):
{
bc_program_logical(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_READ):
{


bc_file_flush(&vm.fout, bc_flush_save);

bc_program_read(p);



BC_SIG_LOCK;
ip = bc_vec_top(&p->stack);
func = bc_vec_item(&p->fns, ip->func);
code = func->code.v;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}

#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_RAND):
{
bc_program_rand(p);
BC_PROG_JUMP(inst, code, ip);
}
#endif

BC_PROG_LBL(BC_INST_MAXIBASE):
BC_PROG_LBL(BC_INST_MAXOBASE):
BC_PROG_LBL(BC_INST_MAXSCALE):
#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_MAXRAND):
#endif
{
BcBigDig dig = vm.maxes[inst - BC_INST_MAXIBASE];
bc_program_pushBigdig(p, dig, BC_RESULT_TEMP);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_LINE_LENGTH):
#if BC_ENABLED
BC_PROG_LBL(BC_INST_GLOBAL_STACKS):
#endif
BC_PROG_LBL(BC_INST_LEADING_ZERO):
{
bc_program_globalSetting(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_VAR):
{
bc_program_pushVar(p, code, &ip->idx, false, false);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_ARRAY_ELEM):
BC_PROG_LBL(BC_INST_ARRAY):
{
bc_program_pushArray(p, code, &ip->idx, inst);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_IBASE):
BC_PROG_LBL(BC_INST_SCALE):
BC_PROG_LBL(BC_INST_OBASE):
{
bc_program_pushGlobal(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_SEED):
{
bc_program_pushSeed(p);
BC_PROG_JUMP(inst, code, ip);
}
#endif

BC_PROG_LBL(BC_INST_LENGTH):
BC_PROG_LBL(BC_INST_SCALE_FUNC):
BC_PROG_LBL(BC_INST_SQRT):
BC_PROG_LBL(BC_INST_ABS):
#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_IRAND):
#endif
{
bc_program_builtin(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_ASCIIFY):
{
bc_program_asciify(p, ip->func);



BC_SIG_LOCK;
ip = bc_vec_top(&p->stack);
func = bc_vec_item(&p->fns, ip->func);
code = func->code.v;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_NUM):
{
bc_program_const(p, code, &ip->idx);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_ZERO):
BC_PROG_LBL(BC_INST_ONE):
#if BC_ENABLED
BC_PROG_LBL(BC_INST_LAST):
#endif
{
r.t = BC_RESULT_ZERO + (inst - BC_INST_ZERO);
bc_vec_push(&p->results, &r);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_PRINT):
BC_PROG_LBL(BC_INST_PRINT_POP):
#if BC_ENABLED
BC_PROG_LBL(BC_INST_PRINT_STR):
#endif
{
bc_program_print(p, inst, 0);



bc_file_flush(&vm.fout, bc_flush_save);

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_STR):
{

r.t = BC_RESULT_STR;
bc_num_clear(&r.d.n);
r.d.n.rdx = bc_program_index(code, &ip->idx);
r.d.n.scale = bc_program_index(code, &ip->idx);
bc_vec_push(&p->results, &r);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_POWER):
BC_PROG_LBL(BC_INST_MULTIPLY):
BC_PROG_LBL(BC_INST_DIVIDE):
BC_PROG_LBL(BC_INST_MODULUS):
BC_PROG_LBL(BC_INST_PLUS):
BC_PROG_LBL(BC_INST_MINUS):
#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_PLACES):
BC_PROG_LBL(BC_INST_LSHIFT):
BC_PROG_LBL(BC_INST_RSHIFT):
#endif
{
bc_program_op(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_NEG):
BC_PROG_LBL(BC_INST_BOOL_NOT):
#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_TRUNC):
#endif
{
bc_program_unary(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

#if BC_ENABLED
BC_PROG_LBL(BC_INST_ASSIGN_POWER):
BC_PROG_LBL(BC_INST_ASSIGN_MULTIPLY):
BC_PROG_LBL(BC_INST_ASSIGN_DIVIDE):
BC_PROG_LBL(BC_INST_ASSIGN_MODULUS):
BC_PROG_LBL(BC_INST_ASSIGN_PLUS):
BC_PROG_LBL(BC_INST_ASSIGN_MINUS):
#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_ASSIGN_PLACES):
BC_PROG_LBL(BC_INST_ASSIGN_LSHIFT):
BC_PROG_LBL(BC_INST_ASSIGN_RSHIFT):
#endif
BC_PROG_LBL(BC_INST_ASSIGN):
BC_PROG_LBL(BC_INST_ASSIGN_POWER_NO_VAL):
BC_PROG_LBL(BC_INST_ASSIGN_MULTIPLY_NO_VAL):
BC_PROG_LBL(BC_INST_ASSIGN_DIVIDE_NO_VAL):
BC_PROG_LBL(BC_INST_ASSIGN_MODULUS_NO_VAL):
BC_PROG_LBL(BC_INST_ASSIGN_PLUS_NO_VAL):
BC_PROG_LBL(BC_INST_ASSIGN_MINUS_NO_VAL):
#if BC_ENABLE_EXTRA_MATH
BC_PROG_LBL(BC_INST_ASSIGN_PLACES_NO_VAL):
BC_PROG_LBL(BC_INST_ASSIGN_LSHIFT_NO_VAL):
BC_PROG_LBL(BC_INST_ASSIGN_RSHIFT_NO_VAL):
#endif
#endif
BC_PROG_LBL(BC_INST_ASSIGN_NO_VAL):
{
bc_program_assign(p, inst);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_POP):
{
#if !defined(BC_PROG_NO_STACK_CHECK)

if (BC_IS_DC) {
if (BC_ERR(!BC_PROG_STACK(&p->results, 1)))
bc_err(BC_ERR_EXEC_STACK);
}
#endif

assert(BC_PROG_STACK(&p->results, 1));

bc_vec_pop(&p->results);

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_SWAP):
{
BcResult *ptr2;


if (BC_ERR(!BC_PROG_STACK(&p->results, 2)))
bc_err(BC_ERR_EXEC_STACK);

assert(BC_PROG_STACK(&p->results, 2));


ptr = bc_vec_item_rev(&p->results, 0);
ptr2 = bc_vec_item_rev(&p->results, 1);


memcpy(&r, ptr, sizeof(BcResult));
memcpy(ptr, ptr2, sizeof(BcResult));
memcpy(ptr2, &r, sizeof(BcResult));

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_MODEXP):
{
bc_program_modexp(p);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_DIVMOD):
{
bc_program_divmod(p);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_PRINT_STREAM):
{
bc_program_printStream(p);
BC_PROG_JUMP(inst, code, ip);
}

#if DC_ENABLED
BC_PROG_LBL(BC_INST_POP_EXEC):
{

assert(BC_PROG_STACK(&p->stack, 2));


bc_vec_pop(&p->stack);
bc_vec_pop(&p->tail_calls);



BC_SIG_LOCK;
ip = bc_vec_top(&p->stack);
func = bc_vec_item(&p->fns, ip->func);
code = func->code.v;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_EXECUTE):
BC_PROG_LBL(BC_INST_EXEC_COND):
{
cond = (inst == BC_INST_EXEC_COND);

bc_program_execStr(p, code, &ip->idx, cond, func->code.len);



BC_SIG_LOCK;
ip = bc_vec_top(&p->stack);
func = bc_vec_item(&p->fns, ip->func);
code = func->code.v;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_PRINT_STACK):
{
bc_program_printStack(p);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_CLEAR_STACK):
{
bc_vec_popAll(&p->results);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_REG_STACK_LEN):
{
bc_program_regStackLen(p, code, &ip->idx);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_STACK_LEN):
{
bc_program_stackLen(p);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_DUPLICATE):
{

if (BC_ERR(!BC_PROG_STACK(&p->results, 1)))
bc_err(BC_ERR_EXEC_STACK);

assert(BC_PROG_STACK(&p->results, 1));


ptr = bc_vec_top(&p->results);

BC_SIG_LOCK;


bc_result_copy(&r, ptr);
bc_vec_push(&p->results, &r);

BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_LOAD):
BC_PROG_LBL(BC_INST_PUSH_VAR):
{
bool copy = (inst == BC_INST_LOAD);
bc_program_pushVar(p, code, &ip->idx, true, copy);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_PUSH_TO_VAR):
{
idx = bc_program_index(code, &ip->idx);
bc_program_copyToVar(p, idx, BC_TYPE_VAR, true);
BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_QUIT):
BC_PROG_LBL(BC_INST_NQUIT):
{
bc_program_nquit(p, inst);



BC_SIG_LOCK;
ip = bc_vec_top(&p->stack);
func = bc_vec_item(&p->fns, ip->func);
code = func->code.v;
bc_program_setVecs(p, func);
BC_SIG_UNLOCK;

BC_PROG_JUMP(inst, code, ip);
}

BC_PROG_LBL(BC_INST_EXEC_STACK_LEN):
{
bc_program_execStackLen(p);
BC_PROG_JUMP(inst, code, ip);
}
#endif

#if BC_HAS_COMPUTED_GOTO
BC_PROG_LBL(BC_INST_INVALID):
{
return;
}
#else
default:
{
BC_UNREACHABLE
#if !defined(NDEBUG)
abort();
#endif
}
#endif
}

#if !BC_HAS_COMPUTED_GOTO
#if !defined(NDEBUG)



assert(jmp_bufs_len == vm.jmp_bufs.len);
#endif
#endif
}
}

#if BC_DEBUG_CODE
#if BC_ENABLED && DC_ENABLED
void bc_program_printStackDebug(BcProgram *p) {
bc_file_puts(&vm.fout, bc_flush_err, "-------------- Stack ----------\n");
bc_program_printStack(p);
bc_file_puts(&vm.fout, bc_flush_err, "-------------- Stack End ------\n");
}

static void bc_program_printIndex(const char *restrict code,
size_t *restrict bgn)
{
uchar byte, i, bytes = (uchar) code[(*bgn)++];
ulong val = 0;

for (byte = 1, i = 0; byte && i < bytes; ++i) {
byte = (uchar) code[(*bgn)++];
if (byte) val |= ((ulong) byte) << (CHAR_BIT * i);
}

bc_vm_printf(" (%lu) ", val);
}

static void bc_program_printStr(const BcProgram *p, const char *restrict code,
size_t *restrict bgn)
{
size_t idx = bc_program_index(code, bgn);
char *s;

s = *((char**) bc_vec_item(p->strs, idx));

bc_vm_printf(" (\"%s\") ", s);
}

void bc_program_printInst(const BcProgram *p, const char *restrict code,
size_t *restrict bgn)
{
uchar inst = (uchar) code[(*bgn)++];

bc_vm_printf("Inst[%zu]: %s [%lu]; ", *bgn - 1,
bc_inst_names[inst], (unsigned long) inst);

if (inst == BC_INST_VAR || inst == BC_INST_ARRAY_ELEM ||
inst == BC_INST_ARRAY)
{
bc_program_printIndex(code, bgn);
}
else if (inst == BC_INST_STR) bc_program_printStr(p, code, bgn);
else if (inst == BC_INST_NUM) {
size_t idx = bc_program_index(code, bgn);
BcConst *c = bc_vec_item(p->consts, idx);
bc_vm_printf("(%s)", c->val);
}
else if (inst == BC_INST_CALL ||
(inst > BC_INST_STR && inst <= BC_INST_JUMP_ZERO))
{
bc_program_printIndex(code, bgn);
if (inst == BC_INST_CALL) bc_program_printIndex(code, bgn);
}

bc_vm_putchar('\n', bc_flush_err);
}

void bc_program_code(const BcProgram* p) {

BcFunc *f;
char *code;
BcInstPtr ip;
size_t i;

for (i = 0; i < p->fns.len; ++i) {

ip.idx = ip.len = 0;
ip.func = i;

f = bc_vec_item(&p->fns, ip.func);
code = f->code.v;

bc_vm_printf("func[%zu]:\n", ip.func);
while (ip.idx < f->code.len) bc_program_printInst(p, code, &ip.idx);
bc_file_puts(&vm.fout, bc_flush_err, "\n\n");
}
}
#endif
#endif
