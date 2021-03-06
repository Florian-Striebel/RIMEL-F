

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#if defined(__cplusplus)
#define class myClass
#endif

#define YYMAJOR 1
#define YYMINOR 9

#define CONCAT(first,second) first #second
#define CONCAT1(string,number) CONCAT(string, number)
#define CONCAT2(first,second) #first "." #second

#if defined(YYPATCH)
#define VSTRING(a,b) CONCAT2(a,b) CONCAT1(" ",YYPATCH)
#else
#define VSTRING(a,b) CONCAT2(a,b)
#endif

#define VERSION VSTRING(YYMAJOR, YYMINOR)
















#define MAXCHAR UCHAR_MAX
#if !defined(MAXTABLE)
#define MAXTABLE 32500
#endif
#if MAXTABLE <= SHRT_MAX
#define YYINT short
#define MAXYYINT SHRT_MAX
#define MINYYINT SHRT_MIN
#elif MAXTABLE <= INT_MAX
#define YYINT int
#define MAXYYINT INT_MAX
#define MINYYINT INT_MIN
#else
#error "MAXTABLE is too large for this machine architecture!"
#endif

#define BITS_PER_WORD ((int) sizeof (unsigned) * CHAR_BIT)
#define WORDSIZE(n) (((n)+(BITS_PER_WORD-1))/BITS_PER_WORD)
#define BIT(r, n) ((((r)[(n)/BITS_PER_WORD])>>((n)&(BITS_PER_WORD-1)))&1)
#define SETBIT(r, n) ((r)[(n)/BITS_PER_WORD]|=((unsigned)1<<((n)&(BITS_PER_WORD-1))))



#define NUL '\0'
#define NEWLINE '\n'
#define SP ' '
#define BS '\b'
#define HT '\t'
#define VT '\013'
#define CR '\r'
#define FF '\f'
#define QUOTE '\''
#define DOUBLE_QUOTE '\"'
#define BACKSLASH '\\'

#define UCH(c) (unsigned char)(c)



#if defined(VMS)
#define CODE_SUFFIX "_code.c"
#define DEFINES_SUFFIX "_tab.h"
#define EXTERNS_SUFFIX "_tab.i"
#define OUTPUT_SUFFIX "_tab.c"
#else
#define CODE_SUFFIX ".code.c"
#define DEFINES_SUFFIX ".tab.h"
#define EXTERNS_SUFFIX ".tab.i"
#define OUTPUT_SUFFIX ".tab.c"
#endif
#define VERBOSE_SUFFIX ".output"
#define GRAPH_SUFFIX ".dot"



typedef enum
{
TOKEN = 0
,LEFT
,RIGHT
,NONASSOC
,MARK
,TEXT
,TYPE
,START
,UNION
,IDENT
,EXPECT
,EXPECT_RR
,PURE_PARSER
,PARSE_PARAM
,LEX_PARAM
,POSIX_YACC
,TOKEN_TABLE
,ERROR_VERBOSE
,XXXDEBUG
,XCODE

#if defined(YYBTYACC)
,LOCATIONS
,DESTRUCTOR
,INITIAL_ACTION
#endif
}
KEY_CASES;



typedef enum
{
UNKNOWN = 0
,TERM
,NONTERM
,ACTION
,ARGUMENT
}
SYM_CASES;



#define UNDEFINED (-1)



#define SHIFT 1
#define REDUCE 2



#define IS_NAME1(c) (isalpha(UCH(c)) || (c) == '_' || (c) == '$')
#define IS_NAME2(c) (isalnum(UCH(c)) || (c) == '_' || (c) == '$')
#define IS_IDENT(c) (isalnum(UCH(c)) || (c) == '_' || (c) == '.' || (c) == '$')
#define IS_OCTAL(c) ((c) >= '0' && (c) <= '7')



#define ISTOKEN(s) ((s) < start_symbol)
#define ISVAR(s) ((s) >= start_symbol)



#define CALLOC(k,n) (calloc((size_t)(k),(size_t)(n)))
#define FREE(x) (free((char*)(x)))
#define MALLOC(n) (malloc((size_t)(n)))
#define TCMALLOC(t,n) ((t*) calloc((size_t)(n), sizeof(t)))
#define TMALLOC(t,n) ((t*) malloc((size_t)(n) * sizeof(t)))
#define NEW(t) ((t*)allocate(sizeof(t)))
#define NEW2(n,t) ((t*)allocate(((size_t)(n)*sizeof(t))))
#define REALLOC(p,n) (realloc((char*)(p),(size_t)(n)))
#define TREALLOC(t,p,n) ((t*)realloc((char*)(p), (size_t)(n) * sizeof(t)))

#define DO_FREE(x) if (x) { FREE(x); x = 0; }

#define NO_SPACE(p) if (p == 0) no_space(); assert(p != 0)


#define PLURAL(n) ((n) > 1 ? "s" : "")





#if defined(YYBTYACC)
#define USE_HEADER_GUARDS 1
#else
#define USE_HEADER_GUARDS 0
#endif

typedef char Assoc_t;
typedef char Class_t;
typedef YYINT Index_t;
typedef YYINT Value_t;



typedef struct bucket bucket;
struct bucket
{
struct bucket *link;
struct bucket *next;
char *name;
char *tag;
#if defined(YYBTYACC)
char **argnames;
char **argtags;
int args;
char *destructor;
#endif
Value_t value;
Index_t index;
Value_t prec;
Class_t class;
Assoc_t assoc;
};



typedef struct core core;
struct core
{
struct core *next;
struct core *link;
Value_t number;
Value_t accessing_symbol;
Value_t nitems;
Value_t items[1];
};



typedef struct shifts shifts;
struct shifts
{
struct shifts *next;
Value_t number;
Value_t nshifts;
Value_t shift[1];
};



typedef struct reductions reductions;
struct reductions
{
struct reductions *next;
Value_t number;
Value_t nreds;
Value_t rules[1];
};



typedef struct action action;
struct action
{
struct action *next;
Value_t symbol;
Value_t number;
Value_t prec;
char action_code;
Assoc_t assoc;
char suppressed;
};


typedef struct param param;
struct param
{
struct param *next;
char *name;
char *type;
char *type2;
};



extern char dflag2;
extern char dflag;
extern char gflag;
extern char iflag;
extern char lflag;
extern char rflag;
extern char sflag;
extern char tflag;
extern char vflag;
extern const char *symbol_prefix;

extern const char *myname;
extern char *cptr;
extern char *line;
extern int lineno;
extern int outline;
extern int exit_code;
extern int pure_parser;
extern int token_table;
extern int error_verbose;
#if defined(YYBTYACC)
extern int locations;
extern int backtrack;
extern int destructor;
extern char *initial_action;
#endif

extern const char *const banner[];
extern const char *const xdecls[];
extern const char *const tables[];
extern const char *const global_vars[];
extern const char *const impure_vars[];
extern const char *const hdr_defs[];
extern const char *const hdr_vars[];
extern const char *const body_1[];
extern const char *const body_vars[];
extern const char *const init_vars[];
extern const char *const body_2[];
extern const char *const body_3[];
extern const char *const trailer[];

extern char *code_file_name;
extern char *input_file_name;
extern size_t input_file_name_len;
extern char *defines_file_name;
extern char *externs_file_name;

extern FILE *action_file;
extern FILE *code_file;
extern FILE *defines_file;
extern FILE *externs_file;
extern FILE *input_file;
extern FILE *output_file;
extern FILE *text_file;
extern FILE *union_file;
extern FILE *verbose_file;
extern FILE *graph_file;

extern Value_t nitems;
extern Value_t nrules;
extern Value_t nsyms;
extern Value_t ntokens;
extern Value_t nvars;
extern int ntags;

extern char unionized;
extern char line_format[];

extern Value_t start_symbol;
extern char **symbol_name;
extern char **symbol_pname;
extern Value_t *symbol_value;
extern Value_t *symbol_prec;
extern char *symbol_assoc;

#if defined(YYBTYACC)
extern Value_t *symbol_pval;
extern char **symbol_destructor;
extern char **symbol_type_tag;
#endif

extern Value_t *ritem;
extern Value_t *rlhs;
extern Value_t *rrhs;
extern Value_t *rprec;
extern Assoc_t *rassoc;

extern Value_t **derives;
extern char *nullable;

extern bucket *first_symbol;
extern bucket *last_symbol;

extern int nstates;
extern core *first_state;
extern shifts *first_shift;
extern reductions *first_reduction;
extern Value_t *accessing_symbol;
extern core **state_table;
extern shifts **shift_table;
extern reductions **reduction_table;
extern unsigned *LA;
extern Value_t *LAruleno;
extern Value_t *lookaheads;
extern Value_t *goto_base;
extern Value_t *goto_map;
extern Value_t *from_state;
extern Value_t *to_state;

extern action **parser;
extern int SRexpect;
extern int RRexpect;
extern int SRtotal;
extern int RRtotal;
extern Value_t *SRconflicts;
extern Value_t *RRconflicts;
extern Value_t *defred;
extern Value_t *rules_used;
extern Value_t nunused;
extern Value_t final_state;

extern Value_t *itemset;
extern Value_t *itemsetend;
extern unsigned *ruleset;

extern param *lex_param;
extern param *parse_param;



#if !defined(GCC_NORETURN)
#if defined(__dead2)
#define GCC_NORETURN __dead2
#elif defined(__dead)
#define GCC_NORETURN __dead
#else
#define GCC_NORETURN
#endif
#endif

#if defined(__GNUC__)
#define ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define ATTRIBUTE_NORETURN
#endif

#if defined(NDEBUG) && defined(_MSC_VER)
#define NODEFAULT __assume(0);
#else
#define NODEFAULT
#endif
#define NOTREACHED NODEFAULT

#if !defined(GCC_UNUSED)
#if defined(__unused)
#define GCC_UNUSED __unused
#else
#define GCC_UNUSED
#endif
#endif

#if !defined(GCC_PRINTFLIKE)
#define GCC_PRINTFLIKE(fmt,var)
#endif


extern void closure(Value_t *nucleus, int n);
extern void finalize_closure(void);
extern void set_first_derives(void);


struct ainfo
{
int a_lineno;
char *a_line;
char *a_cptr;
};

extern void arg_number_disagree_warning(int a_lineno, char *a_name);
extern void arg_type_disagree_warning(int a_lineno, int i, char *a_name);
ATTRIBUTE_NORETURN
extern void at_error(int a_lineno, char *a_line, char *a_cptr) GCC_NORETURN;
extern void at_warning(int a_lineno, int i);
ATTRIBUTE_NORETURN
extern void bad_formals(void) GCC_NORETURN;
extern void default_action_warning(char *s);
extern void destructor_redeclared_warning(const struct ainfo *);
ATTRIBUTE_NORETURN
extern void dollar_error(int a_lineno, char *a_line, char *a_cptr) GCC_NORETURN;
extern void dollar_warning(int a_lineno, int i);
ATTRIBUTE_NORETURN
extern void fatal(const char *msg) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void illegal_character(char *c_cptr) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void illegal_tag(int t_lineno, char *t_line, char *t_cptr) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void missing_brace(void) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void no_grammar(void) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void no_space(void) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void open_error(const char *filename) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void over_unionized(char *u_cptr) GCC_NORETURN;
extern void prec_redeclared(void);
extern void reprec_warning(char *s);
extern void restarted_warning(void);
extern void retyped_warning(char *s);
extern void revalued_warning(char *s);
extern void start_requires_args(char *a_name);
ATTRIBUTE_NORETURN
extern void syntax_error(int st_lineno, char *st_line, char *st_cptr) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void terminal_lhs(int s_lineno) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void terminal_start(char *s) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void tokenized_start(char *s) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void undefined_goal(char *s) GCC_NORETURN;
extern void undefined_symbol_warning(char *s);
ATTRIBUTE_NORETURN
extern void unexpected_EOF(void) GCC_NORETURN;
extern void unknown_arg_warning(int d_lineno, const char *dlr_opt,
const char *d_arg, const char *d_line,
const char *d_cptr);
ATTRIBUTE_NORETURN
extern void unknown_rhs(int i) GCC_NORETURN;
extern void unsupported_flag_warning(const char *flag, const char *details);
ATTRIBUTE_NORETURN
extern void unterminated_action(const struct ainfo *) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void unterminated_comment(const struct ainfo *) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void unterminated_string(const struct ainfo *) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void unterminated_text(const struct ainfo *) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void unterminated_union(const struct ainfo *) GCC_NORETURN;
extern void untyped_arg_warning(int a_lineno, const char *dlr_opt, const char *a_name);
ATTRIBUTE_NORETURN
extern void untyped_lhs(void) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void untyped_rhs(int i, char *s) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void used_reserved(char *s) GCC_NORETURN;
ATTRIBUTE_NORETURN
extern void unterminated_arglist(const struct ainfo *) GCC_NORETURN;
extern void wrong_number_args_warning(const char *which, const char *a_name);
extern void wrong_type_for_arg_warning(int i, char *a_name);


extern void graph(void);


extern void lalr(void);


extern void lr0(void);
extern void show_cores(void);
extern void show_ritems(void);
extern void show_rrhs(void);
extern void show_shifts(void);


extern void *allocate(size_t n);
ATTRIBUTE_NORETURN
extern void done(int k) GCC_NORETURN;


extern void free_parser(void);
extern void make_parser(void);


struct mstring
{
char *base, *ptr, *end;
};

extern void msprintf(struct mstring *, const char *, ...) GCC_PRINTFLIKE(2,3);
extern int mputchar(struct mstring *, int);
extern struct mstring *msnew(void);
extern struct mstring *msrenew(char *);
extern char *msdone(struct mstring *);
extern int strnscmp(const char *, const char *);
extern unsigned int strnshash(const char *);

#define mputc(m, ch) (((m)->ptr == (m)->end) ? mputchar(m,ch) : (*(m)->ptr++ = (char) (ch)))




extern void output(void);


extern void reader(void);

typedef enum
{
CODE_HEADER = 0
,CODE_REQUIRES
,CODE_PROVIDES
,CODE_TOP
,CODE_IMPORTS
,CODE_MAX
}
CODE_CASES;
extern struct code_lines
{
const char *name;
char *lines;
size_t num;
}
code_lines[CODE_MAX];


extern void write_section(FILE * fp, const char *const section[]);


extern bucket *make_bucket(const char *);
extern bucket *lookup(const char *);
extern void create_symbol_table(void);
extern void free_symbol_table(void);
extern void free_symbols(void);


extern void verbose(void);


extern void reflexive_transitive_closure(unsigned *R, int n);

#if defined(DEBUG)

extern void print_closure(int n);
extern void print_EFF(void);
extern void print_first_derives(void);

extern void print_derives(void);
#endif

#if defined(NO_LEAKS)
extern void lr0_leaks(void);
extern void lalr_leaks(void);
extern void mkpar_leaks(void);
extern void output_leaks(void);
extern void mstring_leaks(void);
extern void reader_leaks(void);
#endif
