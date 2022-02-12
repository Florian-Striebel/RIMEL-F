





























#include "file.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

enum __e_fmtcheck_types {
FMTCHECK_START,
FMTCHECK_SHORT,
FMTCHECK_INT,
FMTCHECK_LONG,
FMTCHECK_QUAD,
FMTCHECK_SHORTPOINTER,
FMTCHECK_INTPOINTER,
FMTCHECK_LONGPOINTER,
FMTCHECK_QUADPOINTER,
FMTCHECK_DOUBLE,
FMTCHECK_LONGDOUBLE,
FMTCHECK_STRING,
FMTCHECK_WIDTH,
FMTCHECK_PRECISION,
FMTCHECK_DONE,
FMTCHECK_UNKNOWN
};
typedef enum __e_fmtcheck_types EFT;

#define RETURN(pf,f,r) do { *(pf) = (f); return r; } while (0)




static EFT
get_next_format_from_precision(const char **pf)
{
int sh, lg, quad, longdouble;
const char *f;

sh = lg = quad = longdouble = 0;

f = *pf;
switch (*f) {
case 'h':
f++;
sh = 1;
break;
case 'l':
f++;
if (!*f) RETURN(pf,f,FMTCHECK_UNKNOWN);
if (*f == 'l') {
f++;
quad = 1;
} else {
lg = 1;
}
break;
case 'q':
f++;
quad = 1;
break;
case 'L':
f++;
longdouble = 1;
break;
#if defined(WIN32)
case 'I':
f++;
if (!*f) RETURN(pf,f,FMTCHECK_UNKNOWN);
if (*f == '3' && f[1] == '2') {
f += 2;
} else if (*f == '6' && f[1] == '4') {
f += 2;
quad = 1;
}
#if defined(_WIN64)
else {
quad = 1;
}
#endif
break;
#endif
default:
break;
}
if (!*f) RETURN(pf,f,FMTCHECK_UNKNOWN);
if (strchr("diouxX", *f)) {
if (longdouble)
RETURN(pf,f,FMTCHECK_UNKNOWN);
if (lg)
RETURN(pf,f,FMTCHECK_LONG);
if (quad)
RETURN(pf,f,FMTCHECK_QUAD);
RETURN(pf,f,FMTCHECK_INT);
}
if (*f == 'n') {
if (longdouble)
RETURN(pf,f,FMTCHECK_UNKNOWN);
if (sh)
RETURN(pf,f,FMTCHECK_SHORTPOINTER);
if (lg)
RETURN(pf,f,FMTCHECK_LONGPOINTER);
if (quad)
RETURN(pf,f,FMTCHECK_QUADPOINTER);
RETURN(pf,f,FMTCHECK_INTPOINTER);
}
if (strchr("DOU", *f)) {
if (sh + lg + quad + longdouble)
RETURN(pf,f,FMTCHECK_UNKNOWN);
RETURN(pf,f,FMTCHECK_LONG);
}
if (strchr("eEfg", *f)) {
if (longdouble)
RETURN(pf,f,FMTCHECK_LONGDOUBLE);
if (sh + lg + quad)
RETURN(pf,f,FMTCHECK_UNKNOWN);
RETURN(pf,f,FMTCHECK_DOUBLE);
}
if (*f == 'c') {
if (sh + lg + quad + longdouble)
RETURN(pf,f,FMTCHECK_UNKNOWN);
RETURN(pf,f,FMTCHECK_INT);
}
if (*f == 's') {
if (sh + lg + quad + longdouble)
RETURN(pf,f,FMTCHECK_UNKNOWN);
RETURN(pf,f,FMTCHECK_STRING);
}
if (*f == 'p') {
if (sh + lg + quad + longdouble)
RETURN(pf,f,FMTCHECK_UNKNOWN);
RETURN(pf,f,FMTCHECK_LONG);
}
RETURN(pf,f,FMTCHECK_UNKNOWN);

}

static EFT
get_next_format_from_width(const char **pf)
{
const char *f;

f = *pf;
if (*f == '.') {
f++;
if (*f == '*') {
RETURN(pf,f,FMTCHECK_PRECISION);
}

while (isdigit((unsigned char)*f)) f++;
if (!*f) RETURN(pf,f,FMTCHECK_UNKNOWN);
}
RETURN(pf,f,get_next_format_from_precision(pf));

}

static EFT
get_next_format(const char **pf, EFT eft)
{
int infmt;
const char *f;

if (eft == FMTCHECK_WIDTH) {
(*pf)++;
return get_next_format_from_width(pf);
} else if (eft == FMTCHECK_PRECISION) {
(*pf)++;
return get_next_format_from_precision(pf);
}

f = *pf;
infmt = 0;
while (!infmt) {
f = strchr(f, '%');
if (f == NULL)
RETURN(pf,f,FMTCHECK_DONE);
f++;
if (!*f)
RETURN(pf,f,FMTCHECK_UNKNOWN);
if (*f != '%')
infmt = 1;
else
f++;
}


while (*f && (strchr("#0- +", *f)))
f++;

if (*f == '*') {
RETURN(pf,f,FMTCHECK_WIDTH);
}

while (isdigit((unsigned char)*f)) f++;
if (!*f) {
RETURN(pf,f,FMTCHECK_UNKNOWN);
}

RETURN(pf,f,get_next_format_from_width(pf));

}

const char *
fmtcheck(const char *f1, const char *f2)
{
const char *f1p, *f2p;
EFT f1t, f2t;

if (!f1) return f2;

f1p = f1;
f1t = FMTCHECK_START;
f2p = f2;
f2t = FMTCHECK_START;
while ((f1t = get_next_format(&f1p, f1t)) != FMTCHECK_DONE) {
if (f1t == FMTCHECK_UNKNOWN)
return f2;
f2t = get_next_format(&f2p, f2t);
if (f1t != f2t)
return f2;
}
return f1;
}
