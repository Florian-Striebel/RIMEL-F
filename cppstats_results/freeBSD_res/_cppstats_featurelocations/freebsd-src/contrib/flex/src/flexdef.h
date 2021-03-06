

































#if !defined(FLEXDEF_H)
#define FLEXDEF_H 1

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <ctype.h>
#include <libgen.h>
#include <string.h>

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#else
#define assert(Pred)
#endif

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

#include <unistd.h>
#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif
#if defined(HAVE_SYS_PARAMS_H)
#include <sys/params.h>
#endif

#include <sys/stat.h>

#include <sys/wait.h>
#include <stdbool.h>
#include <stdarg.h>

#include <regex.h>

#include <strings.h>
#include "flexint.h"


#if defined(ENABLE_NLS)
#if defined(HAVE_LOCALE_H)
#include <locale.h>
#endif
#include "gettext.h"
#define _(String) gettext (String)
#else
#define _(STRING) STRING
#endif


#define CSIZE 256


#if !defined(DEFAULT_CSIZE)
#define DEFAULT_CSIZE 128
#endif


#define MAXLINE 2048

#if !defined(MIN)
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#if !defined(MAX)
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif
#if !defined(ABS)
#define ABS(x) ((x) < 0 ? -(x) : (x))
#endif


#define is_power_of_2(n) ((n) > 0 && ((n) & ((n) - 1)) == 0)

#define unspecified -1




#define EOB_POSITION -1
#define ACTION_POSITION -2


#define NUMDATAITEMS 10




#define NUMDATALINES 10


#define NUMCHARLINES 10


#define TRANS_STRUCT_PRINT_LENGTH 14




#define FREE_EPSILON(state) (transchar[state] == SYM_EPSILON && trans2[state] == NO_TRANSITION && finalst[state] != state)







#define SUPER_FREE_EPSILON(state) (transchar[state] == SYM_EPSILON && trans1[state] == NO_TRANSITION)







#define INITIAL_MAX_DFA_SIZE 750
#define MAX_DFA_SIZE_INCREMENT 750











#define YY_TRAILING_MASK 0x2000




#define YY_TRAILING_HEAD_MASK 0x4000


#define MAX_RULE (YY_TRAILING_MASK - 1)





#define NIL 0

#define JAM -1
#define NO_TRANSITION NIL
#define UNIQUE -1
#define INFINITE_REPEAT -1

#define INITIAL_MAX_CCLS 100
#define MAX_CCLS_INCREMENT 100


#define INITIAL_MAX_CCL_TBL_SIZE 500
#define MAX_CCL_TBL_SIZE_INCREMENT 250

#define INITIAL_MAX_RULES 100
#define MAX_RULES_INCREMENT 100

#define INITIAL_MNS 2000
#define MNS_INCREMENT 1000

#define INITIAL_MAX_DFAS 1000
#define MAX_DFAS_INCREMENT 1000

#define JAMSTATE -32766


#define MAXIMUM_MNS 31999
#define MAXIMUM_MNS_LONG 1999999999




#define MARKER_DIFFERENCE (maximum_mns+2)


#define INITIAL_MAX_XPAIRS 2000
#define MAX_XPAIRS_INCREMENT 2000


#define INITIAL_MAX_TEMPLATE_XPAIRS 2500
#define MAX_TEMPLATE_XPAIRS_INCREMENT 2500

#define SYM_EPSILON (CSIZE + 1)

#define INITIAL_MAX_SCS 40
#define MAX_SCS_INCREMENT 40

#define ONE_STACK_SIZE 500
#define SAME_TRANS -1







#define PROTO_SIZE_PERCENTAGE 15







#define CHECK_COM_PERCENTAGE 50






#define FIRST_MATCH_DIFF_PERCENTAGE 10





#define ACCEPTABLE_DIFF_PERCENTAGE 50





#define TEMPLATE_SAME_PERCENTAGE 60





#define NEW_PROTO_DIFF_PERCENTAGE 20





#define INTERIOR_FIT_PERCENTAGE 15




#define PROT_SAVE_SIZE 2000

#define MSP 50





#define MAX_XTIONS_FULL_INTERIOR_FIT 4




#define MAX_ASSOC_RULES 100




#define BAD_SUBSCRIPT -32767




#define MAX_SHORT 32700






















































extern int printstats, syntaxerror, eofseen, ddebug, trace, nowarn,
spprdflt;
extern int interactive, lex_compat, posix_compat, do_yylineno;
extern int useecs, fulltbl, usemecs, fullspd;
extern int gen_line_dirs, performance_report, backing_up_report;
extern int reentrant, bison_bridge_lval, bison_bridge_lloc;
extern int C_plus_plus, long_align, use_read, yytext_is_array, do_yywrap;
extern int csize;
extern int yymore_used, reject, real_reject, continued_action, in_rule;

extern int yymore_really_used, reject_really_used;
extern int trace_hex;

































extern int datapos, dataline, linenum;
extern FILE *skelfile, *backing_up_file;
extern const char *skel[];
extern int skel_ind;
extern char *infilename, *outfilename, *headerfilename;
extern int did_outfilename;
extern char *prefix, *yyclass, *extra_type;
extern int do_stdinit, use_stdout;
extern char **input_files;
extern int num_input_files;
extern char *program_name;

extern char *action_array;
extern int action_size;
extern int defs1_offset, prolog_offset, action_offset, action_index;










extern int onestate[ONE_STACK_SIZE], onesym[ONE_STACK_SIZE];
extern int onenext[ONE_STACK_SIZE], onedef[ONE_STACK_SIZE], onesp;



































extern int maximum_mns, current_mns, current_max_rules;
extern int num_rules, num_eof_rules, default_rule, lastnfa;
extern int *firstst, *lastst, *finalst, *transchar, *trans1, *trans2;
extern int *accptnum, *assoc_rule, *state_type;
extern int *rule_type, *rule_linenum, *rule_useful;
extern bool *rule_has_nl, *ccl_has_nl;
extern int nlch;




#define STATE_NORMAL 0x1
#define STATE_TRAILING_CONTEXT 0x2



extern int current_state_type;


#define RULE_NORMAL 0
#define RULE_VARIABLE 1




extern int variable_trailing_context_rules;














extern int numtemps, numprots, protprev[MSP], protnext[MSP], prottbl[MSP];
extern int protcomst[MSP], firstprot, lastprot, protsave[PROT_SAVE_SIZE];

















extern int numecs, nextecm[CSIZE + 1], ecgroup[CSIZE + 1], nummecs;






extern int tecfwd[CSIZE + 1], tecbck[CSIZE + 1];












extern int lastsc, *scset, *scbol, *scxclu, *sceof;
extern int current_max_scs;
extern char **scname;
































extern int current_max_dfa_size, current_max_xpairs;
extern int current_max_template_xpairs, current_max_dfas;
extern int lastdfa, *nxt, *chk, *tnxt;
extern int *base, *def, *nultrans, NUL_ec, tblend, firstfree, **dss,
*dfasiz;
extern union dfaacc_union {
int *dfaacc_set;
int dfaacc_state;
} *dfaacc;
extern int *accsiz, *dhash, numas;
extern int numsnpairs, jambase, jamstate;
extern int end_of_buffer_state;













extern int lastccl, *cclmap, *ccllen, *cclng, cclreuse;
extern int current_maxccls, current_max_ccl_tbl_size;
extern unsigned char *ccltbl;






















extern char nmstr[MAXLINE];
extern int sectnum, nummt, hshcol, dfaeql, numeps, eps2, num_reallocs;
extern int tmpuses, totnst, peakpairs, numuniq, numdup, hshsave;
extern int num_backing_up, bol_needed;

#if !defined(HAVE_REALLOCARRAY)
void *reallocarray(void *, size_t, size_t);
#endif

void *allocate_array(int, size_t);
void *reallocate_array(void *, int, size_t);

#define allocate_integer_array(size) allocate_array(size, sizeof(int))


#define reallocate_integer_array(array,size) reallocate_array((void *) array, size, sizeof(int))


#define allocate_bool_array(size) allocate_array(size, sizeof(bool))


#define reallocate_bool_array(array,size) reallocate_array((void *) array, size, sizeof(bool))


#define allocate_int_ptr_array(size) allocate_array(size, sizeof(int *))


#define allocate_char_ptr_array(size) allocate_array(size, sizeof(char *))


#define allocate_dfaacc_union(size) allocate_array(size, sizeof(union dfaacc_union))


#define reallocate_int_ptr_array(array,size) reallocate_array((void *) array, size, sizeof(int *))


#define reallocate_char_ptr_array(array,size) reallocate_array((void *) array, size, sizeof(char *))


#define reallocate_dfaacc_union(array, size) reallocate_array((void *) array, size, sizeof(union dfaacc_union))


#define allocate_character_array(size) allocate_array( size, sizeof(char))


#define reallocate_character_array(array,size) reallocate_array((void *) array, size, sizeof(char))


#define allocate_Character_array(size) allocate_array(size, sizeof(unsigned char))


#define reallocate_Character_array(array,size) reallocate_array((void *) array, size, sizeof(unsigned char))








extern void ccladd(int, int);
extern int cclinit(void);
extern void cclnegate(int);
extern int ccl_set_diff (int a, int b);
extern int ccl_set_union (int a, int b);


extern void list_character_set(FILE *, int[]);





extern void check_for_backing_up(int, int[]);


extern void check_trailing_context(int *, int, int *, int);


extern int *epsclosure(int *, int *, int[], int *, int *);


extern void increase_max_dfas(void);

extern void ntod(void);


extern int snstods(int[], int, int[], int, int, int *);





extern void ccl2ecl(void);


extern int cre8ecs(int[], int[], int);


extern void mkeccl(unsigned char[], int, int[], int[], int, int);


extern void mkechar(int, int[], int[]);




extern void do_indent(void);


extern void gen_backing_up(void);


extern void gen_bu_action(void);


extern void genctbl(void);


extern void gen_find_action(void);

extern void genftbl(void);


extern void gen_next_compressed_state(char *);


extern void gen_next_match(void);


extern void gen_next_state(int);


extern void gen_NUL_trans(void);


extern void gen_start_state(void);


extern void gentabs(void);


extern void indent_put2s(const char *, const char *);


extern void indent_puts(const char *);

extern void make_tables(void);




extern void check_options(void);
extern void flexend(int);
extern void usage(void);





extern void action_define(const char *defname, int value);


extern void add_action(const char *new_text);


extern int all_lower(char *);


extern int all_upper(char *);


extern int intcmp(const void *, const void *);


extern void check_char(int c);


extern unsigned char clower(int);


extern char *xstrdup(const char *);


extern int cclcmp(const void *, const void *);


extern void dataend(void);


extern void dataflush(void);


extern void flexerror(const char *);


extern void flexfatal(const char *);


#if HAVE_DECL___FUNC__
#define flex_die(msg) do{ fprintf (stderr,_("%s: fatal internal error at %s:%d (%s): %s\n"),program_name, __FILE__, (int)__LINE__,__func__,msg);FLEX_EXIT(1);}while(0)







#else
#define flex_die(msg) do{ fprintf (stderr,_("%s: fatal internal error at %s:%d %s\n"),program_name, __FILE__, (int)__LINE__,msg);FLEX_EXIT(1);}while(0)







#endif


extern void lerr(const char *, ...)
#if defined(__GNUC__) && __GNUC__ >= 3
__attribute__((__format__(__printf__, 1, 2)))
#endif
;


extern void lerr_fatal(const char *, ...)
#if defined(__GNUC__) && __GNUC__ >= 3
__attribute__((__format__(__printf__, 1, 2)))
#endif
;


extern void line_directive_out(FILE *, int);




extern void mark_defs1(void);


extern void mark_prolog(void);


extern void mk2data(int);

extern void mkdata(int);


extern int myctoi(const char *);


extern unsigned char myesc(unsigned char[]);


extern void out(const char *);
extern void out_dec(const char *, int);
extern void out_dec2(const char *, int, int);
extern void out_hex(const char *, unsigned int);
extern void out_str(const char *, const char *);
extern void out_str3(const char *, const char *, const char *, const char *);
extern void out_str_dec(const char *, const char *, int);
extern void outc(int);
extern void outn(const char *);
extern void out_m4_define(const char* def, const char* val);




extern char *readable_form(int);


extern void skelout(void);


extern void transition_struct_out(int, int);


extern void *yy_flex_xmalloc(int);





extern void add_accept(int, int);


extern int copysingl(int, int);


extern void dumpnfa(int);


extern void finish_rule(int, int, int, int, int);


extern int link_machines(int, int);




extern void mark_beginning_as_normal(int);


extern int mkbranch(int, int);

extern int mkclos(int);
extern int mkopt(int);


extern int mkor(int, int);


extern int mkposcl(int);

extern int mkrep(int, int, int);


extern int mkstate(int);

extern void new_rule(void);





extern void build_eof_action(void);


extern void format_pinpoint_message(const char *, const char *);


extern void pinpoint_message(const char *);


extern void line_warning(const char *, int);


extern void line_pinpoint(const char *, int);


extern void format_synerr(const char *, const char *);
extern void synerr(const char *);
extern void format_warn(const char *, const char *);
extern void lwarn(const char *);
extern void yyerror(const char *);
extern int yyparse(void);





extern int flexscan(void);


extern void set_input_file(char *);





extern void cclinstal(char[], int);


extern int ccllookup(char[]);

extern void ndinstal(const char *, char[]);
extern char *ndlookup(const char *);


extern void scextend(void);
extern void scinstal(const char *, int);


extern int sclookup(const char *);





extern void bldtbl(int[], int, int, int, int);

extern void cmptmps(void);
extern void expand_nxt_chk(void);


extern int find_table_space(int *, int);
extern void inittbl(void);


extern void mkdeftbl(void);




extern void mk1tbl(int, int, int, int);


extern void place_state(int *, int, int);


extern void stack1(int, int, int, int);




extern int yylex(void);


struct Buf {
void *elts;
int nelts;
size_t elt_size;
int nmax;
};

extern void buf_init(struct Buf * buf, size_t elem_size);
extern void buf_destroy(struct Buf * buf);
extern struct Buf *buf_append(struct Buf * buf, const void *ptr, int n_elem);
extern struct Buf *buf_concat(struct Buf* dest, const struct Buf* src);
extern struct Buf *buf_strappend(struct Buf *, const char *str);
extern struct Buf *buf_strnappend(struct Buf *, const char *str, int nchars);
extern struct Buf *buf_strdefine(struct Buf * buf, const char *str, const char *def);
extern struct Buf *buf_prints(struct Buf *buf, const char *fmt, const char* s);
extern struct Buf *buf_m4_define(struct Buf *buf, const char* def, const char* val);
extern struct Buf *buf_m4_undefine(struct Buf *buf, const char* def);
extern struct Buf *buf_print_strings(struct Buf * buf, FILE* out);
extern struct Buf *buf_linedir(struct Buf *buf, const char* filename, int lineno);

extern struct Buf userdef_buf;
extern struct Buf defs_buf;
extern struct Buf yydmap_buf;
extern struct Buf m4defs_buf;
extern struct Buf top_buf;
extern bool no_section3_escape;


#define OUT_BEGIN_CODE() outn("m4_ifdef( [[M4_YY_IN_HEADER]],,[[m4_dnl")
#define OUT_END_CODE() outn("]])")


extern jmp_buf flex_main_jmp_buf;

#define FLEX_EXIT(status) longjmp(flex_main_jmp_buf,(status)+1)


extern char *chomp (char *str);


#define b_isalnum(c) (isalnum(c)?true:false)
#define b_isalpha(c) (isalpha(c)?true:false)
#define b_isascii(c) (isascii(c)?true:false)
#define b_isblank(c) (isblank(c)?true:false)
#define b_iscntrl(c) (iscntrl(c)?true:false)
#define b_isdigit(c) (isdigit(c)?true:false)
#define b_isgraph(c) (isgraph(c)?true:false)
#define b_islower(c) (islower(c)?true:false)
#define b_isprint(c) (isprint(c)?true:false)
#define b_ispunct(c) (ispunct(c)?true:false)
#define b_isspace(c) (isspace(c)?true:false)
#define b_isupper(c) (isupper(c)?true:false)
#define b_isxdigit(c) (isxdigit(c)?true:false)


bool has_case(int c);


int reverse_case(int c);


bool range_covers_case (int c1, int c2);









struct filter {
int (*filter_func)(struct filter*);
void * extra;
int argc;
const char ** argv;
struct filter * next;
};


extern struct filter * output_chain;
extern struct filter *filter_create_ext (struct filter * chain, const char *cmd, ...);
struct filter *filter_create_int(struct filter *chain,
int (*filter_func) (struct filter *),
void *extra);
extern bool filter_apply_chain(struct filter * chain);
extern int filter_truncate(struct filter * chain, int max_len);
extern int filter_tee_header(struct filter *chain);
extern int filter_fix_linedirs(struct filter *chain);






extern regex_t regex_linedir, regex_blank_line;
bool flex_init_regex(void);
void flex_regcomp(regex_t *preg, const char *regex, int cflags);
char *regmatch_dup (regmatch_t * m, const char *src);
char *regmatch_cpy (regmatch_t * m, char *dest, const char *src);
int regmatch_len (regmatch_t * m);
int regmatch_strtol (regmatch_t * m, const char *src, char **endptr, int base);
bool regmatch_empty (regmatch_t * m);


typedef unsigned int scanflags_t;
extern scanflags_t* _sf_stk;
extern size_t _sf_top_ix, _sf_max;
#define _SF_CASE_INS ((scanflags_t) 0x0001)
#define _SF_DOT_ALL ((scanflags_t) 0x0002)
#define _SF_SKIP_WS ((scanflags_t) 0x0004)
#define sf_top() (_sf_stk[_sf_top_ix])
#define sf_case_ins() (sf_top() & _SF_CASE_INS)
#define sf_dot_all() (sf_top() & _SF_DOT_ALL)
#define sf_skip_ws() (sf_top() & _SF_SKIP_WS)
#define sf_set_case_ins(X) ((X) ? (sf_top() |= _SF_CASE_INS) : (sf_top() &= ~_SF_CASE_INS))
#define sf_set_dot_all(X) ((X) ? (sf_top() |= _SF_DOT_ALL) : (sf_top() &= ~_SF_DOT_ALL))
#define sf_set_skip_ws(X) ((X) ? (sf_top() |= _SF_SKIP_WS) : (sf_top() &= ~_SF_SKIP_WS))
extern void sf_init(void);
extern void sf_push(void);
extern void sf_pop(void);


#endif
