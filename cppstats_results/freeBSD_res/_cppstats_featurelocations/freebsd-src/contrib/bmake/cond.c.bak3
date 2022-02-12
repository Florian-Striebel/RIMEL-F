


























































































#include <errno.h>

#include "make.h"
#include "dir.h"


MAKE_RCSID("$NetBSD: cond.c,v 1.302 2021/12/12 09:36:00 rillig Exp $");






























typedef enum Token {
TOK_FALSE, TOK_TRUE, TOK_AND, TOK_OR, TOK_NOT,
TOK_LPAREN, TOK_RPAREN, TOK_EOF, TOK_NONE, TOK_ERROR
} Token;

typedef enum CondResult {
CR_FALSE, CR_TRUE, CR_ERROR
} CondResult;

typedef enum ComparisonOp {
LT, LE, GT, GE, EQ, NE
} ComparisonOp;

typedef struct CondParser {






bool plain;


bool (*evalBare)(const char *);
bool negateEvalBare;













bool leftUnquotedOK;

const char *p;
Token curr;







bool printedError;
} CondParser;

static CondResult CondParser_Or(CondParser *par, bool);

static unsigned int cond_depth = 0;
static unsigned int cond_min_depth = 0;


static const char opname[][3] = { "<", "<=", ">", ">=", "==", "!=" };

static bool
is_token(const char *str, const char *tok, unsigned char len)
{
return strncmp(str, tok, (size_t)len) == 0 && !ch_isalpha(str[len]);
}

static Token
ToToken(bool cond)
{
return cond ? TOK_TRUE : TOK_FALSE;
}

static void
CondParser_SkipWhitespace(CondParser *par)
{
cpp_skip_whitespace(&par->p);
}

















static size_t
ParseWord(CondParser *par, const char **pp, bool doEval, const char *func,
char **out_arg)
{
const char *p = *pp;
Buffer argBuf;
int paren_depth;
size_t argLen;

if (func != NULL)
p++;

cpp_skip_hspace(&p);

Buf_InitSize(&argBuf, 16);

paren_depth = 0;
for (;;) {
char ch = *p;
if (ch == '\0' || ch == ' ' || ch == '\t')
break;
if ((ch == '&' || ch == '|') && paren_depth == 0)
break;
if (*p == '$') {








VarEvalMode emode = doEval
? VARE_UNDEFERR
: VARE_PARSE_ONLY;
FStr nestedVal;
(void)Var_Parse(&p, SCOPE_CMDLINE, emode, &nestedVal);

Buf_AddStr(&argBuf, nestedVal.str);
FStr_Done(&nestedVal);
continue;
}
if (ch == '(')
paren_depth++;
else if (ch == ')' && --paren_depth < 0)
break;
Buf_AddByte(&argBuf, *p);
p++;
}

argLen = argBuf.len;
*out_arg = Buf_DoneData(&argBuf);

cpp_skip_hspace(&p);

if (func != NULL && *p++ != ')') {
Parse_Error(PARSE_FATAL,
"Missing closing parenthesis for %s()", func);
par->printedError = true;
return 0;
}

*pp = p;
return argLen;
}


static bool
FuncDefined(const char *arg)
{
FStr value = Var_Value(SCOPE_CMDLINE, arg);
bool result = value.str != NULL;
FStr_Done(&value);
return result;
}


static bool
FuncMake(const char *arg)
{
StringListNode *ln;

for (ln = opts.create.first; ln != NULL; ln = ln->next)
if (Str_Match(ln->datum, arg))
return true;
return false;
}


static bool
FuncExists(const char *arg)
{
bool result;
char *path;

path = Dir_FindFile(arg, &dirSearchPath);
DEBUG2(COND, "exists(%s) result is \"%s\"\n",
arg, path != NULL ? path : "");
result = path != NULL;
free(path);
return result;
}


static bool
FuncTarget(const char *arg)
{
GNode *gn = Targ_FindNode(arg);
return gn != NULL && GNode_IsTarget(gn);
}





static bool
FuncCommands(const char *arg)
{
GNode *gn = Targ_FindNode(arg);
return gn != NULL && GNode_IsTarget(gn) && !Lst_IsEmpty(&gn->commands);
}










static bool
TryParseNumber(const char *str, double *out_value)
{
char *end;
unsigned long ul_val;
double dbl_val;

if (str[0] == '\0') {
*out_value = 0.0;
return true;
}

errno = 0;
ul_val = strtoul(str, &end, str[1] == 'x' ? 16 : 10);
if (*end == '\0' && errno != ERANGE) {
*out_value = str[0] == '-' ? -(double)-ul_val : (double)ul_val;
return true;
}

if (*end != '\0' && *end != '.' && *end != 'e' && *end != 'E')
return false;
dbl_val = strtod(str, &end);
if (*end != '\0')
return false;

*out_value = dbl_val;
return true;
}

static bool
is_separator(char ch)
{
return ch == '\0' || ch_isspace(ch) || ch == '!' || ch == '=' ||
ch == '>' || ch == '<' || ch == ')' ;
}







static bool
CondParser_StringExpr(CondParser *par, const char *start,
bool doEval, bool quoted,
Buffer *buf, FStr *inout_str)
{
VarEvalMode emode;
const char *nested_p;
bool atStart;
VarParseResult parseResult;

emode = doEval && quoted ? VARE_WANTRES
: doEval ? VARE_UNDEFERR
: VARE_PARSE_ONLY;

nested_p = par->p;
atStart = nested_p == start;
parseResult = Var_Parse(&nested_p, SCOPE_CMDLINE, emode, inout_str);

if (inout_str->str == var_Error) {
if (parseResult == VPR_ERR) {






par->printedError = true;
}




FStr_Done(inout_str);




*inout_str = FStr_InitRefer(NULL);
return false;
}
par->p = nested_p;






if (atStart && is_separator(par->p[0]))
return false;

Buf_AddStr(buf, inout_str->str);
FStr_Done(inout_str);
*inout_str = FStr_InitRefer(NULL);
return true;
}










static void
CondParser_Leaf(CondParser *par, bool doEval, bool unquotedOK,
FStr *out_str, bool *out_quoted)
{
Buffer buf;
FStr str;
bool quoted;
const char *start;

Buf_Init(&buf);
str = FStr_InitRefer(NULL);
*out_quoted = quoted = par->p[0] == '"';
start = par->p;
if (quoted)
par->p++;

while (par->p[0] != '\0' && str.str == NULL) {
switch (par->p[0]) {
case '\\':
par->p++;
if (par->p[0] != '\0') {
Buf_AddByte(&buf, par->p[0]);
par->p++;
}
continue;
case '"':
par->p++;
if (quoted)
goto got_str;
Buf_AddByte(&buf, '"');
continue;
case ')':
case '!':
case '=':
case '>':
case '<':
case ' ':
case '\t':
if (!quoted)
goto got_str;
Buf_AddByte(&buf, par->p[0]);
par->p++;
continue;
case '$':
if (!CondParser_StringExpr(par,
start, doEval, quoted, &buf, &str))
goto cleanup;
continue;
default:
if (!unquotedOK && !quoted && *start != '$' &&
!ch_isdigit(*start)) {




str = FStr_InitRefer(NULL);
goto cleanup;
}
Buf_AddByte(&buf, par->p[0]);
par->p++;
continue;
}
}
got_str:
str = FStr_InitOwn(buf.data);
buf.data = NULL;
cleanup:
Buf_Done(&buf);
*out_str = str;
}

static bool
EvalBare(const CondParser *par, const char *arg)
{
bool res = par->evalBare(arg);
return par->negateEvalBare ? !res : res;
}





static bool
EvalNotEmpty(CondParser *par, const char *value, bool quoted)
{
double num;


if (quoted)
return value[0] != '\0';


if (TryParseNumber(value, &num))
return num != 0.0;








if (par->plain)
return value[0] != '\0';

return EvalBare(par, value);
}


static bool
EvalCompareNum(double lhs, ComparisonOp op, double rhs)
{
DEBUG3(COND, "lhs = %f, rhs = %f, op = %.2s\n", lhs, rhs, opname[op]);

switch (op) {
case LT:
return lhs < rhs;
case LE:
return lhs <= rhs;
case GT:
return lhs > rhs;
case GE:
return lhs >= rhs;
case NE:
return lhs != rhs;
default:
return lhs == rhs;
}
}

static Token
EvalCompareStr(CondParser *par, const char *lhs,
ComparisonOp op, const char *rhs)
{
if (op != EQ && op != NE) {
Parse_Error(PARSE_FATAL,
"String comparison operator must be either == or !=");
par->printedError = true;
return TOK_ERROR;
}

DEBUG3(COND, "lhs = \"%s\", rhs = \"%s\", op = %.2s\n",
lhs, rhs, opname[op]);
return ToToken((op == EQ) == (strcmp(lhs, rhs) == 0));
}


static Token
EvalCompare(CondParser *par, const char *lhs, bool lhsQuoted,
ComparisonOp op, const char *rhs, bool rhsQuoted)
{
double left, right;

if (!rhsQuoted && !lhsQuoted)
if (TryParseNumber(lhs, &left) && TryParseNumber(rhs, &right))
return ToToken(EvalCompareNum(left, op, right));

return EvalCompareStr(par, lhs, op, rhs);
}

static bool
CondParser_ComparisonOp(CondParser *par, ComparisonOp *out_op)
{
const char *p = par->p;

if (p[0] == '<' && p[1] == '=') {
*out_op = LE;
goto length_2;
} else if (p[0] == '<') {
*out_op = LT;
goto length_1;
} else if (p[0] == '>' && p[1] == '=') {
*out_op = GE;
goto length_2;
} else if (p[0] == '>') {
*out_op = GT;
goto length_1;
} else if (p[0] == '=' && p[1] == '=') {
*out_op = EQ;
goto length_2;
} else if (p[0] == '!' && p[1] == '=') {
*out_op = NE;
goto length_2;
}
return false;

length_2:
par->p = p + 2;
return true;
length_1:
par->p = p + 1;
return true;
}









static Token
CondParser_Comparison(CondParser *par, bool doEval)
{
Token t = TOK_ERROR;
FStr lhs, rhs;
ComparisonOp op;
bool lhsQuoted, rhsQuoted;

CondParser_Leaf(par, doEval, par->leftUnquotedOK, &lhs, &lhsQuoted);
if (lhs.str == NULL)
goto done_lhs;

CondParser_SkipWhitespace(par);

if (!CondParser_ComparisonOp(par, &op)) {

t = ToToken(doEval && EvalNotEmpty(par, lhs.str, lhsQuoted));
goto done_lhs;
}

CondParser_SkipWhitespace(par);

if (par->p[0] == '\0') {
Parse_Error(PARSE_FATAL,
"Missing right-hand side of operator '%s'", opname[op]);
par->printedError = true;
goto done_lhs;
}

CondParser_Leaf(par, doEval, true, &rhs, &rhsQuoted);
if (rhs.str == NULL)
goto done_rhs;

if (!doEval) {
t = TOK_FALSE;
goto done_rhs;
}

t = EvalCompare(par, lhs.str, lhsQuoted, op, rhs.str, rhsQuoted);

done_rhs:
FStr_Done(&rhs);
done_lhs:
FStr_Done(&lhs);
return t;
}





static bool
CondParser_FuncCallEmpty(CondParser *par, bool doEval, Token *out_token)
{
const char *cp = par->p;
Token tok;
FStr val;

if (!is_token(cp, "empty", 5))
return false;
cp += 5;

cpp_skip_whitespace(&cp);
if (*cp != '(')
return false;

cp--;
(void)Var_Parse(&cp, SCOPE_CMDLINE,
doEval ? VARE_WANTRES : VARE_PARSE_ONLY, &val);


if (val.str == var_Error)
tok = TOK_ERROR;
else {
cpp_skip_whitespace(&val.str);
tok = val.str[0] != '\0' && doEval ? TOK_FALSE : TOK_TRUE;
}

FStr_Done(&val);
*out_token = tok;
par->p = cp;
return true;
}


static bool
CondParser_FuncCall(CondParser *par, bool doEval, Token *out_token)
{
static const struct fn_def {
const char fn_name[9];
unsigned char fn_name_len;
bool (*fn_eval)(const char *);
} fns[] = {
{ "defined", 7, FuncDefined },
{ "make", 4, FuncMake },
{ "exists", 6, FuncExists },
{ "target", 6, FuncTarget },
{ "commands", 8, FuncCommands }
};
const struct fn_def *fn;
char *arg = NULL;
size_t arglen;
const char *cp = par->p;
const struct fn_def *last_fn = fns + sizeof fns / sizeof fns[0] - 1;

for (fn = fns; !is_token(cp, fn->fn_name, fn->fn_name_len); fn++)
if (fn == last_fn)
return false;

cp += fn->fn_name_len;
cpp_skip_whitespace(&cp);
if (*cp != '(')
return false;

arglen = ParseWord(par, &cp, doEval, fn->fn_name, &arg);
*out_token = ToToken(arglen != 0 && (!doEval || fn->fn_eval(arg)));

free(arg);
par->p = cp;
return true;
}








static Token
CondParser_ComparisonOrLeaf(CondParser *par, bool doEval)
{
Token t;
char *arg = NULL;
const char *cp;
const char *cp1;


cp = par->p;
if (ch_isdigit(cp[0]) || cp[0] == '-' || cp[0] == '+')
return CondParser_Comparison(par, doEval);













(void)ParseWord(par, &cp, doEval, NULL, &arg);
cp1 = cp;
cpp_skip_whitespace(&cp1);
if (*cp1 == '=' || *cp1 == '!' || *cp1 == '<' || *cp1 == '>')
return CondParser_Comparison(par, doEval);
par->p = cp;







t = ToToken(!doEval || EvalBare(par, arg));
free(arg);
return t;
}


static Token
CondParser_Token(CondParser *par, bool doEval)
{
Token t;

t = par->curr;
if (t != TOK_NONE) {
par->curr = TOK_NONE;
return t;
}

cpp_skip_hspace(&par->p);

switch (par->p[0]) {

case '(':
par->p++;
return TOK_LPAREN;

case ')':
par->p++;
return TOK_RPAREN;

case '|':
par->p++;
if (par->p[0] == '|')
par->p++;
else if (opts.strict) {
Parse_Error(PARSE_FATAL, "Unknown operator '|'");
par->printedError = true;
return TOK_ERROR;
}
return TOK_OR;

case '&':
par->p++;
if (par->p[0] == '&')
par->p++;
else if (opts.strict) {
Parse_Error(PARSE_FATAL, "Unknown operator '&'");
par->printedError = true;
return TOK_ERROR;
}
return TOK_AND;

case '!':
par->p++;
return TOK_NOT;

case '#':
case '\n':

case '\0':
return TOK_EOF;

case '"':
case '$':
return CondParser_Comparison(par, doEval);

default:
if (CondParser_FuncCallEmpty(par, doEval, &t))
return t;
if (CondParser_FuncCall(par, doEval, &t))
return t;
return CondParser_ComparisonOrLeaf(par, doEval);
}
}


static bool
CondParser_Skip(CondParser *par, Token t)
{
Token actual;

actual = CondParser_Token(par, false);
if (actual == t)
return true;

assert(par->curr == TOK_NONE);
assert(actual != TOK_NONE);
par->curr = actual;
return false;
}







static CondResult
CondParser_Term(CondParser *par, bool doEval)
{
CondResult res;
Token t;

t = CondParser_Token(par, doEval);
if (t == TOK_TRUE)
return CR_TRUE;
if (t == TOK_FALSE)
return CR_FALSE;

if (t == TOK_LPAREN) {
res = CondParser_Or(par, doEval);
if (res == CR_ERROR)
return CR_ERROR;
if (CondParser_Token(par, doEval) != TOK_RPAREN)
return CR_ERROR;
return res;
}

if (t == TOK_NOT) {
res = CondParser_Term(par, doEval);
if (res == CR_TRUE)
res = CR_FALSE;
else if (res == CR_FALSE)
res = CR_TRUE;
return res;
}

return CR_ERROR;
}




static CondResult
CondParser_And(CondParser *par, bool doEval)
{
CondResult res, rhs;

res = CR_TRUE;
do {
if ((rhs = CondParser_Term(par, doEval)) == CR_ERROR)
return CR_ERROR;
if (rhs == CR_FALSE) {
res = CR_FALSE;
doEval = false;
}
} while (CondParser_Skip(par, TOK_AND));

return res;
}




static CondResult
CondParser_Or(CondParser *par, bool doEval)
{
CondResult res, rhs;

res = CR_FALSE;
do {
if ((rhs = CondParser_And(par, doEval)) == CR_ERROR)
return CR_ERROR;
if (rhs == CR_TRUE) {
res = CR_TRUE;
doEval = false;
}
} while (CondParser_Skip(par, TOK_OR));

return res;
}

static CondEvalResult
CondParser_Eval(CondParser *par, bool *out_value)
{
CondResult res;

DEBUG1(COND, "CondParser_Eval: %s\n", par->p);

res = CondParser_Or(par, true);
if (res == CR_ERROR)
return COND_INVALID;

if (CondParser_Token(par, false) != TOK_EOF)
return COND_INVALID;

*out_value = res == CR_TRUE;
return COND_PARSE;
}












static CondEvalResult
CondEvalExpression(const char *cond, bool *out_value, bool plain,
bool (*evalBare)(const char *), bool negate,
bool eprint, bool leftUnquotedOK)
{
CondParser par;
CondEvalResult rval;

cpp_skip_hspace(&cond);

par.plain = plain;
par.evalBare = evalBare;
par.negateEvalBare = negate;
par.leftUnquotedOK = leftUnquotedOK;
par.p = cond;
par.curr = TOK_NONE;
par.printedError = false;

rval = CondParser_Eval(&par, out_value);

if (rval == COND_INVALID && eprint && !par.printedError)
Parse_Error(PARSE_FATAL, "Malformed conditional (%s)", cond);

return rval;
}





CondEvalResult
Cond_EvalCondition(const char *cond, bool *out_value)
{
return CondEvalExpression(cond, out_value, true,
FuncDefined, false, false, true);
}

static bool
IsEndif(const char *p)
{
return p[0] == 'e' && p[1] == 'n' && p[2] == 'd' &&
p[3] == 'i' && p[4] == 'f' && !ch_isalpha(p[5]);
}

static bool
DetermineKindOfConditional(const char **pp, bool *out_plain,
bool (**out_evalBare)(const char *),
bool *out_negate)
{
const char *p = *pp;

p += 2;
*out_plain = false;
*out_evalBare = FuncDefined;
*out_negate = false;
if (*p == 'n') {
p++;
*out_negate = true;
}
if (is_token(p, "def", 3)) {
p += 3;
} else if (is_token(p, "make", 4)) {
p += 4;
*out_evalBare = FuncMake;
} else if (is_token(p, "", 0) && !*out_negate) {
*out_plain = true;
} else {







return false;
}

*pp = p;
return true;
}































CondEvalResult
Cond_EvalLine(const char *line)
{
typedef enum IfState {


IFS_INITIAL = 0,



IFS_ACTIVE = 1 << 0,


IFS_SEEN_ELSE = 1 << 1,


IFS_WAS_ACTIVE = 1 << 2

} IfState;

static enum IfState *cond_states = NULL;
static unsigned int cond_states_cap = 128;

bool plain;
bool (*evalBare)(const char *);
bool negate;
bool isElif;
bool value;
IfState state;
const char *p = line;

if (cond_states == NULL) {
cond_states = bmake_malloc(
cond_states_cap * sizeof *cond_states);
cond_states[0] = IFS_ACTIVE;
}

p++;
cpp_skip_hspace(&p);

if (IsEndif(p)) {
if (p[5] != '\0') {
Parse_Error(PARSE_FATAL,
"The .endif directive does not take arguments");
}

if (cond_depth == cond_min_depth) {
Parse_Error(PARSE_FATAL, "if-less endif");
return COND_PARSE;
}


cond_depth--;
return cond_states[cond_depth] & IFS_ACTIVE
? COND_PARSE : COND_SKIP;
}


if (p[0] == 'e') {
if (p[1] != 'l') {





return COND_INVALID;
}


p += 2;
if (is_token(p, "se", 2)) {

if (p[2] != '\0')
Parse_Error(PARSE_FATAL,
"The .else directive "
"does not take arguments");

if (cond_depth == cond_min_depth) {
Parse_Error(PARSE_FATAL, "if-less else");
return COND_PARSE;
}

state = cond_states[cond_depth];
if (state == IFS_INITIAL) {
state = IFS_ACTIVE | IFS_SEEN_ELSE;
} else {
if (state & IFS_SEEN_ELSE)
Parse_Error(PARSE_WARNING,
"extra else");
state = IFS_WAS_ACTIVE | IFS_SEEN_ELSE;
}
cond_states[cond_depth] = state;

return state & IFS_ACTIVE ? COND_PARSE : COND_SKIP;
}

isElif = true;
} else
isElif = false;

if (p[0] != 'i' || p[1] != 'f') {




return COND_INVALID;
}

if (!DetermineKindOfConditional(&p, &plain, &evalBare, &negate))
return COND_INVALID;

if (isElif) {
if (cond_depth == cond_min_depth) {
Parse_Error(PARSE_FATAL, "if-less elif");
return COND_PARSE;
}
state = cond_states[cond_depth];
if (state & IFS_SEEN_ELSE) {
Parse_Error(PARSE_WARNING, "extra elif");
cond_states[cond_depth] =
IFS_WAS_ACTIVE | IFS_SEEN_ELSE;
return COND_SKIP;
}
if (state != IFS_INITIAL) {
cond_states[cond_depth] = IFS_WAS_ACTIVE;
return COND_SKIP;
}
} else {

if (cond_depth + 1 >= cond_states_cap) {





cond_states_cap += 32;
cond_states = bmake_realloc(cond_states,
cond_states_cap *
sizeof *cond_states);
}
state = cond_states[cond_depth];
cond_depth++;
if (!(state & IFS_ACTIVE)) {




cond_states[cond_depth] = IFS_WAS_ACTIVE;
return COND_SKIP;
}
}


if (CondEvalExpression(p, &value, plain, evalBare, negate,
true, false) == COND_INVALID) {



cond_states[cond_depth] = IFS_WAS_ACTIVE;
return COND_SKIP;
}

if (!value) {
cond_states[cond_depth] = IFS_INITIAL;
return COND_SKIP;
}
cond_states[cond_depth] = IFS_ACTIVE;
return COND_PARSE;
}

void
Cond_restore_depth(unsigned int saved_depth)
{
unsigned int open_conds = cond_depth - cond_min_depth;

if (open_conds != 0 || saved_depth > cond_depth) {
Parse_Error(PARSE_FATAL, "%u open conditional%s",
open_conds, open_conds == 1 ? "" : "s");
cond_depth = cond_min_depth;
}

cond_min_depth = saved_depth;
}

unsigned int
Cond_save_depth(void)
{
unsigned int depth = cond_min_depth;

cond_min_depth = cond_depth;
return depth;
}
