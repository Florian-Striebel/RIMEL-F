



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
#define YYBTYACC 0
#define YYDEBUGSTR YYPREFIX "debug"

#if !defined(yyparse)
#define yyparse err_inherit4_parse
#endif

#if !defined(yylex)
#define yylex err_inherit4_lex
#endif

#if !defined(yyerror)
#define yyerror err_inherit4_error
#endif

#if !defined(yychar)
#define yychar err_inherit4_char
#endif

#if !defined(yyval)
#define yyval err_inherit4_val
#endif

#if !defined(yylval)
#define yylval err_inherit4_lval
#endif

#if !defined(yydebug)
#define yydebug err_inherit4_debug
#endif

#if !defined(yynerrs)
#define yynerrs err_inherit4_nerrs
#endif

#if !defined(yyerrflag)
#define yyerrflag err_inherit4_errflag
#endif

#if !defined(yylhs)
#define yylhs err_inherit4_lhs
#endif

#if !defined(yylen)
#define yylen err_inherit4_len
#endif

#if !defined(yydefred)
#define yydefred err_inherit4_defred
#endif

#if !defined(yystos)
#define yystos err_inherit4_stos
#endif

#if !defined(yydgoto)
#define yydgoto err_inherit4_dgoto
#endif

#if !defined(yysindex)
#define yysindex err_inherit4_sindex
#endif

#if !defined(yyrindex)
#define yyrindex err_inherit4_rindex
#endif

#if !defined(yygindex)
#define yygindex err_inherit4_gindex
#endif

#if !defined(yytable)
#define yytable err_inherit4_table
#endif

#if !defined(yycheck)
#define yycheck err_inherit4_check
#endif

#if !defined(yyname)
#define yyname err_inherit4_name
#endif

#if !defined(yyrule)
#define yyrule err_inherit4_rule
#endif

#if !defined(yyloc)
#define yyloc err_inherit4_loc
#endif

#if !defined(yylloc)
#define yylloc err_inherit4_lloc
#endif

#if YYBTYACC

#if !defined(yycindex)
#define yycindex err_inherit4_cindex
#endif

#if !defined(yyctable)
#define yyctable err_inherit4_ctable
#endif

#endif

#define YYPREFIX "err_inherit4_"

#define YYPURE 0

#line 3 "err_inherit4.y"
#include <stdlib.h>

typedef enum {cGLOBAL, cLOCAL} class;
typedef enum {tREAL, tINTEGER} type;
typedef char * name;

struct symbol { class c; type t; name id; };
typedef struct symbol symbol;

struct namelist { symbol *s; struct namelist *next; };
typedef struct namelist namelist;

extern symbol *mksymbol(type t, class c, name id);

#if defined(YYBISON)
#define YYLEX_DECL() yylex(void)
#define YYERROR_DECL() yyerror(const char *s)
#endif
#if defined(YYSTYPE)
#undef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#if !defined(YYSTYPE_IS_DECLARED)
#define YYSTYPE_IS_DECLARED 1
#line 41 "err_inherit4.y"
typedef union
{
class cval;
type tval;
namelist * nlist;
name id;
} YYSTYPE;
#endif
#line 163 "err_inherit4.tab.c"

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED

typedef struct YYLTYPE
{
int first_line;
int first_column;
int last_line;
int last_column;
unsigned source;
} YYLTYPE;
#define YYLTYPE_IS_DECLARED 1
#endif
#define YYRHSLOC(rhs, k) ((rhs)[k])


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
#define YYERROR_DECL() yyerror(YYLTYPE *loc, const char *s)
#endif
#if !defined(YYERROR_CALL)
#define YYERROR_CALL(msg) yyerror(&yylloc, msg)
#endif

#if !defined(YYDESTRUCT_DECL)
#define YYDESTRUCT_DECL() yydestruct(const char *msg, int psymb, YYSTYPE *val, YYLTYPE *loc)
#endif
#if !defined(YYDESTRUCT_CALL)
#define YYDESTRUCT_CALL(msg, psymb, val, loc) yydestruct(msg, psymb, val, loc)
#endif

extern int YYPARSE_DECL();

#define GLOBAL 257
#define LOCAL 258
#define REAL 259
#define INTEGER 260
#define NAME 261
#define YYERRCODE 256
typedef int YYINT;
static const YYINT err_inherit4_lhs[] = { -1,
5, 6, 0, 0, 3, 3, 4, 4, 1, 1,
2,
};
static const YYINT err_inherit4_len[] = { 2,
0, 0, 5, 2, 1, 1, 1, 1, 2, 1,
1,
};
static const YYINT err_inherit4_defred[] = { 0,
5, 6, 7, 8, 0, 0, 0, 1, 10, 0,
4, 2, 9, 0, 0,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT err_inherit4_stos[] = { 0,
257, 258, 259, 260, 263, 266, 267, 267, 261, 264,
265, 268, 261, 269, 264,
};
#endif
static const YYINT err_inherit4_dgoto[] = { 5,
10, 11, 6, 7, 12, 14,
};
static const YYINT err_inherit4_sindex[] = { -257,
0, 0, 0, 0, 0, -255, -254, 0, 0, -253,
0, 0, 0, -254, -253,
};
static const YYINT err_inherit4_rindex[] = { 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
0, 0, 0, 0, 9,
};
#if YYBTYACC
static const YYINT err_inherit4_cindex[] = { 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
};
#endif
static const YYINT err_inherit4_gindex[] = { 0,
-4, 0, 0, 5, 0, 0,
};
#define YYTABLESIZE 11
static const YYINT err_inherit4_table[] = { 1,
2, 3, 4, 3, 4, 11, 9, 13, 3, 15,
8,
};
static const YYINT err_inherit4_check[] = { 257,
258, 259, 260, 259, 260, 0, 261, 261, 0, 14,
6,
};
#if YYBTYACC
static const YYINT err_inherit4_ctable[] = { -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
#endif
#define YYFINAL 5
#if !defined(YYDEBUG)
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 261
#define YYUNDFTOKEN 270
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const err_inherit4_name[] = {

"$end",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"error","GLOBAL","LOCAL","REAL","INTEGER",
"NAME","$accept","declaration","namelist","locnamelist","class","type","$$1",
"$$2","illegal-symbol",
};
static const char *const err_inherit4_rule[] = {
"$accept : declaration",
"$$1 :",
"$$2 :",
"declaration : class type $$1 $$2 namelist",
"declaration : type locnamelist",
"class : GLOBAL",
"class : LOCAL",
"type : REAL",
"type : INTEGER",
"namelist : namelist NAME",
"namelist : NAME",
"locnamelist : namelist",

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

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE yyloc;
YYLTYPE yylloc;
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

static YYSTACKDATA yystack;
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
#line 80 "err_inherit4.y"

extern int YYLEX_DECL();
extern void YYERROR_DECL();
#line 444 "err_inherit4.tab.c"


#if ! defined YYDESTRUCT_IS_DECLARED
static void
YYDESTRUCT_DECL()
{
switch (psymb)
{
case 263:
#line 28 "err_inherit4.y"
{ }
break;
#line 457 "err_inherit4.tab.c"
case 264:
#line 28 "err_inherit4.y"
{ }
break;
#line 462 "err_inherit4.tab.c"
case 265:
#line 28 "err_inherit4.y"
{ }
break;
#line 467 "err_inherit4.tab.c"
}
}
#define YYDESTRUCT_IS_DECLARED 1
#endif


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
case 1:
#line 52 "err_inherit4.y"
yyval.cval = yystack.l_mark[-1].cval;
break;
case 2:
#line 52 "err_inherit4.y"
yyval.tval = yystack.l_mark[-1].tval;
break;
case 3:
#line 53 "err_inherit4.y"
{ yyval.nlist = yystack.l_mark[0].nlist; yyloc = yystack.p_mark[0]; }
break;
case 4:
#line 55 "err_inherit4.y"
{ yyval.nlist = yystack.l_mark[0].nlist; yyloc = yystack.p_mark[-3]; }
break;
case 5:
#line 58 "err_inherit4.y"
{ yyval.cval = cGLOBAL; }
break;
case 6:
#line 59 "err_inherit4.y"
{ yyval.cval = cLOCAL; }
break;
case 7:
#line 62 "err_inherit4.y"
{ yyval.tval = tREAL; }
break;
case 8:
#line 63 "err_inherit4.y"
{ yyval.tval = tINTEGER; }
break;
case 9:
#line 67 "err_inherit4.y"
{ yyval.nlist->s = mksymbol(yystack.l_mark[-2].tval, yystack.l_mark[-3].cval, yystack.l_mark[0].id);
yyval.nlist->next = yystack.l_mark[-1].nlist;
}
break;
case 10:
#line 71 "err_inherit4.y"
{ yyval.nlist->s = mksymbol(yystack.l_mark[-1].tval, yystack.l_mark[-2].cval, yystack.l_mark[0].id);
yyval.nlist->next = NULL;
}
break;
case 11:
#line 77 "err_inherit4.y"
{ yyval.nlist = yystack.l_mark[0].nlist; yyloc = yystack.p_mark[1]; }
break;
#line 1189 "err_inherit4.tab.c"
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
