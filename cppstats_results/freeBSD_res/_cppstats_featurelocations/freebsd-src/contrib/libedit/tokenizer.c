

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)tokenizer.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: tokenizer.c,v 1.28 2016/04/11 18:56:31 christos Exp $");
#endif
#endif





#include <stdlib.h>
#include <string.h>

#include "histedit.h"

typedef enum {
Q_none, Q_single, Q_double, Q_one, Q_doubleone
} quote_t;

#define TOK_KEEP 1
#define TOK_EAT 2

#define WINCR 20
#define AINCR 10

#define IFS STR("\t \n")

#define tok_malloc(a) malloc(a)
#define tok_free(a) free(a)
#define tok_realloc(a, b) realloc(a, b)

#if defined(NARROWCHAR)
#define Char char
#define FUN(prefix, rest) prefix ##_ ##rest
#define TYPE(type) type
#define STR(x) x
#define Strchr(s, c) strchr(s, c)
#define tok_strdup(s) strdup(s)
#else
#define Char wchar_t
#define FUN(prefix, rest) prefix ##_w ##rest
#define TYPE(type) type ##W
#define STR(x) L ##x
#define Strchr(s, c) wcschr(s, c)
#define tok_strdup(s) wcsdup(s)
#endif

struct TYPE(tokenizer) {
Char *ifs;
size_t argc, amax;
Char **argv;
Char *wptr, *wmax;
Char *wstart;
Char *wspace;
quote_t quote;
int flags;
};


static void FUN(tok,finish)(TYPE(Tokenizer) *);





static void
FUN(tok,finish)(TYPE(Tokenizer) *tok)
{

*tok->wptr = '\0';
if ((tok->flags & TOK_KEEP) || tok->wptr != tok->wstart) {
tok->argv[tok->argc++] = tok->wstart;
tok->argv[tok->argc] = NULL;
tok->wstart = ++tok->wptr;
}
tok->flags &= ~TOK_KEEP;
}





TYPE(Tokenizer) *
FUN(tok,init)(const Char *ifs)
{
TYPE(Tokenizer) *tok = tok_malloc(sizeof(*tok));

if (tok == NULL)
return NULL;
tok->ifs = tok_strdup(ifs ? ifs : IFS);
if (tok->ifs == NULL) {
tok_free(tok);
return NULL;
}
tok->argc = 0;
tok->amax = AINCR;
tok->argv = tok_malloc(sizeof(*tok->argv) * tok->amax);
if (tok->argv == NULL) {
tok_free(tok->ifs);
tok_free(tok);
return NULL;
}
tok->argv[0] = NULL;
tok->wspace = tok_malloc(WINCR * sizeof(*tok->wspace));
if (tok->wspace == NULL) {
tok_free(tok->argv);
tok_free(tok->ifs);
tok_free(tok);
return NULL;
}
tok->wmax = tok->wspace + WINCR;
tok->wstart = tok->wspace;
tok->wptr = tok->wspace;
tok->flags = 0;
tok->quote = Q_none;

return tok;
}





void
FUN(tok,reset)(TYPE(Tokenizer) *tok)
{

tok->argc = 0;
tok->wstart = tok->wspace;
tok->wptr = tok->wspace;
tok->flags = 0;
tok->quote = Q_none;
}





void
FUN(tok,end)(TYPE(Tokenizer) *tok)
{

tok_free(tok->ifs);
tok_free(tok->wspace);
tok_free(tok->argv);
tok_free(tok);
}




















int
FUN(tok,line)(TYPE(Tokenizer) *tok, const TYPE(LineInfo) *line,
int *argc, const Char ***argv, int *cursorc, int *cursoro)
{
const Char *ptr;
int cc, co;

cc = co = -1;
ptr = line->buffer;
for (ptr = line->buffer; ;ptr++) {
if (ptr >= line->lastchar)
ptr = STR("");
if (ptr == line->cursor) {
cc = (int)tok->argc;
co = (int)(tok->wptr - tok->wstart);
}
switch (*ptr) {
case '\'':
tok->flags |= TOK_KEEP;
tok->flags &= ~TOK_EAT;
switch (tok->quote) {
case Q_none:
tok->quote = Q_single;

break;

case Q_single:
tok->quote = Q_none;
break;

case Q_one:
tok->quote = Q_none;
*tok->wptr++ = *ptr;
break;

case Q_double:
*tok->wptr++ = *ptr;
break;

case Q_doubleone:
tok->quote = Q_double;
*tok->wptr++ = *ptr;
break;

default:
return -1;
}
break;

case '"':
tok->flags &= ~TOK_EAT;
tok->flags |= TOK_KEEP;
switch (tok->quote) {
case Q_none:
tok->quote = Q_double;
break;

case Q_double:
tok->quote = Q_none;
break;

case Q_one:
tok->quote = Q_none;
*tok->wptr++ = *ptr;
break;

case Q_single:
*tok->wptr++ = *ptr;
break;

case Q_doubleone:
tok->quote = Q_double;
*tok->wptr++ = *ptr;
break;

default:
return -1;
}
break;

case '\\':
tok->flags |= TOK_KEEP;
tok->flags &= ~TOK_EAT;
switch (tok->quote) {
case Q_none:
tok->quote = Q_one;
break;

case Q_double:
tok->quote = Q_doubleone;
break;

case Q_one:
*tok->wptr++ = *ptr;
tok->quote = Q_none;
break;

case Q_single:
*tok->wptr++ = *ptr;
break;

case Q_doubleone:
tok->quote = Q_double;
*tok->wptr++ = *ptr;
break;

default:
return -1;
}
break;

case '\n':
tok->flags &= ~TOK_EAT;
switch (tok->quote) {
case Q_none:
goto tok_line_outok;

case Q_single:
case Q_double:
*tok->wptr++ = *ptr;
break;

case Q_doubleone:
tok->flags |= TOK_EAT;
tok->quote = Q_double;
break;

case Q_one:
tok->flags |= TOK_EAT;
tok->quote = Q_none;
break;

default:
return 0;
}
break;

case '\0':
switch (tok->quote) {
case Q_none:

if (tok->flags & TOK_EAT) {
tok->flags &= ~TOK_EAT;
return 3;
}
goto tok_line_outok;

case Q_single:
return 1;

case Q_double:
return 2;

case Q_doubleone:
tok->quote = Q_double;
*tok->wptr++ = *ptr;
break;

case Q_one:
tok->quote = Q_none;
*tok->wptr++ = *ptr;
break;

default:
return -1;
}
break;

default:
tok->flags &= ~TOK_EAT;
switch (tok->quote) {
case Q_none:
if (Strchr(tok->ifs, *ptr) != NULL)
FUN(tok,finish)(tok);
else
*tok->wptr++ = *ptr;
break;

case Q_single:
case Q_double:
*tok->wptr++ = *ptr;
break;


case Q_doubleone:
*tok->wptr++ = '\\';
tok->quote = Q_double;
*tok->wptr++ = *ptr;
break;

case Q_one:
tok->quote = Q_none;
*tok->wptr++ = *ptr;
break;

default:
return -1;

}
break;
}

if (tok->wptr >= tok->wmax - 4) {
size_t size = (size_t)(tok->wmax - tok->wspace + WINCR);
Char *s = tok_realloc(tok->wspace,
size * sizeof(*s));
if (s == NULL)
return -1;

if (s != tok->wspace) {
size_t i;
for (i = 0; i < tok->argc; i++) {
tok->argv[i] =
(tok->argv[i] - tok->wspace) + s;
}
tok->wptr = (tok->wptr - tok->wspace) + s;
tok->wstart = (tok->wstart - tok->wspace) + s;
tok->wspace = s;
}
tok->wmax = s + size;
}
if (tok->argc >= tok->amax - 4) {
Char **p;
tok->amax += AINCR;
p = tok_realloc(tok->argv, tok->amax * sizeof(*p));
if (p == NULL) {
tok->amax -= AINCR;
return -1;
}
tok->argv = p;
}
}
tok_line_outok:
if (cc == -1 && co == -1) {
cc = (int)tok->argc;
co = (int)(tok->wptr - tok->wstart);
}
if (cursorc != NULL)
*cursorc = cc;
if (cursoro != NULL)
*cursoro = co;
FUN(tok,finish)(tok);
*argv = (const Char **)tok->argv;
*argc = (int)tok->argc;
return 0;
}





int
FUN(tok,str)(TYPE(Tokenizer) *tok, const Char *line, int *argc,
const Char ***argv)
{
TYPE(LineInfo) li;

memset(&li, 0, sizeof(li));
li.buffer = line;
li.cursor = li.lastchar = Strchr(line, '\0');
return FUN(tok,line)(tok, &li, argc, argv, NULL, NULL);
}
