

























#include <dialog.h>
#include <dlg_keys.h>
#include <dlg_internals.h>

#include <sys/time.h>

#if defined(HAVE_SETLOCALE)
#include <locale.h>
#endif

#if defined(NEED_WCHAR_H)
#include <wchar.h>
#endif

#if defined(HAVE_SYS_PARAM_H)
#undef MIN
#undef MAX
#include <sys/param.h>
#endif

#if defined(NCURSES_VERSION)
#define CAN_KEEP_TITE 1
#elif defined(__NetBSD_Version__) && (__NetBSD_Version__ >= 800000000)
#define CAN_KEEP_TITE 1
#else
#define CAN_KEEP_TITE 0
#endif

#if CAN_KEEP_TITE
#if defined(NCURSES_VERSION) && defined(HAVE_NCURSESW_TERM_H)
#include <ncursesw/term.h>
#elif defined(NCURSES_VERSION) && defined(HAVE_NCURSES_TERM_H)
#include <ncurses/term.h>
#else
#include <term.h>
#endif
#endif

#if defined(HAVE_WCHGAT)
#if defined(NCURSES_VERSION_PATCH)
#if NCURSES_VERSION_PATCH >= 20060715
#define USE_WCHGAT 1
#else
#define USE_WCHGAT 0
#endif
#else
#define USE_WCHGAT 1
#endif
#else
#define USE_WCHGAT 0
#endif


DIALOG_STATE dialog_state;
DIALOG_VARS dialog_vars;

#if !(defined(HAVE_WGETPARENT) && defined(HAVE_WINDOW__PARENT))
#define NEED_WGETPARENT 1
#else
#undef NEED_WGETPARENT
#endif

#define concat(a,b) a##b

#if defined(HAVE_RC_FILE)
#define RC_DATA(name,comment) , #name "_color", comment " color"
#else
#define RC_DATA(name,comment)
#endif

#if defined(HAVE_COLOR)
#include <dlg_colors.h>
#if defined(HAVE_RC_FILE2)
#define COLOR_DATA(upr) , concat(DLGC_FG_,upr), concat(DLGC_BG_,upr), concat(DLGC_HL_,upr), concat(DLGC_UL_,upr), concat(DLGC_RV_,upr)





#else
#define COLOR_DATA(upr) , concat(DLGC_FG_,upr), concat(DLGC_BG_,upr), concat(DLGC_HL_,upr)



#endif
#else
#define COLOR_DATA(upr)
#endif

#define UseShadow(dw) ((dw) != 0 && (dw)->normal != 0 && (dw)->shadow != 0)





#define DATA(atr,upr,lwr,cmt) { atr COLOR_DATA(upr) RC_DATA(lwr,cmt) }

DIALOG_COLORS dlg_color_table[] =
{
DATA(A_NORMAL, SCREEN, screen, "Screen"),
DATA(A_NORMAL, SHADOW, shadow, "Shadow"),
DATA(A_REVERSE, DIALOG, dialog, "Dialog box"),
DATA(A_REVERSE, TITLE, title, "Dialog box title"),
DATA(A_REVERSE, BORDER, border, "Dialog box border"),
DATA(A_BOLD, BUTTON_ACTIVE, button_active, "Active button"),
DATA(A_DIM, BUTTON_INACTIVE, button_inactive, "Inactive button"),
DATA(A_UNDERLINE, BUTTON_KEY_ACTIVE, button_key_active, "Active button key"),
DATA(A_UNDERLINE, BUTTON_KEY_INACTIVE, button_key_inactive, "Inactive button key"),
DATA(A_NORMAL, BUTTON_LABEL_ACTIVE, button_label_active, "Active button label"),
DATA(A_NORMAL, BUTTON_LABEL_INACTIVE, button_label_inactive, "Inactive button label"),
DATA(A_REVERSE, INPUTBOX, inputbox, "Input box"),
DATA(A_REVERSE, INPUTBOX_BORDER, inputbox_border, "Input box border"),
DATA(A_REVERSE, SEARCHBOX, searchbox, "Search box"),
DATA(A_REVERSE, SEARCHBOX_TITLE, searchbox_title, "Search box title"),
DATA(A_REVERSE, SEARCHBOX_BORDER, searchbox_border, "Search box border"),
DATA(A_REVERSE, POSITION_INDICATOR, position_indicator, "File position indicator"),
DATA(A_REVERSE, MENUBOX, menubox, "Menu box"),
DATA(A_REVERSE, MENUBOX_BORDER, menubox_border, "Menu box border"),
DATA(A_REVERSE, ITEM, item, "Item"),
DATA(A_NORMAL, ITEM_SELECTED, item_selected, "Selected item"),
DATA(A_REVERSE, TAG, tag, "Tag"),
DATA(A_REVERSE, TAG_SELECTED, tag_selected, "Selected tag"),
DATA(A_NORMAL, TAG_KEY, tag_key, "Tag key"),
DATA(A_BOLD, TAG_KEY_SELECTED, tag_key_selected, "Selected tag key"),
DATA(A_REVERSE, CHECK, check, "Check box"),
DATA(A_REVERSE, CHECK_SELECTED, check_selected, "Selected check box"),
DATA(A_REVERSE, UARROW, uarrow, "Up arrow"),
DATA(A_REVERSE, DARROW, darrow, "Down arrow"),
DATA(A_NORMAL, ITEMHELP, itemhelp, "Item help-text"),
DATA(A_BOLD, FORM_ACTIVE_TEXT, form_active_text, "Active form text"),
DATA(A_REVERSE, FORM_TEXT, form_text, "Form text"),
DATA(A_NORMAL, FORM_ITEM_READONLY, form_item_readonly, "Readonly form item"),
DATA(A_REVERSE, GAUGE, gauge, "Dialog box gauge"),
DATA(A_REVERSE, BORDER2, border2, "Dialog box border2"),
DATA(A_REVERSE, INPUTBOX_BORDER2, inputbox_border2, "Input box border2"),
DATA(A_REVERSE, SEARCHBOX_BORDER2, searchbox_border2, "Search box border2"),
DATA(A_REVERSE, MENUBOX_BORDER2, menubox_border2, "Menu box border2")
};
#undef DATA






static void
add_subwindow(WINDOW *parent, WINDOW *child)
{
DIALOG_WINDOWS *p = dlg_calloc(DIALOG_WINDOWS, 1);

if (p != 0) {
p->normal = parent;
p->shadow = child;
p->getc_timeout = WTIMEOUT_OFF;
p->next = dialog_state.all_subwindows;
dialog_state.all_subwindows = p;
}
}

static void
del_subwindows(WINDOW *parent)
{
DIALOG_WINDOWS *p = dialog_state.all_subwindows;
DIALOG_WINDOWS *q = 0;
DIALOG_WINDOWS *r;

while (p != 0) {
if (p->normal == parent) {
delwin(p->shadow);
r = p->next;
if (q == 0) {
dialog_state.all_subwindows = r;
} else {
q->next = r;
}
free(p);
p = r;
} else {
q = p;
p = p->next;
}
}
}




void
dlg_put_backtitle(void)
{

if (dialog_vars.backtitle != NULL) {
chtype attr = A_NORMAL;
int backwidth = dlg_count_columns(dialog_vars.backtitle);
int i;

dlg_attrset(stdscr, screen_attr);
(void) wmove(stdscr, 0, 1);
dlg_print_text(stdscr, dialog_vars.backtitle, COLS - 2, &attr);
for (i = 0; i < COLS - backwidth; i++)
(void) waddch(stdscr, ' ');
(void) wmove(stdscr, 1, 1);
for (i = 0; i < COLS - 2; i++)
(void) waddch(stdscr, dlg_boxchar(ACS_HLINE));
}

(void) wnoutrefresh(stdscr);
}





void
dlg_attr_clear(WINDOW *win, int height, int width, chtype attr)
{
int i, j;

dlg_attrset(win, attr);
for (i = 0; i < height; i++) {
(void) wmove(win, i, 0);
for (j = 0; j < width; j++)
(void) waddch(win, ' ');
}
(void) touchwin(win);
}

void
dlg_clear(void)
{
dlg_attr_clear(stdscr, LINES, COLS, screen_attr);
}

#if defined(KEY_RESIZE)
void
_dlg_resize_cleanup(WINDOW *w)
{
dlg_clear();
dlg_put_backtitle();
dlg_del_window(w);
dlg_mouse_free_regions();
}
#endif

#define isprivate(s) ((s) != 0 && strstr(s, "\033[?") != 0)

#define TTY_DEVICE "/dev/tty"







#define dialog_tty() (dlg_getenv_num("DIALOG_TTY", (int *)0) > 0)





static int
open_terminal(char **result, int mode)
{
const char *device = TTY_DEVICE;
if (!isatty(fileno(stderr))
|| (device = ttyname(fileno(stderr))) == 0) {
if (!isatty(fileno(stdout))
|| (device = ttyname(fileno(stdout))) == 0) {
if (!isatty(fileno(stdin))
|| (device = ttyname(fileno(stdin))) == 0) {
device = TTY_DEVICE;
}
}
}
*result = dlg_strclone(device);
return open(device, mode);
}

#if CAN_KEEP_TITE
static int
my_putc(int ch)
{
char buffer[2];
int fd = fileno(dialog_state.screen_output);

buffer[0] = (char) ch;
return (int) write(fd, buffer, (size_t) 1);
}
#endif











void
init_dialog(FILE *input, FILE *output)
{
int fd1, fd2;
char *device = 0;

setlocale(LC_ALL, "");

dialog_state.output = output;
if (dialog_state.tab_len == 0)
dialog_state.tab_len = TAB_LEN;
if (dialog_state.aspect_ratio == 0)
dialog_state.aspect_ratio = DEFAULT_ASPECT_RATIO;
#if defined(HAVE_COLOR)
dialog_state.use_colors = USE_COLORS;
dialog_state.use_shadow = USE_SHADOW;
#endif

#if defined(HAVE_RC_FILE)
if (dlg_parse_rc() == -1)
dlg_exiterr("init_dialog: dlg_parse_rc");
#endif












dialog_state.pipe_input = stdin;
if (fileno(input) != fileno(stdin)) {
if ((fd1 = dup(fileno(input))) >= 0
&& (fd2 = dup(fileno(stdin))) >= 0) {
(void) dup2(fileno(input), fileno(stdin));
dialog_state.pipe_input = fdopen(fd2, "r");
if (fileno(stdin) != 0)
(void) dup2(fileno(stdin), 0);
} else {
dlg_exiterr("cannot open tty-input");
}
close(fd1);
} else if (!isatty(fileno(stdin))) {
if ((fd1 = open_terminal(&device, O_RDONLY)) >= 0) {
if ((fd2 = dup(fileno(stdin))) >= 0) {
dialog_state.pipe_input = fdopen(fd2, "r");
if (freopen(device, "r", stdin) == 0)
dlg_exiterr("cannot open tty-input");
if (fileno(stdin) != 0)
(void) dup2(fileno(stdin), 0);
}
close(fd1);
}
free(device);
}



















if (!isatty(fileno(stdout))
&& (fileno(stdout) == fileno(output) || dialog_tty())) {
if ((fd1 = open_terminal(&device, O_WRONLY)) >= 0
&& (dialog_state.screen_output = fdopen(fd1, "w")) != 0) {
if (newterm(NULL, dialog_state.screen_output, stdin) == 0) {
dlg_exiterr("cannot initialize curses");
}
free(device);
} else {
dlg_exiterr("cannot open tty-output");
}
} else {
dialog_state.screen_output = stdout;
(void) initscr();
}
dlg_keep_tite(dialog_state.screen_output);
#if defined(HAVE_FLUSHINP)
(void) flushinp();
#endif
(void) keypad(stdscr, TRUE);
(void) cbreak();
(void) noecho();

if (!dialog_state.no_mouse) {
mouse_open();
}

dialog_state.screen_initialized = TRUE;

#if defined(HAVE_COLOR)
if (dialog_state.use_colors || dialog_state.use_shadow)
dlg_color_setup();
#endif


dlg_clear();
}

void
dlg_keep_tite(FILE *output)
{
if (!dialog_vars.keep_tite) {
#if CAN_KEEP_TITE



if ((fileno(output) != fileno(stdout)
|| isatty(fileno(output)))
&& key_mouse != 0
&& isprivate(enter_ca_mode)
&& isprivate(exit_ca_mode)) {
FILE *save = dialog_state.screen_output;








(void) refresh();
dialog_state.screen_output = output;
(void) tputs(exit_ca_mode, 0, my_putc);
(void) tputs(clear_screen, 0, my_putc);
dialog_state.screen_output = save;












enter_ca_mode = 0;
exit_ca_mode = 0;
}
#else






(void) output;
#endif
}
}

#if defined(HAVE_COLOR)
static int defined_colors = 1;



void
dlg_color_setup(void)
{
if (has_colors()) {
unsigned i;

(void) start_color();

#if defined(HAVE_USE_DEFAULT_COLORS)
use_default_colors();
#endif

#if defined(__NetBSD__) && defined(_CURSES_)
#define C_ATTR(x,y) (((x) != 0 ? A_BOLD : 0) | COLOR_PAIR((y)))

for (i = 0; i < sizeof(dlg_color_table) /
sizeof(dlg_color_table[0]); i++) {


(void) init_pair(i + 1,
dlg_color_table[i].fg,
dlg_color_table[i].bg);


dlg_color_table[i].atr = C_ATTR(dlg_color_table[i].hilite, i + 1);
}
defined_colors = i + 1;
#else
for (i = 0; i < sizeof(dlg_color_table) /
sizeof(dlg_color_table[0]); i++) {


chtype atr = dlg_color_pair(dlg_color_table[i].fg,
dlg_color_table[i].bg);

atr |= (dlg_color_table[i].hilite ? A_BOLD : 0);
#if defined(HAVE_RC_FILE2)
atr |= (dlg_color_table[i].ul ? A_UNDERLINE : 0);
atr |= (dlg_color_table[i].rv ? A_REVERSE : 0);
#endif

dlg_color_table[i].atr = atr;
}
#endif
} else {
dialog_state.use_colors = FALSE;
dialog_state.use_shadow = FALSE;
}
}

int
dlg_color_count(void)
{
return TableSize(dlg_color_table);
}




chtype
dlg_get_attrs(WINDOW *win)
{
chtype result;
#if defined(HAVE_GETATTRS)
result = (chtype) getattrs(win);
#else
attr_t my_result;
short my_pair;
wattr_get(win, &my_result, &my_pair, NULL);
result = my_result;
#endif
return result;
}






chtype
dlg_color_pair(int foreground, int background)
{
chtype result = 0;
int pair;
short fg, bg;
bool found = FALSE;

for (pair = 1; pair < defined_colors; ++pair) {
if (pair_content((short) pair, &fg, &bg) != ERR
&& fg == foreground
&& bg == background) {
result = (chtype) COLOR_PAIR(pair);
found = TRUE;
break;
}
}
if (!found && (defined_colors + 1) < COLOR_PAIRS) {
pair = defined_colors++;
(void) init_pair((short) pair, (short) foreground, (short) background);
result = (chtype) COLOR_PAIR(pair);
}
return result;
}






static chtype
define_color(WINDOW *win, int foreground)
{
short fg, bg, background;
if (dialog_state.text_only) {
background = COLOR_BLACK;
} else {
chtype attrs = dlg_get_attrs(win);
int pair;

if ((pair = PAIR_NUMBER(attrs)) != 0
&& pair_content((short) pair, &fg, &bg) != ERR) {
background = bg;
} else {
background = COLOR_BLACK;
}
}
return dlg_color_pair(foreground, background);
}
#endif




void
end_dialog(void)
{
if (dialog_state.screen_initialized) {
dialog_state.screen_initialized = FALSE;
if (dialog_vars.erase_on_exit) {




werase(stdscr);
wrefresh(stdscr);
}
mouse_close();
(void) endwin();
(void) fflush(stdout);
}
}

#define ESCAPE_LEN 3
#define isOurEscape(p) (((p)[0] == '\\') && ((p)[1] == 'Z') && ((p)[2] != 0))

int
dlg_count_real_columns(const char *text)
{
int result = 0;
if (*text) {
result = dlg_count_columns(text);
if (result && dialog_vars.colors) {
int hidden = 0;
while (*text) {
if (dialog_vars.colors && isOurEscape(text)) {
hidden += ESCAPE_LEN;
text += ESCAPE_LEN;
} else {
++text;
}
}
result -= hidden;
}
}
return result;
}

static int
centered(int width, const char *string)
{
int need = dlg_count_real_columns(string);
int left;

left = (width - need) / 2 - 1;
if (left < 0)
left = 0;
return left;
}

#if defined(USE_WIDE_CURSES)
static bool
is_combining(const char *txt, int *combined)
{
bool result = FALSE;

if (*combined == 0) {
if (UCH(*txt) >= 128) {
wchar_t wch;
mbstate_t state;
size_t given = strlen(txt);
size_t len;

memset(&state, 0, sizeof(state));
len = mbrtowc(&wch, txt, given, &state);
if ((int) len > 0 && wcwidth(wch) == 0) {
*combined = (int) len - 1;
result = TRUE;
}
}
} else {
result = TRUE;
*combined -= 1;
}
return result;
}
#endif





void
dlg_print_listitem(WINDOW *win,
const char *text,
int climit,
bool first,
int selected)
{
chtype attr = A_NORMAL;
int limit;
chtype attrs[4];

if (text == 0)
text = "";

if (first && !dialog_vars.no_hot_list) {
const int *indx = dlg_index_wchars(text);
attrs[3] = tag_key_selected_attr;
attrs[2] = tag_key_attr;
attrs[1] = tag_selected_attr;
attrs[0] = tag_attr;

dlg_attrset(win, selected ? attrs[3] : attrs[2]);
if (*text != '\0') {
(void) waddnstr(win, text, indx[1]);

if ((int) strlen(text) > indx[1]) {
limit = dlg_limit_columns(text, climit, 1);
if (limit > 1) {
dlg_attrset(win, selected ? attrs[1] : attrs[0]);
(void) waddnstr(win,
text + indx[1],
indx[limit] - indx[1]);
}
}
}
} else {
const int *cols;

attrs[1] = item_selected_attr;
attrs[0] = item_attr;

cols = dlg_index_columns(text);
limit = dlg_limit_columns(text, climit, 0);

if (limit > 0) {
dlg_attrset(win, selected ? attrs[1] : attrs[0]);
dlg_print_text(win, text, cols[limit], &attr);
}
}
}





void
dlg_print_text(WINDOW *win, const char *txt, int cols, chtype *attr)
{
int y_origin, x_origin;
int y_before, x_before = 0;
int y_after, x_after;
int tabbed = 0;
bool ended = FALSE;
#if defined(USE_WIDE_CURSES)
int combined = 0;
#endif

if (dialog_state.text_only) {
y_origin = y_after = 0;
x_origin = x_after = 0;
} else {
y_after = 0;
x_after = 0;
getyx(win, y_origin, x_origin);
}
while (cols > 0 && (*txt != '\0')) {
bool thisTab;
chtype useattr;

if (dialog_vars.colors) {
while (isOurEscape(txt)) {
int code;

txt += 2;
switch (code = CharOf(*txt)) {
#if defined(HAVE_COLOR)
case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
*attr &= ~A_COLOR;
*attr |= define_color(win, code - '0');
break;
#endif
case 'B':
*attr &= ~A_BOLD;
break;
case 'b':
*attr |= A_BOLD;
break;
case 'R':
*attr &= ~A_REVERSE;
break;
case 'r':
*attr |= A_REVERSE;
break;
case 'U':
*attr &= ~A_UNDERLINE;
break;
case 'u':
*attr |= A_UNDERLINE;
break;
case 'n':
*attr = A_NORMAL;
break;
default:
break;
}
++txt;
}
}
if (ended || *txt == '\n' || *txt == '\0')
break;
useattr = (*attr) & A_ATTRIBUTES;
#if defined(HAVE_COLOR)





if ((useattr & A_COLOR) != 0 && (useattr & A_BOLD) == 0) {
short pair = (short) PAIR_NUMBER(useattr);
short fg, bg;
if (pair_content(pair, &fg, &bg) != ERR
&& fg == bg) {
useattr &= ~A_COLOR;
useattr |= dlg_color_pair(fg, ((bg == COLOR_BLACK)
? COLOR_WHITE
: COLOR_BLACK));
}
}
#endif






thisTab = (CharOf(*txt) == TAB);
if (dialog_state.text_only) {
x_before = x_after;
} else {
if (thisTab) {
getyx(win, y_before, x_before);
(void) y_before;
}
}
if (dialog_state.text_only) {
int ch = CharOf(*txt++);
if (thisTab) {
while ((x_after++) % 8) {
fputc(' ', dialog_state.output);
}
} else {
fputc(ch, dialog_state.output);
x_after++;
}
} else {
(void) waddch(win, CharOf(*txt++) | useattr);
getyx(win, y_after, x_after);
}
if (thisTab && (y_after == y_origin))
tabbed += (x_after - x_before);
if ((y_after != y_origin) ||
(x_after >= (cols + tabbed + x_origin)
#if defined(USE_WIDE_CURSES)
&& !is_combining(txt, &combined)
#endif
)) {
ended = TRUE;
}
}
if (dialog_state.text_only) {
fputc('\n', dialog_state.output);
}
}







const char *
dlg_print_line(WINDOW *win,
chtype *attr,
const char *prompt,
int lm, int rm, int *x)
{
const char *wrap_ptr;
const char *test_ptr;
const char *hide_ptr = 0;
const int *cols = dlg_index_columns(prompt);
const int *indx = dlg_index_wchars(prompt);
int wrap_inx = 0;
int test_inx = 0;
int cur_x = lm;
int hidden = 0;
int limit = dlg_count_wchars(prompt);
int n;
int tabbed = 0;

*x = 1;





for (n = 0; n < limit; ++n) {
int ch = *(test_ptr = prompt + indx[test_inx]);
if (ch == '\n' || ch == '\0' || cur_x >= (rm + hidden))
break;
if (ch == TAB && n == 0) {
tabbed = 8;
} else if (isblank(UCH(ch))
&& n != 0
&& !isblank(UCH(prompt[indx[n - 1]]))) {
wrap_inx = n;
*x = cur_x;
} else if (dialog_vars.colors && isOurEscape(test_ptr)) {
hide_ptr = test_ptr;
hidden += ESCAPE_LEN;
n += (ESCAPE_LEN - 1);
}
cur_x = lm + tabbed + cols[n + 1];
if (cur_x > (rm + hidden))
break;
test_inx = n + 1;
}





test_ptr = prompt + indx[test_inx];
if (*test_ptr == '\n' || isblank(UCH(*test_ptr)) || *test_ptr == '\0') {
wrap_inx = test_inx;
while (wrap_inx > 0 && isblank(UCH(prompt[indx[wrap_inx - 1]]))) {
wrap_inx--;
}
*x = lm + indx[wrap_inx];
} else if (*x == 1 && cur_x >= rm) {



*x = rm;
wrap_inx = test_inx;
}
wrap_ptr = prompt + indx[wrap_inx];
#if defined(USE_WIDE_CURSES)
if (UCH(*wrap_ptr) >= 128) {
int combined = 0;
while (is_combining(wrap_ptr, &combined)) {
++wrap_ptr;
}
}
#endif





if ((hide_ptr != 0) && (hide_ptr >= wrap_ptr)) {
hidden -= ESCAPE_LEN;
test_ptr = wrap_ptr;
while (test_ptr < wrap_ptr) {
if (dialog_vars.colors && isOurEscape(test_ptr)) {
hidden -= ESCAPE_LEN;
test_ptr += ESCAPE_LEN;
} else {
++test_ptr;
}
}
}





if (dialog_state.text_only || win) {
dlg_print_text(win, prompt, (cols[wrap_inx] - hidden), attr);
}


if (*x == 1) {
*x = rm;
}

*x -= hidden;


test_ptr = wrap_ptr;
while (isblank(UCH(*test_ptr)))
test_ptr++;
if (*test_ptr == '\n')
test_ptr++;
dlg_finish_string(prompt);
return (test_ptr);
}

static void
justify_text(WINDOW *win,
const char *prompt,
int limit_y,
int limit_x,
int *high, int *wide)
{
chtype attr = A_NORMAL;
int x;
int y = MARGIN;
int max_x = 2;
int lm = (2 * MARGIN);
int rm = limit_x;
int bm = limit_y;
int last_y = 0, last_x = 0;

dialog_state.text_height = 0;
dialog_state.text_width = 0;
if (dialog_state.text_only || win) {
rm -= (2 * MARGIN);
bm -= (2 * MARGIN);
}
if (prompt == 0)
prompt = "";

if (win != 0)
getyx(win, last_y, last_x);
while (y <= bm && *prompt) {
x = lm;

if (*prompt == '\n') {
while (*prompt == '\n' && y < bm) {
if (*(prompt + 1) != '\0') {
++y;
if (win != 0)
(void) wmove(win, y, lm);
}
prompt++;
}
} else if (win != 0)
(void) wmove(win, y, lm);

if (*prompt) {
prompt = dlg_print_line(win, &attr, prompt, lm, rm, &x);
if (win != 0)
getyx(win, last_y, last_x);
}
if (*prompt) {
++y;
if (win != 0)
(void) wmove(win, y, lm);
}
max_x = MAX(max_x, x);
}

if (win != 0)
(void) wmove(win, last_y, last_x);


if (high != 0)
*high = y;
if (wide != 0)
*wide = max_x;
}






void
dlg_print_autowrap(WINDOW *win, const char *prompt, int height, int width)
{
justify_text(win, prompt,
height,
width,
(int *) 0, (int *) 0);
}









int
dlg_print_scrolled(WINDOW *win,
const char *prompt,
int offset,
int height,
int width,
int pauseopt)
{
int oldy, oldx;
int last = 0;

(void) pauseopt;

getyx(win, oldy, oldx);
#if defined(NCURSES_VERSION)
if (pauseopt) {
int wide = width - (2 * MARGIN);
int high = LINES;
int len;
WINDOW *dummy;

#if defined(NCURSES_VERSION_PATCH) && NCURSES_VERSION_PATCH >= 20040417




if ((len = dlg_count_columns(prompt)) > high)
high = len;
#endif
dummy = newwin(high, width, 0, 0);
if (dummy == 0) {
dlg_attrset(win, dialog_attr);
dlg_print_autowrap(win, prompt, height + 1 + (3 * MARGIN), width);
last = 0;
} else {
int y, x;

wbkgdset(dummy, dialog_attr | ' ');
dlg_attrset(dummy, dialog_attr);
werase(dummy);
dlg_print_autowrap(dummy, prompt, high, width);
getyx(dummy, y, x);
(void) x;

copywin(dummy,
win,
offset + MARGIN,
MARGIN,
MARGIN,
MARGIN,
height,
wide,
FALSE);

delwin(dummy);


if (y > 0 && wide > 4) {
int percent = (int) ((height + offset) * 100.0 / y);

if (percent < 0)
percent = 0;
if (percent > 100)
percent = 100;

if (offset != 0 || percent != 100) {
char buffer[5];

dlg_attrset(win, position_indicator_attr);
(void) wmove(win, MARGIN + height, wide - 4);
(void) sprintf(buffer, "%d%%", percent);
(void) waddstr(win, buffer);
if ((len = (int) strlen(buffer)) < 4) {
dlg_attrset(win, border_attr);
whline(win, dlg_boxchar(ACS_HLINE), 4 - len);
}
}
}
last = (y - height);
}
} else
#endif
{
(void) offset;
dlg_attrset(win, dialog_attr);
dlg_print_autowrap(win, prompt, height + 1 + (3 * MARGIN), width);
last = 0;
}
wmove(win, oldy, oldx);
return last;
}

int
dlg_check_scrolled(int key, int last, int page, bool * show, int *offset)
{
int code = 0;

*show = FALSE;

switch (key) {
case DLGK_PAGE_FIRST:
if (*offset > 0) {
*offset = 0;
*show = TRUE;
}
break;
case DLGK_PAGE_LAST:
if (*offset < last) {
*offset = last;
*show = TRUE;
}
break;
case DLGK_GRID_UP:
if (*offset > 0) {
--(*offset);
*show = TRUE;
}
break;
case DLGK_GRID_DOWN:
if (*offset < last) {
++(*offset);
*show = TRUE;
}
break;
case DLGK_PAGE_PREV:
if (*offset > 0) {
*offset -= page;
if (*offset < 0)
*offset = 0;
*show = TRUE;
}
break;
case DLGK_PAGE_NEXT:
if (*offset < last) {
*offset += page;
if (*offset > last)
*offset = last;
*show = TRUE;
}
break;
default:
code = -1;
break;
}
return code;
}







static void
auto_size_preformatted(const char *prompt, int *height, int *width)
{
int high = 0, wide = 0;
float car;
int max_y = SLINES - 1;
int max_x = SCOLS - 2;
int max_width = max_x;
int ar = dialog_state.aspect_ratio;


justify_text((WINDOW *) 0, prompt, max_y, max_x, &high, &wide);
car = (float) (wide / high);





if (car > ar) {
float diff = car / (float) ar;
max_x = (int) ((float) wide / diff + 4);
justify_text((WINDOW *) 0, prompt, max_y, max_x, &high, &wide);
car = (float) wide / (float) high;
}






while (car < ar && max_x < max_width) {
max_x += 4;
justify_text((WINDOW *) 0, prompt, max_y, max_x, &high, &wide);
car = (float) (wide / high);
}

*height = high;
*width = wide;
}






static int
longest_word(const char *string)
{
int result = 0;

while (*string != '\0') {
int length = 0;
while (*string != '\0' && !isspace(UCH(*string))) {
length++;
string++;
}
result = MAX(result, length);
if (*string != '\0')
string++;
}
return result;
}





static void
real_auto_size(const char *title,
const char *prompt,
int *height, int *width,
int boxlines, int mincols)
{
int x = (dialog_vars.begin_set ? dialog_vars.begin_x : 2);
int y = (dialog_vars.begin_set ? dialog_vars.begin_y : 1);
int title_length = title ? dlg_count_columns(title) : 0;
int high;
int save_high = *height;
int save_wide = *width;
int max_high;
int max_wide;

if (prompt == 0) {
if (*height == 0)
*height = -1;
if (*width == 0)
*width = -1;
}

max_high = (*height < 0);
max_wide = (*width < 0);

if (*height > 0) {
high = *height;
} else {
high = SLINES - y;
}

if (*width <= 0) {
int wide;

if (prompt != 0) {
wide = MAX(title_length, mincols);
if (strchr(prompt, '\n') == 0) {
double val = (dialog_state.aspect_ratio *
dlg_count_real_columns(prompt));
double xxx = sqrt(val);
int tmp = (int) xxx;
wide = MAX(wide, tmp);
wide = MAX(wide, longest_word(prompt));
justify_text((WINDOW *) 0, prompt, high, wide, height, width);
} else {
auto_size_preformatted(prompt, height, width);
}
} else {
wide = SCOLS - x;
justify_text((WINDOW *) 0, prompt, high, wide, height, width);
}
}

if (*width < title_length) {
justify_text((WINDOW *) 0, prompt, high, title_length, height, width);
*width = title_length;
}

dialog_state.text_height = *height;
dialog_state.text_width = *width;

if (*width < mincols && save_wide == 0)
*width = mincols;
if (prompt != 0) {
*width += ((2 * MARGIN) + SHADOW_COLS);
*height += boxlines + (2 * MARGIN);
}

if (save_high > 0)
*height = save_high;
if (save_wide > 0)
*width = save_wide;

if (max_high)
*height = SLINES - (dialog_vars.begin_set ? dialog_vars.begin_y : 0);
if (max_wide)
*width = SCOLS - (dialog_vars.begin_set ? dialog_vars.begin_x : 0);
}



void
dlg_auto_size(const char *title,
const char *prompt,
int *height,
int *width,
int boxlines,
int mincols)
{
DLG_TRACE(("#dlg_auto_size(%d,%d) limits %d,%d\n",
*height, *width,
boxlines, mincols));

real_auto_size(title, prompt, height, width, boxlines, mincols);

if (*width > SCOLS) {
(*height)++;
*width = SCOLS;
}

if (*height > SLINES) {
*height = SLINES;
}
DLG_TRACE(("#...dlg_auto_size(%d,%d) also %d,%d\n",
*height, *width,
dialog_state.text_height, dialog_state.text_width));
}







void
dlg_auto_sizefile(const char *title,
const char *file,
int *height,
int *width,
int boxlines,
int mincols)
{
int count = 0;
int len = title ? dlg_count_columns(title) : 0;
int nc = 4;
int numlines = 2;
FILE *fd;


if ((fd = fopen(file, "rb")) == NULL)
dlg_exiterr("dlg_auto_sizefile: Cannot open input file %s", file);

if ((*height == -1) || (*width == -1)) {
*height = SLINES - (dialog_vars.begin_set ? dialog_vars.begin_y : 0);
*width = SCOLS - (dialog_vars.begin_set ? dialog_vars.begin_x : 0);
}
if ((*height != 0) && (*width != 0)) {
(void) fclose(fd);
if (*width > SCOLS)
*width = SCOLS;
if (*height > SLINES)
*height = SLINES;
return;
}

while (!feof(fd)) {
int ch;
long offset;

if (ferror(fd))
break;

offset = 0;
while (((ch = getc(fd)) != '\n') && !feof(fd)) {
if ((ch == TAB) && (dialog_vars.tab_correct)) {
offset += dialog_state.tab_len - (offset % dialog_state.tab_len);
} else {
offset++;
}
}

if (offset > len)
len = (int) offset;

count++;
}



*height = MIN(SLINES, count + numlines + boxlines);
*width = MIN(SCOLS, MAX((len + nc), mincols));




(void) fclose(fd);
}

















void
dlg_draw_box2(WINDOW *win, int y, int x, int height, int width,
chtype boxchar, chtype borderchar, chtype borderchar2)
{
int i, j;
chtype save = dlg_get_attrs(win);

dlg_attrset(win, 0);
for (i = 0; i < height; i++) {
(void) wmove(win, y + i, x);
for (j = 0; j < width; j++)
if (!i && !j)
(void) waddch(win, borderchar | dlg_boxchar(ACS_ULCORNER));
else if (i == height - 1 && !j)
(void) waddch(win, borderchar | dlg_boxchar(ACS_LLCORNER));
else if (!i && j == width - 1)
(void) waddch(win, borderchar2 | dlg_boxchar(ACS_URCORNER));
else if (i == height - 1 && j == width - 1)
(void) waddch(win, borderchar2 | dlg_boxchar(ACS_LRCORNER));
else if (!i)
(void) waddch(win, borderchar | dlg_boxchar(ACS_HLINE));
else if (i == height - 1)
(void) waddch(win, borderchar2 | dlg_boxchar(ACS_HLINE));
else if (!j)
(void) waddch(win, borderchar | dlg_boxchar(ACS_VLINE));
else if (j == width - 1)
(void) waddch(win, borderchar2 | dlg_boxchar(ACS_VLINE));
else
(void) waddch(win, boxchar | ' ');
}
dlg_attrset(win, save);
}

void
dlg_draw_box(WINDOW *win, int y, int x, int height, int width,
chtype boxchar, chtype borderchar)
{
dlg_draw_box2(win, y, x, height, width, boxchar, borderchar, boxchar);
}












static DIALOG_WINDOWS *
find_window(DIALOG_WINDOWS * list, WINDOW *win, bool normal)
{
DIALOG_WINDOWS *result = 0;
DIALOG_WINDOWS *p;

for (p = list; p != 0; p = p->next) {
WINDOW *check = normal ? p->normal : p->shadow;
if (check == win) {
result = p;
break;
}
}
return result;
}

#define SearchTopWindows(win) find_window(dialog_state.all_windows, win, TRUE)
#define SearchSubWindows(win) find_window(dialog_state.all_subwindows, win, FALSE)







DIALOG_WINDOWS *
_dlg_find_window(WINDOW *win)
{
DIALOG_WINDOWS *result = 0;

if ((result = SearchTopWindows(win)) == NULL)
result = SearchSubWindows(win);
return result;
}

#if defined(HAVE_COLOR)









static WINDOW *
in_window(WINDOW *win, int y, int x)
{
WINDOW *result = 0;
int y_base = getbegy(win);
int x_base = getbegx(win);
int y_last = getmaxy(win) + y_base;
int x_last = getmaxx(win) + x_base;

if (y >= y_base && y <= y_last && x >= x_base && x <= x_last)
result = win;
return result;
}

static WINDOW *
window_at_cell(DIALOG_WINDOWS * dw, int y, int x)
{
WINDOW *result = 0;
DIALOG_WINDOWS *p;
int y_want = y + getbegy(dw->shadow);
int x_want = x + getbegx(dw->shadow);

for (p = dialog_state.all_windows; p != 0; p = p->next) {
if (dw->normal != p->normal
&& dw->shadow != p->normal
&& (result = in_window(p->normal, y_want, x_want)) != 0) {
break;
}
}
if (result == 0) {
result = stdscr;
}
return result;
}

static bool
in_shadow(WINDOW *normal, WINDOW *shadow, int y, int x)
{
bool result = FALSE;
int ybase = getbegy(normal);
int ylast = getmaxy(normal) + ybase;
int xbase = getbegx(normal);
int xlast = getmaxx(normal) + xbase;

y += getbegy(shadow);
x += getbegx(shadow);

if (y >= ybase + SHADOW_ROWS
&& y < ylast + SHADOW_ROWS
&& x >= xlast
&& x < xlast + SHADOW_COLS) {

result = TRUE;
} else if (y >= ylast
&& y < ylast + SHADOW_ROWS
&& x >= ybase + SHADOW_COLS
&& x < ylast + SHADOW_COLS) {

result = TRUE;
}

return result;
}






static bool
last_shadow(DIALOG_WINDOWS * dw, int y, int x)
{
DIALOG_WINDOWS *p;
bool result = TRUE;

for (p = dialog_state.all_windows; p != 0; p = p->next) {
if (p->normal != dw->normal
&& in_shadow(p->normal, dw->shadow, y, x)) {
result = FALSE;
break;
}
}
return result;
}

static void
repaint_cell(DIALOG_WINDOWS * dw, bool draw, int y, int x)
{
WINDOW *win = dw->shadow;
WINDOW *cellwin;
int y2, x2;

if ((cellwin = window_at_cell(dw, y, x)) != 0
&& (draw || last_shadow(dw, y, x))
&& (y2 = (y + getbegy(win) - getbegy(cellwin))) >= 0
&& (x2 = (x + getbegx(win) - getbegx(cellwin))) >= 0
&& wmove(cellwin, y2, x2) != ERR) {
chtype the_cell = dlg_get_attrs(cellwin);
chtype the_attr = (draw ? shadow_attr : the_cell);

if (winch(cellwin) & A_ALTCHARSET) {
the_attr |= A_ALTCHARSET;
}
#if USE_WCHGAT
wchgat(cellwin, 1,
the_attr & (chtype) (~A_COLOR),
(short) PAIR_NUMBER(the_attr),
NULL);
#else
{
chtype the_char = ((winch(cellwin) & A_CHARTEXT) | the_attr);
(void) waddch(cellwin, the_char);
}
#endif
wnoutrefresh(cellwin);
}
}

#define RepaintCell(dw, draw, y, x) repaint_cell(dw, draw, y, x)

static void
repaint_shadow(DIALOG_WINDOWS * dw, bool draw, int y, int x, int height, int width)
{
if (UseShadow(dw)) {
int i, j;

#if !USE_WCHGAT
chtype save = dlg_get_attrs(dw->shadow);
dlg_attrset(dw->shadow, draw ? shadow_attr : screen_attr);
#endif
for (i = 0; i < SHADOW_ROWS; ++i) {
for (j = 0; j < width; ++j) {
RepaintCell(dw, draw, i + y + height, j + x + SHADOW_COLS);
}
}
for (i = 0; i < height; i++) {
for (j = 0; j < SHADOW_COLS; ++j) {
RepaintCell(dw, draw, i + y + SHADOW_ROWS, j + x + width);
}
}
(void) wnoutrefresh(dw->shadow);
#if !USE_WCHGAT
dlg_attrset(dw->shadow, save);
#endif
}
}





static void
draw_childs_shadow(DIALOG_WINDOWS * dw)
{
if (UseShadow(dw)) {
repaint_shadow(dw,
TRUE,
getbegy(dw->normal) - getbegy(dw->shadow),
getbegx(dw->normal) - getbegx(dw->shadow),
getmaxy(dw->normal),
getmaxx(dw->normal));
}
}





static void
erase_childs_shadow(DIALOG_WINDOWS * dw)
{
if (UseShadow(dw)) {
repaint_shadow(dw,
FALSE,
getbegy(dw->normal) - getbegy(dw->shadow),
getbegx(dw->normal) - getbegx(dw->shadow),
getmaxy(dw->normal),
getmaxx(dw->normal));
}
}





void
dlg_draw_shadow(WINDOW *win, int y, int x, int height, int width)
{
repaint_shadow(SearchTopWindows(win), TRUE, y, x, height, width);
}
#endif





void
dlg_exit(int code)
{

static const struct {
int code;
const char *name;
} table[] = {
{ DLG_EXIT_CANCEL, "DIALOG_CANCEL" },
{ DLG_EXIT_ERROR, "DIALOG_ERROR" },
{ DLG_EXIT_ESC, "DIALOG_ESC" },
{ DLG_EXIT_EXTRA, "DIALOG_EXTRA" },
{ DLG_EXIT_HELP, "DIALOG_HELP" },
{ DLG_EXIT_OK, "DIALOG_OK" },
{ DLG_EXIT_ITEM_HELP, "DIALOG_ITEM_HELP" },
{ DLG_EXIT_TIMEOUT, "DIALOG_TIMEOUT" },
};


unsigned n;
bool overridden = FALSE;

retry:
for (n = 0; n < TableSize(table); n++) {
if (table[n].code == code) {
if (dlg_getenv_num(table[n].name, &code)) {
overridden = TRUE;
}
break;
}
}






if (code == DLG_EXIT_ITEM_HELP && !overridden) {
code = DLG_EXIT_HELP;
goto retry;
}
#if defined(HAVE_DLG_TRACE)
dlg_trace((const char *) 0);
#endif

#if defined(NO_LEAKS)
_dlg_inputstr_leaks();
#if defined(NCURSES_VERSION) && (defined(HAVE_CURSES_EXIT) || defined(HAVE__NC_FREE_AND_EXIT))
curses_exit(code);
#endif
#endif

if (dialog_state.input == stdin) {
exit(code);
} else {




if (dialog_state.input) {
fclose(dialog_state.input);
dialog_state.input = 0;
}
if (dialog_state.pipe_input) {
if (dialog_state.pipe_input != stdin) {
fclose(dialog_state.pipe_input);
dialog_state.pipe_input = 0;
}
}
_exit(code);
}
}

#define DATA(name) { DLG_EXIT_ ##name, #name }

static struct {
int code;
const char *name;
} exit_codenames[] = {
DATA(ESC),
DATA(UNKNOWN),
DATA(ERROR),
DATA(OK),
DATA(CANCEL),
DATA(HELP),
DATA(EXTRA),
DATA(ITEM_HELP),
};
#undef DATA


const char *
dlg_exitcode2s(int code)
{
const char *result = "?";
size_t n;

for (n = 0; n < TableSize(exit_codenames); ++n) {
if (exit_codenames[n].code == code) {
result = exit_codenames[n].name;
break;
}
}
return result;
}

int
dlg_exitname2n(const char *name)
{
int result = DLG_EXIT_UNKNOWN;
size_t n;

for (n = 0; n < TableSize(exit_codenames); ++n) {
if (!dlg_strcmp(exit_codenames[n].name, name)) {
result = exit_codenames[n].code;
break;
}
}
return result;
}


void
dlg_exiterr(const char *fmt, ...)
{
int retval;
va_list ap;

end_dialog();

(void) fputc('\n', stderr);
va_start(ap, fmt);
(void) vfprintf(stderr, fmt, ap);
va_end(ap);
(void) fputc('\n', stderr);

#if defined(HAVE_DLG_TRACE)
va_start(ap, fmt);
dlg_trace_msg("##Error: ");
dlg_trace_va_msg(fmt, ap);
va_end(ap);
#endif

dlg_killall_bg(&retval);

(void) fflush(stderr);
(void) fflush(stdout);
dlg_exit(strcmp(fmt, "timeout") == 0 ? DLG_EXIT_TIMEOUT : DLG_EXIT_ERROR);
}




char *
dlg_getenv_str(const char *name)
{
char *result = getenv(name);
if (result != NULL) {
while (*result != '\0' && isspace(UCH(*result)))
++result;
if (*result == '\0')
result = NULL;
}
return result;
}










int
dlg_getenv_num(const char *name, int *value)
{
int result = 0;
char *data = getenv(name);
if (data != NULL) {
char *temp = NULL;
long check = strtol(data, &temp, 0);
if (temp != 0 && temp != data && *temp == '\0') {
result = (int) check;
if (value != NULL) {
*value = result;
result = 1;
}
}
}
return result;
}

void
dlg_beeping(void)
{
if (dialog_vars.beep_signal) {
(void) beep();
dialog_vars.beep_signal = 0;
}
}

void
dlg_print_size(int height, int width)
{
if (dialog_vars.print_siz) {
fprintf(dialog_state.output, "Size: %d, %d\n", height, width);
DLG_TRACE(("#print size: %dx%d\n", height, width));
}
}

void
dlg_ctl_size(int height, int width)
{
if (dialog_vars.size_err) {
if ((width > COLS) || (height > LINES)) {
dlg_exiterr("Window too big. (height, width) = (%d, %d). Max allowed (%d, %d).",
height, width, LINES, COLS);
}
#if defined(HAVE_COLOR)
else if ((dialog_state.use_shadow)
&& ((width > SCOLS || height > SLINES))) {
if ((width <= COLS) && (height <= LINES)) {

dialog_state.use_shadow = 0;
} else {
dlg_exiterr("Window+Shadow too big. (height, width) = (%d, %d). Max allowed (%d, %d).",
height, width, SLINES, SCOLS);
}
}
#endif
}
}




void
dlg_tab_correct_str(char *prompt)
{
char *ptr;

if (dialog_vars.tab_correct) {
while ((ptr = strchr(prompt, TAB)) != NULL) {
*ptr = ' ';
prompt = ptr;
}
}
}

void
dlg_calc_listh(int *height, int *list_height, int item_no)
{

int rows = SLINES - (dialog_vars.begin_set ? dialog_vars.begin_y : 0);
if (rows - (*height) > 0) {
if (rows - (*height) > item_no)
*list_height = item_no;
else
*list_height = rows - (*height);
}
(*height) += (*list_height);
}


int
dlg_calc_listw(int item_no, char **items, int group)
{
int i, len1 = 0, len2 = 0;

for (i = 0; i < (item_no * group); i += group) {
int n;

if ((n = dlg_count_columns(items[i])) > len1)
len1 = n;
if ((n = dlg_count_columns(items[i + 1])) > len2)
len2 = n;
}
return len1 + len2;
}

int
dlg_calc_list_width(int item_no, DIALOG_LISTITEM * items)
{
int n, i, len1 = 0, len2 = 0;
int bits = ((dialog_vars.no_tags ? 1 : 0)
+ (dialog_vars.no_items ? 2 : 0));

for (i = 0; i < item_no; ++i) {
switch (bits) {
case 0:

case 1:
if ((n = dlg_count_columns(items[i].name)) > len1)
len1 = n;
if ((n = dlg_count_columns(items[i].text)) > len2)
len2 = n;
break;
case 2:

case 3:
if ((n = dlg_count_columns(items[i].name)) > len1)
len1 = n;
break;
}
}
return len1 + len2;
}

char *
dlg_strempty(void)
{
static char empty[] = "";
return empty;
}

char *
dlg_strclone(const char *cprompt)
{
char *prompt = 0;
if (cprompt != 0) {
prompt = dlg_malloc(char, strlen(cprompt) + 1);
assert_ptr(prompt, "dlg_strclone");
strcpy(prompt, cprompt);
}
return prompt;
}

chtype
dlg_asciibox(chtype ch)
{
chtype result = 0;

if (ch == ACS_ULCORNER)
result = '+';
else if (ch == ACS_LLCORNER)
result = '+';
else if (ch == ACS_URCORNER)
result = '+';
else if (ch == ACS_LRCORNER)
result = '+';
else if (ch == ACS_HLINE)
result = '-';
else if (ch == ACS_VLINE)
result = '|';
else if (ch == ACS_LTEE)
result = '+';
else if (ch == ACS_RTEE)
result = '+';
else if (ch == ACS_UARROW)
result = '^';
else if (ch == ACS_DARROW)
result = 'v';

return result;
}

chtype
dlg_boxchar(chtype ch)
{
chtype result = dlg_asciibox(ch);

if (result != 0) {
if (dialog_vars.ascii_lines)
ch = result;
else if (dialog_vars.no_lines)
ch = ' ';
}
return ch;
}

int
dlg_box_x_ordinate(int width)
{
int x;

if (dialog_vars.begin_set == 1) {
x = dialog_vars.begin_x;
} else {

x = (SCOLS - width) / 2;
}
return x;
}

int
dlg_box_y_ordinate(int height)
{
int y;

if (dialog_vars.begin_set == 1) {
y = dialog_vars.begin_y;
} else {

y = (SLINES - height) / 2;
}
return y;
}

void
dlg_draw_title(WINDOW *win, const char *title)
{
if (title != NULL) {
chtype attr = A_NORMAL;
chtype save = dlg_get_attrs(win);
int x = centered(getmaxx(win), title);

dlg_attrset(win, title_attr);
wmove(win, 0, x);
dlg_print_text(win, title, getmaxx(win) - x, &attr);
dlg_attrset(win, save);
dlg_finish_string(title);
}
}

void
dlg_draw_bottom_box2(WINDOW *win, chtype on_left, chtype on_right, chtype on_inside)
{
int width = getmaxx(win);
int height = getmaxy(win);
int i;

dlg_attrset(win, on_left);
(void) wmove(win, height - 3, 0);
(void) waddch(win, dlg_boxchar(ACS_LTEE));
for (i = 0; i < width - 2; i++)
(void) waddch(win, dlg_boxchar(ACS_HLINE));
dlg_attrset(win, on_right);
(void) waddch(win, dlg_boxchar(ACS_RTEE));
dlg_attrset(win, on_inside);
(void) wmove(win, height - 2, 1);
for (i = 0; i < width - 2; i++)
(void) waddch(win, ' ');
}

void
dlg_draw_bottom_box(WINDOW *win)
{
dlg_draw_bottom_box2(win, border_attr, dialog_attr, dialog_attr);
}





void
dlg_del_window(WINDOW *win)
{
DIALOG_WINDOWS *p, *q, *r;




if (dialog_vars.keep_window)
return;





if (dialog_state.getc_callbacks != 0) {
touchwin(stdscr);
wnoutrefresh(stdscr);
}

for (p = dialog_state.all_windows, q = r = 0; p != 0; r = p, p = p->next) {
if (p->normal == win) {
q = p;
if (r == 0) {
dialog_state.all_windows = p->next;
} else {
r->next = p->next;
}
} else {
if (p->shadow != 0) {
touchwin(p->shadow);
wnoutrefresh(p->shadow);
}
touchwin(p->normal);
wnoutrefresh(p->normal);
}
}

if (q) {
if (dialog_state.all_windows != 0)
erase_childs_shadow(q);
del_subwindows(q->normal);
dlg_unregister_window(q->normal);
delwin(q->normal);
free(q);
}
doupdate();
}




WINDOW *
dlg_new_window(int height, int width, int y, int x)
{
return dlg_new_modal_window(stdscr, height, width, y, x);
}





WINDOW *
dlg_new_modal_window(WINDOW *parent, int height, int width, int y, int x)
{
WINDOW *win;
DIALOG_WINDOWS *p = dlg_calloc(DIALOG_WINDOWS, 1);

(void) parent;
if (p == 0
|| (win = newwin(height, width, y, x)) == 0) {
dlg_exiterr("Can't make new window at (%d,%d), size (%d,%d).\n",
y, x, height, width);
}
p->next = dialog_state.all_windows;
p->normal = win;
p->getc_timeout = WTIMEOUT_OFF;
dialog_state.all_windows = p;
#if defined(HAVE_COLOR)
if (dialog_state.use_shadow) {
p->shadow = parent;
draw_childs_shadow(p);
}
#endif

(void) keypad(win, TRUE);
return win;
}









int
dlg_set_timeout(WINDOW *win, bool will_getc)
{
DIALOG_WINDOWS *p;
int result = 0;

if ((p = SearchTopWindows(win)) != NULL) {
int interval = (dialog_vars.timeout_secs * 1000);

if (will_getc || dialog_vars.pause_secs) {
interval = WTIMEOUT_VAL;
} else {
result = interval;
if (interval <= 0) {
interval = WTIMEOUT_OFF;
}
}
wtimeout(win, interval);
p->getc_timeout = interval;
}
return result;
}

void
dlg_reset_timeout(WINDOW *win)
{
DIALOG_WINDOWS *p;

if ((p = SearchTopWindows(win)) != NULL) {
wtimeout(win, p->getc_timeout);
} else {
wtimeout(win, WTIMEOUT_OFF);
}
}




#if defined(KEY_RESIZE)
void
dlg_move_window(WINDOW *win, int height, int width, int y, int x)
{
if (win != 0) {
DIALOG_WINDOWS *p;

dlg_ctl_size(height, width);

if ((p = SearchTopWindows(win)) != 0) {
(void) wresize(win, height, width);
(void) mvwin(win, y, x);
#if defined(HAVE_COLOR)
if (p->shadow != 0) {
if (dialog_state.use_shadow) {
(void) mvwin(p->shadow, y + SHADOW_ROWS, x + SHADOW_COLS);
} else {
p->shadow = 0;
}
}
#endif
(void) refresh();

#if defined(HAVE_COLOR)
draw_childs_shadow(p);
#endif
}
}
}





void
dlg_will_resize(WINDOW *win)
{
int n, base;
int caught = 0;

dialog_state.had_resize = TRUE;
dlg_trace_win(win);
wtimeout(win, WTIMEOUT_VAL * 5);

for (n = base = 0; n < base + 10; ++n) {
int ch;

if ((ch = wgetch(win)) != ERR) {
if (ch == KEY_RESIZE) {
base = n;
++caught;
} else if (ch != ERR) {
ungetch(ch);
break;
}
}
}
dlg_reset_timeout(win);
DLG_TRACE(("#caught %d KEY_RESIZE key%s\n",
1 + caught,
caught == 1 ? "" : "s"));
}
#endif

WINDOW *
dlg_der_window(WINDOW *parent, int height, int width, int y, int x)
{
WINDOW *win;




if ((win = derwin(parent, height, width, y, x)) != 0) {
add_subwindow(parent, win);
(void) keypad(win, TRUE);
}
return win;
}

WINDOW *
dlg_sub_window(WINDOW *parent, int height, int width, int y, int x)
{
WINDOW *win;

if ((win = subwin(parent, height, width, y, x)) == 0) {
dlg_exiterr("Can't make sub-window at (%d,%d), size (%d,%d).\n",
y, x, height, width);
}

add_subwindow(parent, win);
(void) keypad(win, TRUE);
return win;
}


int
dlg_default_item(char **items, int llen)
{
int result = 0;

if (dialog_vars.default_item != 0) {
int count = 0;
while (*items != 0) {
if (!strcmp(dialog_vars.default_item, *items)) {
result = count;
break;
}
items += llen;
count++;
}
}
return result;
}

int
dlg_default_listitem(DIALOG_LISTITEM * items)
{
int result = 0;

if (dialog_vars.default_item != 0) {
int count = 0;
while (items->name != 0) {
if (!strcmp(dialog_vars.default_item, items->name)) {
result = count;
break;
}
++items;
count++;
}
}
return result;
}




void
dlg_item_help(const char *txt)
{
if (USE_ITEM_HELP(txt)) {
chtype attr = A_NORMAL;

dlg_attrset(stdscr, itemhelp_attr);
(void) wmove(stdscr, LINES - 1, 0);
(void) wclrtoeol(stdscr);
(void) addch(' ');
dlg_print_text(stdscr, txt, COLS - 1, &attr);

if (itemhelp_attr & A_COLOR) {
int y, x;

getyx(stdscr, y, x);
(void) y;
while (x < COLS) {
(void) addch(' ');
++x;
}
}
(void) wnoutrefresh(stdscr);
}
}

#if !defined(HAVE_STRCASECMP)
int
dlg_strcmp(const char *a, const char *b)
{
int ac, bc, cmp;

for (;;) {
ac = UCH(*a++);
bc = UCH(*b++);
if (isalpha(ac) && islower(ac))
ac = _toupper(ac);
if (isalpha(bc) && islower(bc))
bc = _toupper(bc);
cmp = ac - bc;
if (ac == 0 || bc == 0 || cmp != 0)
break;
}
return cmp;
}
#endif





static bool
trim_blank(char *base, char *dst)
{
int count = !!isblank(UCH(*dst));

while (dst-- != base) {
if (*dst == '\n') {
break;
} else if (isblank(UCH(*dst))) {
count++;
} else {
break;
}
}
return (count > 1);
}







void
dlg_trim_string(char *s)
{
char *base = s;
char *p1;
char *p = s;
int has_newlines = !dialog_vars.no_nl_expand && (strstr(s, "\\n") != 0);

while (*p != '\0') {
if (*p == TAB && !dialog_vars.nocollapse)
*p = ' ';

if (has_newlines) {
if (*p == '\\' && *(p + 1) == 'n') {
*s++ = '\n';
p += 2;
p1 = p;





while (isblank(UCH(*p1)))
p1++;
if (*p1 == '\n')
p = p1 + 1;
} else if (*p == '\n') {
if (dialog_vars.cr_wrap)
*s++ = *p++;
else {

if (!trim_blank(base, p))
*s++ = ' ';
p++;
}
} else
*s++ = *p++;
} else if (dialog_vars.trim_whitespace) {
if (isblank(UCH(*p))) {
if (!isblank(UCH(*(s - 1)))) {
*s++ = ' ';
p++;
} else
p++;
} else if (*p == '\n') {
if (dialog_vars.cr_wrap)
*s++ = *p++;
else if (!isblank(UCH(*(s - 1)))) {

*s++ = ' ';
p++;
} else
p++;
} else
*s++ = *p++;
} else {
if (isblank(UCH(*p)) && !dialog_vars.nocollapse) {
if (!trim_blank(base, p))
*s++ = *p;
p++;
} else
*s++ = *p++;
}
}

*s = '\0';
}

void
dlg_set_focus(WINDOW *parent, WINDOW *win)
{
if (win != 0) {
(void) wmove(parent,
getpary(win) + getcury(win),
getparx(win) + getcurx(win));
(void) wnoutrefresh(win);
(void) doupdate();
}
}




int
dlg_max_input(int max_len)
{
if (dialog_vars.max_input != 0 && dialog_vars.max_input < MAX_LEN)
max_len = dialog_vars.max_input;

return max_len;
}




void
dlg_clr_result(void)
{
if (dialog_vars.input_length) {
dialog_vars.input_length = 0;
if (dialog_vars.input_result)
free(dialog_vars.input_result);
}
dialog_vars.input_result = 0;
}




char *
dlg_set_result(const char *string)
{
unsigned need = string ? (unsigned) strlen(string) + 1 : 0;


if (need < MAX_LEN)
need = MAX_LEN;




if (dialog_vars.input_length != 0
|| dialog_vars.input_result == 0
|| need > MAX_LEN) {

dlg_clr_result();

dialog_vars.input_length = need;
dialog_vars.input_result = dlg_malloc(char, need);
assert_ptr(dialog_vars.input_result, "dlg_set_result");
}

strcpy(dialog_vars.input_result, string ? string : "");

return dialog_vars.input_result;
}





void
dlg_add_result(const char *string)
{
unsigned have = (dialog_vars.input_result
? (unsigned) strlen(dialog_vars.input_result)
: 0);
unsigned want = (unsigned) strlen(string) + 1 + have;

if ((want >= MAX_LEN)
|| (dialog_vars.input_length != 0)
|| (dialog_vars.input_result == 0)) {

if (dialog_vars.input_length == 0
|| dialog_vars.input_result == 0) {

char *save_result = dialog_vars.input_result;

dialog_vars.input_length = want * 2;
dialog_vars.input_result = dlg_malloc(char, dialog_vars.input_length);
assert_ptr(dialog_vars.input_result, "dlg_add_result malloc");
dialog_vars.input_result[0] = '\0';
if (save_result != 0)
strcpy(dialog_vars.input_result, save_result);
} else if (want >= dialog_vars.input_length) {
dialog_vars.input_length = want * 2;
dialog_vars.input_result = dlg_realloc(char,
dialog_vars.input_length,
dialog_vars.input_result);
assert_ptr(dialog_vars.input_result, "dlg_add_result realloc");
}
}
strcat(dialog_vars.input_result, string);
}





#define FIX_SINGLE "\n\\"
#define FIX_DOUBLE FIX_SINGLE "[]{}?*;`~#$^&()|<>"




static const char *
quote_delimiter(void)
{
return dialog_vars.single_quoted ? "'" : "\"";
}




static bool
must_quote(char *string)
{
bool code = FALSE;

if (*string != '\0') {
size_t len = strlen(string);
if (strcspn(string, quote_delimiter()) != len)
code = TRUE;
else if (strcspn(string, "\n\t ") != len)
code = TRUE;
else
code = (strcspn(string, FIX_DOUBLE) != len);
} else {
code = TRUE;
}

return code;
}




void
dlg_add_quoted(char *string)
{
char temp[2];
const char *my_quote = quote_delimiter();
const char *must_fix = (dialog_vars.single_quoted
? FIX_SINGLE
: FIX_DOUBLE);

if (must_quote(string)) {
temp[1] = '\0';
dlg_add_result(my_quote);
while (*string != '\0') {
temp[0] = *string++;
if ((strchr) (my_quote, *temp) || (strchr) (must_fix, *temp))
dlg_add_result("\\");
dlg_add_result(temp);
}
dlg_add_result(my_quote);
} else {
dlg_add_result(string);
}
}




void
dlg_add_string(char *string)
{
if (dialog_vars.quoted) {
dlg_add_quoted(string);
} else {
dlg_add_result(string);
}
}

bool
dlg_need_separator(void)
{
bool result = FALSE;

if (dialog_vars.output_separator) {
result = TRUE;
} else if (dialog_vars.input_result && *(dialog_vars.input_result)) {
result = TRUE;
}
return result;
}

void
dlg_add_separator(void)
{
const char *separator = (dialog_vars.separate_output) ? "\n" : " ";

if (dialog_vars.output_separator)
separator = dialog_vars.output_separator;

dlg_add_result(separator);
}

#define HELP_PREFIX "HELP "

void
dlg_add_help_listitem(int *result, char **tag, DIALOG_LISTITEM * item)
{
dlg_add_result(HELP_PREFIX);
if (USE_ITEM_HELP(item->help)) {
*tag = dialog_vars.help_tags ? item->name : item->help;
*result = DLG_EXIT_ITEM_HELP;
} else {
*tag = item->name;
}
}

void
dlg_add_help_formitem(int *result, char **tag, DIALOG_FORMITEM * item)
{
dlg_add_result(HELP_PREFIX);
if (USE_ITEM_HELP(item->help)) {
*tag = dialog_vars.help_tags ? item->name : item->help;
*result = DLG_EXIT_ITEM_HELP;
} else {
*tag = item->name;
}
}





void
dlg_save_vars(DIALOG_VARS * vars)
{
*vars = dialog_vars;
}






void
dlg_restore_vars(DIALOG_VARS * vars)
{
char *save_result = dialog_vars.input_result;
unsigned save_length = dialog_vars.input_length;

dialog_vars = *vars;
dialog_vars.input_result = save_result;
dialog_vars.input_length = save_length;
}




void
dlg_does_output(void)
{
dialog_state.output_count += 1;
}




#if !(defined(HAVE_GETBEGX) && defined(HAVE_GETBEGY))
int
dlg_getbegx(WINDOW *win)
{
int y, x;
getbegyx(win, y, x);
return x;
}
int
dlg_getbegy(WINDOW *win)
{
int y, x;
getbegyx(win, y, x);
return y;
}
#endif

#if !(defined(HAVE_GETCURX) && defined(HAVE_GETCURY))
int
dlg_getcurx(WINDOW *win)
{
int y, x;
getyx(win, y, x);
return x;
}
int
dlg_getcury(WINDOW *win)
{
int y, x;
getyx(win, y, x);
return y;
}
#endif

#if !(defined(HAVE_GETMAXX) && defined(HAVE_GETMAXY))
int
dlg_getmaxx(WINDOW *win)
{
int y, x;
getmaxyx(win, y, x);
return x;
}
int
dlg_getmaxy(WINDOW *win)
{
int y, x;
getmaxyx(win, y, x);
return y;
}
#endif

#if !(defined(HAVE_GETPARX) && defined(HAVE_GETPARY))
int
dlg_getparx(WINDOW *win)
{
int y, x;
getparyx(win, y, x);
return x;
}
int
dlg_getpary(WINDOW *win)
{
int y, x;
getparyx(win, y, x);
return y;
}
#endif

#if defined(NEED_WGETPARENT)
WINDOW *
dlg_wgetparent(WINDOW *win)
{
#undef wgetparent
WINDOW *result = 0;
DIALOG_WINDOWS *p;

for (p = dialog_state.all_subwindows; p != 0; p = p->next) {
if (p->shadow == win) {
result = p->normal;
break;
}
}
return result;
}
#endif
