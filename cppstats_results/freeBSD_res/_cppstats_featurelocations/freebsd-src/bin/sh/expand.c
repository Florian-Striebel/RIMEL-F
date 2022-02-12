



































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)expand.c 8.5 (Berkeley) 5/15/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>






#include "shell.h"
#include "main.h"
#include "nodes.h"
#include "eval.h"
#include "expand.h"
#include "syntax.h"
#include "parser.h"
#include "jobs.h"
#include "options.h"
#include "var.h"
#include "input.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "mystring.h"
#include "arith.h"
#include "show.h"
#include "builtins.h"

enum wordstate { WORD_IDLE, WORD_WS_DELIMITED, WORD_QUOTEMARK };

struct worddest {
struct arglist *list;
enum wordstate state;
};

static char *expdest;

static const char *argstr(const char *, struct nodelist **restrict, int,
struct worddest *);
static const char *exptilde(const char *, int);
static const char *expari(const char *, struct nodelist **restrict, int,
struct worddest *);
static void expbackq(union node *, int, int, struct worddest *);
static const char *subevalvar_trim(const char *, struct nodelist **restrict,
int, int, int);
static const char *subevalvar_misc(const char *, struct nodelist **restrict,
const char *, int, int, int);
static const char *evalvar(const char *, struct nodelist **restrict, int,
struct worddest *);
static int varisset(const char *, int);
static void strtodest(const char *, int, int, int, struct worddest *);
static void reprocess(int, int, int, int, struct worddest *);
static void varvalue(const char *, int, int, int, struct worddest *);
static void expandmeta(char *, struct arglist *);
static void expmeta(char *, char *, struct arglist *);
static int expsortcmp(const void *, const void *);
static int patmatch(const char *, const char *);
static void cvtnum(int, char *);
static int collate_range_cmp(wchar_t, wchar_t);

void
emptyarglist(struct arglist *list)
{

list->args = list->smallarg;
list->count = 0;
list->capacity = sizeof(list->smallarg) / sizeof(list->smallarg[0]);
}

void
appendarglist(struct arglist *list, char *str)
{
char **newargs;
int newcapacity;

if (list->count >= list->capacity) {
newcapacity = list->capacity * 2;
if (newcapacity < 16)
newcapacity = 16;
if (newcapacity > INT_MAX / (int)sizeof(newargs[0]))
error("Too many entries in arglist");
newargs = stalloc(newcapacity * sizeof(newargs[0]));
memcpy(newargs, list->args, list->count * sizeof(newargs[0]));
list->args = newargs;
list->capacity = newcapacity;
}
list->args[list->count++] = str;
}

static int
collate_range_cmp(wchar_t c1, wchar_t c2)
{
wchar_t s1[2], s2[2];

s1[0] = c1;
s1[1] = L'\0';
s2[0] = c2;
s2[1] = L'\0';
return (wcscoll(s1, s2));
}

static char *
stputs_quotes(const char *data, const char *syntax, char *p)
{
while (*data) {
CHECKSTRSPACE(2, p);
if (syntax[(int)*data] == CCTL)
USTPUTC(CTLESC, p);
USTPUTC(*data++, p);
}
return (p);
}
#define STPUTS_QUOTES(data, syntax, p) p = stputs_quotes((data), syntax, p)

static char *
nextword(char c, int flag, char *p, struct worddest *dst)
{
int is_ws;

is_ws = c == '\t' || c == '\n' || c == ' ';
if (p != stackblock() || (is_ws ? dst->state == WORD_QUOTEMARK :
dst->state != WORD_WS_DELIMITED) || c == '\0') {
STPUTC('\0', p);
if (flag & EXP_GLOB)
expandmeta(grabstackstr(p), dst->list);
else
appendarglist(dst->list, grabstackstr(p));
dst->state = is_ws ? WORD_WS_DELIMITED : WORD_IDLE;
} else if (!is_ws && dst->state == WORD_WS_DELIMITED)
dst->state = WORD_IDLE;

appendarglist(dst->list, NULL);
dst->list->count--;
STARTSTACKSTR(p);
return p;
}
#define NEXTWORD(c, flag, p, dstlist) p = nextword(c, flag, p, dstlist)

static char *
stputs_split(const char *data, const char *syntax, int flag, char *p,
struct worddest *dst)
{
const char *ifs;
char c;

ifs = ifsset() ? ifsval() : " \t\n";
while (*data) {
CHECKSTRSPACE(2, p);
c = *data++;
if (strchr(ifs, c) != NULL) {
NEXTWORD(c, flag, p, dst);
continue;
}
if (flag & EXP_GLOB && syntax[(int)c] == CCTL)
USTPUTC(CTLESC, p);
USTPUTC(c, p);
}
return (p);
}
#define STPUTS_SPLIT(data, syntax, flag, p, dst) p = stputs_split((data), syntax, flag, p, dst)













void
expandarg(union node *arg, struct arglist *arglist, int flag)
{
struct worddest exparg;
struct nodelist *argbackq;

if (fflag)
flag &= ~EXP_GLOB;
argbackq = arg->narg.backquote;
exparg.list = arglist;
exparg.state = WORD_IDLE;
STARTSTACKSTR(expdest);
argstr(arg->narg.text, &argbackq, flag, &exparg);
if (arglist == NULL) {
STACKSTRNUL(expdest);
return;
}
if ((flag & EXP_SPLIT) == 0 || expdest != stackblock() ||
exparg.state == WORD_QUOTEMARK) {
STPUTC('\0', expdest);
if (flag & EXP_SPLIT) {
if (flag & EXP_GLOB)
expandmeta(grabstackstr(expdest), exparg.list);
else
appendarglist(exparg.list, grabstackstr(expdest));
}
}
if ((flag & EXP_SPLIT) == 0)
appendarglist(arglist, grabstackstr(expdest));
}













static const char *
argstr(const char *p, struct nodelist **restrict argbackq, int flag,
struct worddest *dst)
{
char c;
int quotes = flag & (EXP_GLOB | EXP_CASE);
int firsteq = 1;
int split_lit;
int lit_quoted;

split_lit = flag & EXP_SPLIT_LIT;
lit_quoted = flag & EXP_LIT_QUOTED;
flag &= ~(EXP_SPLIT_LIT | EXP_LIT_QUOTED);
if (*p == '~' && (flag & (EXP_TILDE | EXP_VARTILDE)))
p = exptilde(p, flag);
for (;;) {
CHECKSTRSPACE(2, expdest);
switch (c = *p++) {
case '\0':
return (p - 1);
case CTLENDVAR:
case CTLENDARI:
return (p);
case CTLQUOTEMARK:
lit_quoted = 1;

if (p[0] == CTLVAR && (p[1] & VSQUOTE) != 0 &&
p[2] == '@' && p[3] == '=')
break;
if ((flag & EXP_SPLIT) != 0 && expdest == stackblock())
dst->state = WORD_QUOTEMARK;
break;
case CTLQUOTEEND:
lit_quoted = 0;
break;
case CTLESC:
c = *p++;
if (split_lit && !lit_quoted &&
strchr(ifsset() ? ifsval() : " \t\n", c) != NULL) {
NEXTWORD(c, flag, expdest, dst);
break;
}
if (quotes)
USTPUTC(CTLESC, expdest);
USTPUTC(c, expdest);
break;
case CTLVAR:
p = evalvar(p, argbackq, flag, dst);
break;
case CTLBACKQ:
case CTLBACKQ|CTLQUOTE:
expbackq((*argbackq)->n, c & CTLQUOTE, flag, dst);
*argbackq = (*argbackq)->next;
break;
case CTLARI:
p = expari(p, argbackq, flag, dst);
break;
case ':':
case '=':




if (split_lit && !lit_quoted &&
strchr(ifsset() ? ifsval() : " \t\n", c) != NULL) {
NEXTWORD(c, flag, expdest, dst);
break;
}
USTPUTC(c, expdest);
if (flag & EXP_VARTILDE && *p == '~' &&
(c != '=' || firsteq)) {
if (c == '=')
firsteq = 0;
p = exptilde(p, flag);
}
break;
default:
if (split_lit && !lit_quoted &&
strchr(ifsset() ? ifsval() : " \t\n", c) != NULL) {
NEXTWORD(c, flag, expdest, dst);
break;
}
USTPUTC(c, expdest);
}
}
}





static const char *
exptilde(const char *p, int flag)
{
char c;
const char *startp = p;
const char *user;
struct passwd *pw;
char *home;
int len;

for (;;) {
c = *p;
switch(c) {
case CTLESC:
case CTLVAR:
case CTLBACKQ:
case CTLBACKQ | CTLQUOTE:
case CTLARI:
case CTLENDARI:
case CTLQUOTEMARK:
return (startp);
case ':':
if ((flag & EXP_VARTILDE) == 0)
break;

case '\0':
case '/':
case CTLENDVAR:
len = p - startp - 1;
STPUTBIN(startp + 1, len, expdest);
STACKSTRNUL(expdest);
user = expdest - len;
if (*user == '\0') {
home = lookupvar("HOME");
} else {
pw = getpwnam(user);
home = pw != NULL ? pw->pw_dir : NULL;
}
STADJUST(-len, expdest);
if (home == NULL || *home == '\0')
return (startp);
strtodest(home, flag, VSNORMAL, 1, NULL);
return (p);
}
p++;
}
}





static const char *
expari(const char *p, struct nodelist **restrict argbackq, int flag,
struct worddest *dst)
{
char *q, *start;
arith_t result;
int begoff;
int quoted;
int adj;

quoted = *p++ == '"';
begoff = expdest - stackblock();
p = argstr(p, argbackq, 0, NULL);
STPUTC('\0', expdest);
start = stackblock() + begoff;

q = grabstackstr(expdest);
result = arith(start);
ungrabstackstr(q, expdest);

start = stackblock() + begoff;
adj = start - expdest;
STADJUST(adj, expdest);

CHECKSTRSPACE((int)(DIGITS(result) + 1), expdest);
fmtstr(expdest, DIGITS(result), ARITH_FORMAT_STR, result);
adj = strlen(expdest);
STADJUST(adj, expdest);




if (quoted ?
result < 0 && begoff > 1 && flag & (EXP_GLOB | EXP_CASE) :
flag & EXP_SPLIT)
reprocess(expdest - adj - stackblock(), flag, VSNORMAL, quoted,
dst);
return p;
}





static void
expbackq(union node *cmd, int quoted, int flag, struct worddest *dst)
{
struct backcmd in;
int i;
char buf[128];
char *p;
char *dest = expdest;
char lastc;
char const *syntax = quoted? DQSYNTAX : BASESYNTAX;
int quotes = flag & (EXP_GLOB | EXP_CASE);
size_t nnl;
const char *ifs;
int startloc;

INTOFF;
p = grabstackstr(dest);
evalbackcmd(cmd, &in);
ungrabstackstr(p, dest);

p = in.buf;
startloc = dest - stackblock();
nnl = 0;
if (!quoted && flag & EXP_SPLIT)
ifs = ifsset() ? ifsval() : " \t\n";
else
ifs = "";

for (;;) {
if (--in.nleft < 0) {
if (in.fd < 0)
break;
while ((i = read(in.fd, buf, sizeof buf)) < 0 && errno == EINTR)
;
TRACE(("expbackq: read returns %d\n", i));
if (i <= 0)
break;
p = buf;
in.nleft = i - 1;
}
lastc = *p++;
if (lastc == '\0')
continue;
if (nnl > 0 && lastc != '\n') {
NEXTWORD('\n', flag, dest, dst);
nnl = 0;
}
if (strchr(ifs, lastc) != NULL) {
if (lastc == '\n')
nnl++;
else
NEXTWORD(lastc, flag, dest, dst);
} else {
CHECKSTRSPACE(2, dest);
if (quotes && syntax[(int)lastc] == CCTL)
USTPUTC(CTLESC, dest);
USTPUTC(lastc, dest);
}
}
while (dest > stackblock() + startloc && STTOPC(dest) == '\n')
STUNPUTC(dest);

if (in.fd >= 0)
close(in.fd);
if (in.buf)
ckfree(in.buf);
if (in.jp) {
p = grabstackstr(dest);
exitstatus = waitforjob(in.jp, (int *)NULL);
ungrabstackstr(p, dest);
}
TRACE(("expbackq: done\n"));
expdest = dest;
INTON;
}



static void
recordleft(const char *str, const char *loc, char *startp)
{
int amount;

amount = ((str - 1) - (loc - startp)) - expdest;
STADJUST(amount, expdest);
while (loc != str - 1)
*startp++ = *loc++;
}

static const char *
subevalvar_trim(const char *p, struct nodelist **restrict argbackq, int strloc,
int subtype, int startloc)
{
char *startp;
char *loc = NULL;
char *str;
int c = 0;
int amount;

p = argstr(p, argbackq, EXP_CASE | EXP_TILDE, NULL);
STACKSTRNUL(expdest);
startp = stackblock() + startloc;
str = stackblock() + strloc;

switch (subtype) {
case VSTRIMLEFT:
for (loc = startp; loc < str; loc++) {
c = *loc;
*loc = '\0';
if (patmatch(str, startp)) {
*loc = c;
recordleft(str, loc, startp);
return p;
}
*loc = c;
}
break;

case VSTRIMLEFTMAX:
for (loc = str - 1; loc >= startp;) {
c = *loc;
*loc = '\0';
if (patmatch(str, startp)) {
*loc = c;
recordleft(str, loc, startp);
return p;
}
*loc = c;
loc--;
}
break;

case VSTRIMRIGHT:
for (loc = str - 1; loc >= startp;) {
if (patmatch(str, loc)) {
amount = loc - expdest;
STADJUST(amount, expdest);
return p;
}
loc--;
}
break;

case VSTRIMRIGHTMAX:
for (loc = startp; loc < str - 1; loc++) {
if (patmatch(str, loc)) {
amount = loc - expdest;
STADJUST(amount, expdest);
return p;
}
}
break;


default:
abort();
}
amount = (expdest - stackblock() - strloc) + 1;
STADJUST(-amount, expdest);
return p;
}


static const char *
subevalvar_misc(const char *p, struct nodelist **restrict argbackq,
const char *var, int subtype, int startloc, int varflags)
{
const char *end;
char *startp;
int amount;

end = argstr(p, argbackq, EXP_TILDE, NULL);
STACKSTRNUL(expdest);
startp = stackblock() + startloc;

switch (subtype) {
case VSASSIGN:
setvar(var, startp, 0);
amount = startp - expdest;
STADJUST(amount, expdest);
return end;

case VSQUESTION:
if (*p != CTLENDVAR) {
outfmt(out2, "%s\n", startp);
error((char *)NULL);
}
error("%.*s: parameter %snot set", (int)(p - var - 1),
var, (varflags & VSNUL) ? "null or " : "");

default:
abort();
}
}







static const char *
evalvar(const char *p, struct nodelist **restrict argbackq, int flag,
struct worddest *dst)
{
int subtype;
int varflags;
const char *var;
const char *val;
int patloc;
int c;
int set;
int special;
int startloc;
int varlen;
int varlenb;
char buf[21];

varflags = (unsigned char)*p++;
subtype = varflags & VSTYPE;
var = p;
special = 0;
if (! is_name(*p))
special = 1;
p = strchr(p, '=') + 1;
if (varflags & VSLINENO) {
set = 1;
special = 1;
val = NULL;
} else if (special) {
set = varisset(var, varflags & VSNUL);
val = NULL;
} else {
val = bltinlookup(var, 1);
if (val == NULL || ((varflags & VSNUL) && val[0] == '\0')) {
val = NULL;
set = 0;
} else
set = 1;
}
varlen = 0;
startloc = expdest - stackblock();
if (!set && uflag && *var != '@' && *var != '*') {
switch (subtype) {
case VSNORMAL:
case VSTRIMLEFT:
case VSTRIMLEFTMAX:
case VSTRIMRIGHT:
case VSTRIMRIGHTMAX:
case VSLENGTH:
error("%.*s: parameter not set", (int)(p - var - 1),
var);
}
}
if (set && subtype != VSPLUS) {

if (special) {
if (varflags & VSLINENO) {
if (p - var > (ptrdiff_t)sizeof(buf))
abort();
memcpy(buf, var, p - var - 1);
buf[p - var - 1] = '\0';
strtodest(buf, flag, subtype,
varflags & VSQUOTE, dst);
} else
varvalue(var, varflags & VSQUOTE, subtype, flag,
dst);
if (subtype == VSLENGTH) {
varlenb = expdest - stackblock() - startloc;
varlen = varlenb;
if (localeisutf8) {
val = stackblock() + startloc;
for (;val != expdest; val++)
if ((*val & 0xC0) == 0x80)
varlen--;
}
STADJUST(-varlenb, expdest);
}
} else {
if (subtype == VSLENGTH) {
for (;*val; val++)
if (!localeisutf8 ||
(*val & 0xC0) != 0x80)
varlen++;
}
else
strtodest(val, flag, subtype,
varflags & VSQUOTE, dst);
}
}

if (subtype == VSPLUS)
set = ! set;

switch (subtype) {
case VSLENGTH:
cvtnum(varlen, buf);
strtodest(buf, flag, VSNORMAL, varflags & VSQUOTE, dst);
break;

case VSNORMAL:
return p;

case VSPLUS:
case VSMINUS:
if (!set) {
return argstr(p, argbackq,
flag | (flag & EXP_SPLIT ? EXP_SPLIT_LIT : 0) |
(varflags & VSQUOTE ? EXP_LIT_QUOTED : 0), dst);
}
break;

case VSTRIMLEFT:
case VSTRIMLEFTMAX:
case VSTRIMRIGHT:
case VSTRIMRIGHTMAX:
if (!set)
break;




STPUTC('\0', expdest);
patloc = expdest - stackblock();
p = subevalvar_trim(p, argbackq, patloc, subtype, startloc);
reprocess(startloc, flag, VSNORMAL, varflags & VSQUOTE, dst);
if (flag & EXP_SPLIT && *var == '@' && varflags & VSQUOTE)
dst->state = WORD_QUOTEMARK;
return p;

case VSASSIGN:
case VSQUESTION:
if (!set) {
p = subevalvar_misc(p, argbackq, var, subtype,
startloc, varflags);

val = lookupvar(var);
strtodest(val, flag, subtype, varflags & VSQUOTE, dst);
return p;
}
break;

case VSERROR:
c = p - var - 1;
error("${%.*s%s}: Bad substitution", c, var,
(c > 0 && *p != CTLENDVAR) ? "..." : "");

default:
abort();
}

{
int nesting = 1;
for (;;) {
if ((c = *p++) == CTLESC)
p++;
else if (c == CTLBACKQ || c == (CTLBACKQ|CTLQUOTE))
*argbackq = (*argbackq)->next;
else if (c == CTLVAR) {
if ((*p++ & VSTYPE) != VSNORMAL)
nesting++;
} else if (c == CTLENDVAR) {
if (--nesting == 0)
break;
}
}
}
return p;
}







static int
varisset(const char *name, int nulok)
{

if (*name == '!')
return backgndpidset();
else if (*name == '@' || *name == '*') {
if (*shellparam.p == NULL)
return 0;

if (nulok) {
char **av;

for (av = shellparam.p; *av; av++)
if (**av != '\0')
return 1;
return 0;
}
} else if (is_digit(*name)) {
char *ap;
long num;

errno = 0;
num = strtol(name, NULL, 10);
if (errno != 0 || num > shellparam.nparam)
return 0;

if (num == 0)
ap = arg0;
else
ap = shellparam.p[num - 1];

if (nulok && (ap == NULL || *ap == '\0'))
return 0;
}
return 1;
}

static void
strtodest(const char *p, int flag, int subtype, int quoted,
struct worddest *dst)
{
if (subtype == VSLENGTH || subtype == VSTRIMLEFT ||
subtype == VSTRIMLEFTMAX || subtype == VSTRIMRIGHT ||
subtype == VSTRIMRIGHTMAX)
STPUTS(p, expdest);
else if (flag & EXP_SPLIT && !quoted && dst != NULL)
STPUTS_SPLIT(p, BASESYNTAX, flag, expdest, dst);
else if (flag & (EXP_GLOB | EXP_CASE))
STPUTS_QUOTES(p, quoted ? DQSYNTAX : BASESYNTAX, expdest);
else
STPUTS(p, expdest);
}

static void
reprocess(int startloc, int flag, int subtype, int quoted,
struct worddest *dst)
{
static char *buf = NULL;
static size_t buflen = 0;
char *startp;
size_t len, zpos, zlen;

startp = stackblock() + startloc;
len = expdest - startp;
if (len >= SIZE_MAX / 2 || len > PTRDIFF_MAX)
abort();
INTOFF;
if (len >= buflen) {
ckfree(buf);
buf = NULL;
}
if (buflen < 128)
buflen = 128;
while (len >= buflen)
buflen <<= 1;
if (buf == NULL)
buf = ckmalloc(buflen);
INTON;
memcpy(buf, startp, len);
buf[len] = '\0';
STADJUST(-(ptrdiff_t)len, expdest);
for (zpos = 0;;) {
zlen = strlen(buf + zpos);
strtodest(buf + zpos, flag, subtype, quoted, dst);
zpos += zlen + 1;
if (zpos == len + 1)
break;
if (flag & EXP_SPLIT && (quoted || (zlen > 0 && zpos < len)))
NEXTWORD('\0', flag, expdest, dst);
}
}





static void
varvalue(const char *name, int quoted, int subtype, int flag,
struct worddest *dst)
{
int num;
char *p;
int i;
int splitlater;
char sep[2];
char **ap;
char buf[(NSHORTOPTS > 10 ? NSHORTOPTS : 10) + 1];

if (subtype == VSLENGTH)
flag &= ~EXP_FULL;
splitlater = subtype == VSTRIMLEFT || subtype == VSTRIMLEFTMAX ||
subtype == VSTRIMRIGHT || subtype == VSTRIMRIGHTMAX;

switch (*name) {
case '$':
num = rootpid;
break;
case '?':
num = oexitstatus;
break;
case '#':
num = shellparam.nparam;
break;
case '!':
num = backgndpidval();
break;
case '-':
p = buf;
for (i = 0 ; i < NSHORTOPTS ; i++) {
if (optval[i])
*p++ = optletter[i];
}
*p = '\0';
strtodest(buf, flag, subtype, quoted, dst);
return;
case '@':
if (flag & EXP_SPLIT && quoted) {
for (ap = shellparam.p ; (p = *ap++) != NULL ; ) {
strtodest(p, flag, subtype, quoted, dst);
if (*ap) {
if (splitlater)
STPUTC('\0', expdest);
else
NEXTWORD('\0', flag, expdest,
dst);
}
}
if (shellparam.nparam > 0)
dst->state = WORD_QUOTEMARK;
return;
}

case '*':
if (ifsset())
sep[0] = ifsval()[0];
else
sep[0] = ' ';
sep[1] = '\0';
for (ap = shellparam.p ; (p = *ap++) != NULL ; ) {
strtodest(p, flag, subtype, quoted, dst);
if (!*ap)
break;
if (sep[0])
strtodest(sep, flag, subtype, quoted, dst);
else if (flag & EXP_SPLIT && !quoted && **ap != '\0') {
if (splitlater)
STPUTC('\0', expdest);
else
NEXTWORD('\0', flag, expdest, dst);
}
}
return;
default:
if (is_digit(*name)) {
num = atoi(name);
if (num == 0)
p = arg0;
else if (num > 0 && num <= shellparam.nparam)
p = shellparam.p[num - 1];
else
return;
strtodest(p, flag, subtype, quoted, dst);
}
return;
}
cvtnum(num, buf);
strtodest(buf, flag, subtype, quoted, dst);
}



static char expdir[PATH_MAX];
#define expdir_end (expdir + sizeof(expdir))






static void
expandmeta(char *pattern, struct arglist *dstlist)
{
char *p;
int firstmatch;
char c;

firstmatch = dstlist->count;
p = pattern;
for (; (c = *p) != '\0'; p++) {

if (c == '*' || c == '?' || c == '[') {
INTOFF;
expmeta(expdir, pattern, dstlist);
INTON;
break;
}
}
if (dstlist->count == firstmatch) {



rmescapes(pattern);
appendarglist(dstlist, pattern);
} else {
qsort(&dstlist->args[firstmatch],
dstlist->count - firstmatch,
sizeof(dstlist->args[0]), expsortcmp);
}
}






static void
expmeta(char *enddir, char *name, struct arglist *arglist)
{
const char *p;
const char *q;
const char *start;
char *endname;
int metaflag;
struct stat statb;
DIR *dirp;
struct dirent *dp;
int atend;
int matchdot;
int esc;
int namlen;

metaflag = 0;
start = name;
for (p = name; esc = 0, *p; p += esc + 1) {
if (*p == '*' || *p == '?')
metaflag = 1;
else if (*p == '[') {
q = p + 1;
if (*q == '!' || *q == '^')
q++;
for (;;) {
if (*q == CTLESC)
q++;
if (*q == '/' || *q == '\0')
break;
if (*++q == ']') {
metaflag = 1;
break;
}
}
} else if (*p == '\0')
break;
else {
if (*p == CTLESC)
esc++;
if (p[esc] == '/') {
if (metaflag)
break;
start = p + esc + 1;
}
}
}
if (metaflag == 0) {
if (enddir != expdir)
metaflag++;
for (p = name ; ; p++) {
if (*p == CTLESC)
p++;
*enddir++ = *p;
if (*p == '\0')
break;
if (enddir == expdir_end)
return;
}
if (metaflag == 0 || lstat(expdir, &statb) >= 0)
appendarglist(arglist, stsavestr(expdir));
return;
}
endname = name + (p - name);
if (start != name) {
p = name;
while (p < start) {
if (*p == CTLESC)
p++;
*enddir++ = *p++;
if (enddir == expdir_end)
return;
}
}
if (enddir == expdir) {
p = ".";
} else if (enddir == expdir + 1 && *expdir == '/') {
p = "/";
} else {
p = expdir;
enddir[-1] = '\0';
}
if ((dirp = opendir(p)) == NULL)
return;
if (enddir != expdir)
enddir[-1] = '/';
if (*endname == 0) {
atend = 1;
} else {
atend = 0;
*endname = '\0';
endname += esc + 1;
}
matchdot = 0;
p = start;
if (*p == CTLESC)
p++;
if (*p == '.')
matchdot++;
while (! int_pending() && (dp = readdir(dirp)) != NULL) {
if (dp->d_name[0] == '.' && ! matchdot)
continue;
if (patmatch(start, dp->d_name)) {
namlen = dp->d_namlen;
if (enddir + namlen + 1 > expdir_end)
continue;
memcpy(enddir, dp->d_name, namlen + 1);
if (atend)
appendarglist(arglist, stsavestr(expdir));
else {
if (dp->d_type != DT_UNKNOWN &&
dp->d_type != DT_DIR &&
dp->d_type != DT_LNK)
continue;
if (enddir + namlen + 2 > expdir_end)
continue;
enddir[namlen] = '/';
enddir[namlen + 1] = '\0';
expmeta(enddir + namlen + 1, endname, arglist);
}
}
}
closedir(dirp);
if (! atend)
endname[-esc - 1] = esc ? CTLESC : '/';
}


static int
expsortcmp(const void *p1, const void *p2)
{
const char *s1 = *(const char * const *)p1;
const char *s2 = *(const char * const *)p2;

return (strcoll(s1, s2));
}



static wchar_t
get_wc(const char **p)
{
wchar_t c;
int chrlen;

chrlen = mbtowc(&c, *p, 4);
if (chrlen == 0)
return 0;
else if (chrlen == -1)
c = 0;
else
*p += chrlen;
return c;
}









static int
match_charclass(const char *p, wchar_t chr, const char **end)
{
char name[20];
const char *nameend;
wctype_t cclass;

*end = NULL;
p++;
nameend = strstr(p, ":]");
if (nameend == NULL || (size_t)(nameend - p) >= sizeof(name) ||
nameend == p)
return 0;
memcpy(name, p, nameend - p);
name[nameend - p] = '\0';
*end = nameend + 2;
cclass = wctype(name);

if (cclass == 0)
return 0;
return iswctype(chr, cclass);
}






static int
patmatch(const char *pattern, const char *string)
{
const char *p, *q, *end;
const char *bt_p, *bt_q;
char c;
wchar_t wc, wc2;

p = pattern;
q = string;
bt_p = NULL;
bt_q = NULL;
for (;;) {
switch (c = *p++) {
case '\0':
if (*q != '\0')
goto backtrack;
return 1;
case CTLESC:
if (*q++ != *p++)
goto backtrack;
break;
case '?':
if (*q == '\0')
return 0;
if (localeisutf8) {
wc = get_wc(&q);




if (wc == 0)
goto backtrack;
} else
q++;
break;
case '*':
c = *p;
while (c == '*')
c = *++p;




if (c == '\0')
return 1;






bt_p = p;
bt_q = q;
break;
case '[': {
const char *savep, *saveq;
int invert, found;
wchar_t chr;

savep = p, saveq = q;
invert = 0;
if (*p == '!' || *p == '^') {
invert++;
p++;
}
found = 0;
if (*q == '\0')
return 0;
if (localeisutf8) {
chr = get_wc(&q);
if (chr == 0)
goto backtrack;
} else
chr = (unsigned char)*q++;
c = *p++;
do {
if (c == '\0') {
p = savep, q = saveq;
c = '[';
goto dft;
}
if (c == '[' && *p == ':') {
found |= match_charclass(p, chr, &end);
if (end != NULL) {
p = end;
continue;
}
}
if (c == CTLESC)
c = *p++;
if (localeisutf8 && c & 0x80) {
p--;
wc = get_wc(&p);
if (wc == 0)
return 0;
} else
wc = (unsigned char)c;
if (*p == '-' && p[1] != ']') {
p++;
if (*p == CTLESC)
p++;
if (localeisutf8) {
wc2 = get_wc(&p);
if (wc2 == 0)
return 0;
} else
wc2 = (unsigned char)*p++;
if ( collate_range_cmp(chr, wc) >= 0
&& collate_range_cmp(chr, wc2) <= 0
)
found = 1;
} else {
if (chr == wc)
found = 1;
}
} while ((c = *p++) != ']');
if (found == invert)
goto backtrack;
break;
}
dft: default:
if (*q == '\0')
return 0;
if (*q++ == c)
break;
backtrack:





if (bt_p == NULL)
return 0;
if (*bt_q == '\0')
return 0;
bt_q++;
p = bt_p;
q = bt_q;
break;
}
}
}







void
rmescapes(char *str)
{
char *p, *q;

p = str;
while (*p != CTLESC && *p != CTLQUOTEMARK && *p != CTLQUOTEEND) {
if (*p++ == '\0')
return;
}
q = p;
while (*p) {
if (*p == CTLQUOTEMARK || *p == CTLQUOTEEND) {
p++;
continue;
}
if (*p == CTLESC)
p++;
*q++ = *p++;
}
*q = '\0';
}







int
casematch(union node *pattern, const char *val)
{
struct stackmark smark;
struct nodelist *argbackq;
int result;
char *p;

setstackmark(&smark);
argbackq = pattern->narg.backquote;
STARTSTACKSTR(expdest);
argstr(pattern->narg.text, &argbackq, EXP_TILDE | EXP_CASE, NULL);
STPUTC('\0', expdest);
p = grabstackstr(expdest);
result = patmatch(p, val);
popstackmark(&smark);
return result;
}





static void
cvtnum(int num, char *buf)
{
char temp[32];
int neg = num < 0;
char *p = temp + 31;

temp[31] = '\0';

do {
*--p = num % 10 + '0';
} while ((num /= 10) != 0);

if (neg)
*--p = '-';

memcpy(buf, p, temp + 32 - p);
}





int
wordexpcmd(int argc, char **argv)
{
size_t len;
int i;

out1fmt("%08x", argc - 1);
for (i = 1, len = 0; i < argc; i++)
len += strlen(argv[i]);
out1fmt("%08x", (int)len);
for (i = 1; i < argc; i++)
outbin(argv[i], strlen(argv[i]) + 1, out1);
return (0);
}





int
freebsd_wordexpcmd(int argc __unused, char **argv __unused)
{
struct arglist arglist;
union node *args, *n;
size_t len;
int ch;
int protected = 0;
int fd = -1;
int i;

while ((ch = nextopt("f:p")) != '\0') {
switch (ch) {
case 'f':
fd = number(shoptarg);
break;
case 'p':
protected = 1;
break;
}
}
if (*argptr != NULL)
error("wrong number of arguments");
if (fd < 0)
error("missing fd");
INTOFF;
setinputfd(fd, 1);
INTON;
args = parsewordexp();
popfile();
if (protected)
for (n = args; n != NULL; n = n->narg.next) {
if (n->narg.backquote != NULL) {
outcslow('C', out1);
error("command substitution disabled");
}
}
outcslow(' ', out1);
emptyarglist(&arglist);
for (n = args; n != NULL; n = n->narg.next)
expandarg(n, &arglist, EXP_FULL | EXP_TILDE);
for (i = 0, len = 0; i < arglist.count; i++)
len += strlen(arglist.args[i]);
out1fmt("%016x %016zx", arglist.count, len);
for (i = 0; i < arglist.count; i++)
outbin(arglist.args[i], strlen(arglist.args[i]) + 1, out1);
return (0);
}
