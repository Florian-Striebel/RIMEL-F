

























#if !defined(DIALOG_H_included)
#define DIALOG_H_included 1


#include <dlg_config.h>

#if defined(__hpux)
#define __HP_CURSES_COMPAT
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <math.h>


#if defined(ERR) && (ERR == 13)
#undef ERR
#endif

#if defined(HAVE_NCURSESW_NCURSES_H)
#include <ncursesw/ncurses.h>
#elif defined(HAVE_NCURSES_NCURSES_H)
#include <ncurses/ncurses.h>
#elif defined(HAVE_NCURSES_CURSES_H)
#include <ncurses/curses.h>
#elif defined(HAVE_NCURSES_H)
#include <ncurses.h>
#else
#include <curses.h>
#if defined(HAVE_UNCTRL_H)
#include <unctrl.h>
#endif
#endif


#if !defined(KEY_MAX)
#if defined(__KEY_MAX)
#define KEY_MAX __KEY_MAX
#endif
#endif

#if !defined(KEY_MIN)
#if defined(__KEY_MIN)
#define KEY_MIN __KEY_MIN
#endif
#endif


#if defined(color_names)
#undef color_names
#endif

#if defined(buttons)
#undef buttons
#endif

#if defined(ENABLE_NLS)
#include <libintl.h>
#include <langinfo.h>
#define _(s) dgettext(PACKAGE, s)
#else
#undef _
#define _(s) s
#endif

#if !defined(GCC_PRINTFLIKE)
#define GCC_PRINTFLIKE(fmt,var)
#endif

#if !defined(GCC_NORETURN)
#define GCC_NORETURN
#endif

#if !defined(GCC_UNUSED)
#define GCC_UNUSED
#endif

#if !defined(HAVE_WGET_WCH)
#undef USE_WIDE_CURSES
#endif




#if defined(__hpux)
#undef ACS_UARROW
#undef ACS_DARROW
#undef ACS_BLOCK
#endif




#define USE_SHADOW TRUE
#define USE_COLORS TRUE




#define DLG_COLS (COLS ? COLS : dialog_state.screen_width)
#define DLG_LINES (LINES ? LINES : dialog_state.screen_height)




#if defined(HAVE_COLOR)
#define SCOLS (DLG_COLS - (dialog_state.use_shadow ? SHADOW_COLS : 0))
#define SLINES (DLG_LINES - (dialog_state.use_shadow ? SHADOW_ROWS : 0))
#else
#define SCOLS COLS
#define SLINES LINES
#endif





#define DLG_EXIT_ESC 255
#define DLG_EXIT_UNKNOWN -2
#define DLG_EXIT_ERROR -1
#define DLG_EXIT_OK 0
#define DLG_EXIT_CANCEL 1
#define DLG_EXIT_HELP 2
#define DLG_EXIT_EXTRA 3
#define DLG_EXIT_ITEM_HELP 4
#define DLG_EXIT_TIMEOUT 5

#define DLG_CTRL(n) ((n) & 0x1f)

#define CHR_LEAVE DLG_CTRL('D')
#define CHR_HELP DLG_CTRL('E')
#define CHR_BACKSPACE DLG_CTRL('H')
#define CHR_REPAINT DLG_CTRL('L')
#define CHR_NEXT DLG_CTRL('N')
#define CHR_PREVIOUS DLG_CTRL('P')
#define CHR_KILL DLG_CTRL('U')
#define CHR_TRACE DLG_CTRL('T')
#define CHR_LITERAL DLG_CTRL('V')
#define CHR_SPACE ' '
#define CHR_DELETE 127

#define ESC 27
#define TAB DLG_CTRL('I')

#define MARGIN 1
#define GUTTER 2
#define SHADOW_ROWS 1
#define SHADOW_COLS 2
#define ARROWS_COL 5

#define MAX_LEN 2048
#define BUF_SIZE (10L*1024)

#undef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#undef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define DEFAULT_SEPARATE_STR "\t"
#define DEFAULT_ASPECT_RATIO 9

#define TAB_LEN 8
#define WTIMEOUT_VAL 10
#define WTIMEOUT_OFF -1

#if !defined(A_CHARTEXT)
#define A_CHARTEXT 0xff
#endif

#define CharOf(ch) ((ch) & 0xff)

#if !defined(ACS_ULCORNER)
#define ACS_ULCORNER '+'
#endif
#if !defined(ACS_LLCORNER)
#define ACS_LLCORNER '+'
#endif
#if !defined(ACS_URCORNER)
#define ACS_URCORNER '+'
#endif
#if !defined(ACS_LRCORNER)
#define ACS_LRCORNER '+'
#endif
#if !defined(ACS_HLINE)
#define ACS_HLINE '-'
#endif
#if !defined(ACS_VLINE)
#define ACS_VLINE '|'
#endif
#if !defined(ACS_LTEE)
#define ACS_LTEE '+'
#endif
#if !defined(ACS_RTEE)
#define ACS_RTEE '+'
#endif
#if !defined(ACS_UARROW)
#define ACS_UARROW '^'
#endif
#if !defined(ACS_DARROW)
#define ACS_DARROW 'v'
#endif
#if !defined(ACS_BLOCK)
#define ACS_BLOCK '#'
#endif


#if !defined(HAVE_GETBEGYX)
#undef getbegyx
#define getbegyx(win,y,x) (y = (win)?(win)->_begy:ERR, x = (win)?(win)->_begx:ERR)
#endif

#if !defined(HAVE_GETMAXYX)
#undef getmaxyx
#define getmaxyx(win,y,x) (y = (win)?(win)->_maxy:ERR, x = (win)?(win)->_maxx:ERR)
#endif

#if !defined(HAVE_GETPARYX)
#undef getparyx
#define getparyx(win,y,x) (y = (win)?(win)->_pary:ERR, x = (win)?(win)->_parx:ERR)
#endif

#if !defined(HAVE_WGETPARENT) && defined(HAVE_WINDOW__PARENT)
#undef wgetparent
#define wgetparent(win) ((win) ? (win)->_parent : 0)
#endif

#if !defined(HAVE_WSYNCUP)
#undef wsyncup
#define wsyncup(win)
#endif

#if !defined(HAVE_WCURSYNCUP)
#undef wcursyncup
#define wcursyncup(win)
#endif

#if defined(__cplusplus)
extern "C" {
#endif


#if !(defined(HAVE_GETBEGX) && defined(HAVE_GETBEGY))
#undef getbegx
#undef getbegy
#define getbegx(win) dlg_getbegx(win)
#define getbegy(win) dlg_getbegy(win)
extern int dlg_getbegx(WINDOW * );
extern int dlg_getbegy(WINDOW * );
#endif

#if !(defined(HAVE_GETCURX) && defined(HAVE_GETCURY))
#undef getcurx
#undef getcury
#define getcurx(win) dlg_getcurx(win)
#define getcury(win) dlg_getcury(win)
extern int dlg_getcurx(WINDOW * );
extern int dlg_getcury(WINDOW * );
#endif

#if !(defined(HAVE_GETMAXX) && defined(HAVE_GETMAXY))
#undef getmaxx
#undef getmaxy
#define getmaxx(win) dlg_getmaxx(win)
#define getmaxy(win) dlg_getmaxy(win)
extern int dlg_getmaxx(WINDOW * );
extern int dlg_getmaxy(WINDOW * );
#endif

#if !(defined(HAVE_GETPARX) && defined(HAVE_GETPARY))
#undef getparx
#undef getpary
#define getparx(win) dlg_getparx(win)
#define getpary(win) dlg_getpary(win)
extern int dlg_getparx(WINDOW * );
extern int dlg_getpary(WINDOW * );
#endif

#if !(defined(HAVE_WGETPARENT) && defined(HAVE_WINDOW__PARENT))
#undef wgetparent
#define wgetparent(win) dlg_wgetparent(win)
extern WINDOW * dlg_wgetparent(WINDOW * );
#endif







#if defined(__DIALOG_OLD_NAMES__)
#define color_table dlg_color_table
#define attr_clear(win,h,w,a) dlg_attr_clear(win,h,w,a)
#define auto_size(t,s,h,w,xl,mc) dlg_auto_size(t,s,h,w,xl,mc)
#define auto_sizefile(t,f,h,w,xl,mc) dlg_auto_sizefile(t,f,h,w,xl,mc)
#define beeping() dlg_beeping()
#define box_x_ordinate(w) dlg_box_x_ordinate(w)
#define box_y_ordinate(h) dlg_box_y_ordinate(h)
#define calc_listh(h,lh,in) dlg_calc_listh(h,lh,in)
#define calc_listw(in,items,group) dlg_calc_listw(in,items,group)
#define color_setup() dlg_color_setup()
#define create_rc(f) dlg_create_rc(f)
#define ctl_size(h,w) dlg_ctl_size(h,w)
#define del_window(win) dlg_del_window(win)
#define dialog_clear() dlg_clear()
#define draw_bottom_box(win) dlg_draw_bottom_box(win)
#define draw_box(win,y,x,h,w,xc,bc) dlg_draw_box(win,y,x,h,w,xc,bc)
#define draw_shadow(win,h,w,y,x) dlg_draw_shadow(win,h,w,y,x)
#define draw_title(win,t) dlg_draw_title(win,t)
#define exiterr dlg_exiterr
#define killall_bg(n) dlg_killall_bg(n)
#define mouse_bigregion(y,x) dlg_mouse_bigregion(y,x)
#define mouse_free_regions() dlg_mouse_free_regions()
#define mouse_mkbigregion(y,x,h,w,n,ix,iy,m) dlg_mouse_mkbigregion(y,x,h,w,n,ix,iy,m)
#define mouse_mkregion(y,x,h,w,n) dlg_mouse_mkregion(y,x,h,w,n)
#define mouse_region(y,x) dlg_mouse_region(y,x)
#define mouse_setbase(x,y) dlg_mouse_setbase(x,y)
#define mouse_setcode(c) dlg_mouse_setcode(c)
#define mouse_wgetch(w,c) dlg_mouse_wgetch(w,c)
#define new_window(h,w,y,x) dlg_new_window(h,w,y,x)
#define parse_rc() dlg_parse_rc()
#define print_autowrap(win,s,h,w) dlg_print_autowrap(win,s,h,w)
#define print_size(h,w) dlg_print_size(h,w)
#define put_backtitle() dlg_put_backtitle()
#define strclone(cprompt) dlg_strclone(cprompt)
#define sub_window(win,h,w,y,x) dlg_sub_window(win,h,w,y,x)
#define tab_correct_str(s) dlg_tab_correct_str(s)
#endif




#define DIALOG_ATR(n) dlg_color_table[n].atr

#define screen_attr DIALOG_ATR(0)
#define shadow_attr DIALOG_ATR(1)
#define dialog_attr DIALOG_ATR(2)
#define title_attr DIALOG_ATR(3)
#define border_attr DIALOG_ATR(4)
#define button_active_attr DIALOG_ATR(5)
#define button_inactive_attr DIALOG_ATR(6)
#define button_key_active_attr DIALOG_ATR(7)
#define button_key_inactive_attr DIALOG_ATR(8)
#define button_label_active_attr DIALOG_ATR(9)
#define button_label_inactive_attr DIALOG_ATR(10)
#define inputbox_attr DIALOG_ATR(11)
#define inputbox_border_attr DIALOG_ATR(12)
#define searchbox_attr DIALOG_ATR(13)
#define searchbox_title_attr DIALOG_ATR(14)
#define searchbox_border_attr DIALOG_ATR(15)
#define position_indicator_attr DIALOG_ATR(16)
#define menubox_attr DIALOG_ATR(17)
#define menubox_border_attr DIALOG_ATR(18)
#define item_attr DIALOG_ATR(19)
#define item_selected_attr DIALOG_ATR(20)
#define tag_attr DIALOG_ATR(21)
#define tag_selected_attr DIALOG_ATR(22)
#define tag_key_attr DIALOG_ATR(23)
#define tag_key_selected_attr DIALOG_ATR(24)
#define check_attr DIALOG_ATR(25)
#define check_selected_attr DIALOG_ATR(26)
#define uarrow_attr DIALOG_ATR(27)
#define darrow_attr DIALOG_ATR(28)
#define itemhelp_attr DIALOG_ATR(29)
#define form_active_text_attr DIALOG_ATR(30)
#define form_text_attr DIALOG_ATR(31)
#define form_item_readonly_attr DIALOG_ATR(32)
#define gauge_attr DIALOG_ATR(33)
#define border2_attr DIALOG_ATR(34)
#define inputbox_border2_attr DIALOG_ATR(35)
#define searchbox_border2_attr DIALOG_ATR(36)
#define menubox_border2_attr DIALOG_ATR(37)

#define DLGK_max (KEY_MAX + 256)




#if defined(PDCURSES)
#define dlg_attrset(w,a) (void) wattrset((w), (a))
#define dlg_attron(w,a) (void) wattron((w), (a))
#define dlg_attroff(w,a) (void) wattroff((w), (a))
#else
#define dlg_attrset(w,a) (void) wattrset((w), (int)(a))
#define dlg_attron(w,a) (void) wattron((w), (int)(a))
#define dlg_attroff(w,a) (void) wattroff((w), (int)(a))
#endif




struct _dlg_callback;

typedef void (*DIALOG_FREEBACK) (struct _dlg_callback * );

typedef struct _dlg_callback {
struct _dlg_callback *next;
FILE *input;
WINDOW *win;
bool keep_bg;
bool bg_task;
bool (*handle_getc)(struct _dlg_callback *p, int ch, int fkey, int *result);
bool keep_win;

struct _dlg_callback **caller;
DIALOG_FREEBACK freeback;

bool (*handle_input)(struct _dlg_callback *p);
bool input_ready;
} DIALOG_CALLBACK;

typedef struct _dlg_windows {
struct _dlg_windows *next;
WINDOW *normal;
WINDOW *shadow;
int getc_timeout;
} DIALOG_WINDOWS;




typedef struct {
DIALOG_CALLBACK *getc_callbacks;
DIALOG_CALLBACK *getc_redirect;
DIALOG_WINDOWS *all_windows;
DIALOG_WINDOWS *all_subwindows;
FILE *output;
FILE *pipe_input;
FILE *screen_output;
bool screen_initialized;
bool use_colors;
bool use_scrollbar;
bool use_shadow;
bool visit_items;
char *separate_str;
int aspect_ratio;
int output_count;
int tab_len;

FILE *input;
#if defined(HAVE_DLG_TRACE)
FILE *trace_output;
#endif

bool no_mouse;
int visit_cols;

bool finish_string;

bool plain_buttons;

bool text_only;
int text_height;
int text_width;

int screen_height;
int screen_width;
#if defined(KEY_RESIZE)

bool had_resize;
#endif
} DIALOG_STATE;

extern DIALOG_STATE dialog_state;




typedef struct {
bool beep_after_signal;
bool beep_signal;
bool begin_set;
bool cant_kill;
bool colors;
bool cr_wrap;
bool defaultno;
bool dlg_clear_screen;
bool extra_button;
bool help_button;
bool help_status;
bool input_menu;
bool insecure;
bool item_help;
bool keep_window;
bool nocancel;
bool nocollapse;
bool print_siz;
bool separate_output;
bool single_quoted;
bool size_err;
bool tab_correct;
bool trim_whitespace;
char *backtitle;
char *cancel_label;
char *default_item;
char *exit_label;
char *extra_label;
char *help_label;
char *input_result;
char *no_label;
char *ok_label;
char *title;
char *yes_label;
int begin_x;
int begin_y;
int max_input;
int scale_factor;
int sleep_secs;
int timeout_secs;
unsigned input_length;

unsigned formitem_type;

bool keep_tite;
bool ascii_lines;
bool no_lines;

bool nook;

bool quoted;
char *column_header;
char *column_separator;
char *output_separator;

char *date_format;
char *time_format;

char *help_line;
char *help_file;
bool in_helpfile;
bool no_nl_expand;

int default_button;

bool no_tags;
bool no_items;

bool last_key;

bool help_tags;

char *week_start;

bool iso_week;

bool reorder;

int pause_secs;

bool erase_on_exit;
bool cursor_off_label;

bool no_hot_list;
} DIALOG_VARS;

#define USE_ITEM_HELP(s) (dialog_vars.item_help && (s) != 0)





#define DLG__NO_ITEMS (dialog_vars.no_items ? 0 : 1)
#define DLG__ITEM_HELP (dialog_vars.item_help ? 1 : 0)




#define CHECKBOX_TAGS (2 + DLG__ITEM_HELP + DLG__NO_ITEMS)
#define MENUBOX_TAGS (1 + DLG__ITEM_HELP + DLG__NO_ITEMS)
#define FORMBOX_TAGS (8 + DLG__ITEM_HELP)
#define MIXEDFORM_TAGS (1 + FORMBOX_TAGS)
#define MIXEDGAUGE_TAGS 2
#define TREEVIEW_TAGS (3 + DLG__ITEM_HELP + DLG__NO_ITEMS)

extern DIALOG_VARS dialog_vars;

#if !defined(HAVE_TYPE_CHTYPE)
#define chtype long
#endif

#if !defined(isblank)
#define isblank(c) ((c) == ' ' || (c) == TAB)
#endif

#define UCH(ch) ((unsigned char)(ch))

#define assert_ptr(ptr,msg) if ((ptr) == 0) dlg_exiterr("cannot allocate memory in " msg)

#define dlg_malloc(t,n) (t *) malloc((size_t)(n) * sizeof(t))
#define dlg_calloc(t,n) (t *) calloc((size_t)(n), sizeof(t))
#define dlg_realloc(t,n,p) (t *) realloc((p), (n) * sizeof(t))




typedef struct {
chtype atr;
#if defined(HAVE_COLOR)
int fg;
int bg;
int hilite;
#if defined(HAVE_RC_FILE2)
int ul;
int rv;
#endif
#endif
#if defined(HAVE_RC_FILE)
const char *name;
const char *comment;
#endif
} DIALOG_COLORS;

extern DIALOG_COLORS dlg_color_table[];




extern const char *dialog_version(void);


extern int dialog_buildlist(const char * , const char * , int , int , int , int , char ** , int );
extern int dialog_calendar(const char * , const char * , int , int , int , int , int );
extern int dialog_checklist(const char * , const char * , int , int , int , int , char ** , int );
extern int dialog_dselect(const char * , const char * , int , int );
extern int dialog_editbox(const char * , const char * , int , int );
extern int dialog_form(const char * , const char * , int , int , int , int , char ** );
extern int dialog_fselect(const char * , const char * , int , int );
extern int dialog_gauge(const char * , const char * , int , int , int );
extern int dialog_helpfile(const char * , const char * , int , int );
extern int dialog_inputbox(const char * , const char * , int , int , const char * , const int );
extern int dialog_menu(const char * , const char * , int , int , int , int , char ** );
extern int dialog_mixedform(const char * , const char * , int , int , int , int , char ** );
extern int dialog_mixedgauge(const char * , const char * , int , int , int , int , char ** );
extern int dialog_msgbox(const char * , const char * , int , int , int );
extern int dialog_pause(const char * , const char * , int , int , int );
extern int dialog_prgbox(const char * , const char * , const char * , int , int , int );
extern int dialog_progressbox(const char * , const char * , int , int );
extern int dialog_rangebox(const char * , const char * , int , int , int , int , int );
extern int dialog_tailbox(const char * , const char * , int , int , int );
extern int dialog_textbox(const char * , const char * , int , int );
extern int dialog_timebox(const char * , const char * , int , int , int , int , int );
extern int dialog_treeview(const char * , const char * , int , int , int , int , char ** , int );
extern int dialog_yesno(const char * , const char * , int , int );


typedef struct {
char *name;
char *text;
char *help;
int state;
} DIALOG_LISTITEM;

typedef struct {
unsigned type;
char *name;
int name_len;
int name_y;
int name_x;
bool name_free;
char *text;
int text_len;
int text_y;
int text_x;
int text_flen;
int text_ilen;
bool text_free;
char *help;
bool help_free;
} DIALOG_FORMITEM;

typedef int (DIALOG_INPUTMENU) (DIALOG_LISTITEM * , int , char * );

extern int dlg_checklist(const char * , const char * , int , int , int , int , DIALOG_LISTITEM * , const char * , int , int * );
extern int dlg_form(const char * , const char * , int , int , int , int , DIALOG_FORMITEM * , int * );
extern int dlg_menu(const char * , const char * , int , int , int , int , DIALOG_LISTITEM * , int * , DIALOG_INPUTMENU );
extern int dlg_progressbox(const char * , const char * , int , int , int , FILE * );


extern char ** dlg_string_to_argv(char * );
extern int dlg_count_argv(char ** );
extern int dlg_eat_argv(int * , char *** , int , int );


extern void dlg_draw_arrows(WINDOW * , int , int , int , int , int );
extern void dlg_draw_arrows2(WINDOW * , int , int , int , int , int , chtype , chtype );
extern void dlg_draw_helpline(WINDOW * , bool );
extern void dlg_draw_scrollbar(WINDOW * , long , long , long , long , int , int , int , int , chtype , chtype );


extern int dlg_buildlist(const char * , const char * , int , int , int , int , DIALOG_LISTITEM * , const char * , int , int * );


extern const char ** dlg_exit_label(void);
extern const char ** dlg_ok_label(void);
extern const char ** dlg_ok_labels(void);
extern const char ** dlg_yes_labels(void);
extern int dlg_button_count(const char ** );
extern int dlg_button_to_char(const char * );
extern int dlg_button_x_step(const char ** , int , int * , int * , int * );
extern int dlg_char_to_button(int , const char ** );
extern int dlg_exit_buttoncode(int );
extern int dlg_match_char(int , const char * );
extern int dlg_next_button(const char ** , int );
extern int dlg_next_ok_buttonindex(int , int );
extern int dlg_ok_buttoncode(int );
extern int dlg_prev_button(const char ** , int );
extern int dlg_prev_ok_buttonindex(int , int );
extern int dlg_yes_buttoncode(int );
extern void dlg_button_layout(const char ** , int * );
extern void dlg_button_sizes(const char ** , int , int * , int * );
extern void dlg_draw_buttons(WINDOW * , int , int , const char ** , int , int , int );


extern void dlg_align_columns(char ** , int , int );
extern void dlg_free_columns(char ** , int , int );


extern int dlg_editbox(const char * , char *** , int * , int , int );


extern int dlg_default_formitem(DIALOG_FORMITEM * );
extern int dlg_ordinate(const char * );
extern void dlg_free_formitems(DIALOG_FORMITEM * );


extern void * dlg_allocate_gauge(const char * , const char * , int , int , int );
extern void * dlg_reallocate_gauge(void * , const char * , const char * , int , int , int );
extern void dlg_free_gauge(void * );
extern void dlg_update_gauge(void * , int );


extern bool dlg_edit_string(char * , int * , int , int , bool );
extern const int * dlg_index_columns(const char * );
extern const int * dlg_index_wchars(const char * );
extern int dlg_count_columns(const char * );
extern int dlg_count_wchars(const char * );
extern int dlg_edit_offset(char * , int , int );
extern int dlg_find_index(const int * , int , int );
extern int dlg_limit_columns(const char * , int , int );
extern void dlg_finish_string(const char * );
extern void dlg_show_string(WINDOW * , const char * , int , chtype , int , int , int , bool , bool );


extern int dlg_dummy_menutext(DIALOG_LISTITEM * , int , char * );
extern int dlg_renamed_menutext(DIALOG_LISTITEM * , int , char * );


extern FILE * dlg_popen(const char * , const char * );


#if defined(HAVE_RC_FILE)
extern int dlg_parse_rc(void);
extern void dlg_create_rc(const char * );
#endif


extern int dlg_treeview(const char * , const char * , int , int , int , int , DIALOG_LISTITEM * , const char * , int * , int , int * );


extern int dlg_ttysize(int , int * , int * );


extern int dlg_getc(WINDOW * , int * );
extern int dlg_getc_callbacks(int , int , int * );
extern int dlg_last_getc(void);
extern void dlg_add_last_key(int );
extern void dlg_add_callback(DIALOG_CALLBACK * );
extern void dlg_add_callback_ref(DIALOG_CALLBACK ** , DIALOG_FREEBACK );
extern void dlg_flush_getc(void);
extern void dlg_remove_callback(DIALOG_CALLBACK * );
extern void dlg_killall_bg(int *retval);


extern DIALOG_WINDOWS * _dlg_find_window(WINDOW * );
extern WINDOW * dlg_der_window(WINDOW * , int , int , int , int );
extern WINDOW * dlg_new_modal_window(WINDOW * , int , int , int , int );
extern WINDOW * dlg_new_window(int , int , int , int );
extern WINDOW * dlg_sub_window(WINDOW * , int , int , int , int );
extern bool dlg_need_separator(void);
extern char * dlg_getenv_str(const char * );
extern char * dlg_set_result(const char * );
extern char * dlg_strclone(const char * );
extern char * dlg_strempty(void);
extern chtype dlg_asciibox(chtype );
extern chtype dlg_boxchar(chtype );
extern chtype dlg_get_attrs(WINDOW * );
extern const char * dlg_exitcode2s(int );
extern const char * dlg_print_line(WINDOW * , chtype * , const char * , int , int , int * );
extern int dlg_box_x_ordinate(int );
extern int dlg_box_y_ordinate(int );
extern int dlg_calc_list_width(int , DIALOG_LISTITEM * );
extern int dlg_calc_listw(int , char ** , int );
extern int dlg_check_scrolled(int , int , int , bool * , int * );
extern int dlg_count_real_columns(const char * );
extern int dlg_default_item(char ** , int );
extern int dlg_default_listitem(DIALOG_LISTITEM * );
extern int dlg_defaultno_button(void);
extern int dlg_default_button(void);
extern int dlg_exitname2n(const char * );
extern int dlg_getenv_num(const char * , int * );
extern int dlg_max_input(int );
extern int dlg_print_scrolled(WINDOW * , const char * , int , int , int , int );
extern int dlg_set_timeout(WINDOW * , bool );
extern void dlg_add_help_formitem(int * , char ** , DIALOG_FORMITEM * );
extern void dlg_add_help_listitem(int * , char ** , DIALOG_LISTITEM * );
extern void dlg_add_quoted(char * );
extern void dlg_add_result(const char * );
extern void dlg_add_separator(void);
extern void dlg_add_string(char * );
extern void dlg_attr_clear(WINDOW * , int , int , chtype );
extern void dlg_auto_size(const char * , const char * , int * , int * , int , int );
extern void dlg_auto_sizefile(const char * , const char * , int * , int * , int , int );
extern void dlg_beeping(void);
extern void dlg_calc_listh(int * , int * , int );
extern void dlg_clear(void);
extern void dlg_clr_result(void);
extern void dlg_ctl_size(int , int );
extern void dlg_del_window(WINDOW * );
extern void dlg_does_output(void);
extern void dlg_draw_bottom_box(WINDOW * );
extern void dlg_draw_bottom_box2(WINDOW * , chtype , chtype , chtype );
extern void dlg_draw_box(WINDOW * , int , int , int , int , chtype , chtype );
extern void dlg_draw_box2(WINDOW * , int , int , int , int , chtype , chtype , chtype );
extern void dlg_draw_title(WINDOW *win, const char *title);
extern void dlg_exit(int ) GCC_NORETURN;
extern void dlg_item_help(const char * );
extern void dlg_keep_tite(FILE * );
extern void dlg_print_autowrap(WINDOW * , const char * , int , int );
extern void dlg_print_listitem(WINDOW * , const char * , int , bool , int );
extern void dlg_print_size(int , int );
extern void dlg_print_text(WINDOW * , const char * , int , chtype * );
extern void dlg_put_backtitle(void);
extern void dlg_reset_timeout(WINDOW * );
extern void dlg_restore_vars(DIALOG_VARS * );
extern void dlg_save_vars(DIALOG_VARS * );
extern void dlg_set_focus(WINDOW * , WINDOW * );
extern void dlg_tab_correct_str(char * );
extern void dlg_trim_string(char * );
extern void end_dialog(void);
extern void init_dialog(FILE * , FILE * );

extern void dlg_exiterr(const char *, ...) GCC_NORETURN GCC_PRINTFLIKE(1,2);

#if defined(HAVE_COLOR)
extern chtype dlg_color_pair(int , int );
extern int dlg_color_count(void);
extern void dlg_color_setup(void);
extern void dlg_draw_shadow(WINDOW * , int , int , int , int );
#endif

#if defined(HAVE_STRCASECMP)
#define dlg_strcmp(a,b) strcasecmp(a,b)
#else
extern int dlg_strcmp(const char * , const char * );
#endif

#if defined(HAVE_DLG_TRACE)
#define DLG_TRACE(params) dlg_trace_msg params
extern void dlg_trace_msg(const char *fmt, ...) GCC_PRINTFLIKE(1,2);
extern void dlg_trace_va_msg(const char *fmt, va_list ap);
#define DLG_TRACE2S(name,value) dlg_trace_2s (name,value)
#define DLG_TRACE2N(name,value) dlg_trace_2n (name,value)
extern void dlg_trace_2s(const char * , const char * );
extern void dlg_trace_2n(const char * , int );
extern void dlg_trace_win(WINDOW * );
extern void dlg_trace_chr(int , int );
extern void dlg_trace(const char * );
#else
#define DLG_TRACE(params)
#define DLG_TRACE2S(name,value)
#define DLG_TRACE2N(name,value)
#define dlg_trace_va_msg(fmt, ap)
#define dlg_trace_win(win)
#define dlg_trace_chr(ch,fkey)
#define dlg_trace(fname)
#endif

#if defined(KEY_RESIZE)
extern void _dlg_resize_cleanup(WINDOW * );
extern void dlg_move_window(WINDOW * , int , int , int , int );
extern void dlg_will_resize(WINDOW * );
#endif





#define dlg_enter_buttoncode(code) (dialog_vars.nook ? DLG_EXIT_OK : dlg_ok_buttoncode(code))




typedef struct mseRegion {
int x, y, X, Y, code;
int mode, step_x, step_y;
struct mseRegion *next;
} mseRegion;

#if defined(NCURSES_MOUSE_VERSION)

#define mouse_open() mousemask(BUTTON1_PRESSED, (mmask_t *) 0)
#define mouse_close() mousemask(0, (mmask_t *) 0)

extern mseRegion * dlg_mouse_mkregion (int , int , int , int , int );
extern void dlg_mouse_free_regions (void);
extern void dlg_mouse_mkbigregion (int , int , int , int , int , int , int , int );
extern void dlg_mouse_setbase (int , int );
extern void dlg_mouse_setcode (int );

#define USE_MOUSE 1

#else

#define mouse_open()
#define mouse_close()
#define dlg_mouse_free_regions()
#define dlg_mouse_mkregion(y, x, height, width, code)
#define dlg_mouse_mkbigregion(y, x, height, width, code, step_x, step_y, mode)
#define dlg_mouse_setbase(x, y)
#define dlg_mouse_setcode(c)

#define USE_MOUSE 0

#endif

extern mseRegion *dlg_mouse_region (int , int );
extern mseRegion *dlg_mouse_bigregion (int , int );
extern int dlg_mouse_wgetch (WINDOW * , int * );
extern int dlg_mouse_wgetch_nowait (WINDOW * , int * );

#define mouse_mkbutton(y,x,len,code) dlg_mouse_mkregion(y,x,1,len,code);









#define M_EVENT (DLGK_max + 1)





#define FLAG_CHECK 1
#define FLAG_RADIO 0




#if defined(NO_LEAKS)
extern void _dlg_inputstr_leaks(void);
#if defined(NCURSES_VERSION)
#if defined(HAVE_CURSES_EXIT)

#elif defined(HAVE__NC_FREE_AND_EXIT)
extern void _nc_free_and_exit(int);
#define curses_exit(code) _nc_free_and_exit(code)
#endif
#endif
#endif

#if defined(__cplusplus)
}
#endif


#endif
