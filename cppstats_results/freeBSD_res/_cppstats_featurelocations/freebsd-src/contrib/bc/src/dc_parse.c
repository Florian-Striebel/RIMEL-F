


































#if DC_ENABLED

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include <dc.h>
#include <program.h>
#include <vm.h>







static void dc_parse_register(BcParse *p, bool var) {

bc_lex_next(&p->l);
if (p->l.t != BC_LEX_NAME) bc_parse_err(p, BC_ERR_PARSE_TOKEN);

bc_parse_pushName(p, p->l.str.v, var);
}





static inline void dc_parse_string(BcParse *p) {
bc_parse_addString(p);
bc_lex_next(&p->l);
}









static void dc_parse_mem(BcParse *p, uchar inst, bool name, bool store) {


bc_parse_push(p, inst);


if (name) dc_parse_register(p, inst != BC_INST_ARRAY_ELEM);



if (store) {
bc_parse_push(p, BC_INST_SWAP);
bc_parse_push(p, BC_INST_ASSIGN_NO_VAL);
}

bc_lex_next(&p->l);
}






static void dc_parse_cond(BcParse *p, uchar inst) {


bc_parse_push(p, inst);
bc_parse_push(p, BC_INST_EXEC_COND);


dc_parse_register(p, true);

bc_lex_next(&p->l);


if (p->l.t == BC_LEX_KW_ELSE) {
dc_parse_register(p, true);
bc_lex_next(&p->l);
}

else bc_parse_pushIndex(p, SIZE_MAX);
}







static void dc_parse_token(BcParse *p, BcLexType t, uint8_t flags) {

uchar inst;
bool assign, get_token = false;

switch (t) {

case BC_LEX_OP_REL_EQ:
case BC_LEX_OP_REL_LE:
case BC_LEX_OP_REL_GE:
case BC_LEX_OP_REL_NE:
case BC_LEX_OP_REL_LT:
case BC_LEX_OP_REL_GT:
{
inst = (uchar) (t - BC_LEX_OP_REL_EQ + BC_INST_REL_EQ);
dc_parse_cond(p, inst);
break;
}

case BC_LEX_SCOLON:
case BC_LEX_COLON:
{
dc_parse_mem(p, BC_INST_ARRAY_ELEM, true, t == BC_LEX_COLON);
break;
}

case BC_LEX_STR:
{
dc_parse_string(p);
break;
}

case BC_LEX_NEG:
{



if (dc_lex_negCommand(&p->l)) {
bc_parse_push(p, BC_INST_NEG);
get_token = true;
break;
}

bc_lex_next(&p->l);
}

BC_FALLTHROUGH

case BC_LEX_NUMBER:
{
bc_parse_number(p);


if (t == BC_LEX_NEG) bc_parse_push(p, BC_INST_NEG);
get_token = true;

break;
}

case BC_LEX_KW_READ:
{

if (BC_ERR(flags & BC_PARSE_NOREAD))
bc_parse_err(p, BC_ERR_EXEC_REC_READ);
else bc_parse_push(p, BC_INST_READ);

get_token = true;

break;
}

case BC_LEX_OP_ASSIGN:
case BC_LEX_STORE_PUSH:
{
assign = t == BC_LEX_OP_ASSIGN;
inst = assign ? BC_INST_VAR : BC_INST_PUSH_TO_VAR;
dc_parse_mem(p, inst, true, assign);
break;
}

case BC_LEX_LOAD:
case BC_LEX_LOAD_POP:
{
inst = t == BC_LEX_LOAD_POP ? BC_INST_PUSH_VAR : BC_INST_LOAD;
dc_parse_mem(p, inst, true, false);
break;
}

case BC_LEX_REG_STACK_LEVEL:
{
dc_parse_mem(p, BC_INST_REG_STACK_LEN, true, false);
break;
}

case BC_LEX_STORE_IBASE:
case BC_LEX_STORE_OBASE:
case BC_LEX_STORE_SCALE:
#if BC_ENABLE_EXTRA_MATH
case BC_LEX_STORE_SEED:
#endif
{
inst = (uchar) (t - BC_LEX_STORE_IBASE + BC_INST_IBASE);
dc_parse_mem(p, inst, false, true);
break;
}

case BC_LEX_ARRAY_LENGTH:
{

bc_parse_push(p, BC_INST_ARRAY);
dc_parse_register(p, false);

bc_parse_push(p, BC_INST_LENGTH);

get_token = true;

break;
}

default:
{


bc_parse_err(p, BC_ERR_PARSE_TOKEN);
}
}

if (get_token) bc_lex_next(&p->l);
}

void dc_parse_expr(BcParse *p, uint8_t flags) {

BcInst inst;
BcLexType t;
bool need_expr, have_expr = false;

need_expr = ((flags & BC_PARSE_NOREAD) != 0);






while ((t = p->l.t) != BC_LEX_EOF) {


if (t == BC_LEX_NLINE) {
bc_lex_next(&p->l);
continue;
}


inst = dc_parse_insts[t];




if (inst != BC_INST_INVALID) {
bc_parse_push(p, inst);
bc_lex_next(&p->l);
}
else dc_parse_token(p, t, flags);

have_expr = true;
}




if (BC_ERR(need_expr && !have_expr)) bc_err(BC_ERR_EXEC_READ_EXPR);
else if (p->l.t == BC_LEX_EOF && (flags & BC_PARSE_NOCALL))
bc_parse_push(p, BC_INST_POP_EXEC);
}

void dc_parse_parse(BcParse *p) {

assert(p != NULL);

BC_SETJMP_LOCKED(exit);



if (BC_ERR(p->l.t == BC_LEX_EOF)) bc_parse_err(p, BC_ERR_PARSE_EOF);
else dc_parse_expr(p, 0);

exit:


if (BC_SIG_EXC) bc_parse_reset(p);

BC_LONGJMP_CONT;
BC_SIG_MAYLOCK;
}
#endif
