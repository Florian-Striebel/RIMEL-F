











































































#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <errno.h>
#if defined(HAVE_ERR_H)
#include <err.h>
#endif
#include <sys/queue.h>
#include "support.h"
#include "asn1.h"
#include "snmp.h"
#include "snmpagent.h"




static const asn_subid_t prefix[] = { 1, 3, 6 };
#define PREFIX_LEN (sizeof(prefix) / sizeof(prefix[0]))

u_int tree_size;
static const char *file_prefix = "";


static int localincs = 0;


static int debug;

static const char usgtxt[] = "\
Generate SNMP tables.\n\
usage: gensnmptree [-dEeFfhlt] [-I directory] [-i infile] [-p prefix]\n\
[name]...\n\
options:\n\
-d debug mode\n\
-E extract the named or all enums and bits only\n\
-e extract the named oids or enums\n\
-F generate functions for -E into a .c file\n\
-f generate functions for -E into the header\n\
-h print this info\n\
-I directory add directory to include path\n\
-i ifile read from the named file instead of stdin\n\
-l generate local include directives\n\
-p prefix prepend prefix to file and variable names\n\
-t generate a .def file\n\
";




enum op {

OP_GEN,


OP_EXTRACT,


OP_TREE,


OP_ENUMS,
};




enum gen_funcs {

GEN_FUNCS_NONE,


GEN_FUNCS_H,


GEN_FUNCS_C,
};




enum ntype {
NODE_LEAF = 1,
NODE_TREE,
NODE_ENTRY,
NODE_COLUMN
};

enum {
FL_GET = 0x01,
FL_SET = 0x02,
};

struct node;
TAILQ_HEAD(node_list, node);

struct node {
enum ntype type;
asn_subid_t id;
char *name;
TAILQ_ENTRY(node) link;
u_int lno;
u_int flags;

union {
struct tree {
struct node_list subs;
} tree;

struct entry {
uint32_t index;
char *func;
struct node_list subs;
char *subtypes[SNMP_INDEXES_MAX];
} entry;

struct leaf {
enum snmp_syntax syntax;
char *func;
char *subtype;
} leaf;

struct column {
enum snmp_syntax syntax;
char *subtype;
} column;
} u;
};

struct func {
const char *name;
LIST_ENTRY(func) link;
};

static LIST_HEAD(, func) funcs = LIST_HEAD_INITIALIZER(funcs);

struct enums {
const char *name;
long value;
TAILQ_ENTRY(enums) link;
};

struct type {
const char *name;
const char *from_fname;
u_int from_lno;
u_int syntax;
int is_enum;
int is_bits;
TAILQ_HEAD(, enums) enums;
LIST_ENTRY(type) link;
};

static LIST_HEAD(, type) types = LIST_HEAD_INITIALIZER(types);

static void report(const char *, ...) __dead2 __printflike(1, 2);
static void report_node(const struct node *, const char *, ...)
__dead2 __printflike(2, 3);





static void *
xalloc(size_t size)
{
void *ptr;

if ((ptr = calloc(1, size)) == NULL)
err(1, "allocing %zu bytes", size);

return (ptr);
}

static char *
savestr(const char *s)
{

if (s == NULL)
return (NULL);
return (strcpy(xalloc(strlen(s) + 1), s));
}





struct input {
FILE *fp;
u_int lno;
char *fname;
char *path;
LIST_ENTRY(input) link;
};
static LIST_HEAD(, input) inputs = LIST_HEAD_INITIALIZER(inputs);
static struct input *input = NULL;

#define MAX_PATHS 100
static u_int npaths = 2;
static u_int stdpaths = 2;
static const char *paths[MAX_PATHS + 1] = {
"/usr/share/snmp/defs",
"/usr/local/share/snmp/defs",
NULL
};

static int pbchar = -1;

static void
path_new(const char *path)
{
if (npaths >= MAX_PATHS)
report("too many -I directives");
memmove(&paths[npaths - stdpaths + 1], &paths[npaths - stdpaths],
sizeof(path[0]) * stdpaths);
paths[npaths - stdpaths] = savestr(path);
npaths++;
}

static void
input_new(FILE *fp, const char *path, const char *fname)
{
struct input *ip;

ip = xalloc(sizeof(*ip));
ip->fp = fp;
ip->lno = 1;
ip->fname = savestr(fname);
ip->path = savestr(path);
LIST_INSERT_HEAD(&inputs, ip, link);

input = ip;
}

static void
input_close(void)
{

if (input == NULL)
return;
fclose(input->fp);
free(input->fname);
free(input->path);
LIST_REMOVE(input, link);
free(input);

input = LIST_FIRST(&inputs);
}

static FILE *
tryopen(const char *path, const char *fname)
{
char *fn;
FILE *fp;

if (path == NULL)
fn = savestr(fname);
else {
fn = xalloc(strlen(path) + strlen(fname) + 2);
sprintf(fn, "%s/%s", path, fname);
}
fp = fopen(fn, "r");
free(fn);
return (fp);
}

static void
input_fopen(const char *fname, int loc)
{
FILE *fp;
char *path;
u_int p;

if (fname[0] == '/') {
if ((fp = tryopen(NULL, fname)) != NULL) {
input_new(fp, NULL, fname);
return;
}

} else {
if (loc) {
if (input == NULL)
path = NULL;
else
path = input->path;

if ((fp = tryopen(path, fname)) != NULL) {
input_new(fp, NULL, fname);
return;
}
}

for (p = 0; paths[p] != NULL; p++)
if ((fp = tryopen(paths[p], fname)) != NULL) {
input_new(fp, paths[p], fname);
return;
}
}
report("cannot open '%s'", fname);
}

static int
tgetc(void)
{
int c;

if (pbchar != -1) {
c = pbchar;
pbchar = -1;
return (c);
}

for (;;) {
if (input == NULL)
return (EOF);

if ((c = getc(input->fp)) != EOF)
return (c);

input_close();
}
}

static void
tungetc(int c)
{

if (pbchar != -1)
abort();
pbchar = c;
}





enum tok {
TOK_EOF = 0200,
TOK_NUM,
TOK_STR,
TOK_ACCESS,
TOK_TYPE,
TOK_ENUM,
TOK_TYPEDEF,
TOK_DEFTYPE,
TOK_INCLUDE,
TOK_FILENAME,
TOK_BITS,
};

static const struct {
const char *str;
enum tok tok;
u_int val;
} keywords[] = {
{ "GET", TOK_ACCESS, FL_GET },
{ "SET", TOK_ACCESS, FL_SET },
{ "NULL", TOK_TYPE, SNMP_SYNTAX_NULL },
{ "INTEGER", TOK_TYPE, SNMP_SYNTAX_INTEGER },
{ "INTEGER32", TOK_TYPE, SNMP_SYNTAX_INTEGER },
{ "UNSIGNED32", TOK_TYPE, SNMP_SYNTAX_GAUGE },
{ "OCTETSTRING", TOK_TYPE, SNMP_SYNTAX_OCTETSTRING },
{ "IPADDRESS", TOK_TYPE, SNMP_SYNTAX_IPADDRESS },
{ "OID", TOK_TYPE, SNMP_SYNTAX_OID },
{ "TIMETICKS", TOK_TYPE, SNMP_SYNTAX_TIMETICKS },
{ "COUNTER", TOK_TYPE, SNMP_SYNTAX_COUNTER },
{ "GAUGE", TOK_TYPE, SNMP_SYNTAX_GAUGE },
{ "COUNTER64", TOK_TYPE, SNMP_SYNTAX_COUNTER64 },
{ "ENUM", TOK_ENUM, SNMP_SYNTAX_INTEGER },
{ "BITS", TOK_BITS, SNMP_SYNTAX_OCTETSTRING },
{ "typedef", TOK_TYPEDEF, 0 },
{ "include", TOK_INCLUDE, 0 },
{ NULL, 0, 0 }
};


#define MAXSTR 1000
static char str[MAXSTR];
static u_long val;
static int saved_token = -1;




static void
report(const char *fmt, ...)
{
va_list ap;
int c;

va_start(ap, fmt);
fprintf(stderr, "line %u: ", input->lno);
vfprintf(stderr, fmt, ap);
fprintf(stderr, "\n");
fprintf(stderr, "context: \"");
while ((c = tgetc()) != EOF && c != '\n')
fprintf(stderr, "%c", c);
fprintf(stderr, "\n");
va_end(ap);
exit(1);
}
static void
report_node(const struct node *np, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
fprintf(stderr, "line %u, node %s: ", np->lno, np->name);
vfprintf(stderr, fmt, ap);
fprintf(stderr, "\n");
va_end(ap);
exit(1);
}




static char *
savetok(void)
{
return (savestr(str));
}




static int
gettoken_internal(void)
{
int c;
struct type *t;

if (saved_token != -1) {
c = saved_token;
saved_token = -1;
return (c);
}

again:



while ((c = tgetc()) != EOF) {
if (c == '\n')
input->lno++;
if (!isspace(c))
break;
}
if (c == EOF)
return (TOK_EOF);
if (!isascii(c))
report("unexpected character %#2x", (u_int)c);




if (c == '#') {
while ((c = tgetc()) != EOF) {
if (c == '\n') {
input->lno++;
goto again;
}
}
report("unexpected EOF in comment");
}




if (strchr("():|-", c) != NULL)
return (c);

if (c == '"' || c == '<') {
int end = c;
size_t n = 0;

val = 1;
if (c == '<') {
val = 0;
end = '>';
}

while ((c = tgetc()) != EOF) {
if (c == end)
break;
if (n == sizeof(str) - 1) {
str[n++] = '\0';
report("filename too long '%s...'", str);
}
str[n++] = c;
}
str[n++] = '\0';
return (TOK_FILENAME);
}




if (isdigit(c)) {
size_t n = 0;
str[n++] = c;
while ((c = tgetc()) != EOF) {
if (!isdigit(c)) {
tungetc(c);
break;
}
if (n == sizeof(str) - 1) {
str[n++] = '\0';
report("number too long '%s...'", str);
}
str[n++] = c;
}
str[n++] = '\0';
sscanf(str, "%lu", &val);
return (TOK_NUM);
}




if (isalpha(c) || c == '_') {
size_t n = 0;
str[n++] = c;
while ((c = tgetc()) != EOF) {
if (!isalnum(c) && c != '_' && c != '-') {
tungetc(c);
break;
}
if (n == sizeof(str) - 1) {
str[n++] = '\0';
report("string too long '%s...'", str);
}
str[n++] = c;
}
str[n++] = '\0';




for (c = 0; keywords[c].str != NULL; c++)
if (strcmp(keywords[c].str, str) == 0) {
val = keywords[c].val;
return (keywords[c].tok);
}

LIST_FOREACH(t, &types, link) {
if (strcmp(t->name, str) == 0) {
val = t->syntax;
return (TOK_DEFTYPE);
}
}
return (TOK_STR);
}
if (isprint(c))
errx(1, "%u: unexpected character '%c'", input->lno, c);
else
errx(1, "%u: unexpected character 0x%02x", input->lno,
(u_int)c);
}
static int
gettoken(void)
{
int tok = gettoken_internal();

if (debug) {
switch (tok) {

case TOK_EOF:
fprintf(stderr, "EOF ");
break;

case TOK_NUM:
fprintf(stderr, "NUM(%lu) ", val);
break;

case TOK_STR:
fprintf(stderr, "STR(%s) ", str);
break;

case TOK_ACCESS:
fprintf(stderr, "ACCESS(%lu) ", val);
break;

case TOK_TYPE:
fprintf(stderr, "TYPE(%lu) ", val);
break;

case TOK_ENUM:
fprintf(stderr, "ENUM ");
break;

case TOK_BITS:
fprintf(stderr, "BITS ");
break;

case TOK_TYPEDEF:
fprintf(stderr, "TYPEDEF ");
break;

case TOK_DEFTYPE:
fprintf(stderr, "DEFTYPE(%s,%lu) ", str, val);
break;

case TOK_INCLUDE:
fprintf(stderr, "INCLUDE ");
break;

case TOK_FILENAME:
fprintf(stderr, "FILENAME ");
break;

default:
if (tok < TOK_EOF) {
if (isprint(tok))
fprintf(stderr, "'%c' ", tok);
else if (tok == '\n')
fprintf(stderr, "\n");
else
fprintf(stderr, "%02x ", tok);
} else
abort();
break;
}
}
return (tok);
}




static void
pushback(enum tok tok)
{

if (saved_token != -1)
abort();
saved_token = tok;
}




static struct type *
make_type(const char *s)
{
struct type *t;

t = xalloc(sizeof(*t));
t->name = savestr(s);
t->is_enum = 0;
t->syntax = SNMP_SYNTAX_NULL;
t->from_fname = savestr(input->fname);
t->from_lno = input->lno;
TAILQ_INIT(&t->enums);
LIST_INSERT_HEAD(&types, t, link);

return (t);
}





static u_int
parse_type(enum tok *tok, struct type *t, const char *vname, char **subtype)
{
u_int syntax;
struct enums *e;

syntax = val;
if (subtype != NULL)
*subtype = NULL;

if (*tok == TOK_ENUM || *tok == TOK_BITS) {
if (t == NULL && vname != NULL) {
t = make_type(vname);
t->is_enum = (*tok == TOK_ENUM);
t->is_bits = (*tok == TOK_BITS);
t->syntax = syntax;
}
if (gettoken() != '(')
report("'(' expected after ENUM");

if ((*tok = gettoken()) == TOK_EOF)
report("unexpected EOF in ENUM");
do {
e = NULL;
if (t != NULL) {
e = xalloc(sizeof(*e));
}
if (*tok == '-') {
if ((*tok = gettoken()) == TOK_EOF)
report("unexpected EOF in ENUM");
e->value = -(long)val;
} else
e->value = val;

if (*tok != TOK_NUM)
report("need value for ENUM/BITS");
if (gettoken() != TOK_STR)
report("need string in ENUM/BITS");
e->name = savetok();
TAILQ_INSERT_TAIL(&t->enums, e, link);
if ((*tok = gettoken()) == TOK_EOF)
report("unexpected EOF in ENUM/BITS");
} while (*tok != ')');
*tok = gettoken();

} else if (*tok == TOK_DEFTYPE) {
*tok = gettoken();

} else {
if ((*tok = gettoken()) == '|') {
if (gettoken() != TOK_STR)
report("subtype expected after '|'");
if (subtype != NULL)
*subtype = savetok();
*tok = gettoken();
}
}

return (syntax);
}




static struct node *
parse(enum tok tok)
{
struct node *node;
struct node *sub;
u_int index_count;

node = xalloc(sizeof(struct node));
node->lno = input->lno;
node->flags = 0;

if (tok != '(')
report("'(' expected at begin of node");
if (gettoken() != TOK_NUM)
report("node id expected after opening '('");
if (val > ASN_MAXID)
report("subid too large '%lu'", val);
node->id = (asn_subid_t)val;
if (gettoken() != TOK_STR)
report("node name expected after '(' ID");
node->name = savetok();

if ((tok = gettoken()) == TOK_TYPE || tok == TOK_DEFTYPE ||
tok == TOK_ENUM || tok == TOK_BITS) {

char *subtype;
u_int syntax = parse_type(&tok, NULL, node->name, &subtype);

if (tok == TOK_STR) {

node->type = NODE_LEAF;
node->u.leaf.func = savetok();
node->u.leaf.syntax = syntax;
node->u.leaf.subtype = subtype;
tok = gettoken();
} else {

node->type = NODE_COLUMN;
node->u.column.syntax = syntax;
node->u.column.subtype = subtype;
}

while (tok != ')') {
if (tok != TOK_ACCESS)
report("access keyword or ')' expected");
node->flags |= (u_int)val;
tok = gettoken();
}

} else if (tok == ':') {

node->type = NODE_ENTRY;
TAILQ_INIT(&node->u.entry.subs);

index_count = 0;
node->u.entry.index = 0;
tok = gettoken();
while (tok == TOK_TYPE || tok == TOK_DEFTYPE ||
tok == TOK_ENUM || tok == TOK_BITS) {
char *subtype;
u_int syntax = parse_type(&tok, NULL, node->name,
&subtype);
if (index_count == SNMP_INDEXES_MAX)
report("too many table indexes");
node->u.entry.subtypes[index_count++] = subtype;
node->u.entry.index |=
syntax << (SNMP_INDEX_SHIFT * index_count);
}
node->u.entry.index |= index_count;
if (index_count == 0)
report("need at least one index");
if (tok != TOK_STR)
report("function name expected");

node->u.entry.func = savetok();

tok = gettoken();

while (tok != ')') {
sub = parse(tok);
TAILQ_INSERT_TAIL(&node->u.entry.subs, sub, link);
tok = gettoken();
}

} else {

node->type = NODE_TREE;
TAILQ_INIT(&node->u.tree.subs);

while (tok != ')') {
sub = parse(tok);
TAILQ_INSERT_TAIL(&node->u.tree.subs, sub, link);
tok = gettoken();
}
}
return (node);
}





static struct node *
parse_top(enum tok tok)
{
struct type *t;

if (tok == '(')
return (parse(tok));

if (tok == TOK_TYPEDEF) {
if (gettoken() != TOK_STR)
report("type name expected after typedef");

t = make_type(str);

tok = gettoken();
t->is_enum = (tok == TOK_ENUM);
t->is_bits = (tok == TOK_BITS);

t->syntax = parse_type(&tok, t, NULL, NULL);
pushback(tok);

return (NULL);
}

if (tok == TOK_INCLUDE) {
if (gettoken() != TOK_FILENAME)
report("filename expected in include directive");

input_fopen(str, val);
return (NULL);
}

report("'(' or 'typedef' expected");
}




static void
gen_node(FILE *fp, const struct node *np, struct asn_oid *oid, u_int idx,
const char *func)
{
u_int n;
struct node *sub;
u_int syntax;

if (oid->len == ASN_MAXOIDLEN)
report_node(np, "OID too long");
oid->subs[oid->len++] = np->id;

if (np->type == NODE_TREE) {
TAILQ_FOREACH(sub, &np->u.tree.subs, link)
gen_node(fp, sub, oid, 0, NULL);
oid->len--;
return;
}
if (np->type == NODE_ENTRY) {
TAILQ_FOREACH(sub, &np->u.entry.subs, link)
gen_node(fp, sub, oid, np->u.entry.index,
np->u.entry.func);
oid->len--;
return;
}


if ((np->flags & (FL_GET|FL_SET)) == 0) {
oid->len--;
return;
}

fprintf(fp, " {{ %u, {", oid->len);
for (n = 0; n < oid->len; n++)
fprintf(fp, " %u,", oid->subs[n]);
fprintf(fp, " }}, \"%s\", ", np->name);

if (np->type == NODE_COLUMN) {
syntax = np->u.column.syntax;
fprintf(fp, "SNMP_NODE_COLUMN, ");
} else {
syntax = np->u.leaf.syntax;
fprintf(fp, "SNMP_NODE_LEAF, ");
}

switch (syntax) {

case SNMP_SYNTAX_NULL:
fprintf(fp, "SNMP_SYNTAX_NULL, ");
break;

case SNMP_SYNTAX_INTEGER:
fprintf(fp, "SNMP_SYNTAX_INTEGER, ");
break;

case SNMP_SYNTAX_OCTETSTRING:
fprintf(fp, "SNMP_SYNTAX_OCTETSTRING, ");
break;

case SNMP_SYNTAX_IPADDRESS:
fprintf(fp, "SNMP_SYNTAX_IPADDRESS, ");
break;

case SNMP_SYNTAX_OID:
fprintf(fp, "SNMP_SYNTAX_OID, ");
break;

case SNMP_SYNTAX_TIMETICKS:
fprintf(fp, "SNMP_SYNTAX_TIMETICKS, ");
break;

case SNMP_SYNTAX_COUNTER:
fprintf(fp, "SNMP_SYNTAX_COUNTER, ");
break;

case SNMP_SYNTAX_GAUGE:
fprintf(fp, "SNMP_SYNTAX_GAUGE, ");
break;

case SNMP_SYNTAX_COUNTER64:
fprintf(fp, "SNMP_SYNTAX_COUNTER64, ");
break;

case SNMP_SYNTAX_NOSUCHOBJECT:
case SNMP_SYNTAX_NOSUCHINSTANCE:
case SNMP_SYNTAX_ENDOFMIBVIEW:
abort();
}

if (np->type == NODE_COLUMN)
fprintf(fp, "%s, ", func);
else
fprintf(fp, "%s, ", np->u.leaf.func);

fprintf(fp, "0");
if (np->flags & FL_SET)
fprintf(fp, "|SNMP_NODE_CANSET");
fprintf(fp, ", %#x, NULL, NULL },\n", idx);
oid->len--;
return;
}




static void
gen_header(FILE *fp, const struct node *np, u_int oidlen, const char *func)
{
char f[MAXSTR + 4];
struct node *sub;
struct func *ptr;

oidlen++;
if (np->type == NODE_TREE) {
TAILQ_FOREACH(sub, &np->u.tree.subs, link)
gen_header(fp, sub, oidlen, NULL);
return;
}
if (np->type == NODE_ENTRY) {
TAILQ_FOREACH(sub, &np->u.entry.subs, link)
gen_header(fp, sub, oidlen, np->u.entry.func);
return;
}

if((np->flags & (FL_GET|FL_SET)) == 0)
return;

if (np->type == NODE_COLUMN) {
if (func == NULL)
errx(1, "column without function (%s) - probably "
"outside of a table", np->name);
sprintf(f, "%s", func);
} else
sprintf(f, "%s", np->u.leaf.func);

LIST_FOREACH(ptr, &funcs, link)
if (strcmp(ptr->name, f) == 0)
break;

if (ptr == NULL) {
ptr = xalloc(sizeof(*ptr));
ptr->name = savestr(f);
LIST_INSERT_HEAD(&funcs, ptr, link);

fprintf(fp, "int %s(struct snmp_context *, "
"struct snmp_value *, u_int, u_int, "
"enum snmp_op);\n", f);
}

fprintf(fp, "#define LEAF_%s %u\n", np->name, np->id);
}




static void
gen_table(FILE *fp, const struct node *node)
{
struct asn_oid oid;

fprintf(fp, "#include <sys/types.h>\n");
fprintf(fp, "#include <stdio.h>\n");
#if defined(HAVE_STDINT_H)
fprintf(fp, "#include <stdint.h>\n");
#endif
if (localincs) {
fprintf(fp, "#include \"asn1.h\"\n");
fprintf(fp, "#include \"snmp.h\"\n");
fprintf(fp, "#include \"snmpagent.h\"\n");
} else {
fprintf(fp, "#include <bsnmp/asn1.h>\n");
fprintf(fp, "#include <bsnmp/snmp.h>\n");
fprintf(fp, "#include <bsnmp/snmpagent.h>\n");
}
fprintf(fp, "#include \"%stree.h\"\n", file_prefix);
fprintf(fp, "\n");

fprintf(fp, "const struct snmp_node %sctree[] = {\n", file_prefix);

oid.len = PREFIX_LEN;
memcpy(oid.subs, prefix, sizeof(prefix));
gen_node(fp, node, &oid, 0, NULL);

fprintf(fp, "};\n\n");
}

static void
print_syntax(u_int syntax)
{
u_int i;

for (i = 0; keywords[i].str != NULL; i++)
if (keywords[i].tok == TOK_TYPE &&
keywords[i].val == syntax) {
printf(" %s", keywords[i].str);
return;
}
abort();
}




static void
gen_tree(const struct node *np, int level)
{
const struct node *sp;
u_int i;

printf("%*s(%u %s", 2 * level, "", np->id, np->name);

switch (np->type) {

case NODE_LEAF:
print_syntax(np->u.leaf.syntax);
if (np->u.leaf.subtype != NULL)
printf(" | %s", np->u.leaf.subtype);
printf(" %s%s%s)\n", np->u.leaf.func,
(np->flags & FL_GET) ? " GET" : "",
(np->flags & FL_SET) ? " SET" : "");
break;

case NODE_TREE:
if (TAILQ_EMPTY(&np->u.tree.subs)) {
printf(")\n");
} else {
printf("\n");
TAILQ_FOREACH(sp, &np->u.tree.subs, link)
gen_tree(sp, level + 1);
printf("%*s)\n", 2 * level, "");
}
break;

case NODE_ENTRY:
printf(" :");

for (i = 0; i < SNMP_INDEX_COUNT(np->u.entry.index); i++) {
print_syntax(SNMP_INDEX(np->u.entry.index, i));
if (np->u.entry.subtypes[i] != NULL)
printf(" | %s", np->u.entry.subtypes[i]);
}
printf(" %s\n", np->u.entry.func);
TAILQ_FOREACH(sp, &np->u.entry.subs, link)
gen_tree(sp, level + 1);
printf("%*s)\n", 2 * level, "");
break;

case NODE_COLUMN:
print_syntax(np->u.column.syntax);
if (np->u.column.subtype != NULL)
printf(" | %s", np->u.column.subtype);
printf("%s%s)\n", (np->flags & FL_GET) ? " GET" : "",
(np->flags & FL_SET) ? " SET" : "");
break;
}
}

static int
extract(FILE *fp, const struct node *np, struct asn_oid *oid, const char *obj,
const struct asn_oid *idx, const char *iname)
{
struct node *sub;
u_long n;

if (oid->len == ASN_MAXOIDLEN)
report_node(np, "OID too long");
oid->subs[oid->len++] = np->id;

if (strcmp(obj, np->name) == 0) {
if (oid->len + idx->len >= ASN_MAXOIDLEN)
report_node(np, "OID too long");
fprintf(fp, "#define OID_%s%s\t%u\n", np->name,
iname ? iname : "", np->id);
fprintf(fp, "#define OIDLEN_%s%s\t%u\n", np->name,
iname ? iname : "", oid->len + idx->len);
fprintf(fp, "#define OIDX_%s%s\t{ %u, {", np->name,
iname ? iname : "", oid->len + idx->len);
for (n = 0; n < oid->len; n++)
fprintf(fp, " %u,", oid->subs[n]);
for (n = 0; n < idx->len; n++)
fprintf(fp, " %u,", idx->subs[n]);
fprintf(fp, " } }\n");
return (0);
}

if (np->type == NODE_TREE) {
TAILQ_FOREACH(sub, &np->u.tree.subs, link)
if (!extract(fp, sub, oid, obj, idx, iname))
return (0);
} else if (np->type == NODE_ENTRY) {
TAILQ_FOREACH(sub, &np->u.entry.subs, link)
if (!extract(fp, sub, oid, obj, idx, iname))
return (0);
}
oid->len--;
return (1);
}

static int
gen_extract(FILE *fp, const struct node *root, char *object)
{
struct asn_oid oid;
struct asn_oid idx;
char *s, *e, *end, *iname;
u_long ul;
int ret;


idx.len = 0;
iname = NULL;
s = strchr(object, '.');
if (s != NULL) {
iname = malloc(strlen(s) + 1);
if (iname == NULL)
err(1, "cannot allocated index");

strcpy(iname, s);
for (e = iname; *e != '\0'; e++)
if (*e == '.')
*e = '_';

*s++ = '\0';
while (s != NULL) {
if (*s == '\0')
errx(1, "bad index syntax");
if ((e = strchr(s, '.')) != NULL)
*e++ = '\0';

errno = 0;
ul = strtoul(s, &end, 0);
if (*end != '\0')
errx(1, "bad index syntax '%s'", end);
if (errno != 0)
err(1, "bad index syntax");

if (idx.len == ASN_MAXOIDLEN)
errx(1, "index oid too large");
idx.subs[idx.len++] = ul;

s = e;
}
}

oid.len = PREFIX_LEN;
memcpy(oid.subs, prefix, sizeof(prefix));
ret = extract(fp, root, &oid, object, &idx, iname);
if (iname != NULL)
free(iname);

return (ret);
}


static void
check_sub_order(const struct node *np, const struct node_list *subs)
{
int first;
const struct node *sub;
asn_subid_t maxid = 0;


first = 1;
TAILQ_FOREACH(sub, subs, link) {
if (!first && sub->id <= maxid)
report_node(np, "subids not ordered at %s", sub->name);
maxid = sub->id;
first = 0;
}
}




static void
check_tree(struct node *np)
{
struct node *sub;

if (np->type == NODE_LEAF || np->type == NODE_COLUMN) {
if ((np->flags & (FL_GET|FL_SET)) != 0)
tree_size++;
return;
}

if (np->type == NODE_ENTRY) {
check_sub_order(np, &np->u.entry.subs);


TAILQ_FOREACH(sub, &np->u.entry.subs, link) {
if (sub->type != NODE_COLUMN)
report_node(np, "entry subnode '%s' is not "
"a column", sub->name);
check_tree(sub);
}
} else {
check_sub_order(np, &np->u.tree.subs);

TAILQ_FOREACH(sub, &np->u.tree.subs, link)
check_tree(sub);
}
}

static void
merge_subs(struct node_list *s1, struct node_list *s2)
{
struct node *n1, *n2;

while (!TAILQ_EMPTY(s2)) {
n2 = TAILQ_FIRST(s2);
TAILQ_REMOVE(s2, n2, link);

TAILQ_FOREACH(n1, s1, link)
if (n1->id >= n2->id)
break;
if (n1 == NULL)
TAILQ_INSERT_TAIL(s1, n2, link);
else if (n1->id > n2->id)
TAILQ_INSERT_BEFORE(n1, n2, link);
else {
if (n1->type == NODE_TREE && n2->type == NODE_TREE) {
if (strcmp(n1->name, n2->name) != 0)
errx(1, "trees to merge must have "
"same name '%s' '%s'", n1->name,
n2->name);
merge_subs(&n1->u.tree.subs, &n2->u.tree.subs);
free(n2);
} else if (n1->type == NODE_ENTRY &&
n2->type == NODE_ENTRY) {
if (strcmp(n1->name, n2->name) != 0)
errx(1, "entries to merge must have "
"same name '%s' '%s'", n1->name,
n2->name);
if (n1->u.entry.index != n2->u.entry.index)
errx(1, "entries to merge must have "
"same index '%s'", n1->name);
if (strcmp(n1->u.entry.func,
n2->u.entry.func) != 0)
errx(1, "entries to merge must have "
"same op '%s'", n1->name);
merge_subs(&n1->u.entry.subs,
&n2->u.entry.subs);
free(n2);
} else
errx(1, "entities to merge must be both "
"trees or both entries: %s, %s",
n1->name, n2->name);
}
}
}

static void
merge(struct node **root, struct node *t)
{

if (*root == NULL) {
*root = t;
return;
}
if (t == NULL)
return;


if ((*root)->type != NODE_TREE)
errx(1, "root is not a tree");
if (t->type != NODE_TREE)
errx(1, "can merge only with tree");
if ((*root)->id != t->id)
errx(1, "trees to merge must have same id");

merge_subs(&(*root)->u.tree.subs, &t->u.tree.subs);
}

static void
unminus(FILE *fp, const char *s)
{

while (*s != '\0') {
if (*s == '-')
fprintf(fp, "_");
else
fprintf(fp, "%c", *s);
s++;
}
}











static void
gen_enum_funcs(FILE *fp, const struct type *t, int ccode)
{
fprintf(fp, "\n");

if (!ccode)
fprintf(fp, "static inline ");
fprintf(fp, "int\n");
fprintf(fp, "isok_%s(enum %s s)\n", t->name, t->name);
fprintf(fp, "{\n");
fprintf(fp, " switch (s) {\n");

const struct enums *e;
TAILQ_FOREACH(e, &t->enums, link) {
fprintf(fp, "\t case %s_", t->name);
unminus(fp, e->name);
fprintf(fp, ":\n");
}

fprintf(fp, " return (1);\n");
fprintf(fp, " }\n");
fprintf(fp, " return (0);\n");
fprintf(fp, "}\n\n");

if (!ccode)
fprintf(fp, "static inline ");
fprintf(fp, "const char *\n");
fprintf(fp, "tostr_%s(enum %s s)\n", t->name, t->name);
fprintf(fp, "{\n");
fprintf(fp, " static const char *vals[] = { STRING_%s };\n", t->name);
fprintf(fp, "\n");
fprintf(fp, " if (isok_%s(s))\n", t->name);
fprintf(fp, " return (vals[(int)s - STROFF_%s]);\n", t->name);
fprintf(fp, " return (\"%s???\");\n", t->name);
fprintf(fp, "}\n\n");

if (!ccode)
fprintf(fp, "static inline ");
fprintf(fp, "int\n");
fprintf(fp, "fromstr_%s(const char *str, enum %s *s)\n",
t->name, t->name);
fprintf(fp, "{\n");
fprintf(fp, " static const char *vals[] = { STRING_%s };\n", t->name);
fprintf(fp, "\n");
fprintf(fp, " for (size_t i = 0; i < sizeof(vals)/sizeof(vals[0]); i++) {\n");
fprintf(fp, " if (vals[i] != NULL && strcmp(vals[i], str) == 0) {\n");
fprintf(fp, " *s = i + STROFF_%s;\n", t->name);
fprintf(fp, " return (1);\n");
fprintf(fp, " }\n");
fprintf(fp, " }\n");
fprintf(fp, " return (0);\n");
fprintf(fp, "}\n");
}









static void
gen_enum(FILE *fp, const struct type *t, int dof)
{
const struct enums *e;
long min = LONG_MAX;

fprintf(fp, "\n");
fprintf(fp, "#ifndef %s_defined__\n", t->name);
fprintf(fp, "#define %s_defined__\n", t->name);
fprintf(fp, "/*\n");
fprintf(fp, " * From %s:%u\n", t->from_fname, t->from_lno);
fprintf(fp, " */\n");
fprintf(fp, "enum %s {\n", t->name);
TAILQ_FOREACH(e, &t->enums, link) {
fprintf(fp, "\t%s_", t->name);
unminus(fp, e->name);
fprintf(fp, " = %ld,\n", e->value);
if (e->value < min)
min = e->value;
}
fprintf(fp, "};\n");
fprintf(fp, "#define STROFF_%s %ld\n", t->name, min);
fprintf(fp, "#define STRING_%s \\\n", t->name);
TAILQ_FOREACH(e, &t->enums, link) {
fprintf(fp, "\t[%ld] = \"%s_", e->value - min, t->name);
unminus(fp, e->name);
fprintf(fp, "\",\\\n");
}
fprintf(fp, "\n");
if (dof) {
fprintf(fp, "#ifdef SNMPENUM_FUNCS\n");
fprintf(fp, "\n");
gen_enum_funcs(fp, t, 0);
fprintf(fp, "\n");
fprintf(fp, "#endif\n");
fprintf(fp, "\n");
}
fprintf(fp, "#endif /* %s_defined__ */\n", t->name);
}







static int
gen_enum_funcs_str(FILE *fp, const char *name)
{
const struct type *t;

LIST_FOREACH(t, &types, link)
if ((t->is_enum || t->is_bits) && strcmp(t->name, name) == 0) {
gen_enum_funcs(fp, t, 1);
return (0);
}

return (-1);
}







static void
gen_all_enum_funcs(FILE *fp, int ccode)
{
const struct type *t;

LIST_FOREACH(t, &types, link)
if (t->is_enum || t->is_bits)
gen_enum_funcs(fp, t, ccode);
}

static void
gen_enums(FILE *fp, int dof)
{
const struct type *t;

LIST_FOREACH(t, &types, link)
if (t->is_enum || t->is_bits)
gen_enum(fp, t, dof);
}











static int
extract_enum(FILE *fp, const char *name, int gen_funcs)
{
const struct type *t;

LIST_FOREACH(t, &types, link)
if ((t->is_enum || t->is_bits) && strcmp(t->name, name) == 0) {
gen_enum(fp, t, gen_funcs);
return (0);
}
return (-1);
}








static void
extract_all_enums(FILE *fp, int gen_funcs)
{
const struct type *t;

LIST_FOREACH(t, &types, link)
if (t->is_enum || t->is_bits)
gen_enum(fp, t, gen_funcs);
}








static void
make_enums(int argc, char *argv[], enum gen_funcs gen_funcs)
{
if (gen_funcs == GEN_FUNCS_C) {
if (argc == 0)
gen_all_enum_funcs(stdout, 1);
else {
for (int i = 0; i < argc; i++)
if (gen_enum_funcs_str(stdout, argv[i]))
errx(1, "enum not found: %s", argv[i]);
}
} else {
if (argc == 0)
extract_all_enums(stdout, gen_funcs == GEN_FUNCS_H);
else {
for (int i = 0; i < argc; i++)
if (extract_enum(stdout, argv[i],
gen_funcs == GEN_FUNCS_H))
errx(1, "enum not found: %s", argv[i]);
}
}
}







static void
make_table(const struct node *root, int gen_funcs)
{
FILE *fp;

char fname[MAXPATHLEN + 1];
sprintf(fname, "%stree.h", file_prefix);
if ((fp = fopen(fname, "w")) == NULL)
err(1, "%s: ", fname);
gen_header(fp, root, PREFIX_LEN, NULL);

fprintf(fp, "\n#ifdef SNMPTREE_TYPES\n");
gen_enums(fp, gen_funcs);
fprintf(fp, "\n#endif /* SNMPTREE_TYPES */\n\n");

fprintf(fp, "#define %sCTREE_SIZE %u\n", file_prefix, tree_size);
fprintf(fp, "extern const struct snmp_node %sctree[];\n", file_prefix);

fclose(fp);

sprintf(fname, "%stree.c", file_prefix);
if ((fp = fopen(fname, "w")) == NULL)
err(1, "%s: ", fname);
gen_table(fp, root);
fclose(fp);
}

int
main(int argc, char *argv[])
{
enum op op = OP_GEN;
enum gen_funcs gen_funcs = GEN_FUNCS_NONE;

char *infile = NULL;

int opt;
while ((opt = getopt(argc, argv, "dEeFfhI:i:lp:t")) != EOF)
switch (opt) {

case 'd':
debug = 1;
break;

case 'E':
if (op != OP_GEN && op != OP_ENUMS)
errx(1, "-E conflicts with earlier options");
op = OP_ENUMS;
break;

case 'e':
if (op != OP_GEN && op != OP_EXTRACT)
errx(1, "-e conflicts with earlier options");
op = OP_EXTRACT;
break;

case 'F':
if (gen_funcs != GEN_FUNCS_NONE &&
gen_funcs != GEN_FUNCS_C)
errx(1, "-F conflicts with -f");
gen_funcs = GEN_FUNCS_C;
break;

case 'f':
if (gen_funcs != GEN_FUNCS_NONE &&
gen_funcs != GEN_FUNCS_H)
errx(1, "-f conflicts with -F");
gen_funcs = GEN_FUNCS_H;
break;

case 'h':
fprintf(stderr, "%s", usgtxt);
exit(0);

case 'I':
path_new(optarg);
break;

case 'i':
infile = optarg;
break;

case 'l':
localincs = 1;
break;

case 'p':
file_prefix = optarg;
if (strlen(file_prefix) + strlen("tree.c") >
MAXPATHLEN)
errx(1, "prefix too long");
break;

case 't':
if (op != OP_GEN && op != OP_TREE)
errx(1, "-t conflicts with earlier options");
op = OP_TREE;
break;
}

argc -= optind;
argv += optind;


if (infile == NULL) {
input_new(stdin, NULL, "<stdin>");
} else {
FILE *fp;
if ((fp = fopen(infile, "r")) == NULL)
err(1, "%s", infile);
input_new(fp, NULL, infile);
}


struct node *root = parse_top(gettoken());

int tok;
while ((tok = gettoken()) != TOK_EOF)
merge(&root, parse_top(tok));

if (root)
check_tree(root);


switch (op) {

case OP_EXTRACT:
if (argc == 0)
errx(1, "-e requires arguments");

for (int i = 0; i < argc; i++)
if (gen_extract(stdout, root, argv[i]))
errx(1, "object not found: %s", argv[i]);
return (0);

case OP_ENUMS:
make_enums(argc, argv, gen_funcs);
return (0);

case OP_TREE:
if (argc != 0)
errx(1, "-t allows no arguments");
gen_tree(root, 0);
return (0);

case OP_GEN:
if (argc != 0)
errx(1, "tree generation allows no arguments");
make_table(root, gen_funcs == GEN_FUNCS_H);
return (0);
}
}
