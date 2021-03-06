





























#include <sys/param.h>
#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERR (-2)
#define EMOD (-3)
#define FATAL (-4)

#define MINBUFSZ 512
#define SE_MAX 30
#if defined(INT_MAX)
#define LINECHARS INT_MAX
#else
#define LINECHARS MAXINT
#endif


#define GLB 001
#define GPR 002
#define GLS 004
#define GNP 010
#define GSG 020

typedef regex_t pattern_t;


typedef struct line {
struct line *q_forw;
struct line *q_back;
off_t seek;
int len;
} line_t;


typedef struct undo {


#define UADD 0
#define UDEL 1
#define UMOV 2
#define VMOV 3

int type;
line_t *h;
line_t *t;
} undo_t;

#if !defined(max)
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#define INC_MOD(l, k) ((l) + 1 > (k) ? 0 : (l) + 1)
#define DEC_MOD(l, k) ((l) - 1 < 0 ? (k) : (l) - 1)


#define SPL1() mutex++


#define SPL0() if (--mutex == 0) { if (sigflags & (1 << (SIGHUP - 1))) handle_hup(SIGHUP); if (sigflags & (1 << (SIGINT - 1))) handle_int(SIGINT); }






#define STRTOL(i, p) { if (((i = strtol(p, &p, 10)) == LONG_MIN || i == LONG_MAX) && errno == ERANGE) { errmsg = "number out of range"; i = 0; return ERR; } }








#if defined(sun) || defined(NO_REALLOC_NULL)

#define REALLOC(b,n,i,err) if ((i) > (n)) { size_t ti = (n); char *ts; SPL1(); if ((b) != NULL) { if ((ts = (char *) realloc((b), ti += max((i), MINBUFSZ))) == NULL) { fprintf(stderr, "%s\n", strerror(errno)); errmsg = "out of memory"; SPL0(); return err; } } else { if ((ts = (char *) malloc(ti += max((i), MINBUFSZ))) == NULL) { fprintf(stderr, "%s\n", strerror(errno)); errmsg = "out of memory"; SPL0(); return err; } } (n) = ti; (b) = ts; SPL0(); }























#else

#define REALLOC(b,n,i,err) if ((i) > (n)) { size_t ti = (n); char *ts; SPL1(); if ((ts = (char *) realloc((b), ti += max((i), MINBUFSZ))) == NULL) { fprintf(stderr, "%s\n", strerror(errno)); errmsg = "out of memory"; SPL0(); return err; } (n) = ti; (b) = ts; SPL0(); }














#endif


#define REQUE(pred, succ) (pred)->q_forw = (succ), (succ)->q_back = (pred)


#define INSQUE(elem, pred) { REQUE((elem), (pred)->q_forw); REQUE((pred), elem); }






#define REMQUE(elem) REQUE((elem)->q_back, (elem)->q_forw);


#define NUL_TO_NEWLINE(s, l) translit_text(s, l, '\0', '\n')


#define NEWLINE_TO_NUL(s, l) translit_text(s, l, '\n', '\0')



void add_line_node(line_t *);
int append_lines(long);
int apply_subst_template(const char *, regmatch_t *, int, int);
int build_active_list(int);
int cbc_decode(unsigned char *, FILE *);
int cbc_encode(unsigned char *, int, FILE *);
int check_addr_range(long, long);
void clear_active_list(void);
void clear_undo_stack(void);
int close_sbuf(void);
int copy_lines(long);
int delete_lines(long, long);
int display_lines(long, long, int);
line_t *dup_line_node(line_t *);
int exec_command(void);
long exec_global(int, int);
int extract_addr_range(void);
char *extract_pattern(int);
int extract_subst_tail(int *, long *);
char *extract_subst_template(void);
int filter_lines(long, long, char *);
line_t *get_addressed_line_node(long);
pattern_t *get_compiled_pattern(void);
char *get_extended_line(int *, int);
char *get_filename(void);
int get_keyword(void);
long get_line_node_addr(line_t *);
long get_matching_node_addr(pattern_t *, int);
long get_marked_node_addr(int);
char *get_sbuf_line(line_t *);
int get_shell_command(void);
int get_stream_line(FILE *);
int get_tty_line(void);
void handle_hup(int);
void handle_int(int);
void handle_winch(int);
int has_trailing_escape(char *, char *);
int hex_to_binary(int, int);
void init_buffers(void);
int is_legal_filename(char *);
int join_lines(long, long);
int mark_line_node(line_t *, int);
int move_lines(long);
line_t *next_active_node(void);
long next_addr(void);
int open_sbuf(void);
char *parse_char_class(char *);
int pop_undo_stack(void);
undo_t *push_undo_stack(int, long, long);
const char *put_sbuf_line(const char *);
int put_stream_line(FILE *, const char *, int);
int put_tty_line(const char *, int, long, int);
void quit(int);
long read_file(char *, long);
long read_stream(FILE *, long);
int search_and_replace(pattern_t *, int, int);
int set_active_node(line_t *);
void signal_hup(int);
void signal_int(int);
char *strip_escapes(char *);
int substitute_matching_text(pattern_t *, line_t *, int, int);
char *translit_text(char *, int, int, int);
void unmark_line_node(line_t *);
void unset_active_nodes(line_t *, line_t *);
long write_file(char *, const char *, long, long);
long write_stream(FILE *, long, long);


extern char stdinbuf[];
extern char *ibuf;
extern char *ibufp;
extern int ibufsz;


extern int isbinary;
extern int isglobal;
extern int modified;
extern int mutex;
extern int sigflags;


extern long addr_last;
extern long current_addr;
extern const char *errmsg;
extern long first_addr;
extern int lineno;
extern long second_addr;
extern long u_addr_last;
extern long u_current_addr;
extern long rows;
extern int cols;
extern int newline_added;
extern int scripted;
extern int patlock;
