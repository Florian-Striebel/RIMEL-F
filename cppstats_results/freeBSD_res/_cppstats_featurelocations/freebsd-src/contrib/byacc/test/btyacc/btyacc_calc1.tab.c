



#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYCHECK "yyyymmdd"

#define YYEMPTY (-1)
#define yyclearin (yychar = YYEMPTY)
#define yyerrok (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM (-2)
#define YYEOF 0
#undef YYBTYACC
#define YYBTYACC 1
#define YYDEBUGSTR (yytrial ? YYPREFIX "debug(trial)" : YYPREFIX "debug")

#if !defined(yyparse)
#define yyparse calc1_parse
#endif

#if !defined(yylex)
#define yylex calc1_lex
#endif

#if !defined(yyerror)
#define yyerror calc1_error
#endif

#if !defined(yychar)
#define yychar calc1_char
#endif

#if !defined(yyval)
#define yyval calc1_val
#endif

#if !defined(yylval)
#define yylval calc1_lval
#endif

#if !defined(yydebug)
#define yydebug calc1_debug
#endif

#if !defined(yynerrs)
#define yynerrs calc1_nerrs
#endif

#if !defined(yyerrflag)
#define yyerrflag calc1_errflag
#endif

#if !defined(yylhs)
#define yylhs calc1_lhs
#endif

#if !defined(yylen)
#define yylen calc1_len
#endif

#if !defined(yydefred)
#define yydefred calc1_defred
#endif

#if !defined(yystos)
#define yystos calc1_stos
#endif

#if !defined(yydgoto)
#define yydgoto calc1_dgoto
#endif

#if !defined(yysindex)
#define yysindex calc1_sindex
#endif

#if !defined(yyrindex)
#define yyrindex calc1_rindex
#endif

#if !defined(yygindex)
#define yygindex calc1_gindex
#endif

#if !defined(yytable)
#define yytable calc1_table
#endif

#if !defined(yycheck)
#define yycheck calc1_check
#endif

#if !defined(yyname)
#define yyname calc1_name
#endif

#if !defined(yyrule)
#define yyrule calc1_rule
#endif

#if YYBTYACC

#if !defined(yycindex)
#define yycindex calc1_cindex
#endif

#if !defined(yyctable)
#define yyctable calc1_ctable
#endif

#endif

#define YYPREFIX "calc1_"

#define YYPURE 1

#line 3 "btyacc_calc1.y"



#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

typedef struct interval
{
double lo, hi;
}
INTERVAL;

INTERVAL vmul(double, double, INTERVAL);
INTERVAL vdiv(double, double, INTERVAL);

int dcheck(INTERVAL);

double dreg[26];
INTERVAL vreg[26];

#if defined(YYSTYPE)
#undef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#if !defined(YYSTYPE_IS_DECLARED)
#define YYSTYPE_IS_DECLARED 1
#line 29 "btyacc_calc1.y"
typedef union
{
int ival;
double dval;
INTERVAL vval;
} YYSTYPE;
#endif
#line 158 "btyacc_calc1.tab.c"


#if defined(YYPARSE_PARAM)

#if defined(YYPARSE_PARAM_TYPE)
#define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
#else
#define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
#endif
#else
#define YYPARSE_DECL() yyparse(void)
#endif


#if defined(YYLEX_PARAM)
#if defined(YYLEX_PARAM_TYPE)
#define YYLEX_DECL() yylex(YYSTYPE *yylval, YYLEX_PARAM_TYPE YYLEX_PARAM)
#else
#define YYLEX_DECL() yylex(YYSTYPE *yylval, void * YYLEX_PARAM)
#endif
#define YYLEX yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX_DECL() yylex(YYSTYPE *yylval)
#define YYLEX yylex(&yylval)
#endif


#if !defined(YYERROR_DECL)
#define YYERROR_DECL() yyerror(const char *s)
#endif
#if !defined(YYERROR_CALL)
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define DREG 257
#define VREG 258
#define CONST 259
#define UMINUS 260
#define YYERRCODE 256
typedef int YYINT;
static const YYINT calc1_lhs[] = { -1,
0, 0, 0, 3, 3, 3, 3, 1, 1, 1,
1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
};
static const YYINT calc1_len[] = { 2,
0, 3, 3, 1, 1, 3, 3, 1, 1, 3,
3, 3, 3, 2, 3, 1, 5, 1, 3, 3,
3, 3, 3, 3, 3, 3, 2, 3,
};
static const YYINT calc1_defred[] = { 1,
0, 0, 0, 0, 8, 0, 0, 0, 0, 0,
3, 0, 0, 9, 18, 0, 27, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
0, 0, 0, 15, 0, 28, 0, 0, 0, 0,
0, 24, 0, 26, 0, 0, 23, 25, 14, 0,
0, 0, 0, 0, 0, 0, 0, 12, 13, 17,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT calc1_stos[] = { 0,
262, 256, 257, 258, 259, 45, 40, 263, 264, 265,
10, 61, 61, 257, 258, 263, 264, 263, 264, 43,
45, 42, 47, 43, 45, 42, 47, 10, 45, 40,
263, 263, 264, 41, 44, 41, 263, 264, 263, 264,
263, 264, 263, 264, 264, 264, 264, 264, 263, 263,
43, 45, 42, 47, 263, 263, 263, 263, 263, 41,
};
#endif
static const YYINT calc1_dgoto[] = { 1,
32, 9, 10,
};
static const YYINT calc1_sindex[] = { 0,
-40, -9, -59, -54, 0, -37, -37, 0, 82, 4,
0, -34, -37, 0, 0, 0, 0, -31, -25, -37,
-37, -37, -37, -37, -37, -37, -37, 0, -34, -34,
132, 0, 82, 0, -34, 0, 0, -12, 0, -12,
0, 0, 0, 0, -12, -12, 0, 0, 0, 112,
-34, -34, -34, -34, 119, -11, -11, 0, 0, 0,
};
static const YYINT calc1_rindex[] = { 0,
0, 0, 51, 58, 0, 0, 0, 0, 11, 0,
0, 0, 0, 0, 0, -16, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
19, 9, 27, 0, 0, 0, -5, 41, -4, 77,
-2, 0, 8, 0, 78, 85, 0, 0, 0, 0,
0, 0, 0, 0, 0, 92, 99, 0, 0, 0,
};
#if YYBTYACC
static const YYINT calc1_cindex[] = { 0,
0, 0, 0, 0, 0, 0, 0, 65, 0, 0,
0, 0, 0, 0, 0, 2, 0, 126, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 138, 0, 0, 0, 0, 17, 0, 24, 0,
31, 0, 38, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
#endif
static const YYINT calc1_gindex[] = { 0,
3, 125, 0,
};
#define YYTABLESIZE 225
static const YYINT calc1_table[] = { 7,
11, 12, 7, 8, 6, 30, 13, 6, 16, 18,
29, 14, 35, 28, 31, 36, 26, 24, 16, 25,
5, 27, 37, 39, 41, 43, 20, 14, 6, 26,
53, 49, 50, 23, 27, 54, 7, 55, 10, 11,
26, 12, 14, 14, 14, 0, 14, 29, 14, 16,
20, 13, 0, 56, 57, 58, 59, 20, 6, 20,
9, 20, 0, 9, 23, 6, 23, 18, 23, 0,
9, 26, 26, 26, 11, 26, 0, 26, 29, 29,
29, 20, 29, 20, 29, 20, 22, 19, 0, 0,
0, 0, 9, 9, 21, 9, 0, 9, 0, 18,
18, 10, 18, 0, 18, 0, 6, 0, 11, 3,
0, 9, 0, 0, 0, 0, 0, 22, 19, 22,
19, 22, 19, 26, 24, 21, 25, 21, 27, 21,
17, 19, 10, 0, 10, 0, 10, 33, 0, 11,
0, 11, 0, 11, 38, 40, 42, 44, 45, 46,
47, 48, 34, 53, 51, 0, 52, 0, 54, 60,
53, 51, 0, 52, 0, 54, 18, 6, 0, 0,
3, 0, 9, 53, 51, 0, 52, 0, 54, 6,
0, 0, 3, 0, 9, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 2, 3, 4, 5, 14,
15, 5, 14, 0, 5,
};
static const YYINT calc1_check[] = { 40,
10, 61, 40, 1, 45, 40, 61, 45, 6, 7,
45, 10, 44, 10, 12, 41, 42, 43, 10, 45,
10, 47, 20, 21, 22, 23, 10, 44, 10, 42,
42, 29, 30, 10, 47, 47, 10, 35, 44, 44,
10, 44, 41, 42, 43, -1, 45, 10, 47, 41,
10, 44, -1, 51, 52, 53, 54, 41, 42, 43,
10, 45, -1, 47, 41, 42, 43, 10, 45, -1,
47, 41, 42, 43, 10, 45, -1, 47, 41, 42,
43, 41, 45, 43, 47, 45, 10, 10, -1, -1,
-1, -1, 42, 43, 10, 45, -1, 47, -1, 42,
43, 10, 45, -1, 47, -1, 42, 43, 10, 45,
-1, 47, -1, -1, -1, -1, -1, 41, 41, 43,
43, 45, 45, 42, 43, 41, 45, 43, 47, 45,
6, 7, 41, -1, 43, -1, 45, 13, -1, 41,
-1, 43, -1, 45, 20, 21, 22, 23, 24, 25,
26, 27, 41, 42, 43, -1, 45, -1, 47, 41,
42, 43, -1, 45, -1, 47, 41, 42, 43, -1,
45, -1, 47, 42, 43, -1, 45, -1, 47, 42,
43, -1, 45, -1, 47, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, 256, 257, 258, 259, 257,
258, 259, 257, -1, 259,
};
#if YYBTYACC
static const YYINT calc1_ctable[] = { 20,
16, -1, 21, 16, -1, 22, 16, -1, 23, 16,
-1, 4, 16, -1, 14, 16, -1, 34, 16, -1,
10, 16, -1, 11, 16, -1, 12, 16, -1, 13,
16, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1,
};
#endif
#define YYFINAL 1
#if !defined(YYDEBUG)
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 260
#define YYUNDFTOKEN 266
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const calc1_name[] = {

"$end",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'","','","'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,0,
0,"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"error","DREG","VREG","CONST","UMINUS","$accept","lines","dexp","vexp","line",
"illegal-symbol",
};
static const char *const calc1_rule[] = {
"$accept : lines",
"lines :",
"lines : lines line '\\n'",
"lines : lines error '\\n'",
"line : dexp",
"line : vexp",
"line : DREG '=' dexp",
"line : VREG '=' vexp",
"dexp : CONST",
"dexp : DREG",
"dexp : dexp '+' dexp",
"dexp : dexp '-' dexp",
"dexp : dexp '*' dexp",
"dexp : dexp '/' dexp",
"dexp : '-' dexp",
"dexp : '(' dexp ')'",
"vexp : dexp",
"vexp : '(' dexp ',' dexp ')'",
"vexp : VREG",
"vexp : vexp '+' vexp",
"vexp : dexp '+' vexp",
"vexp : vexp '-' vexp",
"vexp : dexp '-' vexp",
"vexp : vexp '*' vexp",
"vexp : dexp '*' vexp",
"vexp : vexp '/' vexp",
"vexp : dexp '/' vexp",
"vexp : '-' vexp",
"vexp : '(' vexp ')'",

};
#endif

#if YYDEBUG
int yydebug;
#endif

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
#if !defined(YYLLOC_DEFAULT)
#define YYLLOC_DEFAULT(loc, rhs, n) do { if (n == 0) { (loc).first_line = YYRHSLOC(rhs, 0).last_line; (loc).first_column = YYRHSLOC(rhs, 0).last_column; (loc).last_line = YYRHSLOC(rhs, 0).last_line; (loc).last_column = YYRHSLOC(rhs, 0).last_column; } else { (loc).first_line = YYRHSLOC(rhs, 1).first_line; (loc).first_column = YYRHSLOC(rhs, 1).first_column; (loc).last_line = YYRHSLOC(rhs, n).last_line; (loc).last_column = YYRHSLOC(rhs, n).last_column; } } while (0)

















#endif
#endif
#if YYBTYACC

#if !defined(YYLVQUEUEGROWTH)
#define YYLVQUEUEGROWTH 32
#endif
#endif


#if defined(YYSTACKSIZE)
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#if defined(YYMAXDEPTH)
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif

#if !defined(YYINITSTACKSIZE)
#define YYINITSTACKSIZE 200
#endif

typedef struct {
unsigned stacksize;
YYINT *s_base;
YYINT *s_mark;
YYINT *s_last;
YYSTYPE *l_base;
YYSTYPE *l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE *p_base;
YYLTYPE *p_mark;
#endif
} YYSTACKDATA;
#if YYBTYACC

struct YYParseState_s
{
struct YYParseState_s *save;
YYSTACKDATA yystack;
int state;
int errflag;
int lexeme;
YYINT ctry;
};
typedef struct YYParseState_s YYParseState;
#endif
#line 174 "btyacc_calc1.y"


int
main (void)
{
while(!feof(stdin)) {
yyparse();
}
return 0;
}

#define BSZ 50

static void
YYERROR_DECL()
{
fprintf(stderr, "%s\n", s);
}



static int
YYLEX_DECL()
{
int c;

while ((c = getchar()) == ' ')
{
}

if (isupper(c))
{
#if YYPURE
(*yylval).ival = c - 'A';
#else
yylval.ival = c - 'A';
#endif
return (VREG);
}
if (islower(c))
{
#if YYPURE
(*yylval).ival = c - 'a';
#else
yylval.ival = c - 'a';
#endif
return (DREG);
}

if (isdigit(c) || c == '.')
{

char buf[BSZ + 1], *cp = buf;
int dot = 0, expr = 0;

for (; (cp - buf) < BSZ; ++cp, c = getchar())
{

*cp = (char) c;
if (isdigit(c))
continue;
if (c == '.')
{
if (dot++ || expr)
return ('.');
continue;
}

if (c == 'e')
{
if (expr++)
return ('e');
continue;
}


break;
}
*cp = '\0';

if ((cp - buf) >= BSZ)
printf("constant too long: truncated\n");
else
ungetc(c, stdin);
#if YYPURE
(*yylval).dval = atof(buf);
#else
yylval.dval = atof(buf);
#endif
return (CONST);
}
return (c);
}

static INTERVAL
hilo(double a, double b, double c, double d)
{


INTERVAL v;

if (a > b)
{
v.hi = a;
v.lo = b;
}
else
{
v.hi = b;
v.lo = a;
}

if (c > d)
{
if (c > v.hi)
v.hi = c;
if (d < v.lo)
v.lo = d;
}
else
{
if (d > v.hi)
v.hi = d;
if (c < v.lo)
v.lo = c;
}
return (v);
}

INTERVAL
vmul(double a, double b, INTERVAL v)
{
return (hilo(a * v.hi, a * v.lo, b * v.hi, b * v.lo));
}

int
dcheck(INTERVAL v)
{
if (v.hi >= 0. && v.lo <= 0.)
{
printf("divisor interval contains 0.\n");
return (1);
}
return (0);
}

INTERVAL
vdiv(double a, double b, INTERVAL v)
{
return (hilo(a / v.hi, a / v.lo, b / v.hi, b / v.lo));
}
#line 620 "btyacc_calc1.tab.c"


#define yydepth (int)(yystack.s_mark - yystack.s_base)
#if YYBTYACC
#define yytrial (yyps->save)
#endif

#if YYDEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>


static int yygrowstack(YYSTACKDATA *data)
{
int i;
unsigned newsize;
YYINT *newss;
YYSTYPE *newvs;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE *newps;
#endif

if ((newsize = data->stacksize) == 0)
newsize = YYINITSTACKSIZE;
else if (newsize >= YYMAXDEPTH)
return YYENOMEM;
else if ((newsize *= 2) > YYMAXDEPTH)
newsize = YYMAXDEPTH;

i = (int) (data->s_mark - data->s_base);
newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
if (newss == NULL)
return YYENOMEM;

data->s_base = newss;
data->s_mark = newss + i;

newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
if (newvs == NULL)
return YYENOMEM;

data->l_base = newvs;
data->l_mark = newvs + i;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
newps = (YYLTYPE *)realloc(data->p_base, newsize * sizeof(*newps));
if (newps == NULL)
return YYENOMEM;

data->p_base = newps;
data->p_mark = newps + i;
#endif

data->stacksize = newsize;
data->s_last = data->s_base + newsize - 1;

#if YYDEBUG
if (yydebug)
fprintf(stderr, "%sdebug: stack size increased to %d\n", YYPREFIX, newsize);
#endif
return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
free(data->s_base);
free(data->l_base);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
free(data->p_base);
#endif
memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data)
#endif
#if YYBTYACC

static YYParseState *
yyNewState(unsigned size)
{
YYParseState *p = (YYParseState *) malloc(sizeof(YYParseState));
if (p == NULL) return NULL;

p->yystack.stacksize = size;
if (size == 0)
{
p->yystack.s_base = NULL;
p->yystack.l_base = NULL;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
p->yystack.p_base = NULL;
#endif
return p;
}
p->yystack.s_base = (YYINT *) malloc(size * sizeof(YYINT));
if (p->yystack.s_base == NULL) return NULL;
p->yystack.l_base = (YYSTYPE *) malloc(size * sizeof(YYSTYPE));
if (p->yystack.l_base == NULL) return NULL;
memset(p->yystack.l_base, 0, size * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
p->yystack.p_base = (YYLTYPE *) malloc(size * sizeof(YYLTYPE));
if (p->yystack.p_base == NULL) return NULL;
memset(p->yystack.p_base, 0, size * sizeof(YYLTYPE));
#endif

return p;
}

static void
yyFreeState(YYParseState *p)
{
yyfreestack(&p->yystack);
free(p);
}
#endif

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
#if YYBTYACC
#define YYVALID do { if (yyps->save) goto yyvalid; } while(0)
#define YYVALID_NESTED do { if (yyps->save && yyps->save->save == 0) goto yyvalid; } while(0)

#endif

int
YYPARSE_DECL()
{
int yyerrflag;
int yychar;
YYSTYPE yyval;
YYSTYPE yylval;
int yynerrs;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE yyloc;
YYLTYPE yylloc;
#endif


YYSTACKDATA yystack;
#if YYBTYACC


static YYParseState *yyps = 0;


static YYParseState *yypath = 0;


static YYSTYPE *yylvals = 0;


static YYSTYPE *yylvp = 0;


static YYSTYPE *yylve = 0;


static YYSTYPE *yylvlim = 0;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

static YYLTYPE *yylpsns = 0;


static YYLTYPE *yylpp = 0;


static YYLTYPE *yylpe = 0;


static YYLTYPE *yylplim = 0;
#endif


static YYINT *yylexp = 0;

static YYINT *yylexemes = 0;
#endif
int yym, yyn, yystate, yyresult;
#if YYBTYACC
int yynewerrflag;
YYParseState *yyerrctx = NULL;
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE yyerror_loc_range[3];
#endif
#if YYDEBUG
const char *yys;

if ((yys = getenv("YYDEBUG")) != NULL)
{
yyn = *yys;
if (yyn >= '0' && yyn <= '9')
yydebug = yyn - '0';
}
if (yydebug)
fprintf(stderr, "%sdebug[<#of symbols on state stack>]\n", YYPREFIX);
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
memset(yyerror_loc_range, 0, sizeof(yyerror_loc_range));
#endif

yyerrflag = 0;
yychar = 0;
memset(&yyval, 0, sizeof(yyval));
memset(&yylval, 0, sizeof(yylval));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
memset(&yyloc, 0, sizeof(yyloc));
memset(&yylloc, 0, sizeof(yylloc));
#endif

#if YYBTYACC
yyps = yyNewState(0); if (yyps == 0) goto yyenomem;
yyps->save = 0;
#endif
yym = 0;
yyn = 0;
yynerrs = 0;
yyerrflag = 0;
yychar = YYEMPTY;
yystate = 0;

#if YYPURE
memset(&yystack, 0, sizeof(yystack));
#endif

if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
yystack.s_mark = yystack.s_base;
yystack.l_mark = yystack.l_base;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yystack.p_mark = yystack.p_base;
#endif
yystate = 0;
*yystack.s_mark = 0;

yyloop:
if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
if (yychar < 0)
{
#if YYBTYACC
do {
if (yylvp < yylve)
{

yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylloc = *yylpp++;
#endif
yychar = *yylexp++;
break;
}
if (yyps->save)
{

if (yylvp == yylvlim)
{
size_t p = (size_t) (yylvp - yylvals);
size_t s = (size_t) (yylvlim - yylvals);

s += YYLVQUEUEGROWTH;
if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL) goto yyenomem;
if ((yylvals = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL) goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
if ((yylpsns = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL) goto yyenomem;
#endif
yylvp = yylve = yylvals + p;
yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp = yylpe = yylpsns + p;
yylplim = yylpsns + s;
#endif
yylexp = yylexemes + p;
}
*yylexp = (YYINT) YYLEX;
*yylvp++ = yylval;
yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
*yylpp++ = yylloc;
yylpe++;
#endif
yychar = *yylexp++;
break;
}

#endif
yychar = YYLEX;
#if YYBTYACC
} while (0);
#endif
if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
if (yydebug)
{
if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)",
YYDEBUGSTR, yydepth, yystate, yychar, yys);
#if defined(YYSTYPE_TOSTRING)
#if YYBTYACC
if (!yytrial)
#endif
fprintf(stderr, " <%s>", YYSTYPE_TOSTRING(yychar, yylval));
#endif
fputc('\n', stderr);
}
#endif
}
#if YYBTYACC


if (((yyn = yycindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
{
YYINT ctry;

if (yypath)
{
YYParseState *save;
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%s[%d]: CONFLICT in state %d: following successful trial parse\n",
YYDEBUGSTR, yydepth, yystate);
#endif

save = yypath;
yypath = save->save;
save->save = NULL;
ctry = save->ctry;
if (save->state != yystate) YYABORT;
yyFreeState(save);

}
else
{


YYParseState *save;
#if YYDEBUG
if (yydebug)
{
fprintf(stderr, "%s[%d]: CONFLICT in state %d. ", YYDEBUGSTR, yydepth, yystate);
if (yyps->save)
fputs("ALREADY in conflict, continuing trial parse.\n", stderr);
else
fputs("Starting trial parse.\n", stderr);
}
#endif
save = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
if (save == NULL) goto yyenomem;
save->save = yyps->save;
save->state = yystate;
save->errflag = yyerrflag;
save->yystack.s_mark = save->yystack.s_base + (yystack.s_mark - yystack.s_base);
memcpy (save->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
save->yystack.l_mark = save->yystack.l_base + (yystack.l_mark - yystack.l_base);
memcpy (save->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
save->yystack.p_mark = save->yystack.p_base + (yystack.p_mark - yystack.p_base);
memcpy (save->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
ctry = yytable[yyn];
if (yyctable[ctry] == -1)
{
#if YYDEBUG
if (yydebug && yychar >= YYEOF)
fprintf(stderr, "%s[%d]: backtracking 1 token\n", YYDEBUGSTR, yydepth);
#endif
ctry++;
}
save->ctry = ctry;
if (yyps->save == NULL)
{

if (!yylexemes)
{
yylexemes = (YYINT *) malloc((YYLVQUEUEGROWTH) * sizeof(YYINT));
if (yylexemes == NULL) goto yyenomem;
yylvals = (YYSTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYSTYPE));
if (yylvals == NULL) goto yyenomem;
yylvlim = yylvals + YYLVQUEUEGROWTH;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpsns = (YYLTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYLTYPE));
if (yylpsns == NULL) goto yyenomem;
yylplim = yylpsns + YYLVQUEUEGROWTH;
#endif
}
if (yylvp == yylve)
{
yylvp = yylve = yylvals;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp = yylpe = yylpsns;
#endif
yylexp = yylexemes;
if (yychar >= YYEOF)
{
*yylve++ = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
*yylpe++ = yylloc;
#endif
*yylexp = (YYINT) yychar;
yychar = YYEMPTY;
}
}
}
if (yychar >= YYEOF)
{
yylvp--;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp--;
#endif
yylexp--;
yychar = YYEMPTY;
}
save->lexeme = (int) (yylvp - yylvals);
yyps->save = save;
}
if (yytable[yyn] == ctry)
{
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
YYDEBUGSTR, yydepth, yystate, yyctable[ctry]);
#endif
if (yychar < 0)
{
yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp++;
#endif
yylexp++;
}
if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
goto yyoverflow;
yystate = yyctable[ctry];
*++yystack.s_mark = (YYINT) yystate;
*++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
*++yystack.p_mark = yylloc;
#endif
yychar = YYEMPTY;
if (yyerrflag > 0) --yyerrflag;
goto yyloop;
}
else
{
yyn = yyctable[ctry];
goto yyreduce;
}
}
#endif
if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
{
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
YYDEBUGSTR, yydepth, yystate, yytable[yyn]);
#endif
if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
yystate = yytable[yyn];
*++yystack.s_mark = yytable[yyn];
*++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
*++yystack.p_mark = yylloc;
#endif
yychar = YYEMPTY;
if (yyerrflag > 0) --yyerrflag;
goto yyloop;
}
if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
{
yyn = yytable[yyn];
goto yyreduce;
}
if (yyerrflag != 0) goto yyinrecovery;
#if YYBTYACC

yynewerrflag = 1;
goto yyerrhandler;
goto yyerrlab;

yyerrlab:


yystack.s_mark -= yym;
yystate = *yystack.s_mark;
yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yystack.p_mark -= yym;
#endif

yynewerrflag = 0;
yyerrhandler:
while (yyps->save)
{
int ctry;
YYParseState *save = yyps->save;
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%s[%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
YYDEBUGSTR, yydepth, yystate, yyps->save->state,
(int)(yylvp - yylvals - yyps->save->lexeme));
#endif

if (yyerrctx == NULL || yyerrctx->lexeme < yylvp - yylvals)
{

if (yyerrctx) yyFreeState(yyerrctx);

yyerrctx = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
if (yyerrctx == NULL) goto yyenomem;
yyerrctx->save = yyps->save;
yyerrctx->state = yystate;
yyerrctx->errflag = yyerrflag;
yyerrctx->yystack.s_mark = yyerrctx->yystack.s_base + (yystack.s_mark - yystack.s_base);
memcpy (yyerrctx->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
yyerrctx->yystack.l_mark = yyerrctx->yystack.l_base + (yystack.l_mark - yystack.l_base);
memcpy (yyerrctx->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yyerrctx->yystack.p_mark = yyerrctx->yystack.p_base + (yystack.p_mark - yystack.p_base);
memcpy (yyerrctx->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
yyerrctx->lexeme = (int) (yylvp - yylvals);
}
yylvp = yylvals + save->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp = yylpsns + save->lexeme;
#endif
yylexp = yylexemes + save->lexeme;
yychar = YYEMPTY;
yystack.s_mark = yystack.s_base + (save->yystack.s_mark - save->yystack.s_base);
memcpy (yystack.s_base, save->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
yystack.l_mark = yystack.l_base + (save->yystack.l_mark - save->yystack.l_base);
memcpy (yystack.l_base, save->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yystack.p_mark = yystack.p_base + (save->yystack.p_mark - save->yystack.p_base);
memcpy (yystack.p_base, save->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
ctry = ++save->ctry;
yystate = save->state;

if ((yyn = yyctable[ctry]) >= 0) goto yyreduce;
yyps->save = save->save;
save->save = NULL;
yyFreeState(save);


if (!yyps->save)
{
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%sdebug[%d,trial]: trial parse FAILED, entering ERROR mode\n",
YYPREFIX, yydepth);
#endif

yylvp = yylvals + yyerrctx->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp = yylpsns + yyerrctx->lexeme;
#endif
yylexp = yylexemes + yyerrctx->lexeme;
yychar = yylexp[-1];
yylval = yylvp[-1];
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylloc = yylpp[-1];
#endif
yystack.s_mark = yystack.s_base + (yyerrctx->yystack.s_mark - yyerrctx->yystack.s_base);
memcpy (yystack.s_base, yyerrctx->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
yystack.l_mark = yystack.l_base + (yyerrctx->yystack.l_mark - yyerrctx->yystack.l_base);
memcpy (yystack.l_base, yyerrctx->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yystack.p_mark = yystack.p_base + (yyerrctx->yystack.p_mark - yyerrctx->yystack.p_base);
memcpy (yystack.p_base, yyerrctx->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
yystate = yyerrctx->state;
yyFreeState(yyerrctx);
yyerrctx = NULL;
}
yynewerrflag = 1;
}
if (yynewerrflag == 0) goto yyinrecovery;
#endif

YYERROR_CALL("syntax error");
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yyerror_loc_range[1] = yylloc;
#endif

#if !YYBTYACC
goto yyerrlab;
yyerrlab:
#endif
++yynerrs;

yyinrecovery:
if (yyerrflag < 3)
{
yyerrflag = 3;
for (;;)
{
if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
{
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%s[%d]: state %d, error recovery shifting to state %d\n",
YYDEBUGSTR, yydepth, *yystack.s_mark, yytable[yyn]);
#endif
if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
yystate = yytable[yyn];
*++yystack.s_mark = yytable[yyn];
*++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

yyerror_loc_range[2] = yylloc;
YYLLOC_DEFAULT(yyloc, yyerror_loc_range, 2);
*++yystack.p_mark = yyloc;
#endif
goto yyloop;
}
else
{
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%s[%d]: error recovery discarding state %d\n",
YYDEBUGSTR, yydepth, *yystack.s_mark);
#endif
if (yystack.s_mark <= yystack.s_base) goto yyabort;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

yyerror_loc_range[1] = *yystack.p_mark;
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
if (!yytrial)
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYDESTRUCT_CALL("error: discarding state",
yystos[*yystack.s_mark], yystack.l_mark, yystack.p_mark);
#else
YYDESTRUCT_CALL("error: discarding state",
yystos[*yystack.s_mark], yystack.l_mark);
#endif
#endif
--yystack.s_mark;
--yystack.l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
--yystack.p_mark;
#endif
}
}
}
else
{
if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
if (yydebug)
{
if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
fprintf(stderr, "%s[%d]: state %d, error recovery discarding token %d (%s)\n",
YYDEBUGSTR, yydepth, yystate, yychar, yys);
}
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
if (!yytrial)
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYDESTRUCT_CALL("error: discarding token", yychar, &yylval, &yylloc);
#else
YYDESTRUCT_CALL("error: discarding token", yychar, &yylval);
#endif
#endif
yychar = YYEMPTY;
goto yyloop;
}

yyreduce:
yym = yylen[yyn];
#if YYDEBUG
if (yydebug)
{
fprintf(stderr, "%s[%d]: state %d, reducing by rule %d (%s)",
YYDEBUGSTR, yydepth, yystate, yyn, yyrule[yyn]);
#if defined(YYSTYPE_TOSTRING)
#if YYBTYACC
if (!yytrial)
#endif
if (yym > 0)
{
int i;
fputc('<', stderr);
for (i = yym; i > 0; i--)
{
if (i != yym) fputs(", ", stderr);
fputs(YYSTYPE_TOSTRING(yystos[yystack.s_mark[1-i]],
yystack.l_mark[1-i]), stderr);
}
fputc('>', stderr);
}
#endif
fputc('\n', stderr);
}
#endif
if (yym > 0)
yyval = yystack.l_mark[1-yym];
else
memset(&yyval, 0, sizeof yyval);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)


memset(&yyloc, 0, sizeof(yyloc));
#if YYBTYACC
if (!yytrial)
#endif
{
YYLLOC_DEFAULT(yyloc, &yystack.p_mark[-yym], yym);


yyerror_loc_range[1] = yystack.p_mark[1-yym];
}
#endif

switch (yyn)
{
case 2:
#line 51 "btyacc_calc1.y"
{YYVALID;}
break;
case 3:
#line 52 "btyacc_calc1.y"
{YYVALID;} if (!yytrial)
#line 53 "btyacc_calc1.y"
{
yyerrok;
}
break;
case 4:
if (!yytrial)
#line 59 "btyacc_calc1.y"
{
(void) printf("%15.8f\n", yystack.l_mark[0].dval);
}
break;
case 5:
if (!yytrial)
#line 63 "btyacc_calc1.y"
{
(void) printf("(%15.8f, %15.8f)\n", yystack.l_mark[0].vval.lo, yystack.l_mark[0].vval.hi);
}
break;
case 6:
if (!yytrial)
#line 67 "btyacc_calc1.y"
{
dreg[yystack.l_mark[-2].ival] = yystack.l_mark[0].dval;
}
break;
case 7:
if (!yytrial)
#line 71 "btyacc_calc1.y"
{
vreg[yystack.l_mark[-2].ival] = yystack.l_mark[0].vval;
}
break;
case 9:
if (!yytrial)
#line 78 "btyacc_calc1.y"
{
yyval.dval = dreg[yystack.l_mark[0].ival];
}
break;
case 10:
if (!yytrial)
#line 82 "btyacc_calc1.y"
{
yyval.dval = yystack.l_mark[-2].dval + yystack.l_mark[0].dval;
}
break;
case 11:
if (!yytrial)
#line 86 "btyacc_calc1.y"
{
yyval.dval = yystack.l_mark[-2].dval - yystack.l_mark[0].dval;
}
break;
case 12:
if (!yytrial)
#line 90 "btyacc_calc1.y"
{
yyval.dval = yystack.l_mark[-2].dval * yystack.l_mark[0].dval;
}
break;
case 13:
if (!yytrial)
#line 94 "btyacc_calc1.y"
{
yyval.dval = yystack.l_mark[-2].dval / yystack.l_mark[0].dval;
}
break;
case 14:
if (!yytrial)
#line 98 "btyacc_calc1.y"
{
yyval.dval = -yystack.l_mark[0].dval;
}
break;
case 15:
if (!yytrial)
#line 102 "btyacc_calc1.y"
{
yyval.dval = yystack.l_mark[-1].dval;
}
break;
case 16:
if (!yytrial)
#line 108 "btyacc_calc1.y"
{
yyval.vval.hi = yyval.vval.lo = yystack.l_mark[0].dval;
}
break;
case 17:
if (!yytrial)
#line 112 "btyacc_calc1.y"
{
yyval.vval.lo = yystack.l_mark[-3].dval;
yyval.vval.hi = yystack.l_mark[-1].dval;
if ( yyval.vval.lo > yyval.vval.hi )
{
(void) printf("interval out of order\n");
YYERROR;
}
}
break;
case 18:
if (!yytrial)
#line 122 "btyacc_calc1.y"
{
yyval.vval = vreg[yystack.l_mark[0].ival];
}
break;
case 19:
if (!yytrial)
#line 126 "btyacc_calc1.y"
{
yyval.vval.hi = yystack.l_mark[-2].vval.hi + yystack.l_mark[0].vval.hi;
yyval.vval.lo = yystack.l_mark[-2].vval.lo + yystack.l_mark[0].vval.lo;
}
break;
case 20:
if (!yytrial)
#line 131 "btyacc_calc1.y"
{
yyval.vval.hi = yystack.l_mark[-2].dval + yystack.l_mark[0].vval.hi;
yyval.vval.lo = yystack.l_mark[-2].dval + yystack.l_mark[0].vval.lo;
}
break;
case 21:
if (!yytrial)
#line 136 "btyacc_calc1.y"
{
yyval.vval.hi = yystack.l_mark[-2].vval.hi - yystack.l_mark[0].vval.lo;
yyval.vval.lo = yystack.l_mark[-2].vval.lo - yystack.l_mark[0].vval.hi;
}
break;
case 22:
if (!yytrial)
#line 141 "btyacc_calc1.y"
{
yyval.vval.hi = yystack.l_mark[-2].dval - yystack.l_mark[0].vval.lo;
yyval.vval.lo = yystack.l_mark[-2].dval - yystack.l_mark[0].vval.hi;
}
break;
case 23:
if (!yytrial)
#line 146 "btyacc_calc1.y"
{
yyval.vval = vmul( yystack.l_mark[-2].vval.lo, yystack.l_mark[-2].vval.hi, yystack.l_mark[0].vval );
}
break;
case 24:
if (!yytrial)
#line 150 "btyacc_calc1.y"
{
yyval.vval = vmul (yystack.l_mark[-2].dval, yystack.l_mark[-2].dval, yystack.l_mark[0].vval );
}
break;
case 25:
if (!yytrial)
#line 154 "btyacc_calc1.y"
{
if (dcheck(yystack.l_mark[0].vval)) YYERROR;
yyval.vval = vdiv ( yystack.l_mark[-2].vval.lo, yystack.l_mark[-2].vval.hi, yystack.l_mark[0].vval );
}
break;
case 26:
if (!yytrial)
#line 159 "btyacc_calc1.y"
{
if (dcheck ( yystack.l_mark[0].vval )) YYERROR;
yyval.vval = vdiv (yystack.l_mark[-2].dval, yystack.l_mark[-2].dval, yystack.l_mark[0].vval );
}
break;
case 27:
if (!yytrial)
#line 164 "btyacc_calc1.y"
{
yyval.vval.hi = -yystack.l_mark[0].vval.lo;
yyval.vval.lo = -yystack.l_mark[0].vval.hi;
}
break;
case 28:
if (!yytrial)
#line 169 "btyacc_calc1.y"
{
yyval.vval = yystack.l_mark[-1].vval;
}
break;
#line 1544 "btyacc_calc1.tab.c"
default:
break;
}
yystack.s_mark -= yym;
yystate = *yystack.s_mark;
yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yystack.p_mark -= yym;
#endif
yym = yylhs[yyn];
if (yystate == 0 && yym == 0)
{
#if YYDEBUG
if (yydebug)
{
fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#if defined(YYSTYPE_TOSTRING)
#if YYBTYACC
if (!yytrial)
#endif
fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[YYFINAL], yyval));
#endif
fprintf(stderr, "shifting from state 0 to final state %d\n", YYFINAL);
}
#endif
yystate = YYFINAL;
*++yystack.s_mark = YYFINAL;
*++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
*++yystack.p_mark = yyloc;
#endif
if (yychar < 0)
{
#if YYBTYACC
do {
if (yylvp < yylve)
{

yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylloc = *yylpp++;
#endif
yychar = *yylexp++;
break;
}
if (yyps->save)
{

if (yylvp == yylvlim)
{
size_t p = (size_t) (yylvp - yylvals);
size_t s = (size_t) (yylvlim - yylvals);

s += YYLVQUEUEGROWTH;
if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL)
goto yyenomem;
if ((yylvals = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL)
goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
if ((yylpsns = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL)
goto yyenomem;
#endif
yylvp = yylve = yylvals + p;
yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp = yylpe = yylpsns + p;
yylplim = yylpsns + s;
#endif
yylexp = yylexemes + p;
}
*yylexp = (YYINT) YYLEX;
*yylvp++ = yylval;
yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
*yylpp++ = yylloc;
yylpe++;
#endif
yychar = *yylexp++;
break;
}

#endif
yychar = YYLEX;
#if YYBTYACC
} while (0);
#endif
if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
if (yydebug)
{
if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)\n",
YYDEBUGSTR, yydepth, YYFINAL, yychar, yys);
}
#endif
}
if (yychar == YYEOF) goto yyaccept;
goto yyloop;
}
if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
yystate = yytable[yyn];
else
yystate = yydgoto[yym];
#if YYDEBUG
if (yydebug)
{
fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#if defined(YYSTYPE_TOSTRING)
#if YYBTYACC
if (!yytrial)
#endif
fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[yystate], yyval));
#endif
fprintf(stderr, "shifting from state %d to state %d\n", *yystack.s_mark, yystate);
}
#endif
if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
*++yystack.s_mark = (YYINT) yystate;
*++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
*++yystack.p_mark = yyloc;
#endif
goto yyloop;
#if YYBTYACC


yyvalid:
if (yypath) YYABORT;
while (yyps->save)
{
YYParseState *save = yyps->save;
yyps->save = save->save;
save->save = yypath;
yypath = save;
}
#if YYDEBUG
if (yydebug)
fprintf(stderr, "%s[%d]: state %d, CONFLICT trial successful, backtracking to state %d, %d tokens\n",
YYDEBUGSTR, yydepth, yystate, yypath->state, (int)(yylvp - yylvals - yypath->lexeme));
#endif
if (yyerrctx)
{
yyFreeState(yyerrctx);
yyerrctx = NULL;
}
yylvp = yylvals + yypath->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yylpp = yylpsns + yypath->lexeme;
#endif
yylexp = yylexemes + yypath->lexeme;
yychar = YYEMPTY;
yystack.s_mark = yystack.s_base + (yypath->yystack.s_mark - yypath->yystack.s_base);
memcpy (yystack.s_base, yypath->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
yystack.l_mark = yystack.l_base + (yypath->yystack.l_mark - yypath->yystack.l_base);
memcpy (yystack.l_base, yypath->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
yystack.p_mark = yystack.p_base + (yypath->yystack.p_mark - yypath->yystack.p_base);
memcpy (yystack.p_base, yypath->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
yystate = yypath->state;
goto yyloop;
#endif

yyoverflow:
YYERROR_CALL("yacc stack overflow");
#if YYBTYACC
goto yyabort_nomem;
yyenomem:
YYERROR_CALL("memory exhausted");
yyabort_nomem:
#endif
yyresult = 2;
goto yyreturn;

yyabort:
yyresult = 1;
goto yyreturn;

yyaccept:
#if YYBTYACC
if (yyps->save) goto yyvalid;
#endif
yyresult = 0;

yyreturn:
#if defined(YYDESTRUCT_CALL)
if (yychar != YYEOF && yychar != YYEMPTY)
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval, &yylloc);
#else
YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval);
#endif

{
YYSTYPE *pv;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE *pp;

for (pv = yystack.l_base, pp = yystack.p_base; pv <= yystack.l_mark; ++pv, ++pp)
YYDESTRUCT_CALL("cleanup: discarding state",
yystos[*(yystack.s_base + (pv - yystack.l_base))], pv, pp);
#else
for (pv = yystack.l_base; pv <= yystack.l_mark; ++pv)
YYDESTRUCT_CALL("cleanup: discarding state",
yystos[*(yystack.s_base + (pv - yystack.l_base))], pv);
#endif
}
#endif

#if YYBTYACC
if (yyerrctx)
{
yyFreeState(yyerrctx);
yyerrctx = NULL;
}
while (yyps)
{
YYParseState *save = yyps;
yyps = save->save;
save->save = NULL;
yyFreeState(save);
}
while (yypath)
{
YYParseState *save = yypath;
yypath = save->save;
save->save = NULL;
yyFreeState(save);
}
#endif
yyfreestack(&yystack);
return (yyresult);
}
