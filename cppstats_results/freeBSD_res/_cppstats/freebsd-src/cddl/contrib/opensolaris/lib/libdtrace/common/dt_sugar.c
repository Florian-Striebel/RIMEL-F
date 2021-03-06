#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>
#include <assert.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <dt_module.h>
#include <dt_program.h>
#include <dt_provider.h>
#include <dt_printf.h>
#include <dt_pid.h>
#include <dt_grammar.h>
#include <dt_ident.h>
#include <dt_string.h>
#include <dt_impl.h>
typedef struct dt_sugar_parse {
dtrace_hdl_t *dtsp_dtp;
dt_node_t *dtsp_pdescs;
int dtsp_num_conditions;
int dtsp_num_ifs;
dt_node_t *dtsp_clause_list;
} dt_sugar_parse_t;
static void dt_sugar_visit_stmts(dt_sugar_parse_t *, dt_node_t *, int);
static dt_node_t *
dt_sugar_new_error_var(void)
{
return (dt_node_op2(DT_TOK_PTR, dt_node_ident(strdup("self")),
dt_node_ident(strdup("%error"))));
}
static void
dt_sugar_append_clause(dt_sugar_parse_t *dp, dt_node_t *clause)
{
dp->dtsp_clause_list = dt_node_link(dp->dtsp_clause_list, clause);
}
static void
dt_sugar_prepend_clause(dt_sugar_parse_t *dp, dt_node_t *clause)
{
dp->dtsp_clause_list = dt_node_link(clause, dp->dtsp_clause_list);
}
static dt_node_t *
dt_sugar_new_condition_var(int condid)
{
char *str;
if (condid == 0)
return (NULL);
assert(condid > 0);
(void) asprintf(&str, "%%condition_%d", ABS(condid));
return (dt_node_op2(DT_TOK_PTR, dt_node_ident(strdup("this")),
dt_node_ident(str)));
}
static dt_node_t *
dt_sugar_new_condition_impl(dt_sugar_parse_t *dp,
dt_node_t *pred, int condid, int newcond)
{
dt_node_t *value, *body, *newpred;
newpred = dt_node_op1(DT_TOK_LNEG, dt_sugar_new_error_var());
if (condid == 0) {
value = dt_node_op2(DT_TOK_LAND, dt_node_int(1), pred);
} else {
value = dt_node_op2(DT_TOK_LAND,
dt_sugar_new_condition_var(condid), pred);
}
body = dt_node_statement(dt_node_op2(DT_TOK_ASGN,
dt_sugar_new_condition_var(newcond), value));
return (dt_node_clause(dp->dtsp_pdescs, newpred, body));
}
static int
dt_sugar_new_condition(dt_sugar_parse_t *dp, dt_node_t *pred, int condid)
{
dp->dtsp_num_conditions++;
dt_sugar_append_clause(dp, dt_sugar_new_condition_impl(dp,
pred, condid, dp->dtsp_num_conditions));
return (dp->dtsp_num_conditions);
}
static void
dt_sugar_visit_all(dt_sugar_parse_t *dp, dt_node_t *dnp)
{
dt_node_t *arg;
switch (dnp->dn_kind) {
case DT_NODE_FREE:
case DT_NODE_INT:
case DT_NODE_STRING:
case DT_NODE_SYM:
case DT_NODE_TYPE:
case DT_NODE_PROBE:
case DT_NODE_PDESC:
case DT_NODE_IDENT:
break;
case DT_NODE_FUNC:
for (arg = dnp->dn_args; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
break;
case DT_NODE_OP1:
dt_sugar_visit_all(dp, dnp->dn_child);
break;
case DT_NODE_OP2:
dt_sugar_visit_all(dp, dnp->dn_left);
dt_sugar_visit_all(dp, dnp->dn_right);
if (dnp->dn_op == DT_TOK_LBRAC) {
dt_node_t *ln = dnp->dn_right;
while (ln->dn_list != NULL) {
dt_sugar_visit_all(dp, ln->dn_list);
ln = ln->dn_list;
}
}
break;
case DT_NODE_OP3:
dt_sugar_visit_all(dp, dnp->dn_expr);
dt_sugar_visit_all(dp, dnp->dn_left);
dt_sugar_visit_all(dp, dnp->dn_right);
break;
case DT_NODE_DEXPR:
case DT_NODE_DFUNC:
dt_sugar_visit_all(dp, dnp->dn_expr);
break;
case DT_NODE_AGG:
for (arg = dnp->dn_aggtup; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
if (dnp->dn_aggfun)
dt_sugar_visit_all(dp, dnp->dn_aggfun);
break;
case DT_NODE_CLAUSE:
for (arg = dnp->dn_pdescs; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
if (dnp->dn_pred != NULL)
dt_sugar_visit_all(dp, dnp->dn_pred);
for (arg = dnp->dn_acts; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
break;
case DT_NODE_INLINE: {
const dt_idnode_t *inp = dnp->dn_ident->di_iarg;
dt_sugar_visit_all(dp, inp->din_root);
break;
}
case DT_NODE_MEMBER:
if (dnp->dn_membexpr)
dt_sugar_visit_all(dp, dnp->dn_membexpr);
break;
case DT_NODE_XLATOR:
for (arg = dnp->dn_members; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
break;
case DT_NODE_PROVIDER:
for (arg = dnp->dn_probes; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
break;
case DT_NODE_PROG:
for (arg = dnp->dn_list; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
break;
case DT_NODE_IF:
dp->dtsp_num_ifs++;
dt_sugar_visit_all(dp, dnp->dn_conditional);
for (arg = dnp->dn_body; arg != NULL; arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
for (arg = dnp->dn_alternate_body; arg != NULL;
arg = arg->dn_list)
dt_sugar_visit_all(dp, arg);
break;
default:
(void) dnerror(dnp, D_UNKNOWN, "bad node %p, kind %d\n",
(void *)dnp, dnp->dn_kind);
}
}
static dt_node_t *
dt_sugar_new_clearerror_clause(dt_sugar_parse_t *dp)
{
dt_node_t *stmt = dt_node_statement(dt_node_op2(DT_TOK_ASGN,
dt_sugar_new_error_var(), dt_node_int(0)));
return (dt_node_clause(dp->dtsp_pdescs, NULL, stmt));
}
static void
dt_sugar_do_if(dt_sugar_parse_t *dp, dt_node_t *if_stmt, int precondition)
{
int newid;
assert(if_stmt->dn_kind == DT_NODE_IF);
newid = dt_sugar_new_condition(dp,
if_stmt->dn_conditional, precondition);
dt_sugar_visit_stmts(dp, if_stmt->dn_body, newid);
if (if_stmt->dn_alternate_body != NULL) {
dt_node_t *pred =
dt_node_op1(DT_TOK_LNEG, dt_sugar_new_condition_var(newid));
dt_sugar_visit_stmts(dp, if_stmt->dn_alternate_body,
dt_sugar_new_condition(dp, pred, precondition));
}
}
static void
dt_sugar_new_basic_block(dt_sugar_parse_t *dp, int condid, dt_node_t *stmts)
{
dt_node_t *pred = NULL;
if (condid == 0) {
if (dp->dtsp_num_conditions != 0) {
pred = dt_node_op1(DT_TOK_LNEG,
dt_sugar_new_error_var());
}
} else {
pred = dt_node_op2(DT_TOK_LAND,
dt_node_op1(DT_TOK_LNEG, dt_sugar_new_error_var()),
dt_sugar_new_condition_var(condid));
}
dt_sugar_append_clause(dp,
dt_node_clause(dp->dtsp_pdescs, pred, stmts));
}
static void
dt_sugar_visit_stmts(dt_sugar_parse_t *dp, dt_node_t *stmts, int precondition)
{
dt_node_t *stmt;
dt_node_t *prev_stmt = NULL;
dt_node_t *next_stmt;
dt_node_t *first_stmt_in_basic_block = NULL;
for (stmt = stmts; stmt != NULL; stmt = next_stmt) {
next_stmt = stmt->dn_list;
if (stmt->dn_kind != DT_NODE_IF) {
if (first_stmt_in_basic_block == NULL)
first_stmt_in_basic_block = stmt;
prev_stmt = stmt;
continue;
}
if (prev_stmt != NULL)
prev_stmt->dn_list = NULL;
if (first_stmt_in_basic_block != NULL) {
dt_sugar_new_basic_block(dp, precondition,
first_stmt_in_basic_block);
}
dt_sugar_do_if(dp, stmt, precondition);
first_stmt_in_basic_block = NULL;
prev_stmt = stmt;
}
if (first_stmt_in_basic_block != NULL) {
dt_sugar_new_basic_block(dp, precondition,
first_stmt_in_basic_block);
}
}
static dt_node_t *
dt_sugar_makeerrorclause(void)
{
dt_node_t *acts, *pdesc;
pdesc = dt_node_pdesc_by_name(strdup("dtrace:::ERROR"));
acts = dt_node_statement(dt_node_op2(DT_TOK_ASGN,
dt_sugar_new_error_var(), dt_node_int(1)));
return (dt_node_clause(pdesc, NULL, acts));
}
dt_node_t *
dt_compile_sugar(dtrace_hdl_t *dtp, dt_node_t *clause)
{
dt_sugar_parse_t dp = { 0 };
int condid = 0;
dp.dtsp_dtp = dtp;
dp.dtsp_pdescs = clause->dn_pdescs;
yyintdecimal = B_TRUE;
yyintsuffix[0] = '\0';
yyintprefix = 0;
dt_sugar_visit_all(&dp, clause);
if (dp.dtsp_num_ifs == 0 && dp.dtsp_num_conditions == 0) {
dt_sugar_append_clause(&dp, dt_node_clause(clause->dn_pdescs,
clause->dn_pred, clause->dn_acts));
} else {
if (clause->dn_pred != NULL) {
condid = dt_sugar_new_condition(&dp,
clause->dn_pred, condid);
}
if (clause->dn_acts == NULL) {
dt_sugar_new_basic_block(&dp, condid, NULL);
} else {
dt_sugar_visit_stmts(&dp, clause->dn_acts, condid);
}
}
if (dp.dtsp_num_conditions != 0) {
dt_sugar_prepend_clause(&dp,
dt_sugar_new_clearerror_clause(&dp));
}
if (dp.dtsp_clause_list != NULL &&
dp.dtsp_clause_list->dn_list != NULL && !dtp->dt_has_sugar) {
dtp->dt_has_sugar = B_TRUE;
dt_sugar_prepend_clause(&dp, dt_sugar_makeerrorclause());
}
return (dp.dtsp_clause_list);
}
