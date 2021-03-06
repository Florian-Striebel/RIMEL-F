#if !defined(_err_inherit4__defines_h_)
#define _err_inherit4__defines_h_

#define GLOBAL 257
#define LOCAL 258
#define REAL 259
#define INTEGER 260
#define NAME 261
#if defined(YYSTYPE)
#undef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#if !defined(YYSTYPE_IS_DECLARED)
#define YYSTYPE_IS_DECLARED 1
typedef union
{
class cval;
type tval;
namelist * nlist;
name id;
} YYSTYPE;
#endif
extern YYSTYPE err_inherit4_lval;

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
extern YYLTYPE err_inherit4_lloc;

#endif
