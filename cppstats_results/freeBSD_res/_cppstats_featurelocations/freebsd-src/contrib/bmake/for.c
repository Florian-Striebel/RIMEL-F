
























































#include "make.h"


MAKE_RCSID("$NetBSD: for.c,v 1.150 2021/12/12 15:44:41 rillig Exp $");



typedef struct ForVar {
char *name;
size_t nameLen;
} ForVar;

typedef struct ForLoop {
Buffer body;
Vector vars;
SubstringWords items;
Buffer curBody;
unsigned int nextItem;
} ForLoop;


static ForLoop *accumFor;
static int forLevel = 0;


static ForLoop *
ForLoop_New(void)
{
ForLoop *f = bmake_malloc(sizeof *f);

Buf_Init(&f->body);
Vector_Init(&f->vars, sizeof(ForVar));
SubstringWords_Init(&f->items);
Buf_Init(&f->curBody);
f->nextItem = 0;

return f;
}

static void
ForLoop_Free(ForLoop *f)
{
Buf_Done(&f->body);

while (f->vars.len > 0) {
ForVar *var = Vector_Pop(&f->vars);
free(var->name);
}
Vector_Done(&f->vars);

SubstringWords_Free(f->items);
Buf_Done(&f->curBody);

free(f);
}

static void
ForLoop_AddVar(ForLoop *f, const char *name, size_t len)
{
ForVar *var = Vector_Push(&f->vars);
var->name = bmake_strldup(name, len);
var->nameLen = len;
}

static bool
ForLoop_ParseVarnames(ForLoop *f, const char **pp)
{
const char *p = *pp;

for (;;) {
size_t len;

cpp_skip_whitespace(&p);
if (*p == '\0') {
Parse_Error(PARSE_FATAL, "missing `in' in for");
return false;
}





for (len = 1; p[len] != '\0' && !ch_isspace(p[len]); len++)
continue;

if (len == 2 && p[0] == 'i' && p[1] == 'n') {
p += 2;
break;
}

ForLoop_AddVar(f, p, len);
p += len;
}

if (f->vars.len == 0) {
Parse_Error(PARSE_FATAL, "no iteration variables in for");
return false;
}

*pp = p;
return true;
}

static bool
ForLoop_ParseItems(ForLoop *f, const char *p)
{
char *items;

cpp_skip_whitespace(&p);

if (Var_Subst(p, SCOPE_GLOBAL, VARE_WANTRES, &items) != VPR_OK) {
Parse_Error(PARSE_FATAL, "Error in .for loop items");
return false;
}

f->items = Substring_Words(items, false);
free(items);

if (f->items.len == 1 && Substring_IsEmpty(f->items.words[0]))
f->items.len = 0;

if (f->items.len != 0 && f->items.len % f->vars.len != 0) {
Parse_Error(PARSE_FATAL,
"Wrong number of words (%u) in .for "
"substitution list with %u variables",
(unsigned)f->items.len, (unsigned)f->vars.len);
return false;
}

return true;
}

static bool
IsFor(const char *p)
{
return p[0] == 'f' && p[1] == 'o' && p[2] == 'r' && ch_isspace(p[3]);
}

static bool
IsEndfor(const char *p)
{
return p[0] == 'e' && strncmp(p, "endfor", 6) == 0 &&
(p[6] == '\0' || ch_isspace(p[6]));
}













int
For_Eval(const char *line)
{
ForLoop *f;
const char *p;

p = line + 1;
cpp_skip_whitespace(&p);

if (!IsFor(p)) {
if (IsEndfor(p)) {
Parse_Error(PARSE_FATAL, "for-less endfor");
return -1;
}
return 0;
}
p += 3;

f = ForLoop_New();

if (!ForLoop_ParseVarnames(f, &p)) {
ForLoop_Free(f);
return -1;
}

if (!ForLoop_ParseItems(f, p)) {

f->items.len = 0;
}

accumFor = f;
forLevel = 1;
return 1;
}





bool
For_Accum(const char *line)
{
const char *p = line;

if (*p == '.') {
p++;
cpp_skip_whitespace(&p);

if (IsEndfor(p)) {
DEBUG1(FOR, "For: end for %d\n", forLevel);
if (--forLevel <= 0)
return false;
} else if (IsFor(p)) {
forLevel++;
DEBUG1(FOR, "For: new loop %d\n", forLevel);
}
}

Buf_AddStr(&accumFor->body, line);
Buf_AddByte(&accumFor->body, '\n');
return true;
}


static size_t
ExprLen(const char *s, const char *e)
{
char expr_open, expr_close;
int depth;
const char *p;

if (s == e)
return 0;

expr_open = s[0];
if (expr_open == '(')
expr_close = ')';
else if (expr_open == '{')
expr_close = '}';
else
return 1;

depth = 1;
for (p = s + 1; p != e; p++) {
if (*p == expr_open)
depth++;
else if (*p == expr_close && --depth == 0)
return (size_t)(p + 1 - s);
}


return 0;
}





static bool
NeedsEscapes(Substring value, char endc)
{
const char *p;

for (p = value.start; p != value.end; p++) {
if (*p == ':' || *p == '$' || *p == '\\' || *p == endc ||
*p == '\n')
return true;
}
return false;
}







static void
Buf_AddEscaped(Buffer *cmds, Substring item, char endc)
{
const char *p;
char ch;

if (!NeedsEscapes(item, endc)) {
Buf_AddBytesBetween(cmds, item.start, item.end);
return;
}



for (p = item.start; p != item.end; p++) {
ch = *p;
if (ch == '$') {
size_t len = ExprLen(p + 1, item.end);
if (len != 0) {




Buf_AddBytes(cmds, p, 1 + len);
p += len;
continue;
}
Buf_AddByte(cmds, '\\');
} else if (ch == ':' || ch == '\\' || ch == endc)
Buf_AddByte(cmds, '\\');
else if (ch == '\n') {
Parse_Error(PARSE_FATAL, "newline in .for value");
ch = ' ';
}
Buf_AddByte(cmds, ch);
}
}





static void
ForLoop_SubstVarLong(ForLoop *f, const char **pp, const char *bodyEnd,
char endc, const char **inout_mark)
{
size_t i;
const char *p = *pp;

for (i = 0; i < f->vars.len; i++) {
const ForVar *forVar = Vector_Get(&f->vars, i);
const char *varname = forVar->name;
size_t varnameLen = forVar->nameLen;

if (varnameLen >= (size_t)(bodyEnd - p))
continue;
if (memcmp(p, varname, varnameLen) != 0)
continue;

if (p[varnameLen] != ':' && p[varnameLen] != endc &&
p[varnameLen] != '\\')
continue;





Buf_AddBytesBetween(&f->curBody, *inout_mark, p);
Buf_AddStr(&f->curBody, ":U");
Buf_AddEscaped(&f->curBody,
f->items.words[f->nextItem + i], endc);

p += varnameLen;
*inout_mark = p;
*pp = p;
return;
}
}





static void
ForLoop_SubstVarShort(ForLoop *f, const char *p, const char **inout_mark)
{
const char ch = *p;
const ForVar *vars;
size_t i;


if (ch == '}' || ch == ')' || ch == ':' || ch == '$')
return;

vars = Vector_Get(&f->vars, 0);
for (i = 0; i < f->vars.len; i++) {
const char *varname = vars[i].name;
if (varname[0] == ch && varname[1] == '\0')
goto found;
}
return;

found:
Buf_AddBytesBetween(&f->curBody, *inout_mark, p);
*inout_mark = p + 1;


Buf_AddStr(&f->curBody, "{:U");
Buf_AddEscaped(&f->curBody, f->items.words[f->nextItem + i], '}');
Buf_AddByte(&f->curBody, '}');
}














static void
ForLoop_SubstBody(ForLoop *f)
{
const char *p, *bodyEnd;
const char *mark;

Buf_Empty(&f->curBody);

mark = f->body.data;
bodyEnd = f->body.data + f->body.len;
for (p = mark; (p = strchr(p, '$')) != NULL;) {
if (p[1] == '{' || p[1] == '(') {
char endc = p[1] == '{' ? '}' : ')';
p += 2;
ForLoop_SubstVarLong(f, &p, bodyEnd, endc, &mark);
} else if (p[1] != '\0') {
ForLoop_SubstVarShort(f, p + 1, &mark);
p += 2;
} else
break;
}

Buf_AddBytesBetween(&f->curBody, mark, bodyEnd);
}





static char *
ForReadMore(void *v_arg, size_t *out_len)
{
ForLoop *f = v_arg;

if (f->nextItem == f->items.len) {

ForLoop_Free(f);
return NULL;
}

ForLoop_SubstBody(f);
DEBUG1(FOR, "For: loop body:\n%s", f->curBody.data);
f->nextItem += (unsigned int)f->vars.len;

*out_len = f->curBody.len;
return f->curBody.data;
}


void
For_Run(int lineno)
{
ForLoop *f = accumFor;
accumFor = NULL;

if (f->items.len == 0) {




ForLoop_Free(f);
return;
}

Parse_PushInput(NULL, lineno, -1, ForReadMore, f);
}
