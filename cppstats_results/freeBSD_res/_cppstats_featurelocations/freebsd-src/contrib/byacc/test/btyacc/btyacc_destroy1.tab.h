#if !defined(_destroy1__defines_h_)
#define _destroy1__defines_h_

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
extern YYSTYPE destroy1_lval;

#endif
