#define DREG 257
#define VREG 258
#define CONST 259
#define UMINUS 260
#if defined(YYSTYPE)
#undef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#if !defined(YYSTYPE_IS_DECLARED)
#define YYSTYPE_IS_DECLARED 1
typedef union
{
int ival;
double dval;
INTERVAL vval;
} YYSTYPE;
#endif
extern YYSTYPE calc1_lval;
