
































#include <ctype.h>
#include "flexdef.h"
#include "parse.h"



extern char *yytext;
extern FILE *yyout;
extern int yylval;
bool no_section3_escape = false;
int yylex (void)
{
int toktype;
static int beglin = false;

if (eofseen) {
toktype = EOF;
} else {
toktype = flexscan ();
}
if (toktype == EOF || toktype == 0) {
eofseen = 1;

if (sectnum == 1) {
synerr (_("premature EOF"));
sectnum = 2;
toktype = SECTEND;
}

else
toktype = 0;
}

if (trace) {
if (beglin) {
fprintf (stderr, "%d\t", num_rules + 1);
beglin = 0;
}

switch (toktype) {
case '<':
case '>':
case '^':
case '$':
case '"':
case '[':
case ']':
case '{':
case '}':
case '|':
case '(':
case ')':
case '-':
case '/':
case '\\':
case '?':
case '.':
case '*':
case '+':
case ',':
(void) putc (toktype, stderr);
break;

case '\n':
(void) putc ('\n', stderr);

if (sectnum == 2)
beglin = 1;

break;

case SCDECL:
fputs ("%s", stderr);
break;

case XSCDECL:
fputs ("%x", stderr);
break;

case SECTEND:
fputs ("%%\n", stderr);





if (sectnum == 2)
beglin = 1;

break;

case NAME:
fprintf (stderr, "'%s'", nmstr);
break;

case CHAR:
switch (yylval) {
case '<':
case '>':
case '^':
case '$':
case '"':
case '[':
case ']':
case '{':
case '}':
case '|':
case '(':
case ')':
case '-':
case '/':
case '\\':
case '?':
case '.':
case '*':
case '+':
case ',':
fprintf (stderr, "\\%c", yylval);
break;

default:
if (!isascii (yylval) || !isprint (yylval)) {
if(trace_hex)
fprintf (stderr, "\\x%02x", (unsigned int) yylval);
else
fprintf (stderr, "\\%.3o", (unsigned int) yylval);
} else
(void) putc (yylval, stderr);
break;
}

break;

case NUMBER:
fprintf (stderr, "%d", yylval);
break;

case PREVCCL:
fprintf (stderr, "[%d]", yylval);
break;

case EOF_OP:
fprintf (stderr, "<<EOF>>");
break;

case TOK_OPTION:
fprintf (stderr, "%s ", yytext);
break;

case TOK_OUTFILE:
case TOK_PREFIX:
case CCE_ALNUM:
case CCE_ALPHA:
case CCE_BLANK:
case CCE_CNTRL:
case CCE_DIGIT:
case CCE_GRAPH:
case CCE_LOWER:
case CCE_PRINT:
case CCE_PUNCT:
case CCE_SPACE:
case CCE_UPPER:
case CCE_XDIGIT:
fprintf (stderr, "%s", yytext);
break;

case 0:
fprintf (stderr, _("End Marker\n"));
break;

default:
fprintf (stderr,
_
("*Something Weird* - tok: %d val: %d\n"),
toktype, yylval);
break;
}
}

return toktype;
}
