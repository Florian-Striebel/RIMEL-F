


































#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <lang.h>
#include <program.h>
#include <vm.h>

void bc_const_free(void *constant) {

BcConst *c = constant;

BC_SIG_ASSERT_LOCKED;

assert(c->val != NULL);

bc_num_free(&c->num);
}

#if BC_ENABLED
void bc_func_insert(BcFunc *f, BcProgram *p, char *name,
BcType type, size_t line)
{
BcAuto a;
size_t i, idx;


assert(f != NULL);


idx = bc_program_search(p, name, type == BC_TYPE_VAR);


for (i = 0; i < f->autos.len; ++i) {


BcAuto *aptr = bc_vec_item(&f->autos, i);


if (BC_ERR(idx == aptr->idx && type == aptr->type)) {

const char *array = type == BC_TYPE_ARRAY ? "[]" : "";

bc_error(BC_ERR_PARSE_DUP_LOCAL, line, name, array);
}
}


a.idx = idx;
a.type = type;


bc_vec_push(&f->autos, &a);
}
#endif

void bc_func_init(BcFunc *f, const char *name) {

BC_SIG_ASSERT_LOCKED;

assert(f != NULL && name != NULL);

bc_vec_init(&f->code, sizeof(uchar), BC_DTOR_NONE);

bc_vec_init(&f->consts, sizeof(BcConst), BC_DTOR_CONST);

bc_vec_init(&f->strs, sizeof(char*), BC_DTOR_NONE);

#if BC_ENABLED


if (BC_IS_BC) {

bc_vec_init(&f->autos, sizeof(BcAuto), BC_DTOR_NONE);
bc_vec_init(&f->labels, sizeof(size_t), BC_DTOR_NONE);

f->nparams = 0;
f->voidfn = false;
}

#endif

f->name = name;
}

void bc_func_reset(BcFunc *f) {

BC_SIG_ASSERT_LOCKED;
assert(f != NULL);

bc_vec_popAll(&f->code);

bc_vec_popAll(&f->consts);

bc_vec_popAll(&f->strs);

#if BC_ENABLED
if (BC_IS_BC) {

bc_vec_popAll(&f->autos);
bc_vec_popAll(&f->labels);

f->nparams = 0;
f->voidfn = false;
}
#endif
}

#if !defined(NDEBUG)
void bc_func_free(void *func) {

BcFunc *f = (BcFunc*) func;

BC_SIG_ASSERT_LOCKED;
assert(f != NULL);

bc_vec_free(&f->code);

bc_vec_free(&f->consts);

bc_vec_free(&f->strs);

#if BC_ENABLED
if (BC_IS_BC) {

bc_vec_free(&f->autos);
bc_vec_free(&f->labels);
}
#endif
}
#endif

void bc_array_init(BcVec *a, bool nums) {

BC_SIG_ASSERT_LOCKED;


if (nums) bc_vec_init(a, sizeof(BcNum), BC_DTOR_NUM);
else bc_vec_init(a, sizeof(BcVec), BC_DTOR_VEC);


bc_array_expand(a, 1);
}

void bc_array_copy(BcVec *d, const BcVec *s) {

size_t i;

BC_SIG_ASSERT_LOCKED;

assert(d != NULL && s != NULL);
assert(d != s && d->size == s->size && d->dtor == s->dtor);





bc_vec_popAll(d);


bc_vec_expand(d, s->cap);
d->len = s->len;

for (i = 0; i < s->len; ++i) {

BcNum *dnum, *snum;

dnum = bc_vec_item(d, i);
snum = bc_vec_item(s, i);


if (BC_PROG_STR(snum)) memcpy(dnum, snum, sizeof(BcNum));
else bc_num_createCopy(dnum, snum);
}
}

void bc_array_expand(BcVec *a, size_t len) {

assert(a != NULL);

BC_SIG_ASSERT_LOCKED;

bc_vec_expand(a, len);


if (a->size == sizeof(BcNum) && a->dtor == BC_DTOR_NUM) {


while (len > a->len) {
BcNum *n = bc_vec_pushEmpty(a);
bc_num_init(n, BC_NUM_DEF_SIZE);
}
}
else {

assert(a->size == sizeof(BcVec) && a->dtor == BC_DTOR_VEC);




while (len > a->len) {
BcVec *v = bc_vec_pushEmpty(a);
bc_array_init(v, true);
}
}
}

void bc_result_clear(BcResult *r) {
r->t = BC_RESULT_TEMP;
bc_num_clear(&r->d.n);
}

#if DC_ENABLED
void bc_result_copy(BcResult *d, BcResult *src) {

assert(d != NULL && src != NULL);

BC_SIG_ASSERT_LOCKED;


d->t = src->t;


switch (d->t) {

case BC_RESULT_TEMP:
case BC_RESULT_IBASE:
case BC_RESULT_SCALE:
case BC_RESULT_OBASE:
#if BC_ENABLE_EXTRA_MATH
case BC_RESULT_SEED:
#endif
{
bc_num_createCopy(&d->d.n, &src->d.n);
break;
}

case BC_RESULT_VAR:
case BC_RESULT_ARRAY:
case BC_RESULT_ARRAY_ELEM:
{
memcpy(&d->d.loc, &src->d.loc, sizeof(BcLoc));
break;
}

case BC_RESULT_STR:
{
memcpy(&d->d.n, &src->d.n, sizeof(BcNum));
break;
}

case BC_RESULT_ZERO:
case BC_RESULT_ONE:
{

break;
}

#if BC_ENABLED
case BC_RESULT_VOID:
case BC_RESULT_LAST:
{
#if !defined(NDEBUG)

abort();
#endif
}
#endif
}
}
#endif

void bc_result_free(void *result) {

BcResult *r = (BcResult*) result;

BC_SIG_ASSERT_LOCKED;

assert(r != NULL);

switch (r->t) {

case BC_RESULT_TEMP:
case BC_RESULT_IBASE:
case BC_RESULT_SCALE:
case BC_RESULT_OBASE:
#if BC_ENABLE_EXTRA_MATH
case BC_RESULT_SEED:
#endif
{
bc_num_free(&r->d.n);
break;
}

case BC_RESULT_VAR:
case BC_RESULT_ARRAY:
case BC_RESULT_ARRAY_ELEM:
case BC_RESULT_STR:
case BC_RESULT_ZERO:
case BC_RESULT_ONE:
#if BC_ENABLED
case BC_RESULT_VOID:
case BC_RESULT_LAST:
#endif
{

break;
}
}
}
