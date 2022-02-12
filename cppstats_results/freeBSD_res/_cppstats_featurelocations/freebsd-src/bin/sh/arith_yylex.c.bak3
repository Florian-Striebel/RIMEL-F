

































#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "arith_yacc.h"
#include "expand.h"
#include "error.h"
#include "memalloc.h"
#include "parser.h"
#include "syntax.h"

#if ARITH_BOR + 11 != ARITH_BORASS || ARITH_ASS + 11 != ARITH_EQ
#error Arithmetic tokens are out of order.
#endif

arith_t
strtoarith_t(const char *restrict nptr, char **restrict endptr)
{
arith_t val;

while (isspace((unsigned char)*nptr))
nptr++;
switch (*nptr) {
case '-':
return strtoimax(nptr, endptr, 0);
case '0':
return (arith_t)strtoumax(nptr, endptr, 0);
default:
val = (arith_t)strtoumax(nptr, endptr, 0);
if (val >= 0)
return val;
else if (val == ARITH_MIN) {
errno = ERANGE;
return ARITH_MIN;
} else {
errno = ERANGE;
return ARITH_MAX;
}
}
}

int
yylex(void)
{
int value;
const char *buf = arith_buf;
char *end;
const char *p;

for (;;) {
value = *buf;
switch (value) {
case ' ':
case '\t':
case '\n':
buf++;
continue;
default:
return ARITH_BAD;
case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
yylval.val = strtoarith_t(buf, &end);
arith_buf = end;
return ARITH_NUM;
case 'A':
case 'B':
case 'C':
case 'D':
case 'E':
case 'F':
case 'G':
case 'H':
case 'I':
case 'J':
case 'K':
case 'L':
case 'M':
case 'N':
case 'O':
case 'P':
case 'Q':
case 'R':
case 'S':
case 'T':
case 'U':
case 'V':
case 'W':
case 'X':
case 'Y':
case 'Z':
case '_':
case 'a':
case 'b':
case 'c':
case 'd':
case 'e':
case 'f':
case 'g':
case 'h':
case 'i':
case 'j':
case 'k':
case 'l':
case 'm':
case 'n':
case 'o':
case 'p':
case 'q':
case 'r':
case 's':
case 't':
case 'u':
case 'v':
case 'w':
case 'x':
case 'y':
case 'z':
p = buf;
while (buf++, is_in_name(*buf))
;
yylval.name = stalloc(buf - p + 1);
memcpy(yylval.name, p, buf - p);
yylval.name[buf - p] = '\0';
value = ARITH_VAR;
goto out;
case '=':
value += ARITH_ASS - '=';
checkeq:
buf++;
checkeqcur:
if (*buf != '=')
goto out;
value += 11;
break;
case '>':
switch (*++buf) {
case '=':
value += ARITH_GE - '>';
break;
case '>':
value += ARITH_RSHIFT - '>';
goto checkeq;
default:
value += ARITH_GT - '>';
goto out;
}
break;
case '<':
switch (*++buf) {
case '=':
value += ARITH_LE - '<';
break;
case '<':
value += ARITH_LSHIFT - '<';
goto checkeq;
default:
value += ARITH_LT - '<';
goto out;
}
break;
case '|':
if (*++buf != '|') {
value += ARITH_BOR - '|';
goto checkeqcur;
}
value += ARITH_OR - '|';
break;
case '&':
if (*++buf != '&') {
value += ARITH_BAND - '&';
goto checkeqcur;
}
value += ARITH_AND - '&';
break;
case '!':
if (*++buf != '=') {
value += ARITH_NOT - '!';
goto out;
}
value += ARITH_NE - '!';
break;
case 0:
goto out;
case '(':
value += ARITH_LPAREN - '(';
break;
case ')':
value += ARITH_RPAREN - ')';
break;
case '*':
value += ARITH_MUL - '*';
goto checkeq;
case '/':
value += ARITH_DIV - '/';
goto checkeq;
case '%':
value += ARITH_REM - '%';
goto checkeq;
case '+':
if (buf[1] == '+')
return ARITH_BAD;
value += ARITH_ADD - '+';
goto checkeq;
case '-':
if (buf[1] == '-')
return ARITH_BAD;
value += ARITH_SUB - '-';
goto checkeq;
case '~':
value += ARITH_BNOT - '~';
break;
case '^':
value += ARITH_BXOR - '^';
goto checkeq;
case '?':
value += ARITH_QMARK - '?';
break;
case ':':
value += ARITH_COLON - ':';
break;
}
break;
}

buf++;
out:
arith_buf = buf;
return value;
}
