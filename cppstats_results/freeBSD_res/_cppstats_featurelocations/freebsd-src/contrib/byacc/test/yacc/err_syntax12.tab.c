



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
#define yyparse err_syntax12_parse
#endif

#if !defined(yylex)
#define yylex err_syntax12_lex
#endif

#if !defined(yyerror)
#define yyerror err_syntax12_error
#endif

#if !defined(yychar)
#define yychar err_syntax12_char
#endif

#if !defined(yyval)
#define yyval err_syntax12_val
#endif

#if !defined(yylval)
#define yylval err_syntax12_lval
#endif

#if !defined(yydebug)
#define yydebug err_syntax12_debug
#endif

#if !defined(yynerrs)
#define yynerrs err_syntax12_nerrs
#endif

#if !defined(yyerrflag)
#define yyerrflag err_syntax12_errflag
#endif

#if !defined(yylhs)
#define yylhs err_syntax12_lhs
#endif

#if !defined(yylen)
#define yylen err_syntax12_len
#endif

#if !defined(yydefred)
#define yydefred err_syntax12_defred
#endif

#if !defined(yydgoto)
#define yydgoto err_syntax12_dgoto
#endif

#if !defined(yysindex)
#define yysindex err_syntax12_sindex
#endif

#if !defined(yyrindex)
#define yyrindex err_syntax12_rindex
#endif

#if !defined(yygindex)
#define yygindex err_syntax12_gindex
#endif

#if !defined(yytable)
#define yytable err_syntax12_table
#endif

#if !defined(yycheck)
#define yycheck err_syntax12_check
#endif

#if !defined(yyname)
#define yyname err_syntax12_name
#endif

#if !defined(yyrule)
#define yyrule err_syntax12_rule
#endif
#define YYPREFIX "err_syntax12_"

#define YYPURE 0

#line 2 "err_syntax12.y"
int yylex(void);
static void yyerror(const char *);
#line 104 "err_syntax12.tab.c"

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

extern int YYPARSE_DECL();

#define text 456
#define YYERRCODE 256
typedef int YYINT;
static const YYINT err_syntax12_lhs[] = { -1,
0,
};
static const YYINT err_syntax12_len[] = { 2,
1,
};
static const YYINT err_syntax12_defred[] = { 0,
1, 0,
};
static const YYINT err_syntax12_dgoto[] = { 2,
};
static const YYINT err_syntax12_sindex[] = { -256,
0, 0,
};
static const YYINT err_syntax12_rindex[] = { 0,
0, 0,
};
static const YYINT err_syntax12_gindex[] = { 0,
};
#define YYTABLESIZE 0
static const YYINT err_syntax12_table[] = { 1,
};
static const YYINT err_syntax12_check[] = { 256,
};
#define YYFINAL 2
#if !defined(YYDEBUG)
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 456
#define YYUNDFTOKEN 459
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const err_syntax12_name[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"text",0,0,"illegal-symbol",
};
static const char *const err_syntax12_rule[] = {
"$accept : S",
"S : error",

};
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
#line 12 "err_syntax12.y"

#include <stdio.h>

int
main(void)
{
printf("yyparse() = %d\n", yyparse());
return 0;
}

int
yylex(void)
{
return -1;
}

static void
yyerror(const char* s)
{
printf("%s\n", s);
}
#line 257 "err_syntax12.tab.c"

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
