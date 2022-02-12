






































#if !defined(_h_el)
#define _h_el



#define KSHVI
#define VIDEFAULT
#define ANCHOR

#include "histedit.h"
#include "chartype.h"

#define EL_BUFSIZ ((size_t)1024)

#define HANDLE_SIGNALS 0x001
#define NO_TTY 0x002
#define EDIT_DISABLED 0x004
#define UNBUFFERED 0x008
#define NARROW_HISTORY 0x040
#define NO_RESET 0x080
#define FIXIO 0x100

typedef unsigned char el_action_t;

typedef struct coord_t {
int h;
int v;
} coord_t;

typedef struct el_line_t {
wchar_t *buffer;
wchar_t *cursor;
wchar_t *lastchar;
const wchar_t *limit;
} el_line_t;




typedef struct el_state_t {
int inputmode;
int doingarg;
int argument;
int metanext;
el_action_t lastcmd;
el_action_t thiscmd;
wchar_t thisch;
} el_state_t;




#define el_malloc(a) malloc(a)
#define el_calloc(a,b) calloc(a, b)
#define el_realloc(a,b) realloc(a, b)
#define el_free(a) free(a)

#include "tty.h"
#include "prompt.h"
#include "literal.h"
#include "keymacro.h"
#include "terminal.h"
#include "refresh.h"
#include "chared.h"
#include "search.h"
#include "hist.h"
#include "map.h"
#include "sig.h"

struct el_read_t;

struct editline {
wchar_t *el_prog;
FILE *el_infile;
FILE *el_outfile;
FILE *el_errfile;
int el_infd;
int el_outfd;
int el_errfd;
int el_flags;
coord_t el_cursor;
wint_t **el_display;
wint_t **el_vdisplay;
void *el_data;
el_line_t el_line;
el_state_t el_state;
el_terminal_t el_terminal;
el_tty_t el_tty;
el_refresh_t el_refresh;
el_prompt_t el_prompt;
el_prompt_t el_rprompt;
el_literal_t el_literal;
el_chared_t el_chared;
el_map_t el_map;
el_keymacro_t el_keymacro;
el_history_t el_history;
el_search_t el_search;
el_signal_t el_signal;
struct el_read_t *el_read;
ct_buffer_t el_visual;
ct_buffer_t el_scratch;
ct_buffer_t el_lgcyconv;
LineInfo el_lgcylinfo;
};

libedit_private int el_editmode(EditLine *, int, const wchar_t **);
libedit_private EditLine *el_init_internal(const char *, FILE *, FILE *,
FILE *, int, int, int, int);

#if defined(DEBUG)
#define EL_ABORT(a) do { fprintf(el->el_errfile, "%s, %d: ", __FILE__, __LINE__); fprintf a; abort(); } while( 0);





#else
#define EL_ABORT(a) abort()
#endif
#endif
