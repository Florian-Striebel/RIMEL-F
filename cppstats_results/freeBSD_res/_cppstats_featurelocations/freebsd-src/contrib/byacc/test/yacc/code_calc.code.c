



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

#if !defined(yyparse)
#define yyparse calc_parse
#endif

#if !defined(yylex)
#define yylex calc_lex
#endif

#if !defined(yyerror)
#define yyerror calc_error
#endif

#if !defined(yychar)
#define yychar calc_char
#endif

#if !defined(yyval)
#define yyval calc_val
#endif

#if !defined(yylval)
#define yylval calc_lval
#endif

#if !defined(yydebug)
#define yydebug calc_debug
#endif

#if !defined(yynerrs)
#define yynerrs calc_nerrs
#endif

#if !defined(yyerrflag)
#define yyerrflag calc_errflag
#endif

#if !defined(yylhs)
#define yylhs calc_lhs
#endif

#if !defined(yylen)
#define yylen calc_len
#endif

#if !defined(yydefred)
#define yydefred calc_defred
#endif

#if !defined(yydgoto)
#define yydgoto calc_dgoto
#endif

#if !defined(yysindex)
#define yysindex calc_sindex
#endif

#if !defined(yyrindex)
#define yyrindex calc_rindex
#endif

#if !defined(yygindex)
#define yygindex calc_gindex
#endif

#if !defined(yytable)
#define yytable calc_table
#endif

#if !defined(yycheck)
#define yycheck calc_check
#endif

#if !defined(yyname)
#define yyname calc_name
#endif

#if !defined(yyrule)
#define yyrule calc_rule
#endif
#define YYPREFIX "calc_"

#define YYPURE 0

#line 4 "code_calc.y"
#include <stdio.h>
#include <ctype.h>

int regs[26];
int base;

#if defined(YYBISON)
int yylex(void);
static void yyerror(const char *s);
#endif

#line 113 "code_calc.code.c"

#if ! defined(YYSTYPE) && ! defined(YYSTYPE_IS_DECLARED)

typedef int YYSTYPE;
#define YYSTYPE_IS_DECLARED 1
#endif


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
#define YYLEX_DECL() yylex(void *YYLEX_PARAM)
#define YYLEX yylex(YYLEX_PARAM)
#else
#define YYLEX_DECL() yylex(void)
#define YYLEX yylex()
#endif


#if !defined(YYERROR_DECL)
#define YYERROR_DECL() yyerror(const char *s)
#endif
#if !defined(YYERROR_CALL)
#define YYERROR_CALL(msg) yyerror(msg)
#endif

#define DIGIT 257
#define LETTER 258
#define UMINUS 259
#define YYERRCODE 256
#undef yytname
#define yytname yyname
#define YYTABLESIZE 220
#define YYFINAL 1
#if !defined(YYDEBUG)
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 259
#define YYUNDFTOKEN 265
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))

extern int YYPARSE_DECL();
typedef int YYINT;
extern YYINT yylhs[];
extern YYINT yylen[];
extern YYINT yydefred[];
extern YYINT yydgoto[];
extern YYINT yysindex[];
extern YYINT yyrindex[];
extern YYINT yygindex[];
extern YYINT yytable[];
extern YYINT yycheck[];

#if YYDEBUG || defined(yytname)
extern char *yyname[];
#endif
#if YYDEBUG
extern char *yyrule[];
#endif

#if YYDEBUG
int yydebug;
#endif

int yyerrflag;
int yychar;
YYSTYPE yyval;
YYSTYPE yylval;
int yynerrs;


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

#define YYINITSTACKSIZE 200

typedef struct {
unsigned stacksize;
YYINT *s_base;
YYINT *s_mark;
YYINT *s_last;
YYSTYPE *l_base;
YYSTYPE *l_mark;
} YYSTACKDATA;

static YYSTACKDATA yystack;
#line 70 "code_calc.y"


#if defined(YYBYACC)
extern int YYLEX_DECL();
#endif

int
main (void)
{
while(!feof(stdin)) {
yyparse();
}
return 0;
}

static void
yyerror(const char *s)
{
fprintf(stderr, "%s\n", s);
}

int
yylex(void)
{





int c;

while( (c=getchar()) == ' ' ) { }



if( islower( c )) {
yylval = c - 'a';
return ( LETTER );
}
if( isdigit( c )) {
yylval = c - '0';
return ( DIGIT );
}
return( c );
}
#line 265 "code_calc.code.c"

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

data->stacksize = newsize;
data->s_last = data->s_base + newsize - 1;
return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
free(data->s_base);
free(data->l_base);
memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data)
#endif

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

int
YYPARSE_DECL()
{
int yym, yyn, yystate;
#if YYDEBUG
const char *yys;

if ((yys = getenv("YYDEBUG")) != NULL)
{
yyn = *yys;
if (yyn >= '0' && yyn <= '9')
yydebug = yyn - '0';
}
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
yystate = 0;
*yystack.s_mark = 0;

yyloop:
if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
if (yychar < 0)
{
yychar = YYLEX;
if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
if (yydebug)
{
if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
printf("%sdebug: state %d, reading %d (%s)\n",
YYPREFIX, yystate, yychar, yys);
}
#endif
}
if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
{
#if YYDEBUG
if (yydebug)
printf("%sdebug: state %d, shifting to state %d\n",
YYPREFIX, yystate, yytable[yyn]);
#endif
if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
yystate = yytable[yyn];
*++yystack.s_mark = yytable[yyn];
*++yystack.l_mark = yylval;
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

YYERROR_CALL("syntax error");

goto yyerrlab;
yyerrlab:
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
printf("%sdebug: state %d, error recovery shifting\
to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
yystate = yytable[yyn];
*++yystack.s_mark = yytable[yyn];
*++yystack.l_mark = yylval;
goto yyloop;
}
else
{
#if YYDEBUG
if (yydebug)
printf("%sdebug: error recovery discarding state %d\n",
YYPREFIX, *yystack.s_mark);
#endif
if (yystack.s_mark <= yystack.s_base) goto yyabort;
--yystack.s_mark;
--yystack.l_mark;
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
printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
YYPREFIX, yystate, yychar, yys);
}
#endif
yychar = YYEMPTY;
goto yyloop;
}

yyreduce:
#if YYDEBUG
if (yydebug)
printf("%sdebug: state %d, reducing by rule %d (%s)\n",
YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
yym = yylen[yyn];
if (yym > 0)
yyval = yystack.l_mark[1-yym];
else
memset(&yyval, 0, sizeof yyval);

switch (yyn)
{
case 3:
#line 32 "code_calc.y"
{ yyerrok ; }
break;
case 4:
#line 36 "code_calc.y"
{ printf("%d\n",yystack.l_mark[0]);}
break;
case 5:
#line 38 "code_calc.y"
{ regs[yystack.l_mark[-2]] = yystack.l_mark[0]; }
break;
case 6:
#line 42 "code_calc.y"
{ yyval = yystack.l_mark[-1]; }
break;
case 7:
#line 44 "code_calc.y"
{ yyval = yystack.l_mark[-2] + yystack.l_mark[0]; }
break;
case 8:
#line 46 "code_calc.y"
{ yyval = yystack.l_mark[-2] - yystack.l_mark[0]; }
break;
case 9:
#line 48 "code_calc.y"
{ yyval = yystack.l_mark[-2] * yystack.l_mark[0]; }
break;
case 10:
#line 50 "code_calc.y"
{ yyval = yystack.l_mark[-2] / yystack.l_mark[0]; }
break;
case 11:
#line 52 "code_calc.y"
{ yyval = yystack.l_mark[-2] % yystack.l_mark[0]; }
break;
case 12:
#line 54 "code_calc.y"
{ yyval = yystack.l_mark[-2] & yystack.l_mark[0]; }
break;
case 13:
#line 56 "code_calc.y"
{ yyval = yystack.l_mark[-2] | yystack.l_mark[0]; }
break;
case 14:
#line 58 "code_calc.y"
{ yyval = - yystack.l_mark[0]; }
break;
case 15:
#line 60 "code_calc.y"
{ yyval = regs[yystack.l_mark[0]]; }
break;
case 17:
#line 65 "code_calc.y"
{ yyval = yystack.l_mark[0]; base = (yystack.l_mark[0]==0) ? 8 : 10; }
break;
case 18:
#line 67 "code_calc.y"
{ yyval = base * yystack.l_mark[-1] + yystack.l_mark[0]; }
break;
#line 524 "code_calc.code.c"
}
yystack.s_mark -= yym;
yystate = *yystack.s_mark;
yystack.l_mark -= yym;
yym = yylhs[yyn];
if (yystate == 0 && yym == 0)
{
#if YYDEBUG
if (yydebug)
printf("%sdebug: after reduction, shifting from state 0 to\
state %d\n", YYPREFIX, YYFINAL);
#endif
yystate = YYFINAL;
*++yystack.s_mark = YYFINAL;
*++yystack.l_mark = yyval;
if (yychar < 0)
{
yychar = YYLEX;
if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
if (yydebug)
{
if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
printf("%sdebug: state %d, reading %d (%s)\n",
YYPREFIX, YYFINAL, yychar, yys);
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
printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
*++yystack.s_mark = (YYINT) yystate;
*++yystack.l_mark = yyval;
goto yyloop;

yyoverflow:
YYERROR_CALL("yacc stack overflow");

yyabort:
yyfreestack(&yystack);
return (1);

yyaccept:
yyfreestack(&yystack);
return (0);
}
