



























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "ed.h"

const char *errmsg = "";



pattern_t *
get_compiled_pattern(void)
{
static pattern_t *expr = NULL;
static char error[1024];

char *exprs;
char delimiter;
int n;

if ((delimiter = *ibufp) == ' ') {
errmsg = "invalid pattern delimiter";
return NULL;
} else if (delimiter == '\n' || *++ibufp == '\n' || *ibufp == delimiter) {
if (!expr)
errmsg = "no previous pattern";
return expr;
} else if ((exprs = extract_pattern(delimiter)) == NULL)
return NULL;

if (expr && !patlock)
regfree(expr);
else if ((expr = (pattern_t *) malloc(sizeof(pattern_t))) == NULL) {
fprintf(stderr, "%s\n", strerror(errno));
errmsg = "out of memory";
return NULL;
}
patlock = 0;
if ((n = regcomp(expr, exprs, 0))) {
regerror(n, expr, error, sizeof error);
errmsg = error;
free(expr);
return expr = NULL;
}
return expr;
}




char *
extract_pattern(int delimiter)
{
static char *lhbuf = NULL;
static int lhbufsz = 0;

char *nd;
int len;

for (nd = ibufp; *nd != delimiter && *nd != '\n'; nd++)
switch (*nd) {
default:
break;
case '[':
if ((nd = parse_char_class(nd + 1)) == NULL) {
errmsg = "unbalanced brackets ([])";
return NULL;
}
break;
case '\\':
if (*++nd == '\n') {
errmsg = "trailing backslash (\\)";
return NULL;
}
break;
}
len = nd - ibufp;
REALLOC(lhbuf, lhbufsz, len + 1, NULL);
memcpy(lhbuf, ibufp, len);
lhbuf[len] = '\0';
ibufp = nd;
return (isbinary) ? NUL_TO_NEWLINE(lhbuf, len) : lhbuf;
}



char *
parse_char_class(char *s)
{
int c, d;

if (*s == '^')
s++;
if (*s == ']')
s++;
for (; *s != ']' && *s != '\n'; s++)
if (*s == '[' && ((d = *(s+1)) == '.' || d == ':' || d == '='))
for (s++, c = *++s; *s != ']' || c != d; s++)
if ((c = *s) == '\n')
return NULL;
return (*s == ']') ? s : NULL;
}
