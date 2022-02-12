













#include "less.h"
#include "option.h"




public int quiet;
public int how_search;
public int top_scroll;

public int pr_type;
public int bs_mode;
public int know_dumb;
public int quit_at_eof;
public int quit_if_one_screen;
public int squeeze;
public int tabstop;
public int back_scroll;
public int forw_scroll;
public int caseless;
public int linenums;
public int autobuf;
public int bufspace;
public int ctldisp;
public int force_open;
public int swindow;
public int jump_sline;
public long jump_sline_fraction = -1;
public long shift_count_fraction = -1;
public int chopline;
public int no_init;
public int no_keypad;
public int twiddle;
public int show_attn;
public int shift_count;
public int status_col;
public int use_lessopen;
public int quit_on_intr;
public int follow_mode;
public int oldbot;
public int opt_use_backslash;
public char rscroll_char;
public int rscroll_attr;
public int no_hist_dups;
public int mousecap;
public int wheel_lines;
public int perma_marks;
public int linenum_width;
public int status_col_width;
public int incr_search;
public int use_color;
public int want_filesize;
#if HILITE_SEARCH
public int hilite_search;
#endif

public int less_is_more = 0;




static struct optname a_optname = { "search-skip-screen", NULL };
static struct optname b_optname = { "buffers", NULL };
static struct optname B__optname = { "auto-buffers", NULL };
static struct optname c_optname = { "clear-screen", NULL };
static struct optname d_optname = { "dumb", NULL };
static struct optname D__optname = { "color", NULL };
static struct optname e_optname = { "quit-at-eof", NULL };
static struct optname f_optname = { "force", NULL };
static struct optname F__optname = { "quit-if-one-screen", NULL };
#if HILITE_SEARCH
static struct optname g_optname = { "hilite-search", NULL };
#endif
static struct optname h_optname = { "max-back-scroll", NULL };
static struct optname i_optname = { "ignore-case", NULL };
static struct optname j_optname = { "jump-target", NULL };
static struct optname J__optname = { "status-column", NULL };
#if USERFILE
static struct optname k_optname = { "lesskey-file", NULL };
#if HAVE_LESSKEYSRC
static struct optname ks_optname = { "lesskey-src", NULL };
#endif
#endif
static struct optname K__optname = { "quit-on-intr", NULL };
static struct optname L__optname = { "no-lessopen", NULL };
static struct optname m_optname = { "long-prompt", NULL };
static struct optname n_optname = { "line-numbers", NULL };
#if LOGFILE
static struct optname o_optname = { "log-file", NULL };
static struct optname O__optname = { "LOG-FILE", NULL };
#endif
static struct optname p_optname = { "pattern", NULL };
static struct optname P__optname = { "prompt", NULL };
static struct optname q2_optname = { "silent", NULL };
static struct optname q_optname = { "quiet", &q2_optname };
static struct optname r_optname = { "raw-control-chars", NULL };
static struct optname s_optname = { "squeeze-blank-lines", NULL };
static struct optname S__optname = { "chop-long-lines", NULL };
#if TAGS
static struct optname t_optname = { "tag", NULL };
static struct optname T__optname = { "tag-file", NULL };
#endif
static struct optname u_optname = { "underline-special", NULL };
static struct optname V__optname = { "version", NULL };
static struct optname w_optname = { "hilite-unread", NULL };
static struct optname x_optname = { "tabs", NULL };
static struct optname X__optname = { "no-init", NULL };
static struct optname y_optname = { "max-forw-scroll", NULL };
static struct optname z_optname = { "window", NULL };
static struct optname quote_optname = { "quotes", NULL };
static struct optname tilde_optname = { "tilde", NULL };
static struct optname query_optname = { "help", NULL };
static struct optname pound_optname = { "shift", NULL };
static struct optname keypad_optname = { "no-keypad", NULL };
static struct optname oldbot_optname = { "old-bot", NULL };
static struct optname follow_optname = { "follow-name", NULL };
static struct optname use_backslash_optname = { "use-backslash", NULL };
static struct optname rscroll_optname = { "rscroll", NULL };
static struct optname nohistdups_optname = { "no-histdups", NULL };
static struct optname mousecap_optname = { "mouse", NULL };
static struct optname wheel_lines_optname = { "wheel-lines", NULL };
static struct optname perma_marks_optname = { "save-marks", NULL };
static struct optname linenum_width_optname = { "line-num-width", NULL };
static struct optname status_col_width_optname = { "status-col-width", NULL };
static struct optname incr_search_optname = { "incsearch", NULL };
static struct optname use_color_optname = { "use-color", NULL };
static struct optname want_filesize_optname = { "file-size", NULL };
#if LESSTEST
static struct optname ttyin_name_optname = { "tty", NULL };
static struct optname rstat_optname = { "rstat", NULL };
#endif














static struct loption option[] =
{
{ 'a', &a_optname,
TRIPLE, OPT_ONPLUS, &how_search, NULL,
{
"Search includes displayed screen",
"Search skips displayed screen",
"Search includes all of displayed screen"
}
},

{ 'b', &b_optname,
NUMBER|INIT_HANDLER, 64, &bufspace, opt_b,
{
"Max buffer space per file (K): ",
"Max buffer space per file: %dK",
NULL
}
},
{ 'B', &B__optname,
BOOL, OPT_ON, &autobuf, NULL,
{
"Don't automatically allocate buffers",
"Automatically allocate buffers when needed",
NULL
}
},
{ 'c', &c_optname,
TRIPLE, OPT_OFF, &top_scroll, NULL,
{
"Repaint by scrolling from bottom of screen",
"Repaint by painting from top of screen",
"Repaint by painting from top of screen"
}
},
{ 'd', &d_optname,
BOOL|NO_TOGGLE, OPT_OFF, &know_dumb, NULL,
{
"Assume intelligent terminal",
"Assume dumb terminal",
NULL
}
},
{ 'D', &D__optname,
STRING|REPAINT|NO_QUERY, 0, NULL, opt_D,
{
"color desc: ",
NULL,
NULL
}
},
{ 'e', &e_optname,
TRIPLE, OPT_OFF, &quit_at_eof, NULL,
{
"Don't quit at end-of-file",
"Quit at end-of-file",
"Quit immediately at end-of-file"
}
},
{ 'f', &f_optname,
BOOL, OPT_OFF, &force_open, NULL,
{
"Open only regular files",
"Open even non-regular files",
NULL
}
},
{ 'F', &F__optname,
BOOL, OPT_OFF, &quit_if_one_screen, NULL,
{
"Don't quit if end-of-file on first screen",
"Quit if end-of-file on first screen",
NULL
}
},
#if HILITE_SEARCH
{ 'g', &g_optname,
TRIPLE|HL_REPAINT, OPT_ONPLUS, &hilite_search, NULL,
{
"Don't highlight search matches",
"Highlight matches for previous search only",
"Highlight all matches for previous search pattern",
}
},
#endif
{ 'h', &h_optname,
NUMBER, -1, &back_scroll, NULL,
{
"Backwards scroll limit: ",
"Backwards scroll limit is %d lines",
NULL
}
},
{ 'i', &i_optname,
TRIPLE|HL_REPAINT, OPT_OFF, &caseless, opt_i,
{
"Case is significant in searches",
"Ignore case in searches",
"Ignore case in searches and in patterns"
}
},
{ 'j', &j_optname,
STRING, 0, NULL, opt_j,
{
"Target line: ",
"0123456789.-",
NULL
}
},
{ 'J', &J__optname,
BOOL|REPAINT, OPT_OFF, &status_col, NULL,
{
"Don't display a status column",
"Display a status column",
NULL
}
},
#if USERFILE
{ 'k', &k_optname,
STRING|NO_TOGGLE|NO_QUERY, 0, NULL, opt_k,
{ NULL, NULL, NULL }
},
#if HAVE_LESSKEYSRC
{ OLETTER_NONE, &ks_optname,
STRING|NO_TOGGLE|NO_QUERY, 0, NULL, opt_ks,
{ NULL, NULL, NULL }
},
#endif
#endif
{ 'K', &K__optname,
BOOL, OPT_OFF, &quit_on_intr, NULL,
{
"Interrupt (ctrl-C) returns to prompt",
"Interrupt (ctrl-C) exits less",
NULL
}
},
{ 'L', &L__optname,
BOOL, OPT_ON, &use_lessopen, NULL,
{
"Don't use the LESSOPEN filter",
"Use the LESSOPEN filter",
NULL
}
},
{ 'm', &m_optname,
TRIPLE, OPT_OFF, &pr_type, NULL,
{
"Short prompt",
"Medium prompt",
"Long prompt"
}
},
{ 'n', &n_optname,
TRIPLE|REPAINT, OPT_ON, &linenums, NULL,
{
"Don't use line numbers",
"Use line numbers",
"Constantly display line numbers"
}
},
#if LOGFILE
{ 'o', &o_optname,
STRING, 0, NULL, opt_o,
{ "log file: ", NULL, NULL }
},
{ 'O', &O__optname,
STRING, 0, NULL, opt__O,
{ "Log file: ", NULL, NULL }
},
#endif
{ 'p', &p_optname,
STRING|NO_TOGGLE|NO_QUERY, 0, NULL, opt_p,
{ NULL, NULL, NULL }
},
{ 'P', &P__optname,
STRING, 0, NULL, opt__P,
{ "prompt: ", NULL, NULL }
},
{ 'q', &q_optname,
TRIPLE, OPT_OFF, &quiet, NULL,
{
"Ring the bell for errors AND at eof/bof",
"Ring the bell for errors but not at eof/bof",
"Never ring the bell"
}
},
{ 'r', &r_optname,
TRIPLE|REPAINT, OPT_OFF, &ctldisp, NULL,
{
"Display control characters as ^X",
"Display control characters directly (not recommended)",
"Display ANSI sequences directly, other control characters as ^X"
}
},
{ 's', &s_optname,
BOOL|REPAINT, OPT_OFF, &squeeze, NULL,
{
"Display all blank lines",
"Squeeze multiple blank lines",
NULL
}
},
{ 'S', &S__optname,
BOOL|REPAINT, OPT_OFF, &chopline, NULL,
{
"Fold long lines",
"Chop long lines",
NULL
}
},
#if TAGS
{ 't', &t_optname,
STRING|NO_QUERY, 0, NULL, opt_t,
{ "tag: ", NULL, NULL }
},
{ 'T', &T__optname,
STRING, 0, NULL, opt__T,
{ "tags file: ", NULL, NULL }
},
#endif
{ 'u', &u_optname,
TRIPLE|REPAINT, OPT_OFF, &bs_mode, NULL,
{
"Display underlined text in underline mode",
"Backspaces cause overstrike",
"Print backspace as ^H"
}
},
{ 'V', &V__optname,
NOVAR, 0, NULL, opt__V,
{ NULL, NULL, NULL }
},
{ 'w', &w_optname,
TRIPLE|REPAINT, OPT_OFF, &show_attn, NULL,
{
"Don't highlight first unread line",
"Highlight first unread line after forward-screen",
"Highlight first unread line after any forward movement",
}
},
{ 'x', &x_optname,
STRING|REPAINT, 0, NULL, opt_x,
{
"Tab stops: ",
"0123456789,",
NULL
}
},
{ 'X', &X__optname,
BOOL|NO_TOGGLE, OPT_OFF, &no_init, NULL,
{
"Send init/deinit strings to terminal",
"Don't use init/deinit strings",
NULL
}
},
{ 'y', &y_optname,
NUMBER, -1, &forw_scroll, NULL,
{
"Forward scroll limit: ",
"Forward scroll limit is %d lines",
NULL
}
},
{ 'z', &z_optname,
NUMBER, -1, &swindow, NULL,
{
"Scroll window size: ",
"Scroll window size is %d lines",
NULL
}
},
{ '"', &quote_optname,
STRING, 0, NULL, opt_quote,
{ "quotes: ", NULL, NULL }
},
{ '~', &tilde_optname,
BOOL|REPAINT, OPT_ON, &twiddle, NULL,
{
"Don't show tildes after end of file",
"Show tildes after end of file",
NULL
}
},
{ '?', &query_optname,
NOVAR, 0, NULL, opt_query,
{ NULL, NULL, NULL }
},
{ '#', &pound_optname,
STRING, 0, NULL, opt_shift,
{
"Horizontal shift: ",
"0123456789.",
NULL
}
},
{ OLETTER_NONE, &keypad_optname,
BOOL|NO_TOGGLE, OPT_OFF, &no_keypad, NULL,
{
"Use keypad mode",
"Don't use keypad mode",
NULL
}
},
{ OLETTER_NONE, &oldbot_optname,
BOOL, OPT_OFF, &oldbot, NULL,
{
"Use new bottom of screen behavior",
"Use old bottom of screen behavior",
NULL
}
},
{ OLETTER_NONE, &follow_optname,
BOOL, FOLLOW_DESC, &follow_mode, NULL,
{
"F command follows file descriptor",
"F command follows file name",
NULL
}
},
{ OLETTER_NONE, &use_backslash_optname,
BOOL, OPT_OFF, &opt_use_backslash, NULL,
{
"Use backslash escaping in command line parameters",
"Don't use backslash escaping in command line parameters",
NULL
}
},
{ OLETTER_NONE, &rscroll_optname,
STRING|REPAINT|INIT_HANDLER, 0, NULL, opt_rscroll,
{ "right scroll character: ", NULL, NULL }
},
{ OLETTER_NONE, &nohistdups_optname,
BOOL, OPT_OFF, &no_hist_dups, NULL,
{
"Allow duplicates in history list",
"Remove duplicates from history list",
NULL
}
},
{ OLETTER_NONE, &mousecap_optname,
TRIPLE, OPT_OFF, &mousecap, opt_mousecap,
{
"Ignore mouse input",
"Use the mouse for scrolling",
"Use the mouse for scrolling (reverse)"
}
},
{ OLETTER_NONE, &wheel_lines_optname,
NUMBER|INIT_HANDLER, 0, &wheel_lines, opt_wheel_lines,
{
"Lines to scroll on mouse wheel: ",
"Scroll %d line(s) on mouse wheel",
NULL
}
},
{ OLETTER_NONE, &perma_marks_optname,
BOOL, OPT_OFF, &perma_marks, NULL,
{
"Don't save marks in history file",
"Save marks in history file",
NULL
}
},
{ OLETTER_NONE, &linenum_width_optname,
NUMBER|REPAINT, MIN_LINENUM_WIDTH, &linenum_width, opt_linenum_width,
{
"Line number width: ",
"Line number width is %d chars",
NULL
}
},
{ OLETTER_NONE, &status_col_width_optname,
NUMBER|REPAINT, 2, &status_col_width, opt_status_col_width,
{
"Status column width: ",
"Status column width is %d chars",
NULL
}
},
{ OLETTER_NONE, &incr_search_optname,
BOOL, OPT_OFF, &incr_search, NULL,
{
"Incremental search is off",
"Incremental search is on",
NULL
}
},
{ OLETTER_NONE, &use_color_optname,
BOOL|REPAINT, OPT_OFF, &use_color, NULL,
{
"Don't use color",
"Use color",
NULL
}
},
{ OLETTER_NONE, &want_filesize_optname,
BOOL|REPAINT, OPT_OFF, &want_filesize, opt_filesize,
{
"Don't get size of each file",
"Get size of each file",
NULL
}
},
#if LESSTEST
{ OLETTER_NONE, &ttyin_name_optname,
STRING|NO_TOGGLE, 0, NULL, opt_ttyin_name,
{
NULL,
NULL,
NULL
}
},
{ OLETTER_NONE, &rstat_optname,
STRING|NO_TOGGLE, 0, NULL, opt_rstat,
{
NULL,
NULL,
NULL
}
},
#endif
{ '\0', NULL, NOVAR, 0, NULL, NULL, { NULL, NULL, NULL } }
};





public void
init_option(VOID_PARAM)
{
struct loption *o;
char *p;

p = lgetenv("LESS_IS_MORE");
if (!isnullenv(p))
less_is_more = 1;

for (o = option; o->oletter != '\0'; o++)
{



if (o->ovar != NULL)
*(o->ovar) = o->odefault;
if (o->otype & INIT_HANDLER)
(*(o->ofunc))(INIT, (char *) NULL);
}
}




public struct loption *
findopt(c)
int c;
{
struct loption *o;

for (o = option; o->oletter != '\0'; o++)
{
if (o->oletter == c)
return (o);
if ((o->otype & TRIPLE) && ASCII_TO_UPPER(o->oletter) == c)
return (o);
}
return (NULL);
}




static int
is_optchar(c)
char c;
{
if (ASCII_IS_UPPER(c))
return 1;
if (ASCII_IS_LOWER(c))
return 1;
if (c == '-')
return 1;
return 0;
}







public struct loption *
findopt_name(p_optname, p_oname, p_err)
char **p_optname;
char **p_oname;
int *p_err;
{
char *optname = *p_optname;
struct loption *o;
struct optname *oname;
int len;
int uppercase;
struct loption *maxo = NULL;
struct optname *maxoname = NULL;
int maxlen = 0;
int ambig = 0;
int exact = 0;




for (o = option; o->oletter != '\0'; o++)
{



for (oname = o->onames; oname != NULL; oname = oname->onext)
{





for (uppercase = 0; uppercase <= 1; uppercase++)
{
len = sprefix(optname, oname->oname, uppercase);
if (len <= 0 || is_optchar(optname[len]))
{



continue;
}
if (!exact && len == maxlen)





ambig = 1;
else if (len > maxlen)
{




maxo = o;
maxoname = oname;
maxlen = len;
ambig = 0;
exact = (len == (int)strlen(oname->oname));
}
if (!(o->otype & TRIPLE))
break;
}
}
}
if (ambig)
{



if (p_err != NULL)
*p_err = OPT_AMBIG;
return (NULL);
}
*p_optname = optname + maxlen;
if (p_oname != NULL)
*p_oname = maxoname == NULL ? NULL : maxoname->oname;
return (maxo);
}
