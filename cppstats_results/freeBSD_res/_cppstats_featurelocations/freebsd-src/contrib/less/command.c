














#include "less.h"
#if MSDOS_COMPILER==WIN32C
#include <windows.h>
#endif
#include "position.h"
#include "option.h"
#include "cmd.h"

extern int erase_char, erase2_char, kill_char;
extern int sigs;
extern int quit_if_one_screen;
extern int squished;
extern int sc_width;
extern int sc_height;
extern char *kent;
extern int swindow;
extern int jump_sline;
extern int quitting;
extern int wscroll;
extern int top_scroll;
extern int ignore_eoi;
extern int secure;
extern int hshift;
extern int bs_mode;
extern int show_attn;
extern int less_is_more;
extern int status_col;
extern POSITION highest_hilite;
extern POSITION start_attnpos;
extern POSITION end_attnpos;
extern char *every_first_cmd;
extern char version[];
extern struct scrpos initial_scrpos;
extern IFILE curr_ifile;
extern void *ml_search;
extern void *ml_examine;
extern int wheel_lines;
#if SHELL_ESCAPE || PIPEC
extern void *ml_shell;
#endif
#if EDITOR
extern char *editor;
extern char *editproto;
#endif
extern int screen_trashed;
extern int shift_count;
extern int oldbot;
extern int forw_prompt;
extern int incr_search;
#if MSDOS_COMPILER==WIN32C
extern int utf_mode;
#endif

#if SHELL_ESCAPE
static char *shellcmd = NULL;
#endif
static int mca;
static int search_type;
static LINENUM number;
static long fraction;
static struct loption *curropt;
static int opt_lower;
static int optflag;
static int optgetname;
static POSITION bottompos;
static int save_hshift;
static int save_bs_mode;
#if PIPEC
static char pipec;
#endif


struct ungot {
struct ungot *ug_next;
LWCHAR ug_char;
};
static struct ungot* ungot = NULL;

static void multi_search LESSPARAMS((char *pattern, int n, int silent));






static void
cmd_exec(VOID_PARAM)
{
clear_attn();
clear_bot();
flush();
}




static void
set_mca(action)
int action;
{
mca = action;
clear_bot();
clear_cmd();
}




static void
clear_mca(VOID_PARAM)
{
if (mca == 0)
return;
mca = 0;
}




static void
start_mca(action, prompt, mlist, cmdflags)
int action;
constant char *prompt;
void *mlist;
int cmdflags;
{
set_mca(action);
cmd_putstr(prompt);
set_mlist(mlist, cmdflags);
}

public int
in_mca(VOID_PARAM)
{
return (mca != 0 && mca != A_PREFIX);
}




static void
mca_search(VOID_PARAM)
{
#if HILITE_SEARCH
if (search_type & SRCH_FILTER)
set_mca(A_FILTER);
else
#endif
if (search_type & SRCH_FORW)
set_mca(A_F_SEARCH);
else
set_mca(A_B_SEARCH);

if (search_type & SRCH_NO_MATCH)
cmd_putstr("Non-match ");
if (search_type & SRCH_FIRST_FILE)
cmd_putstr("First-file ");
if (search_type & SRCH_PAST_EOF)
cmd_putstr("EOF-ignore ");
if (search_type & SRCH_NO_MOVE)
cmd_putstr("Keep-pos ");
if (search_type & SRCH_NO_REGEX)
cmd_putstr("Regex-off ");
if (search_type & SRCH_WRAP)
cmd_putstr("Wrap ");

#if HILITE_SEARCH
if (search_type & SRCH_FILTER)
cmd_putstr("&/");
else
#endif
if (search_type & SRCH_FORW)
cmd_putstr("/");
else
cmd_putstr("?");
forw_prompt = 0;
set_mlist(ml_search, 0);
}




static void
mca_opt_toggle(VOID_PARAM)
{
int no_prompt;
int flag;
char *dash;

no_prompt = (optflag & OPT_NO_PROMPT);
flag = (optflag & ~OPT_NO_PROMPT);
dash = (flag == OPT_NO_TOGGLE) ? "_" : "-";

set_mca(A_OPT_TOGGLE);
cmd_putstr(dash);
if (optgetname)
cmd_putstr(dash);
if (no_prompt)
cmd_putstr("(P)");
switch (flag)
{
case OPT_UNSET:
cmd_putstr("+");
break;
case OPT_SET:
cmd_putstr("!");
break;
}
forw_prompt = 0;
set_mlist(NULL, 0);
}




static void
exec_mca(VOID_PARAM)
{
char *cbuf;

cmd_exec();
cbuf = get_cmdbuf();

switch (mca)
{
case A_F_SEARCH:
case A_B_SEARCH:
multi_search(cbuf, (int) number, 0);
break;
#if HILITE_SEARCH
case A_FILTER:
search_type ^= SRCH_NO_MATCH;
set_filter_pattern(cbuf, search_type);
break;
#endif
case A_FIRSTCMD:



while (*cbuf == '+' || *cbuf == ' ')
cbuf++;
if (every_first_cmd != NULL)
free(every_first_cmd);
if (*cbuf == '\0')
every_first_cmd = NULL;
else
every_first_cmd = save(cbuf);
break;
case A_OPT_TOGGLE:
toggle_option(curropt, opt_lower, cbuf, optflag);
curropt = NULL;
break;
case A_F_BRACKET:
match_brac(cbuf[0], cbuf[1], 1, (int) number);
break;
case A_B_BRACKET:
match_brac(cbuf[1], cbuf[0], 0, (int) number);
break;
#if EXAMINE
case A_EXAMINE:
if (secure)
break;
edit_list(cbuf);
#if TAGS

cleantags();
#endif
break;
#endif
#if SHELL_ESCAPE
case A_SHELL:





if (*cbuf != '!')
{
if (shellcmd != NULL)
free(shellcmd);
shellcmd = fexpand(cbuf);
}

if (secure)
break;
if (shellcmd == NULL)
lsystem("", "!done");
else
lsystem(shellcmd, "!done");
break;
#endif
#if PIPEC
case A_PIPE:
if (secure)
break;
(void) pipe_mark(pipec, cbuf);
error("|done", NULL_PARG);
break;
#endif
}
}




static int
is_erase_char(c)
int c;
{
return (c == erase_char || c == erase2_char || c == kill_char);
}




static int
is_newline_char(c)
int c;
{
return (c == '\n' || c == '\r');
}




static int
mca_opt_first_char(c)
int c;
{
int flag = (optflag & ~OPT_NO_PROMPT);
if (flag == OPT_NO_TOGGLE)
{
switch (c)
{
case '_':

optgetname = TRUE;
mca_opt_toggle();
return (MCA_MORE);
}
} else
{
switch (c)
{
case '+':

optflag = (flag == OPT_UNSET) ?
OPT_TOGGLE : OPT_UNSET;
mca_opt_toggle();
return (MCA_MORE);
case '!':

optflag = (flag == OPT_SET) ?
OPT_TOGGLE : OPT_SET;
mca_opt_toggle();
return (MCA_MORE);
case CONTROL('P'):
optflag ^= OPT_NO_PROMPT;
mca_opt_toggle();
return (MCA_MORE);
case '-':

optgetname = TRUE;
mca_opt_toggle();
return (MCA_MORE);
}
}

return (NO_MCA);
}







static int
mca_opt_nonfirst_char(c)
int c;
{
char *p;
char *oname;
int err;

if (curropt != NULL)
{




if (is_erase_char(c))
return (MCA_DONE);
return (MCA_MORE);
}




if (cmd_char(c) == CC_QUIT)
return (MCA_DONE);
p = get_cmdbuf();
opt_lower = ASCII_IS_LOWER(p[0]);
err = 0;
curropt = findopt_name(&p, &oname, &err);
if (curropt != NULL)
{





cmd_reset();
mca_opt_toggle();
for (p = oname; *p != '\0'; p++)
{
c = *p;
if (!opt_lower && ASCII_IS_LOWER(c))
c = ASCII_TO_UPPER(c);
if (cmd_char(c) != CC_OK)
return (MCA_DONE);
}
} else if (err != OPT_AMBIG)
{
bell();
}
return (MCA_MORE);
}




static int
mca_opt_char(c)
int c;
{
PARG parg;






if (curropt == NULL && len_cmdbuf() == 0)
{
int ret = mca_opt_first_char(c);
if (ret != NO_MCA)
return (ret);
}
if (optgetname)
{

if (!is_newline_char(c))
return (mca_opt_nonfirst_char(c));
if (curropt == NULL)
{
parg.p_string = get_cmdbuf();
error("There is no --%s option", &parg);
return (MCA_DONE);
}
optgetname = FALSE;
cmd_reset();
} else
{
if (is_erase_char(c))
return (NO_MCA);
if (curropt != NULL)

return (NO_MCA);
curropt = findopt(c);
if (curropt == NULL)
{
parg.p_string = propt(c);
error("There is no %s option", &parg);
return (MCA_DONE);
}
opt_lower = ASCII_IS_LOWER(c);
}





if ((optflag & ~OPT_NO_PROMPT) != OPT_TOGGLE ||
!opt_has_param(curropt))
{
toggle_option(curropt, opt_lower, "", optflag);
return (MCA_DONE);
}



start_mca(A_OPT_TOGGLE, opt_prompt(curropt), (void*)NULL, 0);
return (MCA_MORE);
}




static int
mca_search_char(c)
int c;
{
int flag = 0;








if (len_cmdbuf() > 0)
return (NO_MCA);

switch (c)
{
case '*':
if (less_is_more)
break;
case CONTROL('E'):
if (mca != A_FILTER)
flag = SRCH_PAST_EOF;
break;
case '@':
if (less_is_more)
break;
case CONTROL('F'):
if (mca != A_FILTER)
flag = SRCH_FIRST_FILE;
break;
case CONTROL('K'):
if (mca != A_FILTER)
flag = SRCH_NO_MOVE;
break;
case CONTROL('W'):
if (mca != A_FILTER)
flag = SRCH_WRAP;
break;
case CONTROL('R'):
flag = SRCH_NO_REGEX;
break;
case CONTROL('N'):
case '!':
flag = SRCH_NO_MATCH;
break;
}

if (flag != 0)
{

search_type ^= flag | (search_type & (SRCH_PAST_EOF|SRCH_WRAP));
mca_search();
return (MCA_MORE);
}
return (NO_MCA);
}




static int
mca_char(c)
int c;
{
int ret;

switch (mca)
{
case 0:



return (NO_MCA);

case A_PREFIX:






return (NO_MCA);

case A_DIGIT:




if ((c >= '0' && c <= '9') || c == '.')
break;
switch (editchar(c, ECF_PEEK|ECF_NOHISTORY|ECF_NOCOMPLETE|ECF_NORIGHTLEFT))
{
case A_NOACTION:



return (MCA_MORE);
case A_INVALID:





number = cmd_int(&fraction);
clear_mca();
cmd_accept();
return (NO_MCA);
}
break;

case A_OPT_TOGGLE:
ret = mca_opt_char(c);
if (ret != NO_MCA)
return (ret);
break;

case A_F_SEARCH:
case A_B_SEARCH:
case A_FILTER:
ret = mca_search_char(c);
if (ret != NO_MCA)
return (ret);
break;

default:

break;
}




if (is_newline_char(c))
{



exec_mca();
return (MCA_DONE);
}




if (cmd_char(c) == CC_QUIT)



return (MCA_DONE);

switch (mca)
{
case A_F_BRACKET:
case A_B_BRACKET:
if (len_cmdbuf() >= 2)
{





exec_mca();
return (MCA_DONE);
}
break;
case A_F_SEARCH:
case A_B_SEARCH:
if (incr_search)
{

int st = (search_type & (SRCH_FORW|SRCH_BACK|SRCH_NO_MATCH|SRCH_NO_REGEX|SRCH_NO_MOVE|SRCH_WRAP));
char *pattern = get_cmdbuf();
cmd_exec();
if (*pattern == '\0')
{

undo_search(1);
} else
{
if (search(st | SRCH_INCR, pattern, 1) != 0)

undo_search(1);
}

mca_search();
cmd_repaint(NULL);
}
break;
}




return (MCA_MORE);
}




static void
clear_buffers(VOID_PARAM)
{
if (!(ch_getflags() & CH_CANSEEK))
return;
ch_flush();
clr_linenum();
#if HILITE_SEARCH
clr_hilite();
#endif
}




static void
make_display(VOID_PARAM)
{



if (empty_screen())
{
if (initial_scrpos.pos == NULL_POSITION)
jump_loc(ch_zero(), 1);
else
jump_loc(initial_scrpos.pos, initial_scrpos.ln);
} else if (screen_trashed)
{
int save_top_scroll = top_scroll;
int save_ignore_eoi = ignore_eoi;
top_scroll = 1;
ignore_eoi = 0;
if (screen_trashed == 2)
{


reopen_curr_ifile();
jump_forw();
}
repaint();
top_scroll = save_top_scroll;
ignore_eoi = save_ignore_eoi;
}
}




static void
prompt(VOID_PARAM)
{
constant char *p;

if (ungot != NULL && ungot->ug_char != CHAR_END_COMMAND)
{




return;
}




make_display();
bottompos = position(BOTTOM_PLUS_ONE);




if (get_quit_at_eof() == OPT_ONPLUS &&
eof_displayed() && !(ch_getflags() & CH_HELPFILE) &&
next_ifile(curr_ifile) == NULL_IFILE)
quit(QUIT_OK);




if (quit_if_one_screen &&
entire_file_displayed() && !(ch_getflags() & CH_HELPFILE) &&
next_ifile(curr_ifile) == NULL_IFILE)
quit(QUIT_OK);
quit_if_one_screen = FALSE;

#if MSDOS_COMPILER==WIN32C



if (!(ch_getflags() & CH_HELPFILE))
{
WCHAR w[MAX_PATH+16];
p = pr_expand("Less?f - %f.", 0);
MultiByteToWideChar(CP_ACP, 0, p, -1, w, sizeof(w)/sizeof(*w));
SetConsoleTitleW(w);
}
#endif














if (!forw_prompt)
clear_bot();
clear_cmd();
forw_prompt = 0;
p = pr_string();
#if HILITE_SEARCH
if (is_filtering())
putstr("& ");
#endif
if (p == NULL || *p == '\0')
{
at_enter(AT_NORMAL|AT_COLOR_PROMPT);
putchr(':');
at_exit();
} else
{
#if MSDOS_COMPILER==WIN32C
WCHAR w[MAX_PATH*2];
char a[MAX_PATH*2];
MultiByteToWideChar(CP_ACP, 0, p, -1, w, sizeof(w)/sizeof(*w));
WideCharToMultiByte(utf_mode ? CP_UTF8 : GetConsoleOutputCP(),
0, w, -1, a, sizeof(a), NULL, NULL);
p = a;
#endif
at_enter(AT_STANDOUT|AT_COLOR_PROMPT);
putstr(p);
at_exit();
}
clear_eol();
}




public void
dispversion(VOID_PARAM)
{
PARG parg;

parg.p_string = version;
error("less %s", &parg);
}




static LWCHAR
getcc_end_command(VOID_PARAM)
{
switch (mca)
{
case A_DIGIT:

return ('g');
case A_F_SEARCH:
case A_B_SEARCH:

return ('\n');
default:

return ((ungot == NULL) ? getchr() : 0);
}
}







static LWCHAR
getccu(VOID_PARAM)
{
LWCHAR c = 0;
while (c == 0)
{
if (ungot == NULL)
{


c = getchr();
} else
{


struct ungot *ug = ungot;
c = ug->ug_char;
ungot = ug->ug_next;
free(ug);

if (c == CHAR_END_COMMAND)
c = getcc_end_command();
}
}
return (c);
}





static LWCHAR
getcc_repl(orig, repl, gr_getc, gr_ungetc)
char const* orig;
char const* repl;
LWCHAR (*gr_getc)(VOID_PARAM);
void (*gr_ungetc)(LWCHAR);
{
LWCHAR c;
LWCHAR keys[16];
int ki = 0;

c = (*gr_getc)();
if (orig == NULL || orig[0] == '\0')
return c;
for (;;)
{
keys[ki] = c;
if (c != orig[ki] || ki >= sizeof(keys)-1)
{



while (ki > 0)
(*gr_ungetc)(keys[ki--]);
return keys[0];
}
if (orig[++ki] == '\0')
{


ki = strlen(repl)-1;
while (ki > 0)
(*gr_ungetc)(repl[ki--]);
return repl[0];
}


c = (*gr_getc)();
}
}




public int
getcc(VOID_PARAM)
{

return getcc_repl(kent, "\n", getccu, ungetcc);
}





public void
ungetcc(c)
LWCHAR c;
{
struct ungot *ug = (struct ungot *) ecalloc(1, sizeof(struct ungot));

ug->ug_char = c;
ug->ug_next = ungot;
ungot = ug;
}





public void
ungetcc_back(c)
LWCHAR c;
{
struct ungot *ug = (struct ungot *) ecalloc(1, sizeof(struct ungot));
ug->ug_char = c;
ug->ug_next = NULL;
if (ungot == NULL)
ungot = ug;
else
{
struct ungot *pu;
for (pu = ungot; pu->ug_next != NULL; pu = pu->ug_next)
continue;
pu->ug_next = ug;
}
}





public void
ungetsc(s)
char *s;
{
while (*s != '\0')
ungetcc_back(*s++);
}




public LWCHAR
peekcc(VOID_PARAM)
{
LWCHAR c = getcc();
ungetcc(c);
return c;
}






static void
multi_search(pattern, n, silent)
char *pattern;
int n;
int silent;
{
int nomore;
IFILE save_ifile;
int changed_file;

changed_file = 0;
save_ifile = save_curr_ifile();

if (search_type & SRCH_FIRST_FILE)
{




if (search_type & SRCH_FORW)
nomore = edit_first();
else
nomore = edit_last();
if (nomore)
{
unsave_ifile(save_ifile);
return;
}
changed_file = 1;
search_type &= ~SRCH_FIRST_FILE;
}

for (;;)
{
n = search(search_type, pattern, n);





search_type &= ~SRCH_NO_MOVE;
if (n == 0)
{



unsave_ifile(save_ifile);
return;
}

if (n < 0)




break;

if ((search_type & SRCH_PAST_EOF) == 0)




break;



if (search_type & SRCH_FORW)
nomore = edit_next(1);
else
nomore = edit_prev(1);
if (nomore)
break;
changed_file = 1;
}





if (n > 0 && !silent)
error("Pattern not found", NULL_PARG);

if (changed_file)
{



reedit_ifile(save_ifile);
} else
{
unsave_ifile(save_ifile);
}
}




static int
forw_loop(until_hilite)
int until_hilite;
{
POSITION curr_len;

if (ch_getflags() & CH_HELPFILE)
return (A_NOACTION);

cmd_exec();
jump_forw_buffered();
curr_len = ch_length();
highest_hilite = until_hilite ? curr_len : NULL_POSITION;
ignore_eoi = 1;
while (!sigs)
{
if (until_hilite && highest_hilite > curr_len)
{
bell();
break;
}
make_display();
forward(1, 0, 0);
}
ignore_eoi = 0;
ch_set_eof();





if (sigs && !ABORT_SIGS())
return (until_hilite ? A_F_UNTIL_HILITE : A_F_FOREVER);

return (A_NOACTION);
}





public void
commands(VOID_PARAM)
{
int c;
int action;
char *cbuf;
int newaction;
int save_jump_sline;
int save_search_type;
char *extra;
char tbuf[2];
PARG parg;
IFILE old_ifile;
IFILE new_ifile;
char *tagfile;

search_type = SRCH_FORW;
wscroll = (sc_height + 1) / 2;
newaction = A_NOACTION;

for (;;)
{
clear_mca();
cmd_accept();
number = 0;
curropt = NULL;




if (sigs)
{
psignals();
if (quitting)
quit(QUIT_SAVED_STATUS);
}





check_winch();




cmd_reset();
prompt();
if (sigs)
continue;
if (newaction == A_NOACTION)
c = getcc();

again:
if (sigs)
continue;

if (newaction != A_NOACTION)
{
action = newaction;
newaction = A_NOACTION;
} else
{





if (mca)
switch (mca_char(c))
{
case MCA_MORE:



c = getcc();
goto again;
case MCA_DONE:




continue;
case NO_MCA:




break;
}




if (mca)
{







if (cmd_char(c) == CC_QUIT || len_cmdbuf() == 0)
continue;
cbuf = get_cmdbuf();
} else
{








tbuf[0] = c;
tbuf[1] = '\0';
cbuf = tbuf;
}
extra = NULL;
action = fcmd_decode(cbuf, &extra);




if (extra != NULL)
ungetsc(extra);
}





if (action != A_PREFIX)
cmd_reset();

switch (action)
{
case A_DIGIT:



start_mca(A_DIGIT, ":", (void*)NULL, CF_QUIT_ON_ERASE);
goto again;

case A_F_WINDOW:



if (number > 0)
swindow = (int) number;

case A_F_SCREEN:



if (number <= 0)
number = get_swindow();
cmd_exec();
if (show_attn)
set_attnpos(bottompos);
forward((int) number, 0, 1);
break;

case A_B_WINDOW:



if (number > 0)
swindow = (int) number;

case A_B_SCREEN:



if (number <= 0)
number = get_swindow();
cmd_exec();
backward((int) number, 0, 1);
break;

case A_F_LINE:



if (number <= 0)
number = 1;
cmd_exec();
if (show_attn == OPT_ONPLUS && number > 1)
set_attnpos(bottompos);
forward((int) number, 0, 0);
break;

case A_B_LINE:



if (number <= 0)
number = 1;
cmd_exec();
backward((int) number, 0, 0);
break;

case A_F_MOUSE:



cmd_exec();
forward(wheel_lines, 0, 0);
break;

case A_B_MOUSE:



cmd_exec();
backward(wheel_lines, 0, 0);
break;

case A_FF_LINE:



if (number <= 0)
number = 1;
cmd_exec();
if (show_attn == OPT_ONPLUS && number > 1)
set_attnpos(bottompos);
forward((int) number, 1, 0);
break;

case A_BF_LINE:



if (number <= 0)
number = 1;
cmd_exec();
backward((int) number, 1, 0);
break;

case A_FF_SCREEN:



if (number <= 0)
number = get_swindow();
cmd_exec();
if (show_attn == OPT_ONPLUS)
set_attnpos(bottompos);
forward((int) number, 1, 0);
break;

case A_F_FOREVER:



if (show_attn)
set_attnpos(bottompos);
newaction = forw_loop(0);
break;

case A_F_UNTIL_HILITE:
newaction = forw_loop(1);
break;

case A_F_SCROLL:




if (number > 0)
wscroll = (int) number;
cmd_exec();
if (show_attn == OPT_ONPLUS)
set_attnpos(bottompos);
forward(wscroll, 0, 0);
break;

case A_B_SCROLL:




if (number > 0)
wscroll = (int) number;
cmd_exec();
backward(wscroll, 0, 0);
break;

case A_FREPAINT:




clear_buffers();

case A_REPAINT:



cmd_exec();
repaint();
break;

case A_GOLINE:





save_jump_sline = jump_sline;
if (number <= 0)
{
number = 1;
jump_sline = 0;
}
cmd_exec();
jump_back(number);
jump_sline = save_jump_sline;
break;

case A_PERCENT:



if (number < 0)
{
number = 0;
fraction = 0;
}
if (number > 100 || (number == 100 && fraction != 0))
{
number = 100;
fraction = 0;
}
cmd_exec();
jump_percent((int) number, fraction);
break;

case A_GOEND:



cmd_exec();
if (number <= 0)
jump_forw();
else
jump_back(number);
break;

case A_GOEND_BUF:



cmd_exec();
if (number <= 0)
jump_forw_buffered();
else
jump_back(number);
break;

case A_GOPOS:



cmd_exec();
if (number < 0)
number = 0;
jump_line_loc((POSITION) number, jump_sline);
break;

case A_STAT:



if (ch_getflags() & CH_HELPFILE)
break;
cmd_exec();
parg.p_string = eq_message();
error("%s", &parg);
break;

case A_VERSION:



cmd_exec();
dispversion();
break;

case A_QUIT:



if (curr_ifile != NULL_IFILE &&
ch_getflags() & CH_HELPFILE)
{





hshift = save_hshift;
bs_mode = save_bs_mode;
if (edit_prev(1) == 0)
break;
}
if (extra != NULL)
quit(*extra);
quit(QUIT_OK);
break;




#define DO_SEARCH() if (number <= 0) number = 1; mca_search(); cmd_exec(); multi_search((char *)NULL, (int) number, 0);






case A_F_SEARCH:




search_type = SRCH_FORW;
if (number <= 0)
number = 1;
mca_search();
c = getcc();
goto again;

case A_B_SEARCH:




search_type = SRCH_BACK;
if (number <= 0)
number = 1;
mca_search();
c = getcc();
goto again;

case A_FILTER:
#if HILITE_SEARCH
search_type = SRCH_FORW | SRCH_FILTER;
mca_search();
c = getcc();
goto again;
#else
error("Command not available", NULL_PARG);
break;
#endif

case A_AGAIN_SEARCH:



DO_SEARCH();
break;

case A_T_AGAIN_SEARCH:



search_type |= SRCH_PAST_EOF;
DO_SEARCH();
break;

case A_REVERSE_SEARCH:



save_search_type = search_type;
search_type = SRCH_REVERSE(search_type);
DO_SEARCH();
search_type = save_search_type;
break;

case A_T_REVERSE_SEARCH:




save_search_type = search_type;
search_type = SRCH_REVERSE(search_type);
search_type |= SRCH_PAST_EOF;
DO_SEARCH();
search_type = save_search_type;
break;

case A_UNDO_SEARCH:
case A_CLR_SEARCH:



undo_search(action == A_CLR_SEARCH);
break;

case A_HELP:



if (ch_getflags() & CH_HELPFILE)
break;
cmd_exec();
save_hshift = hshift;
hshift = 0;
save_bs_mode = bs_mode;
bs_mode = BS_SPECIAL;
(void) edit(FAKE_HELPFILE);
break;

case A_EXAMINE:



#if EXAMINE
if (!secure)
{
start_mca(A_EXAMINE, "Examine: ", ml_examine, 0);
c = getcc();
goto again;
}
#endif
error("Command not available", NULL_PARG);
break;

case A_VISUAL:



#if EDITOR
if (!secure)
{
if (ch_getflags() & CH_HELPFILE)
break;
if (strcmp(get_filename(curr_ifile), "-") == 0)
{
error("Cannot edit standard input", NULL_PARG);
break;
}
if (get_altfilename(curr_ifile) != NULL)
{
error("WARNING: This file was viewed via LESSOPEN",
NULL_PARG);
}
start_mca(A_SHELL, "!", ml_shell, 0);






make_display();
cmd_exec();
lsystem(pr_expand(editproto, 0), (char*)NULL);
break;
}
#endif
error("Command not available", NULL_PARG);
break;

case A_NEXT_FILE:



#if TAGS
if (ntags())
{
error("No next file", NULL_PARG);
break;
}
#endif
if (number <= 0)
number = 1;
if (edit_next((int) number))
{
if (get_quit_at_eof() && eof_displayed() &&
!(ch_getflags() & CH_HELPFILE))
quit(QUIT_OK);
parg.p_string = (number > 1) ? "(N-th) " : "";
error("No %snext file", &parg);
}
break;

case A_PREV_FILE:



#if TAGS
if (ntags())
{
error("No previous file", NULL_PARG);
break;
}
#endif
if (number <= 0)
number = 1;
if (edit_prev((int) number))
{
parg.p_string = (number > 1) ? "(N-th) " : "";
error("No %sprevious file", &parg);
}
break;

case A_NEXT_TAG:



#if TAGS
if (number <= 0)
number = 1;
tagfile = nexttag((int) number);
if (tagfile == NULL)
{
error("No next tag", NULL_PARG);
break;
}
cmd_exec();
if (edit(tagfile) == 0)
{
POSITION pos = tagsearch();
if (pos != NULL_POSITION)
jump_loc(pos, jump_sline);
}
#else
error("Command not available", NULL_PARG);
#endif
break;

case A_PREV_TAG:



#if TAGS
if (number <= 0)
number = 1;
tagfile = prevtag((int) number);
if (tagfile == NULL)
{
error("No previous tag", NULL_PARG);
break;
}
cmd_exec();
if (edit(tagfile) == 0)
{
POSITION pos = tagsearch();
if (pos != NULL_POSITION)
jump_loc(pos, jump_sline);
}
#else
error("Command not available", NULL_PARG);
#endif
break;

case A_INDEX_FILE:



if (number <= 0)
number = 1;
if (edit_index((int) number))
error("No such file", NULL_PARG);
break;

case A_REMOVE_FILE:



if (ch_getflags() & CH_HELPFILE)
break;
old_ifile = curr_ifile;
new_ifile = getoff_ifile(curr_ifile);
if (new_ifile == NULL_IFILE)
{
bell();
break;
}
if (edit_ifile(new_ifile) != 0)
{
reedit_ifile(old_ifile);
break;
}
del_ifile(old_ifile);
break;

case A_OPT_TOGGLE:



optflag = OPT_TOGGLE;
optgetname = FALSE;
mca_opt_toggle();
c = getcc();
cbuf = opt_toggle_disallowed(c);
if (cbuf != NULL)
{
error(cbuf, NULL_PARG);
break;
}
goto again;

case A_DISP_OPTION:



optflag = OPT_NO_TOGGLE;
optgetname = FALSE;
mca_opt_toggle();
c = getcc();
goto again;

case A_FIRSTCMD:



start_mca(A_FIRSTCMD, "+", (void*)NULL, 0);
c = getcc();
goto again;

case A_SHELL:



#if SHELL_ESCAPE
if (!secure)
{
start_mca(A_SHELL, "!", ml_shell, 0);
c = getcc();
goto again;
}
#endif
error("Command not available", NULL_PARG);
break;

case A_SETMARK:
case A_SETMARKBOT:



if (ch_getflags() & CH_HELPFILE)
break;
start_mca(A_SETMARK, "set mark: ", (void*)NULL, 0);
c = getcc();
if (is_erase_char(c) || is_newline_char(c))
break;
setmark(c, action == A_SETMARKBOT ? BOTTOM : TOP);
repaint();
break;

case A_CLRMARK:



start_mca(A_CLRMARK, "clear mark: ", (void*)NULL, 0);
c = getcc();
if (is_erase_char(c) || is_newline_char(c))
break;
clrmark(c);
repaint();
break;

case A_GOMARK:



start_mca(A_GOMARK, "goto mark: ", (void*)NULL, 0);
c = getcc();
if (is_erase_char(c) || is_newline_char(c))
break;
cmd_exec();
gomark(c);
break;

case A_PIPE:



#if PIPEC
if (!secure)
{
start_mca(A_PIPE, "|mark: ", (void*)NULL, 0);
c = getcc();
if (is_erase_char(c))
break;
if (is_newline_char(c))
c = '.';
if (badmark(c))
break;
pipec = c;
start_mca(A_PIPE, "!", ml_shell, 0);
c = getcc();
goto again;
}
#endif
error("Command not available", NULL_PARG);
break;

case A_B_BRACKET:
case A_F_BRACKET:
start_mca(action, "Brackets: ", (void*)NULL, 0);
c = getcc();
goto again;

case A_LSHIFT:



if (number > 0)
shift_count = number;
else
number = (shift_count > 0) ?
shift_count : sc_width / 2;
if (number > hshift)
number = hshift;
hshift -= number;
screen_trashed = 1;
break;

case A_RSHIFT:



if (number > 0)
shift_count = number;
else
number = (shift_count > 0) ?
shift_count : sc_width / 2;
hshift += number;
screen_trashed = 1;
break;

case A_LLSHIFT:



hshift = 0;
screen_trashed = 1;
break;

case A_RRSHIFT:



hshift = rrshift();
screen_trashed = 1;
break;

case A_PREFIX:





if (mca != A_PREFIX)
{
cmd_reset();
start_mca(A_PREFIX, " ", (void*)NULL,
CF_QUIT_ON_ERASE);
(void) cmd_char(c);
}
c = getcc();
goto again;

case A_NOACTION:
break;

default:
bell();
break;
}
}
}
