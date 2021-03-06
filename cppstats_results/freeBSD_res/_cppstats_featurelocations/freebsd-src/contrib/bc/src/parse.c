


































#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>

#include <parse.h>
#include <program.h>
#include <vm.h>

void bc_parse_updateFunc(BcParse *p, size_t fidx) {
p->fidx = fidx;
p->func = bc_vec_item(&p->prog->fns, fidx);
}

inline void bc_parse_pushName(const BcParse *p, char *name, bool var) {
bc_parse_pushIndex(p, bc_program_search(p->prog, name, var));
}








static void bc_parse_update(BcParse *p, uchar inst, size_t idx) {
bc_parse_updateFunc(p, p->fidx);
bc_parse_push(p, inst);
bc_parse_pushIndex(p, idx);
}

void bc_parse_addString(BcParse *p) {

size_t idx;

idx = bc_program_addString(p->prog, p->l.str.v, p->fidx);


bc_parse_update(p, BC_INST_STR, p->fidx);
bc_parse_pushIndex(p, idx);
}

static void bc_parse_addNum(BcParse *p, const char *string) {

BcVec *consts = &p->func->consts;
size_t idx;
BcConst *c;
BcVec *slabs;

BC_SIG_ASSERT_LOCKED;


if (bc_parse_zero[0] == string[0] && bc_parse_zero[1] == string[1]) {
bc_parse_push(p, BC_INST_ZERO);
return;
}


if (bc_parse_one[0] == string[0] && bc_parse_one[1] == string[1]) {
bc_parse_push(p, BC_INST_ONE);
return;
}


idx = consts->len;


slabs = p->fidx == BC_PROG_MAIN || p->fidx == BC_PROG_READ ?
&vm.main_const_slab : &vm.other_slabs;


c = bc_vec_pushEmpty(consts);


c->val = bc_slabvec_strdup(slabs, string);
c->base = BC_NUM_BIGDIG_MAX;


bc_num_clear(&c->num);

bc_parse_update(p, BC_INST_NUM, idx);
}

void bc_parse_number(BcParse *p) {

#if BC_ENABLE_EXTRA_MATH
char *exp = strchr(p->l.str.v, 'e');
size_t idx = SIZE_MAX;



if (exp != NULL) {
idx = ((size_t) (exp - p->l.str.v));
*exp = 0;
}
#endif

bc_parse_addNum(p, p->l.str.v);

#if BC_ENABLE_EXTRA_MATH

if (exp != NULL) {

bool neg;


neg = (*((char*) bc_vec_item(&p->l.str, idx + 1)) == BC_LEX_NEG_CHAR);


bc_parse_addNum(p, bc_vec_item(&p->l.str, idx + 1 + neg));
bc_parse_push(p, BC_INST_LSHIFT + neg);
}
#endif
}

void bc_parse_text(BcParse *p, const char *text, bool is_stdin) {

BC_SIG_LOCK;


p->func = bc_vec_item(&p->prog->fns, p->fidx);
bc_lex_text(&p->l, text, is_stdin);

BC_SIG_UNLOCK;
}

void bc_parse_reset(BcParse *p) {

BC_SIG_ASSERT_LOCKED;


if (p->fidx != BC_PROG_MAIN) {
bc_func_reset(p->func);
bc_parse_updateFunc(p, BC_PROG_MAIN);
}


p->l.i = p->l.len;
p->l.t = BC_LEX_EOF;

#if BC_ENABLED
if (BC_IS_BC) {


p->auto_part = false;
bc_vec_npop(&p->flags, p->flags.len - 1);
bc_vec_popAll(&p->exits);
bc_vec_popAll(&p->conds);
bc_vec_popAll(&p->ops);
}
#endif


bc_program_reset(p->prog);


if (BC_ERR(vm.status)) BC_JMP;
}

#if !defined(NDEBUG)
void bc_parse_free(BcParse *p) {

BC_SIG_ASSERT_LOCKED;

assert(p != NULL);

#if BC_ENABLED
if (BC_IS_BC) {
bc_vec_free(&p->flags);
bc_vec_free(&p->exits);
bc_vec_free(&p->conds);
bc_vec_free(&p->ops);
bc_vec_free(&p->buf);
}
#endif

bc_lex_free(&p->l);
}
#endif

void bc_parse_init(BcParse *p, BcProgram *prog, size_t func) {

#if BC_ENABLED
uint16_t flag = 0;
#endif

BC_SIG_ASSERT_LOCKED;

assert(p != NULL && prog != NULL);

#if BC_ENABLED
if (BC_IS_BC) {


bc_vec_init(&p->flags, sizeof(uint16_t), BC_DTOR_NONE);
bc_vec_push(&p->flags, &flag);

bc_vec_init(&p->exits, sizeof(BcInstPtr), BC_DTOR_NONE);
bc_vec_init(&p->conds, sizeof(size_t), BC_DTOR_NONE);
bc_vec_init(&p->ops, sizeof(BcLexType), BC_DTOR_NONE);
bc_vec_init(&p->buf, sizeof(char), BC_DTOR_NONE);

p->auto_part = false;
}
#endif

bc_lex_init(&p->l);


p->prog = prog;
bc_parse_updateFunc(p, func);
}
