#define ID 257
#define CONST 258
#if defined(YYSTYPE)
#undef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#if !defined(YYSTYPE_IS_DECLARED)
#define YYSTYPE_IS_DECLARED 1
typedef union {
struct yyyOxAttrbs {
struct yyyStackItem *yyyOxStackItem;
} yyyOxAttrbs;
} YYSTYPE;
#endif
extern YYSTYPE expr.oxout_lval;
