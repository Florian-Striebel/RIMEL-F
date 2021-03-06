























































#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

char *ee_copyright_message =
"Copyright (c) 1986, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 2009 Hugh Mahon ";

#include "ee_version.h"

char *version = "@(#) ee, version " EE_VERSION " $Revision: 1.104 $";

#if defined(NCURSE)
#include "new_curse.h"
#elif HAS_NCURSES
#include <ncurses.h>
#else
#include <curses.h>
#endif

#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <locale.h>

#if defined(HAS_SYS_WAIT)
#include <sys/wait.h>
#endif

#if defined(HAS_STDLIB)
#include <stdlib.h>
#endif

#if defined(HAS_STDARG)
#include <stdarg.h>
#endif

#if defined(HAS_UNISTD)
#include <unistd.h>
#endif

#if !defined(NO_CATGETS)
#include <nl_types.h>

nl_catd catalog;
#else
#define catgetlocal(a, b) (b)
#endif

#if !defined(SIGCHLD)
#define SIGCHLD SIGCLD
#endif

#define TAB 9
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)





#define CONTROL_KEYS 1
#define COMMANDS 2

struct text {
unsigned char *line;
int line_number;
int line_length;
int max_length;
struct text *next_line;
struct text *prev_line;
};

struct text *first_line;
struct text *dlt_line;
struct text *curr_line;
struct text *tmp_line;
struct text *srch_line;

struct files {
unsigned char *name;
struct files *next_name;
};

struct files *top_of_stack = NULL;

int d_wrd_len;
int position;
int scr_pos;
int scr_vert;
int scr_horz;
int absolute_lin;
int tmp_vert, tmp_horz;
int input_file;
int recv_file;
int edit;
int gold;
int fildes;
int case_sen;
int last_line;
int last_col;
int horiz_offset = 0;
int clear_com_win;
int text_changes = FALSE;
int get_fd;
int info_window = TRUE;
int info_type = CONTROL_KEYS;
int expand_tabs = TRUE;
int right_margin = 0;
int observ_margins = TRUE;
int shell_fork;
int temp_stdin;
int temp_stdout;
int temp_stderr;
int pipe_out[2];
int pipe_in[2];
int out_pipe;
int in_pipe;
int formatted = FALSE;
int auto_format = FALSE;
int restricted = FALSE;
int nohighlight = FALSE;
int eightbit = TRUE;
int local_LINES = 0;
int local_COLS = 0;
int curses_initialized = FALSE;
int emacs_keys_mode = FALSE;
int ee_chinese = FALSE;




unsigned char *point;
unsigned char *srch_str;
unsigned char *u_srch_str;
unsigned char *srch_1;
unsigned char *srch_2;
unsigned char *srch_3;
unsigned char *in_file_name = NULL;
char *tmp_file;
unsigned char *d_char;
unsigned char *d_word;
unsigned char *d_line;
char in_string[513];
unsigned char *print_command = (unsigned char *)"lpr";
unsigned char *start_at_line = NULL;
int in;

FILE *temp_fp;
FILE *bit_bucket;

char *table[] = {
"^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "^H", "\t", "^J",
"^K", "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S", "^T", "^U",
"^V", "^W", "^X", "^Y", "^Z", "^[", "^\\", "^]", "^^", "^_"
};

WINDOW *com_win;
WINDOW *text_win;
WINDOW *help_win;
WINDOW *info_win;

#if defined(__STDC__) || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif





















struct menu_entries {
char *item_string;
int (*procedure)P_((struct menu_entries *));
struct menu_entries *ptr_argument;
int (*iprocedure)P_((int));
void (*nprocedure)P_((void));
int argument;
};

int main P_((int argc, char *argv[]));
unsigned char *resiz_line P_((int factor, struct text *rline, int rpos));
void insert P_((int character));
void delete P_((int disp));
void scanline P_((unsigned char *pos));
int tabshift P_((int temp_int));
int out_char P_((WINDOW *window, int character, int column));
int len_char P_((int character, int column));
void draw_line P_((int vertical, int horiz, unsigned char *ptr, int t_pos, int length));
void insert_line P_((int disp));
struct text *txtalloc P_((void));
struct files *name_alloc P_((void));
unsigned char *next_word P_((unsigned char *string));
void prev_word P_((void));
void control P_((void));
void emacs_control P_((void));
void bottom P_((void));
void top P_((void));
void nextline P_((void));
void prevline P_((void));
void left P_((int disp));
void right P_((int disp));
void find_pos P_((void));
void up P_((void));
void down P_((void));
void function_key P_((void));
void print_buffer P_((void));
void command_prompt P_((void));
void command P_((char *cmd_str1));
int scan P_((char *line, int offset, int column));
char *get_string P_((char *prompt, int advance));
int compare P_((char *string1, char *string2, int sensitive));
void goto_line P_((char *cmd_str));
void midscreen P_((int line, unsigned char *pnt));
void get_options P_((int numargs, char *arguments[]));
void check_fp P_((void));
void get_file P_((char *file_name));
void get_line P_((int length, unsigned char *in_string, int *append));
void draw_screen P_((void));
void finish P_((void));
int quit P_((int noverify));
void edit_abort P_((int arg));
void delete_text P_((void));
int write_file P_((char *file_name, int warn_if_exists));
int search P_((int display_message));
void search_prompt P_((void));
void del_char P_((void));
void undel_char P_((void));
void del_word P_((void));
void undel_word P_((void));
void del_line P_((void));
void undel_line P_((void));
void adv_word P_((void));
void move_rel P_((int direction, int lines));
void eol P_((void));
void bol P_((void));
void adv_line P_((void));
void sh_command P_((char *string));
void set_up_term P_((void));
void resize_check P_((void));
int menu_op P_((struct menu_entries *));
void paint_menu P_((struct menu_entries menu_list[], int max_width, int max_height, int list_size, int top_offset, WINDOW *menu_win, int off_start, int vert_size));
void help P_((void));
void paint_info_win P_((void));
void no_info_window P_((void));
void create_info_window P_((void));
int file_op P_((int arg));
void shell_op P_((void));
void leave_op P_((void));
void redraw P_((void));
int Blank_Line P_((struct text *test_line));
void Format P_((void));
void ee_init P_((void));
void dump_ee_conf P_((void));
void echo_string P_((char *string));
void spell_op P_((void));
void ispell_op P_((void));
int first_word_len P_((struct text *test_line));
void Auto_Format P_((void));
void modes_op P_((void));
char *is_in_string P_((char *string, char *substring));
char *resolve_name P_((char *name));
int restrict_mode P_((void));
int unique_test P_((char *string, char *list[]));
void strings_init P_((void));

#undef P_




struct menu_entries modes_menu[] = {
{"", NULL, NULL, NULL, NULL, 0},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, dump_ee_conf, -1},
{NULL, NULL, NULL, NULL, NULL, -1}
};

char *mode_strings[11];

#define NUM_MODES_ITEMS 10

struct menu_entries config_dump_menu[] = {
{"", NULL, NULL, NULL, NULL, 0},
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, NULL, -1},
{NULL, NULL, NULL, NULL, NULL, -1}
};

struct menu_entries leave_menu[] = {
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, finish, -1},
{"", NULL, NULL, quit, NULL, TRUE},
{NULL, NULL, NULL, NULL, NULL, -1}
};

#define READ_FILE 1
#define WRITE_FILE 2
#define SAVE_FILE 3

struct menu_entries file_menu[] = {
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, file_op, NULL, READ_FILE},
{"", NULL, NULL, file_op, NULL, WRITE_FILE},
{"", NULL, NULL, file_op, NULL, SAVE_FILE},
{"", NULL, NULL, NULL, print_buffer, -1},
{NULL, NULL, NULL, NULL, NULL, -1}
};

struct menu_entries search_menu[] = {
{"", NULL, NULL, NULL, NULL, 0},
{"", NULL, NULL, NULL, search_prompt, -1},
{"", NULL, NULL, search, NULL, TRUE},
{NULL, NULL, NULL, NULL, NULL, -1}
};

struct menu_entries spell_menu[] = {
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, spell_op, -1},
{"", NULL, NULL, NULL, ispell_op, -1},
{NULL, NULL, NULL, NULL, NULL, -1}
};

struct menu_entries misc_menu[] = {
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, Format, -1},
{"", NULL, NULL, NULL, shell_op, -1},
{"", menu_op, spell_menu, NULL, NULL, -1},
{NULL, NULL, NULL, NULL, NULL, -1}
};

struct menu_entries main_menu[] = {
{"", NULL, NULL, NULL, NULL, -1},
{"", NULL, NULL, NULL, leave_op, -1},
{"", NULL, NULL, NULL, help, -1},
{"", menu_op, file_menu, NULL, NULL, -1},
{"", NULL, NULL, NULL, redraw, -1},
{"", NULL, NULL, NULL, modes_op, -1},
{"", menu_op, search_menu, NULL, NULL, -1},
{"", menu_op, misc_menu, NULL, NULL, -1},
{NULL, NULL, NULL, NULL, NULL, -1}
};

char *help_text[23];
char *control_keys[5];

char *emacs_help_text[22];
char *emacs_control_keys[5];

char *command_strings[5];
char *commands[32];
char *init_strings[22];

#define MENU_WARN 1

#define max_alpha_char 36





char *com_win_message;
char *no_file_string;
char *ascii_code_str;
char *printer_msg_str;
char *command_str;
char *file_write_prompt_str;
char *file_read_prompt_str;
char *char_str;
char *unkn_cmd_str;
char *non_unique_cmd_msg;
char *line_num_str;
char *line_len_str;
char *current_file_str;
char *usage0;
char *usage1;
char *usage2;
char *usage3;
char *usage4;
char *file_is_dir_msg;
char *new_file_msg;
char *cant_open_msg;
char *open_file_msg;
char *file_read_fin_msg;
char *reading_file_msg;
char *read_only_msg;
char *file_read_lines_msg;
char *save_file_name_prompt;
char *file_not_saved_msg;
char *changes_made_prompt;
char *yes_char;
char *file_exists_prompt;
char *create_file_fail_msg;
char *writing_file_msg;
char *file_written_msg;
char *searching_msg;
char *str_not_found_msg;
char *search_prompt_str;
char *exec_err_msg;
char *continue_msg;
char *menu_cancel_msg;
char *menu_size_err_msg;
char *press_any_key_msg;
char *shell_prompt;
char *formatting_msg;
char *shell_echo_msg;
char *spell_in_prog_msg;
char *margin_prompt;
char *restricted_msg;
char *ON;
char *OFF;
char *HELP;
char *WRITE;
char *READ;
char *LINE;
char *FILE_str;
char *CHARACTER;
char *REDRAW;
char *RESEQUENCE;
char *AUTHOR;
char *VERSION;
char *CASE;
char *NOCASE;
char *EXPAND;
char *NOEXPAND;
char *Exit_string;
char *QUIT_string;
char *INFO;
char *NOINFO;
char *MARGINS;
char *NOMARGINS;
char *AUTOFORMAT;
char *NOAUTOFORMAT;
char *Echo;
char *PRINTCOMMAND;
char *RIGHTMARGIN;
char *HIGHLIGHT;
char *NOHIGHLIGHT;
char *EIGHTBIT;
char *NOEIGHTBIT;
char *EMACS_string;
char *NOEMACS_string;
char *conf_dump_err_msg;
char *conf_dump_success_msg;
char *conf_not_saved_msg;
char *ree_no_file_msg;
char *cancel_string;
char *menu_too_lrg_msg;
char *more_above_str, *more_below_str;
char *separator = "===============================================================================";

char *chinese_cmd, *nochinese_cmd;

#if !defined(__STDC__)
#if !defined(HAS_STDLIB)
extern char *malloc();
extern char *realloc();
extern char *getenv();
FILE *fopen();
#endif
#endif

int
main(argc, argv)
int argc;
char *argv[];
{
int counter;

for (counter = 1; counter < 24; counter++)
signal(counter, SIG_IGN);


if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) {
fprintf(stderr,
"ee's standard input and output must be a terminal\n");
exit(1);
}

signal(SIGCHLD, SIG_DFL);
signal(SIGSEGV, SIG_DFL);
signal(SIGINT, edit_abort);
d_char = malloc(3);
d_word = malloc(150);
*d_word = '\0';
d_line = NULL;
dlt_line = txtalloc();
dlt_line->line = d_line;
dlt_line->line_length = 0;
curr_line = first_line = txtalloc();
curr_line->line = point = malloc(10);
curr_line->line_length = 1;
curr_line->max_length = 10;
curr_line->prev_line = NULL;
curr_line->next_line = NULL;
curr_line->line_number = 1;
srch_str = NULL;
u_srch_str = NULL;
position = 1;
scr_pos =0;
scr_vert = 0;
scr_horz = 0;
absolute_lin = 1;
bit_bucket = fopen("/dev/null", "w");
edit = TRUE;
gold = case_sen = FALSE;
shell_fork = TRUE;
strings_init();
ee_init();
if (argc > 0 )
get_options(argc, argv);
set_up_term();
if (right_margin == 0)
right_margin = COLS - 1;
if (top_of_stack == NULL)
{
if (restrict_mode())
{
wmove(com_win, 0, 0);
werase(com_win);
wprintw(com_win, ree_no_file_msg);
wrefresh(com_win);
edit_abort(0);
}
wprintw(com_win, no_file_string);
wrefresh(com_win);
}
else
check_fp();

clear_com_win = TRUE;

counter = 0;

while(edit)
{



if (info_window)
{
if (!nohighlight)
wstandout(info_win);
wmove(info_win, 5, 0);
wprintw(info_win, separator);
wmove(info_win, 5, 5);
wprintw(info_win, "line %d col %d lines from top %d ",
curr_line->line_number, scr_horz, absolute_lin);
wstandend(info_win);
wrefresh(info_win);
}

wrefresh(text_win);
in = wgetch(text_win);
if (in == -1)
exit(0);



resize_check();

if (clear_com_win)
{
clear_com_win = FALSE;
wmove(com_win, 0, 0);
werase(com_win);
if (!info_window)
{
wprintw(com_win, "%s", com_win_message);
}
wrefresh(com_win);
}

if (in > 255)
function_key();
else if ((in == '\10') || (in == 127))
{
in = 8;
delete(TRUE);
}
else if ((in > 31) || (in == 9))
insert(in);
else if ((in >= 0) && (in <= 31))
{
if (emacs_keys_mode)
emacs_control();
else
control();
}
}
return(0);
}

unsigned char *
resiz_line(factor, rline, rpos)
int factor;
struct text *rline;
int rpos;
{
unsigned char *rpoint;
int resiz_var;

rline->max_length += factor;
rpoint = rline->line = realloc(rline->line, rline->max_length );
for (resiz_var = 1 ; (resiz_var < rpos) ; resiz_var++)
rpoint++;
return(rpoint);
}

void
insert(character)
int character;
{
int counter;
int value;
unsigned char *temp;
unsigned char *temp2;

if ((character == '\011') && (expand_tabs))
{
counter = len_char('\011', scr_horz);
for (; counter > 0; counter--)
insert(' ');
if (auto_format)
Auto_Format();
return;
}
text_changes = TRUE;
if ((curr_line->max_length - curr_line->line_length) < 5)
point = resiz_line(10, curr_line, position);
curr_line->line_length++;
temp = point;
counter = position;
while (counter < curr_line->line_length)
{
counter++;
temp++;
}
temp++;
while (point < temp)
{
temp2=temp - 1;
*temp= *temp2;
temp--;
}
*point = character;
wclrtoeol(text_win);
if (!isprint((unsigned char)character))
{
scr_pos = scr_horz += out_char(text_win, character, scr_horz);
point++;
position++;
}
else
{
waddch(text_win, (unsigned char)character);
scr_pos = ++scr_horz;
point++;
position ++;
}

if ((observ_margins) && (right_margin < scr_pos))
{
counter = position;
while (scr_pos > right_margin)
prev_word();
if (scr_pos == 0)
{
while (position < counter)
right(TRUE);
}
else
{
counter -= position;
insert_line(TRUE);
for (value = 0; value < counter; value++)
right(TRUE);
}
}

if ((scr_horz - horiz_offset) > last_col)
{
horiz_offset += 8;
midscreen(scr_vert, point);
}

if ((auto_format) && (character == ' ') && (!formatted))
Auto_Format();
else if ((character != ' ') && (character != '\t'))
formatted = FALSE;

draw_line(scr_vert, scr_horz, point, position, curr_line->line_length);
}

void
delete(disp)
int disp;
{
unsigned char *tp;
unsigned char *temp2;
struct text *temp_buff;
int temp_vert;
int temp_pos;
int del_width = 1;

if (point != curr_line->line)
{
text_changes = TRUE;
temp2 = tp = point;
if ((ee_chinese) && (position >= 2) && (*(point - 2) > 127))
{
del_width = 2;
}
tp -= del_width;
point -= del_width;
position -= del_width;
temp_pos = position;
curr_line->line_length -= del_width;
if ((*tp < ' ') || (*tp >= 127))
scanline(tp);
else
scr_horz -= del_width;
scr_pos = scr_horz;
if (in == 8)
{
if (del_width == 1)
*d_char = *point;
else
{
d_char[0] = *point;
d_char[1] = *(point + 1);
}
d_char[del_width] = '\0';
}
while (temp_pos <= curr_line->line_length)
{
temp_pos++;
*tp = *temp2;
tp++;
temp2++;
}
if ((scr_horz < horiz_offset) && (horiz_offset > 0))
{
horiz_offset -= 8;
midscreen(scr_vert, point);
}
}
else if (curr_line->prev_line != NULL)
{
text_changes = TRUE;
left(disp);
temp_buff = curr_line->next_line;
point = resiz_line(temp_buff->line_length, curr_line, position);
if (temp_buff->next_line != NULL)
temp_buff->next_line->prev_line = curr_line;
curr_line->next_line = temp_buff->next_line;
temp2 = temp_buff->line;
if (in == 8)
{
d_char[0] = '\n';
d_char[1] = '\0';
}
tp = point;
temp_pos = 1;
while (temp_pos < temp_buff->line_length)
{
curr_line->line_length++;
temp_pos++;
*tp = *temp2;
tp++;
temp2++;
}
*tp = '\0';
free(temp_buff->line);
free(temp_buff);
temp_buff = curr_line;
temp_vert = scr_vert;
scr_pos = scr_horz;
if (scr_vert < last_line)
{
wmove(text_win, scr_vert + 1, 0);
wdeleteln(text_win);
}
while ((temp_buff != NULL) && (temp_vert < last_line))
{
temp_buff = temp_buff->next_line;
temp_vert++;
}
if ((temp_vert == last_line) && (temp_buff != NULL))
{
tp = temp_buff->line;
wmove(text_win, last_line,0);
wclrtobot(text_win);
draw_line(last_line, 0, tp, 1, temp_buff->line_length);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
}
}
draw_line(scr_vert, scr_horz, point, position, curr_line->line_length);
formatted = FALSE;
}

void
scanline(pos)
unsigned char *pos;
{
int temp;
unsigned char *ptr;

ptr = curr_line->line;
temp = 0;
while (ptr < pos)
{
if (*ptr <= 8)
temp += 2;
else if (*ptr == 9)
temp += tabshift(temp);
else if ((*ptr >= 10) && (*ptr <= 31))
temp += 2;
else if ((*ptr >= 32) && (*ptr < 127))
temp++;
else if (*ptr == 127)
temp += 2;
else if (!eightbit)
temp += 5;
else
temp++;
ptr++;
}
scr_horz = temp;
if ((scr_horz - horiz_offset) > last_col)
{
horiz_offset = (scr_horz - (scr_horz % 8)) - (COLS - 8);
midscreen(scr_vert, point);
}
else if (scr_horz < horiz_offset)
{
horiz_offset = max(0, (scr_horz - (scr_horz % 8)));
midscreen(scr_vert, point);
}
}

int
tabshift(temp_int)
int temp_int;
{
int leftover;

leftover = ((temp_int + 1) % 8);
if (leftover == 0)
return (1);
else
return (9 - leftover);
}

int
out_char(window, character, column)
WINDOW *window;
int character;
int column;
{
int i1, i2;
char *string;
char string2[8];

if (character == TAB)
{
i1 = tabshift(column);
for (i2 = 0;
(i2 < i1) && (((column+i2+1)-horiz_offset) < last_col); i2++)
{
waddch(window, ' ');
}
return(i1);
}
else if ((character >= '\0') && (character < ' '))
{
string = table[(int) character];
}
else if ((character < 0) || (character >= 127))
{
if (character == 127)
string = "^?";
else if (!eightbit)
{
sprintf(string2, "<%d>", (character < 0) ? (character + 256) : character);
string = string2;
}
else
{
waddch(window, (unsigned char)character );
return(1);
}
}
else
{
waddch(window, (unsigned char)character);
return(1);
}
for (i2 = 0; (string[i2] != '\0') && (((column+i2+1)-horiz_offset) < last_col); i2++)
waddch(window, (unsigned char)string[i2]);
return(strlen(string));
}

int
len_char(character, column)
int character;
int column;
{
int length;

if (character == '\t')
length = tabshift(column);
else if ((character >= 0) && (character < 32))
length = 2;
else if ((character >= 32) && (character <= 126))
length = 1;
else if (character == 127)
length = 2;
else if (((character > 126) || (character < 0)) && (!eightbit))
length = 5;
else
length = 1;

return(length);
}

void
draw_line(vertical, horiz, ptr, t_pos, length)
int vertical;
int horiz;
unsigned char *ptr;
int t_pos;
int length;
{
int d;
unsigned char *temp;
int abs_column;
int column;
int row;
int posit;

abs_column = horiz;
column = horiz - horiz_offset;
row = vertical;
temp = ptr;
d = 0;
posit = t_pos;
if (column < 0)
{
wmove(text_win, row, 0);
wclrtoeol(text_win);
}
while (column < 0)
{
d = len_char(*temp, abs_column);
abs_column += d;
column += d;
posit++;
temp++;
}
wmove(text_win, row, column);
wclrtoeol(text_win);
while ((posit < length) && (column <= last_col))
{
if (!isprint(*temp))
{
column += len_char(*temp, abs_column);
abs_column += out_char(text_win, *temp, abs_column);
}
else
{
abs_column++;
column++;
waddch(text_win, *temp);
}
posit++;
temp++;
}
if (column < last_col)
wclrtoeol(text_win);
wmove(text_win, vertical, (horiz - horiz_offset));
}

void
insert_line(disp)
int disp;
{
int temp_pos;
int temp_pos2;
unsigned char *temp;
unsigned char *extra;
struct text *temp_nod;

text_changes = TRUE;
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
wclrtoeol(text_win);
temp_nod= txtalloc();
temp_nod->line = extra= malloc(10);
temp_nod->line_length = 1;
temp_nod->max_length = 10;
temp_nod->line_number = curr_line->line_number + 1;
temp_nod->next_line = curr_line->next_line;
if (temp_nod->next_line != NULL)
temp_nod->next_line->prev_line = temp_nod;
temp_nod->prev_line = curr_line;
curr_line->next_line = temp_nod;
temp_pos2 = position;
temp = point;
if (temp_pos2 < curr_line->line_length)
{
temp_pos = 1;
while (temp_pos2 < curr_line->line_length)
{
if ((temp_nod->max_length - temp_nod->line_length)< 5)
extra = resiz_line(10, temp_nod, temp_pos);
temp_nod->line_length++;
temp_pos++;
temp_pos2++;
*extra= *temp;
extra++;
temp++;
}
temp=point;
*temp = '\0';
temp = resiz_line((1 - temp_nod->line_length), curr_line, position);
curr_line->line_length = 1 + temp - curr_line->line;
}
curr_line->line_length = position;
absolute_lin++;
curr_line = temp_nod;
*extra = '\0';
position = 1;
point= curr_line->line;
if (disp)
{
if (scr_vert < last_line)
{
scr_vert++;
wclrtoeol(text_win);
wmove(text_win, scr_vert, 0);
winsertln(text_win);
}
else
{
wmove(text_win, 0,0);
wdeleteln(text_win);
wmove(text_win, last_line,0);
wclrtobot(text_win);
}
scr_pos = scr_horz = 0;
if (horiz_offset)
{
horiz_offset = 0;
midscreen(scr_vert, point);
}
draw_line(scr_vert, scr_horz, point, position,
curr_line->line_length);
}
}

struct text *txtalloc()
{
return((struct text *) malloc(sizeof( struct text)));
}

struct files *name_alloc()
{
return((struct files *) malloc(sizeof( struct files)));
}

unsigned char *next_word(string)
unsigned char *string;
{
while ((*string != '\0') && ((*string != 32) && (*string != 9)))
string++;
while ((*string != '\0') && ((*string == 32) || (*string == 9)))
string++;
return(string);
}

void
prev_word()
{
if (position != 1)
{
if ((position != 1) && ((point[-1] == ' ') || (point[-1] == '\t')))
{
while ((position != 1) && ((*point != ' ') && (*point != '\t')))
left(TRUE);
}
while ((position != 1) && ((*point == ' ') || (*point == '\t')))
left(TRUE);
while ((position != 1) && ((*point != ' ') && (*point != '\t')))
left(TRUE);
if ((position != 1) && ((*point == ' ') || (*point == '\t')))
right(TRUE);
}
else
left(TRUE);
}

void
control()
{
char *string;

if (in == 1)
{
string = get_string(ascii_code_str, TRUE);
if (*string != '\0')
{
in = atoi(string);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
insert(in);
}
free(string);
}
else if (in == 2)
bottom();
else if (in == 3)
{
command_prompt();
}
else if (in == 4)
down();
else if (in == 5)
search_prompt();
else if (in == 6)
undel_char();
else if (in == 7)
bol();
else if (in == 8)
delete(TRUE);
else if (in == 9)
;
else if (in == 10)
insert_line(TRUE);
else if (in == 11)
del_char();
else if (in == 12)
left(TRUE);
else if (in == 13)
insert_line(TRUE);
else if (in == 14)
move_rel('d', max(5, (last_line - 5)));
else if (in == 15)
eol();
else if (in == 16)
move_rel('u', max(5, (last_line - 5)));
else if (in == 17)
;
else if (in == 18)
right(TRUE);
else if (in == 19)
;
else if (in == 20)
top();
else if (in == 21)
up();
else if (in == 22)
undel_word();
else if (in == 23)
del_word();
else if (in == 24)
search(TRUE);
else if (in == 25)
del_line();
else if (in == 26)
undel_line();
else if (in == 27)
{
menu_op(main_menu);
}
}





void
emacs_control()
{
char *string;

if (in == 1)
bol();
else if (in == 2)
left(TRUE);
else if (in == 3)
{
command_prompt();
}
else if (in == 4)
del_char();
else if (in == 5)
eol();
else if (in == 6)
right(TRUE);
else if (in == 7)
move_rel('u', max(5, (last_line - 5)));
else if (in == 8)
delete(TRUE);
else if (in == 9)
;
else if (in == 10)
undel_char();
else if (in == 11)
del_line();
else if (in == 12)
undel_line();
else if (in == 13)
insert_line(TRUE);
else if (in == 14)
down();
else if (in == 15)
{
string = get_string(ascii_code_str, TRUE);
if (*string != '\0')
{
in = atoi(string);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
insert(in);
}
free(string);
}
else if (in == 16)
up();
else if (in == 17)
;
else if (in == 18)
undel_word();
else if (in == 19)
;
else if (in == 20)
top();
else if (in == 21)
bottom();
else if (in == 22)
move_rel('d', max(5, (last_line - 5)));
else if (in == 23)
del_word();
else if (in == 24)
search(TRUE);
else if (in == 25)
search_prompt();
else if (in == 26)
adv_word();
else if (in == 27)
{
menu_op(main_menu);
}
}

void
bottom()
{
while (curr_line->next_line != NULL)
{
curr_line = curr_line->next_line;
absolute_lin++;
}
point = curr_line->line;
if (horiz_offset)
horiz_offset = 0;
position = 1;
midscreen(last_line, point);
scr_pos = scr_horz;
}

void
top()
{
while (curr_line->prev_line != NULL)
{
curr_line = curr_line->prev_line;
absolute_lin--;
}
point = curr_line->line;
if (horiz_offset)
horiz_offset = 0;
position = 1;
midscreen(0, point);
scr_pos = scr_horz;
}

void
nextline()
{
curr_line = curr_line->next_line;
absolute_lin++;
point = curr_line->line;
position = 1;
if (scr_vert == last_line)
{
wmove(text_win, 0,0);
wdeleteln(text_win);
wmove(text_win, last_line,0);
wclrtobot(text_win);
draw_line(last_line,0,point,1,curr_line->line_length);
}
else
scr_vert++;
}

void
prevline()
{
curr_line = curr_line->prev_line;
absolute_lin--;
point = curr_line->line;
position = 1;
if (scr_vert == 0)
{
winsertln(text_win);
draw_line(0,0,point,1,curr_line->line_length);
}
else
scr_vert--;
while (position < curr_line->line_length)
{
position++;
point++;
}
}

void
left(disp)
int disp;
{
if (point != curr_line->line)
{
if ((ee_chinese) && (position >= 2) && (*(point - 2) > 127))
{
point--;
position--;
}
point--;
position--;
scanline(point);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
scr_pos = scr_horz;
}
else if (curr_line->prev_line != NULL)
{
if (!disp)
{
absolute_lin--;
curr_line = curr_line->prev_line;
point = curr_line->line + curr_line->line_length;
position = curr_line->line_length;
return;
}
position = 1;
prevline();
scanline(point);
scr_pos = scr_horz;
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
}
}

void
right(disp)
int disp;
{
if (position < curr_line->line_length)
{
if ((ee_chinese) && (*point > 127) &&
((curr_line->line_length - position) >= 2))
{
point++;
position++;
}
point++;
position++;
scanline(point);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
scr_pos = scr_horz;
}
else if (curr_line->next_line != NULL)
{
if (!disp)
{
absolute_lin++;
curr_line = curr_line->next_line;
point = curr_line->line;
position = 1;
return;
}
nextline();
scr_pos = scr_horz = 0;
if (horiz_offset)
{
horiz_offset = 0;
midscreen(scr_vert, point);
}
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
position = 1;
}
}

void
find_pos()
{
scr_horz = 0;
position = 1;
while ((scr_horz < scr_pos) && (position < curr_line->line_length))
{
if (*point == 9)
scr_horz += tabshift(scr_horz);
else if (*point < ' ')
scr_horz += 2;
else if ((ee_chinese) && (*point > 127) &&
((curr_line->line_length - position) >= 2))
{
scr_horz += 2;
point++;
position++;
}
else
scr_horz++;
position++;
point++;
}
if ((scr_horz - horiz_offset) > last_col)
{
horiz_offset = (scr_horz - (scr_horz % 8)) - (COLS - 8);
midscreen(scr_vert, point);
}
else if (scr_horz < horiz_offset)
{
horiz_offset = max(0, (scr_horz - (scr_horz % 8)));
midscreen(scr_vert, point);
}
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
}

void
up()
{
if (curr_line->prev_line != NULL)
{
prevline();
point = curr_line->line;
find_pos();
}
}

void
down()
{
if (curr_line->next_line != NULL)
{
nextline();
find_pos();
}
}

void
function_key()
{
if (in == KEY_LEFT)
left(TRUE);
else if (in == KEY_RIGHT)
right(TRUE);
else if (in == KEY_HOME)
bol();
else if (in == KEY_END)
eol();
else if (in == KEY_UP)
up();
else if (in == KEY_DOWN)
down();
else if (in == KEY_NPAGE)
move_rel('d', max( 5, (last_line - 5)));
else if (in == KEY_PPAGE)
move_rel('u', max(5, (last_line - 5)));
else if (in == KEY_DL)
del_line();
else if (in == KEY_DC)
del_char();
else if (in == KEY_BACKSPACE)
delete(TRUE);
else if (in == KEY_IL)
{
insert_line(TRUE);
left(TRUE);
}
else if (in == KEY_F(1))
gold = !gold;
else if (in == KEY_F(2))
{
if (gold)
{
gold = FALSE;
undel_line();
}
else
undel_char();
}
else if (in == KEY_F(3))
{
if (gold)
{
gold = FALSE;
undel_word();
}
else
del_word();
}
else if (in == KEY_F(4))
{
if (gold)
{
gold = FALSE;
paint_info_win();
midscreen(scr_vert, point);
}
else
adv_word();
}
else if (in == KEY_F(5))
{
if (gold)
{
gold = FALSE;
search_prompt();
}
else
search(TRUE);
}
else if (in == KEY_F(6))
{
if (gold)
{
gold = FALSE;
bottom();
}
else
top();
}
else if (in == KEY_F(7))
{
if (gold)
{
gold = FALSE;
eol();
}
else
bol();
}
else if (in == KEY_F(8))
{
if (gold)
{
gold = FALSE;
command_prompt();
}
else
adv_line();
}
}

void
print_buffer()
{
char buffer[256];

sprintf(buffer, ">!%s", print_command);
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, printer_msg_str, print_command);
wrefresh(com_win);
command(buffer);
}

void
command_prompt()
{
char *cmd_str;
int result;

info_type = COMMANDS;
paint_info_win();
cmd_str = get_string(command_str, TRUE);
if ((result = unique_test(cmd_str, commands)) != 1)
{
werase(com_win);
wmove(com_win, 0, 0);
if (result == 0)
wprintw(com_win, unkn_cmd_str, cmd_str);
else
wprintw(com_win, non_unique_cmd_msg);

wrefresh(com_win);

info_type = CONTROL_KEYS;
paint_info_win();

if (cmd_str != NULL)
free(cmd_str);
return;
}
command(cmd_str);
wrefresh(com_win);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
info_type = CONTROL_KEYS;
paint_info_win();
if (cmd_str != NULL)
free(cmd_str);
}

void
command(cmd_str1)
char *cmd_str1;
{
char *cmd_str2 = NULL;
char *cmd_str = cmd_str1;

clear_com_win = TRUE;
if (compare(cmd_str, HELP, FALSE))
help();
else if (compare(cmd_str, WRITE, FALSE))
{
if (restrict_mode())
{
return;
}
cmd_str = next_word(cmd_str);
if (*cmd_str == '\0')
{
cmd_str = cmd_str2 = get_string(file_write_prompt_str, TRUE);
}
tmp_file = resolve_name(cmd_str);
write_file(tmp_file, 1);
if (tmp_file != cmd_str)
free(tmp_file);
}
else if (compare(cmd_str, READ, FALSE))
{
if (restrict_mode())
{
return;
}
cmd_str = next_word(cmd_str);
if (*cmd_str == '\0')
{
cmd_str = cmd_str2 = get_string(file_read_prompt_str, TRUE);
}
tmp_file = cmd_str;
recv_file = TRUE;
tmp_file = resolve_name(cmd_str);
check_fp();
if (tmp_file != cmd_str)
free(tmp_file);
}
else if (compare(cmd_str, LINE, FALSE))
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, line_num_str, curr_line->line_number);
wprintw(com_win, line_len_str, curr_line->line_length);
}
else if (compare(cmd_str, FILE_str, FALSE))
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
if (in_file_name == NULL)
wprintw(com_win, no_file_string);
else
wprintw(com_win, current_file_str, in_file_name);
}
else if ((*cmd_str >= '0') && (*cmd_str <= '9'))
goto_line(cmd_str);
else if (compare(cmd_str, CHARACTER, FALSE))
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, char_str, *point);
}
else if (compare(cmd_str, REDRAW, FALSE))
redraw();
else if (compare(cmd_str, RESEQUENCE, FALSE))
{
tmp_line = first_line->next_line;
while (tmp_line != NULL)
{
tmp_line->line_number = tmp_line->prev_line->line_number + 1;
tmp_line = tmp_line->next_line;
}
}
else if (compare(cmd_str, AUTHOR, FALSE))
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, "written by Hugh Mahon");
}
else if (compare(cmd_str, VERSION, FALSE))
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, "%s", version);
}
else if (compare(cmd_str, CASE, FALSE))
case_sen = TRUE;
else if (compare(cmd_str, NOCASE, FALSE))
case_sen = FALSE;
else if (compare(cmd_str, EXPAND, FALSE))
expand_tabs = TRUE;
else if (compare(cmd_str, NOEXPAND, FALSE))
expand_tabs = FALSE;
else if (compare(cmd_str, Exit_string, FALSE))
finish();
else if (compare(cmd_str, chinese_cmd, FALSE))
{
ee_chinese = TRUE;
#if defined(NCURSE)
nc_setattrib(A_NC_BIG5);
#endif
}
else if (compare(cmd_str, nochinese_cmd, FALSE))
{
ee_chinese = FALSE;
#if defined(NCURSE)
nc_clearattrib(A_NC_BIG5);
#endif
}
else if (compare(cmd_str, QUIT_string, FALSE))
quit(0);
else if (*cmd_str == '!')
{
cmd_str++;
if ((*cmd_str == ' ') || (*cmd_str == 9))
cmd_str = next_word(cmd_str);
sh_command(cmd_str);
}
else if ((*cmd_str == '<') && (!in_pipe))
{
in_pipe = TRUE;
shell_fork = FALSE;
cmd_str++;
if ((*cmd_str == ' ') || (*cmd_str == '\t'))
cmd_str = next_word(cmd_str);
command(cmd_str);
in_pipe = FALSE;
shell_fork = TRUE;
}
else if ((*cmd_str == '>') && (!out_pipe))
{
out_pipe = TRUE;
cmd_str++;
if ((*cmd_str == ' ') || (*cmd_str == '\t'))
cmd_str = next_word(cmd_str);
command(cmd_str);
out_pipe = FALSE;
}
else
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, unkn_cmd_str, cmd_str);
}
if (cmd_str2 != NULL)
free(cmd_str2);
}

int
scan(line, offset, column)
char *line;
int offset;
int column;
{
char *stemp;
int i;
int j;

stemp = line;
i = 0;
j = column;
while (i < offset)
{
i++;
j += len_char(*stemp, j);
stemp++;
}
return(j);
}

char *
get_string(prompt, advance)
char *prompt;
int advance;
{
char *string;
char *tmp_string;
char *nam_str;
char *g_point;
int tmp_int;
int g_horz, g_position, g_pos;
int esc_flag;

g_point = tmp_string = malloc(512);
wmove(com_win,0,0);
wclrtoeol(com_win);
waddstr(com_win, prompt);
wrefresh(com_win);
nam_str = tmp_string;
clear_com_win = TRUE;
g_horz = g_position = scan(prompt, strlen(prompt), 0);
g_pos = 0;
do
{
esc_flag = FALSE;
in = wgetch(com_win);
if (in == -1)
exit(0);
if (((in == 8) || (in == 127) || (in == KEY_BACKSPACE)) && (g_pos > 0))
{
tmp_int = g_horz;
g_pos--;
g_horz = scan(g_point, g_pos, g_position);
tmp_int = tmp_int - g_horz;
for (; 0 < tmp_int; tmp_int--)
{
if ((g_horz+tmp_int) < (last_col - 1))
{
waddch(com_win, '\010');
waddch(com_win, ' ');
waddch(com_win, '\010');
}
}
nam_str--;
}
else if ((in != 8) && (in != 127) && (in != '\n') && (in != '\r') && (in < 256))
{
if (in == '\026')
{
esc_flag = TRUE;
in = wgetch(com_win);
if (in == -1)
exit(0);
}
*nam_str = in;
g_pos++;
if (!isprint((unsigned char)in) && (g_horz < (last_col - 1)))
g_horz += out_char(com_win, in, g_horz);
else
{
g_horz++;
if (g_horz < (last_col - 1))
waddch(com_win, (unsigned char)in);
}
nam_str++;
}
wrefresh(com_win);
if (esc_flag)
in = '\0';
} while ((in != '\n') && (in != '\r'));
*nam_str = '\0';
nam_str = tmp_string;
if (((*nam_str == ' ') || (*nam_str == 9)) && (advance))
nam_str = next_word(nam_str);
string = malloc(strlen(nam_str) + 1);
strcpy(string, nam_str);
free(tmp_string);
wrefresh(com_win);
return(string);
}

int
compare(string1, string2, sensitive)
char *string1;
char *string2;
int sensitive;
{
char *strng1;
char *strng2;
int tmp;
int equal;

strng1 = string1;
strng2 = string2;
tmp = 0;
if ((strng1 == NULL) || (strng2 == NULL) || (*strng1 == '\0') || (*strng2 == '\0'))
return(FALSE);
equal = TRUE;
while (equal)
{
if (sensitive)
{
if (*strng1 != *strng2)
equal = FALSE;
}
else
{
if (toupper((unsigned char)*strng1) != toupper((unsigned char)*strng2))
equal = FALSE;
}
strng1++;
strng2++;
if ((*strng1 == '\0') || (*strng2 == '\0') || (*strng1 == ' ') || (*strng2 == ' '))
break;
tmp++;
}
return(equal);
}

void
goto_line(cmd_str)
char *cmd_str;
{
int number;
int i;
char *ptr;
char direction = '\0';
struct text *t_line;

ptr = cmd_str;
i= 0;
while ((*ptr >='0') && (*ptr <= '9'))
{
i= i * 10 + (*ptr - '0');
ptr++;
}
number = i;
i = 0;
t_line = curr_line;
while ((t_line->line_number > number) && (t_line->prev_line != NULL))
{
i++;
t_line = t_line->prev_line;
direction = 'u';
}
while ((t_line->line_number < number) && (t_line->next_line != NULL))
{
i++;
direction = 'd';
t_line = t_line->next_line;
}
if ((i < 30) && (i > 0))
{
move_rel(direction, i);
}
else
{
if (direction != 'd')
{
absolute_lin += i;
}
else
{
absolute_lin -= i;
}
curr_line = t_line;
point = curr_line->line;
position = 1;
midscreen((last_line / 2), point);
scr_pos = scr_horz;
}
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, line_num_str, curr_line->line_number);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
}

void
midscreen(line, pnt)
int line;
unsigned char *pnt;
{
struct text *mid_line;
int i;

line = min(line, last_line);
mid_line = curr_line;
for (i = 0; ((i < line) && (curr_line->prev_line != NULL)); i++)
curr_line = curr_line->prev_line;
scr_vert = scr_horz = 0;
wmove(text_win, 0, 0);
draw_screen();
scr_vert = i;
curr_line = mid_line;
scanline(pnt);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
}

void
get_options(numargs, arguments)
int numargs;
char *arguments[];
{
char *buff;
int count;
struct files *temp_names = NULL;
char *name;
char *ptr;
int no_more_opts = FALSE;





if (!(name = strrchr(arguments[0], '/')))
name = arguments[0];
else
name++;
if (!strcmp(name, "ree"))
restricted = TRUE;

top_of_stack = NULL;
input_file = FALSE;
recv_file = FALSE;
count = 1;
while ((count < numargs)&& (!no_more_opts))
{
buff = arguments[count];
if (!strcmp("-i", buff))
{
info_window = FALSE;
}
else if (!strcmp("-e", buff))
{
expand_tabs = FALSE;
}
else if (!strcmp("-h", buff))
{
nohighlight = TRUE;
}
else if (!strcmp("-?", buff))
{
fprintf(stderr, usage0, arguments[0]);
fputs(usage1, stderr);
fputs(usage2, stderr);
fputs(usage3, stderr);
fputs(usage4, stderr);
exit(1);
}
else if ((*buff == '+') && (start_at_line == NULL))
{
buff++;
start_at_line = buff;
}
else if (!(strcmp("--", buff)))
no_more_opts = TRUE;
else
{
count--;
no_more_opts = TRUE;
}
count++;
}
while (count < numargs)
{
buff = arguments[count];
if (top_of_stack == NULL)
{
temp_names = top_of_stack = name_alloc();
}
else
{
temp_names->next_name = name_alloc();
temp_names = temp_names->next_name;
}
ptr = temp_names->name = malloc(strlen(buff) + 1);
while (*buff != '\0')
{
*ptr = *buff;
buff++;
ptr++;
}
*ptr = '\0';
temp_names->next_name = NULL;
input_file = TRUE;
recv_file = TRUE;
count++;
}
}

void
check_fp()
{
int line_num;
int temp;
struct stat buf;

clear_com_win = TRUE;
tmp_vert = scr_vert;
tmp_horz = scr_horz;
tmp_line = curr_line;
if (input_file)
{
in_file_name = tmp_file = top_of_stack->name;
top_of_stack = top_of_stack->next_name;
}
temp = stat(tmp_file, &buf);
buf.st_mode &= ~07777;
if ((temp != -1) && (buf.st_mode != 0100000) && (buf.st_mode != 0))
{
wprintw(com_win, file_is_dir_msg, tmp_file);
wrefresh(com_win);
if (input_file)
{
quit(0);
return;
}
else
return;
}
if ((get_fd = open(tmp_file, O_RDONLY)) == -1)
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
if (input_file)
wprintw(com_win, new_file_msg, tmp_file);
else
wprintw(com_win, cant_open_msg, tmp_file);
wrefresh(com_win);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
wrefresh(text_win);
recv_file = FALSE;
input_file = FALSE;
return;
}
else
get_file(tmp_file);

recv_file = FALSE;
line_num = curr_line->line_number;
scr_vert = tmp_vert;
scr_horz = tmp_horz;
if (input_file)
curr_line= first_line;
else
curr_line = tmp_line;
point = curr_line->line;
draw_screen();
if (input_file)
{
input_file = FALSE;
if (start_at_line != NULL)
{
line_num = atoi(start_at_line) - 1;
move_rel('d', line_num);
line_num = 0;
start_at_line = NULL;
}
}
else
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
text_changes = TRUE;
if ((tmp_file != NULL) && (*tmp_file != '\0'))
wprintw(com_win, file_read_fin_msg, tmp_file);
}
wrefresh(com_win);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
wrefresh(text_win);
}

void
get_file(file_name)
char *file_name;
{
int can_read;
int length;
int append;
struct text *temp_line;
char ro_flag = FALSE;

if (recv_file)
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, reading_file_msg, file_name);
if (access(file_name, 2))
{
if ((errno == ENOTDIR) || (errno == EACCES) || (errno == EROFS) || (errno == ETXTBSY) || (errno == EFAULT))
{
wprintw(com_win, read_only_msg);
ro_flag = TRUE;
}
}
wrefresh(com_win);
}
if (curr_line->line_length > 1)
{
insert_line(FALSE);
left(FALSE);
append = FALSE;
}
else
append = TRUE;
can_read = FALSE;
while (((length = read(get_fd, in_string, 512)) != 0) && (length != -1))
{
can_read = TRUE;
get_line(length, in_string, &append);
}
if ((can_read) && (curr_line->line_length == 1))
{
temp_line = curr_line->prev_line;
temp_line->next_line = curr_line->next_line;
if (temp_line->next_line != NULL)
temp_line->next_line->prev_line = temp_line;
if (curr_line->line != NULL)
free(curr_line->line);
free(curr_line);
curr_line = temp_line;
}
if (input_file)
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, file_read_lines_msg, in_file_name, curr_line->line_number);
if (ro_flag)
wprintw(com_win, read_only_msg);
wrefresh(com_win);
}
else if (can_read)
text_changes = TRUE;

if (recv_file)
{
in = EOF;
}
}

void
get_line(length, in_string, append)
int length;
unsigned char *in_string;
int *append;
{
unsigned char *str1;
unsigned char *str2;
int num;
int char_count;
int temp_counter;
struct text *tline;
int first_time;

str2 = in_string;
num = 0;
first_time = TRUE;
while (num < length)
{
if (!first_time)
{
if (num < length)
{
str2++;
num++;
}
}
else
first_time = FALSE;
str1 = str2;
char_count = 1;

while ((*str2 != '\n') && (num < length))
{
str2++;
num++;
char_count++;
}
if (!(*append))
{
tline = txtalloc();
tline->line_number = curr_line->line_number + 1;
tline->next_line = curr_line->next_line;
tline->prev_line = curr_line;
curr_line->next_line = tline;
if (tline->next_line != NULL)
tline->next_line->prev_line = tline;
curr_line = tline;
curr_line->line = point = (unsigned char *) malloc(char_count);
curr_line->line_length = char_count;
curr_line->max_length = char_count;
}
else
{
point = resiz_line(char_count, curr_line, curr_line->line_length);
curr_line->line_length += (char_count - 1);
}
for (temp_counter = 1; temp_counter < char_count; temp_counter++)
{
*point = *str1;
point++;
str1++;
}
*point = '\0';
*append = FALSE;
if ((num == length) && (*str2 != '\n'))
*append = TRUE;
}
}

void
draw_screen()
{
struct text *temp_line;
unsigned char *line_out;
int temp_vert;

temp_line = curr_line;
temp_vert = scr_vert;
wclrtobot(text_win);
while ((temp_line != NULL) && (temp_vert <= last_line))
{
line_out = temp_line->line;
draw_line(temp_vert, 0, line_out, 1, temp_line->line_length);
temp_vert++;
temp_line = temp_line->next_line;
}
wmove(text_win, temp_vert, 0);
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
}

void
finish()
{
char *file_name = in_file_name;






if ((file_name == NULL) || (*file_name == '\0'))
file_name = get_string(save_file_name_prompt, TRUE);

if ((file_name == NULL) || (*file_name == '\0'))
{
wmove(com_win, 0, 0);
wprintw(com_win, file_not_saved_msg);
wclrtoeol(com_win);
wrefresh(com_win);
clear_com_win = TRUE;
return;
}

tmp_file = resolve_name(file_name);
if (tmp_file != file_name)
{
free(file_name);
file_name = tmp_file;
}

if (write_file(file_name, 1))
{
text_changes = FALSE;
quit(0);
}
}

int
quit(noverify)
int noverify;
{
char *ans;

touchwin(text_win);
wrefresh(text_win);
if ((text_changes) && (!noverify))
{
ans = get_string(changes_made_prompt, TRUE);
if (toupper((unsigned char)*ans) == toupper((unsigned char)*yes_char))
text_changes = FALSE;
else
return(0);
free(ans);
}
if (top_of_stack == NULL)
{
if (info_window)
wrefresh(info_win);
wrefresh(com_win);
resetty();
endwin();
putchar('\n');
exit(0);
}
else
{
delete_text();
recv_file = TRUE;
input_file = TRUE;
check_fp();
}
return(0);
}

void
edit_abort(arg)
int arg;
{
wrefresh(com_win);
resetty();
endwin();
putchar('\n');
exit(1);
}

void
delete_text()
{
while (curr_line->next_line != NULL)
curr_line = curr_line->next_line;
while (curr_line != first_line)
{
free(curr_line->line);
curr_line = curr_line->prev_line;
absolute_lin--;
free(curr_line->next_line);
}
curr_line->next_line = NULL;
*curr_line->line = '\0';
curr_line->line_length = 1;
curr_line->line_number = 1;
point = curr_line->line;
scr_pos = scr_vert = scr_horz = 0;
position = 1;
}

int
write_file(file_name, warn_if_exists)
char *file_name;
int warn_if_exists;
{
char cr;
char *tmp_point;
struct text *out_line;
int lines, charac;
int temp_pos;
int write_flag = TRUE;

charac = lines = 0;
if (warn_if_exists &&
((in_file_name == NULL) || strcmp(in_file_name, file_name)))
{
if ((temp_fp = fopen(file_name, "r")))
{
tmp_point = get_string(file_exists_prompt, TRUE);
if (toupper((unsigned char)*tmp_point) == toupper((unsigned char)*yes_char))
write_flag = TRUE;
else
write_flag = FALSE;
fclose(temp_fp);
free(tmp_point);
}
}

clear_com_win = TRUE;

if (write_flag)
{
if ((temp_fp = fopen(file_name, "w")) == NULL)
{
clear_com_win = TRUE;
wmove(com_win,0,0);
wclrtoeol(com_win);
wprintw(com_win, create_file_fail_msg, file_name);
wrefresh(com_win);
return(FALSE);
}
else
{
wmove(com_win,0,0);
wclrtoeol(com_win);
wprintw(com_win, writing_file_msg, file_name);
wrefresh(com_win);
cr = '\n';
out_line = first_line;
while (out_line != NULL)
{
temp_pos = 1;
tmp_point= out_line->line;
while (temp_pos < out_line->line_length)
{
putc(*tmp_point, temp_fp);
tmp_point++;
temp_pos++;
}
charac += out_line->line_length;
out_line = out_line->next_line;
putc(cr, temp_fp);
lines++;
}
fclose(temp_fp);
wmove(com_win,0,0);
wclrtoeol(com_win);
wprintw(com_win, file_written_msg, file_name, lines, charac);
wrefresh(com_win);
return(TRUE);
}
}
else
return(FALSE);
}

int
search(display_message)
int display_message;
{
int lines_moved;
int iter;
int found;

if ((srch_str == NULL) || (*srch_str == '\0'))
return(FALSE);
if (display_message)
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, searching_msg);
wrefresh(com_win);
clear_com_win = TRUE;
}
lines_moved = 0;
found = FALSE;
srch_line = curr_line;
srch_1 = point;
if (position < curr_line->line_length)
srch_1++;
iter = position + 1;
while ((!found) && (srch_line != NULL))
{
while ((iter < srch_line->line_length) && (!found))
{
srch_2 = srch_1;
if (case_sen)
{
srch_3 = srch_str;
while ((*srch_2 == *srch_3) && (*srch_3 != '\0'))
{
found = TRUE;
srch_2++;
srch_3++;
}
}
else
{
srch_3 = u_srch_str;
while ((toupper(*srch_2) == *srch_3) && (*srch_3 != '\0'))
{
found = TRUE;
srch_2++;
srch_3++;
}
}
if (!((*srch_3 == '\0') && (found)))
{
found = FALSE;
if (iter < srch_line->line_length)
srch_1++;
iter++;
}
}
if (!found)
{
srch_line = srch_line->next_line;
if (srch_line != NULL)
srch_1 = srch_line->line;
iter = 1;
lines_moved++;
}
}
if (found)
{
if (display_message)
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wrefresh(com_win);
}
if (lines_moved == 0)
{
while (position < iter)
right(TRUE);
}
else
{
if (lines_moved < 30)
{
move_rel('d', lines_moved);
while (position < iter)
right(TRUE);
}
else
{
absolute_lin += lines_moved;
curr_line = srch_line;
point = srch_1;
position = iter;
scanline(point);
scr_pos = scr_horz;
midscreen((last_line / 2), point);
}
}
}
else
{
if (display_message)
{
wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, str_not_found_msg, srch_str);
wrefresh(com_win);
}
wmove(text_win, scr_vert,(scr_horz - horiz_offset));
}
return(found);
}

void
search_prompt()
{
if (srch_str != NULL)
free(srch_str);
if ((u_srch_str != NULL) && (*u_srch_str != '\0'))
free(u_srch_str);
srch_str = get_string(search_prompt_str, FALSE);
gold = FALSE;
srch_3 = srch_str;
srch_1 = u_srch_str = malloc(strlen(srch_str) + 1);
while (*srch_3 != '\0')
{
*srch_1 = toupper(*srch_3);
srch_1++;
srch_3++;
}
*srch_1 = '\0';
search(TRUE);
}

void
del_char()
{
in = 8;
if (position < curr_line->line_length)
{
if ((ee_chinese) && (*point > 127) &&
((curr_line->line_length - position) >= 2))
{
point++;
position++;
}
position++;
point++;
scanline(point);
delete(TRUE);
}
else
{
right(TRUE);
delete(TRUE);
}
}

void
undel_char()
{
if (d_char[0] == '\n')
insert_line(TRUE);
else
{
in = d_char[0];
insert(in);
if (d_char[1] != '\0')
{
in = d_char[1];
insert(in);
}
}
}

void
del_word()
{
int tposit;
int difference;
unsigned char *d_word2;
unsigned char *d_word3;
unsigned char tmp_char[3];

if (d_word != NULL)
free(d_word);
d_word = malloc(curr_line->line_length);
tmp_char[0] = d_char[0];
tmp_char[1] = d_char[1];
tmp_char[2] = d_char[2];
d_word3 = point;
d_word2 = d_word;
tposit = position;
while ((tposit < curr_line->line_length) &&
((*d_word3 != ' ') && (*d_word3 != '\t')))
{
tposit++;
*d_word2 = *d_word3;
d_word2++;
d_word3++;
}
while ((tposit < curr_line->line_length) &&
((*d_word3 == ' ') || (*d_word3 == '\t')))
{
tposit++;
*d_word2 = *d_word3;
d_word2++;
d_word3++;
}
*d_word2 = '\0';
d_wrd_len = difference = d_word2 - d_word;
d_word2 = point;
while (tposit < curr_line->line_length)
{
tposit++;
*d_word2 = *d_word3;
d_word2++;
d_word3++;
}
curr_line->line_length -= difference;
*d_word2 = '\0';
draw_line(scr_vert, scr_horz,point,position,curr_line->line_length);
d_char[0] = tmp_char[0];
d_char[1] = tmp_char[1];
d_char[2] = tmp_char[2];
text_changes = TRUE;
formatted = FALSE;
}

void
undel_word()
{
int temp;
int tposit;
unsigned char *tmp_old_ptr;
unsigned char *tmp_space;
unsigned char *tmp_ptr;
unsigned char *d_word_ptr;




if ((curr_line->max_length - (curr_line->line_length + d_wrd_len)) < 5)
point = resiz_line(d_wrd_len, curr_line, position);
tmp_ptr = tmp_space = malloc(curr_line->line_length + d_wrd_len);
d_word_ptr = d_word;
temp = 1;



while (temp <= d_wrd_len)
{
temp++;
*tmp_ptr = *d_word_ptr;
tmp_ptr++;
d_word_ptr++;
}
tmp_old_ptr = point;
tposit = position;




while (tposit < curr_line->line_length)
{
temp++;
tposit++;
*tmp_ptr = *tmp_old_ptr;
tmp_ptr++;
tmp_old_ptr++;
}
curr_line->line_length += d_wrd_len;
tmp_old_ptr = point;
*tmp_ptr = '\0';
tmp_ptr = tmp_space;
tposit = 1;



while (tposit < temp)
{
tposit++;
*tmp_old_ptr = *tmp_ptr;
tmp_ptr++;
tmp_old_ptr++;
}
*tmp_old_ptr = '\0';
free(tmp_space);
draw_line(scr_vert, scr_horz, point, position, curr_line->line_length);
}

void
del_line()
{
unsigned char *dl1;
unsigned char *dl2;
int tposit;

if (d_line != NULL)
free(d_line);
d_line = malloc(curr_line->line_length);
dl1 = d_line;
dl2 = point;
tposit = position;
while (tposit < curr_line->line_length)
{
*dl1 = *dl2;
dl1++;
dl2++;
tposit++;
}
dlt_line->line_length = 1 + tposit - position;
*dl1 = '\0';
*point = '\0';
curr_line->line_length = position;
wclrtoeol(text_win);
if (curr_line->next_line != NULL)
{
right(FALSE);
delete(FALSE);
}
text_changes = TRUE;
}

void
undel_line()
{
unsigned char *ud1;
unsigned char *ud2;
int tposit;

if (dlt_line->line_length == 0)
return;

insert_line(TRUE);
left(TRUE);
point = resiz_line(dlt_line->line_length, curr_line, position);
curr_line->line_length += dlt_line->line_length - 1;
ud1 = point;
ud2 = d_line;
tposit = 1;
while (tposit < dlt_line->line_length)
{
tposit++;
*ud1 = *ud2;
ud1++;
ud2++;
}
*ud1 = '\0';
draw_line(scr_vert, scr_horz,point,position,curr_line->line_length);
}

void
adv_word()
{
while ((position < curr_line->line_length) && ((*point != 32) && (*point != 9)))
right(TRUE);
while ((position < curr_line->line_length) && ((*point == 32) || (*point == 9)))
right(TRUE);
}

void
move_rel(direction, lines)
int direction;
int lines;
{
int i;
char *tmp;

if (direction == 'u')
{
scr_pos = 0;
while (position > 1)
left(TRUE);
for (i = 0; i < lines; i++)
{
up();
}
if ((last_line > 5) && ( scr_vert < 4))
{
tmp = point;
tmp_line = curr_line;
for (i= 0;(i<5)&&(curr_line->prev_line != NULL); i++)
{
up();
}
scr_vert = scr_vert + i;
curr_line = tmp_line;
absolute_lin += i;
point = tmp;
scanline(point);
}
}
else
{
if ((position != 1) && (curr_line->next_line != NULL))
{
nextline();
scr_pos = scr_horz = 0;
if (horiz_offset)
{
horiz_offset = 0;
midscreen(scr_vert, point);
}
}
else
adv_line();
for (i = 1; i < lines; i++)
{
down();
}
if ((last_line > 10) && (scr_vert > (last_line - 5)))
{
tmp = point;
tmp_line = curr_line;
for (i=0; (i<5) && (curr_line->next_line != NULL); i++)
{
down();
}
absolute_lin -= i;
scr_vert = scr_vert - i;
curr_line = tmp_line;
point = tmp;
scanline(point);
}
}
wmove(text_win, scr_vert, (scr_horz - horiz_offset));
}

void
eol()
{
if (position < curr_line->line_length)
{
while (position < curr_line->line_length)
right(TRUE);
}
else if (curr_line->next_line != NULL)
{
right(TRUE);
while (position < curr_line->line_length)
right(TRUE);
}
}

void
bol()
{
if (point != curr_line->line)
{
while (point != curr_line->line)
left(TRUE);
}
else if (curr_line->prev_line != NULL)
{
scr_pos = 0;
up();
}
}

void
adv_line()
{
if ((point != curr_line->line) || (scr_pos > 0))
{
while (position < curr_line->line_length)
right(TRUE);
right(TRUE);
}
else if (curr_line->next_line != NULL)
{
scr_pos = 0;
down();
}
}

void
from_top()
{
struct text *tmpline = first_line;
int x = 1;

while ((tmpline != NULL) && (tmpline != curr_line))
{
x++;
tmpline = tmpline->next_line;
}
absolute_lin = x;
}

void
sh_command(string)
char *string;
{
char *temp_point;
char *last_slash;
char *path;
int parent;
int value;
int return_val;
struct text *line_holder;

if (restrict_mode())
{
return;
}

if (!(path = getenv("SHELL")))
path = "/bin/sh";
last_slash = temp_point = path;
while (*temp_point != '\0')
{
if (*temp_point == '/')
last_slash = ++temp_point;
else
temp_point++;
}






if (!in_pipe)
{
keypad(com_win, FALSE);
keypad(text_win, FALSE);
echo();
nl();
noraw();
resetty();

#if !defined(NCURSE)
endwin();
#endif
}

if (in_pipe)
{
pipe(pipe_in);
parent = fork();
if (!parent)
{




in_pipe = FALSE;



temp_stdout = dup(1);
close(1);
dup(pipe_in[1]);



temp_stderr = dup(2);
close(2);
dup(pipe_in[1]);
close(pipe_in[1]);




}
else
{



signal(SIGCHLD, SIG_IGN);
line_holder = curr_line;
tmp_vert = scr_vert;
close(pipe_in[1]);
get_fd = pipe_in[0];
get_file("");
close(pipe_in[0]);
scr_vert = tmp_vert;
scr_horz = scr_pos = 0;
position = 1;
curr_line = line_holder;
from_top();
point = curr_line->line;
out_pipe = FALSE;
signal(SIGCHLD, SIG_DFL);





}
}
if (!in_pipe)
{
signal(SIGINT, SIG_IGN);
if (out_pipe)
{
pipe(pipe_out);
}



parent = fork();
if (!parent)
{
if (shell_fork)
putchar('\n');
if (out_pipe)
{




close(0);
dup(pipe_out[0]);
close(pipe_out[0]);
close(pipe_out[1]);
}
for (value = 1; value < 24; value++)
signal(value, SIG_DFL);
execl(path, last_slash, "-c", string, NULL);
fprintf(stderr, exec_err_msg, path);
exit(-1);
}
else
{
if (out_pipe)
{




close(pipe_out[0]);
line_holder = first_line;
while (line_holder != NULL)
{
write(pipe_out[1], line_holder->line, (line_holder->line_length-1));
write(pipe_out[1], "\n", 1);
line_holder = line_holder->next_line;
}
close(pipe_out[1]);
out_pipe = FALSE;
}
do
{
return_val = wait((int *) 0);
}
while ((return_val != parent) && (return_val != -1));










if (!shell_fork)
exit(0);
}
signal(SIGINT, edit_abort);
}
if (shell_fork)
{
fputs(continue_msg, stdout);
fflush(stdout);
while ((in = getchar()) != '\n')
;
}

if (!in_pipe)
{
fixterm();
noecho();
nonl();
raw();
keypad(text_win, TRUE);
keypad(com_win, TRUE);
if (info_window)
clearok(info_win, TRUE);
}

redraw();
}

void
set_up_term()
{
if (!curses_initialized)
{
initscr();
savetty();
noecho();
raw();
nonl();
curses_initialized = TRUE;
}

if (((LINES > 15) && (COLS >= 80)) && info_window)
last_line = LINES - 8;
else
{
info_window = FALSE;
last_line = LINES - 2;
}

idlok(stdscr, TRUE);
com_win = newwin(1, COLS, (LINES - 1), 0);
keypad(com_win, TRUE);
idlok(com_win, TRUE);
wrefresh(com_win);
if (!info_window)
text_win = newwin((LINES - 1), COLS, 0, 0);
else
text_win = newwin((LINES - 7), COLS, 6, 0);
keypad(text_win, TRUE);
idlok(text_win, TRUE);
wrefresh(text_win);
help_win = newwin((LINES - 1), COLS, 0, 0);
keypad(help_win, TRUE);
idlok(help_win, TRUE);
if (info_window)
{
info_type = CONTROL_KEYS;
info_win = newwin(6, COLS, 0, 0);
werase(info_win);
paint_info_win();
}

last_col = COLS - 1;
local_LINES = LINES;
local_COLS = COLS;

#if defined(NCURSE)
if (ee_chinese)
nc_setattrib(A_NC_BIG5);
#endif

}

void
resize_check()
{
if ((LINES == local_LINES) && (COLS == local_COLS))
return;

if (info_window)
delwin(info_win);
delwin(text_win);
delwin(com_win);
delwin(help_win);
set_up_term();
redraw();
wrefresh(text_win);
}

static char item_alpha[] = "abcdefghijklmnopqrstuvwxyz0123456789 ";

int
menu_op(menu_list)
struct menu_entries menu_list[];
{
WINDOW *temp_win;
int max_width, max_height;
int x_off, y_off;
int counter;
int length;
int input;
int temp;
int list_size;
int top_offset;
int vert_pos;
int vert_size;
int off_start = 1;






list_size = 1;
while (menu_list[list_size + 1].item_string != NULL)
list_size++;
max_width = 0;
for (counter = 0; counter <= list_size; counter++)
{
if ((length = strlen(menu_list[counter].item_string)) > max_width)
max_width = length;
}
max_width += 3;
max_width = max(max_width, strlen(menu_cancel_msg));
max_width = max(max_width, max(strlen(more_above_str), strlen(more_below_str)));
max_width += 6;






if (max_width > COLS)
{
wmove(com_win, 0, 0);
werase(com_win);
wprintw(com_win, menu_too_lrg_msg);
wrefresh(com_win);
clear_com_win = TRUE;
return(0);
}

top_offset = 0;

if (list_size > LINES)
{
max_height = LINES;
if (max_height > 11)
vert_size = max_height - 8;
else
vert_size = max_height;
}
else
{
vert_size = list_size;
max_height = list_size;
}

if (LINES >= (vert_size + 8))
{
if (menu_list[0].argument != MENU_WARN)
max_height = vert_size + 8;
else
max_height = vert_size + 7;
top_offset = 4;
}
x_off = (COLS - max_width) / 2;
y_off = (LINES - max_height - 1) / 2;
temp_win = newwin(max_height, max_width, y_off, x_off);
keypad(temp_win, TRUE);

paint_menu(menu_list, max_width, max_height, list_size, top_offset, temp_win, off_start, vert_size);

counter = 1;
vert_pos = 0;
do
{
if (off_start > 2)
wmove(temp_win, (1 + counter + top_offset - off_start), 3);
else
wmove(temp_win, (counter + top_offset - off_start), 3);

wrefresh(temp_win);
in = wgetch(temp_win);
input = in;
if (input == -1)
exit(0);

if (isascii(input) && isalnum(input))
{
if (isalpha(input))
{
temp = 1 + tolower(input) - 'a';
}
else if (isdigit(input))
{
temp = (2 + 'z' - 'a') + (input - '0');
}

if (temp <= list_size)
{
input = '\n';
counter = temp;
}
}
else
{
switch (input)
{
case ' ':
case '\004':
case KEY_RIGHT:
case KEY_DOWN:
counter++;
if (counter > list_size)
counter = 1;
break;
case '\010':
case '\025':
case 127:
case KEY_BACKSPACE:
case KEY_LEFT:
case KEY_UP:
counter--;
if (counter == 0)
counter = list_size;
break;
case '\033':
if (menu_list[0].argument != MENU_WARN)
counter = 0;
break;
case '\014':
case '\022':
paint_menu(menu_list, max_width, max_height,
list_size, top_offset, temp_win,
off_start, vert_size);
break;
default:
break;
}
}

if (((list_size - off_start) >= (vert_size - 1)) &&
(counter > (off_start + vert_size - 3)) &&
(off_start > 1))
{
if (counter == list_size)
off_start = (list_size - vert_size) + 2;
else
off_start++;

paint_menu(menu_list, max_width, max_height,
list_size, top_offset, temp_win, off_start,
vert_size);
}
else if ((list_size != vert_size) &&
(counter > (off_start + vert_size - 2)))
{
if (counter == list_size)
off_start = 2 + (list_size - vert_size);
else if (off_start == 1)
off_start = 3;
else
off_start++;

paint_menu(menu_list, max_width, max_height,
list_size, top_offset, temp_win, off_start,
vert_size);
}
else if (counter < off_start)
{
if (counter <= 2)
off_start = 1;
else
off_start = counter;

paint_menu(menu_list, max_width, max_height,
list_size, top_offset, temp_win, off_start,
vert_size);
}
}
while ((input != '\r') && (input != '\n') && (counter != 0));

werase(temp_win);
wrefresh(temp_win);
delwin(temp_win);

if ((menu_list[counter].procedure != NULL) ||
(menu_list[counter].iprocedure != NULL) ||
(menu_list[counter].nprocedure != NULL))
{
if (menu_list[counter].argument != -1)
(*menu_list[counter].iprocedure)(menu_list[counter].argument);
else if (menu_list[counter].ptr_argument != NULL)
(*menu_list[counter].procedure)(menu_list[counter].ptr_argument);
else
(*menu_list[counter].nprocedure)();
}

if (info_window)
paint_info_win();
redraw();

return(counter);
}

void
paint_menu(menu_list, max_width, max_height, list_size, top_offset, menu_win,
off_start, vert_size)
struct menu_entries menu_list[];
int max_width, max_height, list_size, top_offset;
WINDOW *menu_win;
int off_start, vert_size;
{
int counter, temp_int;

werase(menu_win);






if (max_height > vert_size)
{
wmove(menu_win, 1, 1);
if (!nohighlight)
wstandout(menu_win);
waddch(menu_win, '+');
for (counter = 0; counter < (max_width - 4); counter++)
waddch(menu_win, '-');
waddch(menu_win, '+');

wmove(menu_win, (max_height - 2), 1);
waddch(menu_win, '+');
for (counter = 0; counter < (max_width - 4); counter++)
waddch(menu_win, '-');
waddch(menu_win, '+');
wstandend(menu_win);
wmove(menu_win, 2, 3);
waddstr(menu_win, menu_list[0].item_string);
wmove(menu_win, (max_height - 3), 3);
if (menu_list[0].argument != MENU_WARN)
waddstr(menu_win, menu_cancel_msg);
}
if (!nohighlight)
wstandout(menu_win);

for (counter = 0; counter < (vert_size + top_offset); counter++)
{
if (top_offset == 4)
{
temp_int = counter + 2;
}
else
temp_int = counter;

wmove(menu_win, temp_int, 1);
waddch(menu_win, '|');
wmove(menu_win, temp_int, (max_width - 2));
waddch(menu_win, '|');
}
wstandend(menu_win);

if (list_size > vert_size)
{
if (off_start >= 3)
{
temp_int = 1;
wmove(menu_win, top_offset, 3);
waddstr(menu_win, more_above_str);
}
else
temp_int = 0;

for (counter = off_start;
((temp_int + counter - off_start) < (vert_size - 1));
counter++)
{
wmove(menu_win, (top_offset + temp_int +
(counter - off_start)), 3);
if (list_size > 1)
wprintw(menu_win, "%c) ", item_alpha[min((counter - 1), max_alpha_char)]);
waddstr(menu_win, menu_list[counter].item_string);
}

wmove(menu_win, (top_offset + (vert_size - 1)), 3);

if (counter == list_size)
{
if (list_size > 1)
wprintw(menu_win, "%c) ", item_alpha[min((counter - 1), max_alpha_char)]);
wprintw(menu_win, menu_list[counter].item_string);
}
else
wprintw(menu_win, more_below_str);
}
else
{
for (counter = 1; counter <= list_size; counter++)
{
wmove(menu_win, (top_offset + counter - 1), 3);
if (list_size > 1)
wprintw(menu_win, "%c) ", item_alpha[min((counter - 1), max_alpha_char)]);
waddstr(menu_win, menu_list[counter].item_string);
}
}
}

void
help()
{
int counter;

werase(help_win);
clearok(help_win, TRUE);
for (counter = 0; counter < 22; counter++)
{
wmove(help_win, counter, 0);
waddstr(help_win, (emacs_keys_mode) ?
emacs_help_text[counter] : help_text[counter]);
}
wrefresh(help_win);
werase(com_win);
wmove(com_win, 0, 0);
wprintw(com_win, press_any_key_msg);
wrefresh(com_win);
counter = wgetch(com_win);
if (counter == -1)
exit(0);
werase(com_win);
wmove(com_win, 0, 0);
werase(help_win);
wrefresh(help_win);
wrefresh(com_win);
redraw();
}

void
paint_info_win()
{
int counter;

if (!info_window)
return;

werase(info_win);
for (counter = 0; counter < 5; counter++)
{
wmove(info_win, counter, 0);
wclrtoeol(info_win);
if (info_type == CONTROL_KEYS)
waddstr(info_win, (emacs_keys_mode) ?
emacs_control_keys[counter] : control_keys[counter]);
else if (info_type == COMMANDS)
waddstr(info_win, command_strings[counter]);
}
wmove(info_win, 5, 0);
if (!nohighlight)
wstandout(info_win);
waddstr(info_win, separator);
wstandend(info_win);
wrefresh(info_win);
}

void
no_info_window()
{
if (!info_window)
return;
delwin(info_win);
delwin(text_win);
info_window = FALSE;
last_line = LINES - 2;
text_win = newwin((LINES - 1), COLS, 0, 0);
keypad(text_win, TRUE);
idlok(text_win, TRUE);
clearok(text_win, TRUE);
midscreen(scr_vert, point);
wrefresh(text_win);
clear_com_win = TRUE;
}

void
create_info_window()
{
if (info_window)
return;
last_line = LINES - 8;
delwin(text_win);
text_win = newwin((LINES - 7), COLS, 6, 0);
keypad(text_win, TRUE);
idlok(text_win, TRUE);
werase(text_win);
info_window = TRUE;
info_win = newwin(6, COLS, 0, 0);
werase(info_win);
info_type = CONTROL_KEYS;
midscreen(min(scr_vert, last_line), point);
clearok(info_win, TRUE);
paint_info_win();
wrefresh(text_win);
clear_com_win = TRUE;
}

int
file_op(arg)
int arg;
{
char *string;
int flag;

if (restrict_mode())
{
return(0);
}

if (arg == READ_FILE)
{
string = get_string(file_read_prompt_str, TRUE);
recv_file = TRUE;
tmp_file = resolve_name(string);
check_fp();
if (tmp_file != string)
free(tmp_file);
free(string);
}
else if (arg == WRITE_FILE)
{
string = get_string(file_write_prompt_str, TRUE);
tmp_file = resolve_name(string);
write_file(tmp_file, 1);
if (tmp_file != string)
free(tmp_file);
free(string);
}
else if (arg == SAVE_FILE)
{




if (in_file_name)
flag = TRUE;
else
flag = FALSE;

string = in_file_name;
if ((string == NULL) || (*string == '\0'))
string = get_string(save_file_name_prompt, TRUE);
if ((string == NULL) || (*string == '\0'))
{
wmove(com_win, 0, 0);
wprintw(com_win, file_not_saved_msg);
wclrtoeol(com_win);
wrefresh(com_win);
clear_com_win = TRUE;
return(0);
}
if (!flag)
{
tmp_file = resolve_name(string);
if (tmp_file != string)
{
free(string);
string = tmp_file;
}
}
if (write_file(string, 1))
{
in_file_name = string;
text_changes = FALSE;
}
else if (!flag)
free(string);
}
return(0);
}

void
shell_op()
{
char *string;

if (((string = get_string(shell_prompt, TRUE)) != NULL) &&
(*string != '\0'))
{
sh_command(string);
free(string);
}
}

void
leave_op()
{
if (text_changes)
{
menu_op(leave_menu);
}
else
quit(TRUE);
}

void
redraw()
{
if (info_window)
{
clearok(info_win, TRUE);
paint_info_win();
}
else
clearok(text_win, TRUE);
midscreen(scr_vert, point);
}






int
Blank_Line(test_line)
struct text *test_line;
{
unsigned char *line;
int length;

if (test_line == NULL)
return(TRUE);

length = 1;
line = test_line->line;







if ((*line == '.') || (*line == '>'))
return(TRUE);

while (((*line == ' ') || (*line == '\t')) && (length < test_line->line_length))
{
length++;
line++;
}
if (length != test_line->line_length)
return(FALSE);
else
return(TRUE);
}

void
Format()
{
int string_count;
int offset;
int temp_case;
int status;
int tmp_af;
int counter;
unsigned char *line;
unsigned char *tmp_srchstr;
unsigned char *temp1, *temp2;
unsigned char *temp_dword;
unsigned char temp_d_char[3];

temp_d_char[0] = d_char[0];
temp_d_char[1] = d_char[1];
temp_d_char[2] = d_char[2];






if ((!observ_margins) || (Blank_Line(curr_line)))
return;





wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, formatting_msg);
wrefresh(com_win);






tmp_af = auto_format;
auto_format = FALSE;
offset = position;
if (position != 1)
prev_word();
temp_dword = d_word;
d_word = NULL;
temp_case = case_sen;
case_sen = TRUE;
tmp_srchstr = srch_str;
temp2 = srch_str = (unsigned char *) malloc(1 + curr_line->line_length - position);
if ((*point == ' ') || (*point == '\t'))
adv_word();
offset -= position;
counter = position;
line = temp1 = point;
while ((*temp1 != '\0') && (*temp1 != ' ') && (*temp1 != '\t') && (counter < curr_line->line_length))
{
*temp2 = *temp1;
temp2++;
temp1++;
counter++;
}
*temp2 = '\0';
if (position != 1)
bol();
while (!Blank_Line(curr_line->prev_line))
bol();
string_count = 0;
status = TRUE;
while ((line != point) && (status))
{
status = search(FALSE);
string_count++;
}

wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, formatting_msg);
wrefresh(com_win);





if (position != 1)
bol();
while (!Blank_Line(curr_line->prev_line))
bol();

observ_margins = FALSE;







while (!Blank_Line(curr_line->next_line))
{
eol();
left(TRUE);
if (*point != ' ')
{
right(TRUE);
insert(' ');
}
else
right(TRUE);
del_char();
if ((*point == ' ') || (*point == '\t'))
del_word();
}







bol();
adv_word();
while (position < curr_line->line_length)
{
if ((*point == ' ') && (*(point + 1) == ' '))
del_char();
else
right(TRUE);
}





bol();
while (position < curr_line->line_length)
{
if ((*point == '.') && (*(point + 1) == ' '))
{
right(TRUE);
insert(' ');
insert(' ');
while (*point == ' ')
del_char();
}
right(TRUE);
}

observ_margins = TRUE;
bol();

wmove(com_win, 0, 0);
wclrtoeol(com_win);
wprintw(com_win, formatting_msg);
wrefresh(com_win);





while (position < curr_line->line_length)
{
while ((scr_pos < right_margin) && (position < curr_line->line_length))
right(TRUE);
if (position < curr_line->line_length)
{
prev_word();
if (position == 1)
adv_word();
insert_line(TRUE);
}
}





bol();
while (!Blank_Line(curr_line->prev_line))
bol();





while ((status) && (string_count > 0))
{
search(FALSE);
string_count--;
}





while (offset > 0)
{
offset--;
right(TRUE);
}





if (d_word != NULL)
free(d_word);
d_word = temp_dword;
case_sen = temp_case;
free(srch_str);
srch_str = tmp_srchstr;
d_char[0] = temp_d_char[0];
d_char[1] = temp_d_char[1];
d_char[2] = temp_d_char[2];
auto_format = tmp_af;

midscreen(scr_vert, point);
werase(com_win);
wrefresh(com_win);
}

unsigned char *init_name[3] = {
"/usr/share/misc/init.ee",
NULL,
".init.ee"
};

void
ee_init()
{
FILE *init_file;
unsigned char *string;
unsigned char *str1;
unsigned char *str2;
char *home;
int counter;
int temp_int;

string = getenv("HOME");
if (string == NULL)
string = "/tmp";
str1 = home = malloc(strlen(string)+10);
strcpy(home, string);
strcat(home, "/.init.ee");
init_name[1] = home;
string = malloc(512);

for (counter = 0; counter < 3; counter++)
{
if (!(access(init_name[counter], 4)))
{
init_file = fopen(init_name[counter], "r");
while ((str2 = fgets(string, 512, init_file)) != NULL)
{
str1 = str2 = string;
while (*str2 != '\n')
str2++;
*str2 = '\0';

if (unique_test(string, init_strings) != 1)
continue;

if (compare(str1, CASE, FALSE))
case_sen = TRUE;
else if (compare(str1, NOCASE, FALSE))
case_sen = FALSE;
else if (compare(str1, EXPAND, FALSE))
expand_tabs = TRUE;
else if (compare(str1, NOEXPAND, FALSE))
expand_tabs = FALSE;
else if (compare(str1, INFO, FALSE))
info_window = TRUE;
else if (compare(str1, NOINFO, FALSE))
info_window = FALSE;
else if (compare(str1, MARGINS, FALSE))
observ_margins = TRUE;
else if (compare(str1, NOMARGINS, FALSE))
observ_margins = FALSE;
else if (compare(str1, AUTOFORMAT, FALSE))
{
auto_format = TRUE;
observ_margins = TRUE;
}
else if (compare(str1, NOAUTOFORMAT, FALSE))
auto_format = FALSE;
else if (compare(str1, Echo, FALSE))
{
str1 = next_word(str1);
if (*str1 != '\0')
echo_string(str1);
}
else if (compare(str1, PRINTCOMMAND, FALSE))
{
str1 = next_word(str1);
print_command = malloc(strlen(str1)+1);
strcpy(print_command, str1);
}
else if (compare(str1, RIGHTMARGIN, FALSE))
{
str1 = next_word(str1);
if ((*str1 >= '0') && (*str1 <= '9'))
{
temp_int = atoi(str1);
if (temp_int > 0)
right_margin = temp_int;
}
}
else if (compare(str1, HIGHLIGHT, FALSE))
nohighlight = FALSE;
else if (compare(str1, NOHIGHLIGHT, FALSE))
nohighlight = TRUE;
else if (compare(str1, EIGHTBIT, FALSE))
eightbit = TRUE;
else if (compare(str1, NOEIGHTBIT, FALSE))
{
eightbit = FALSE;
ee_chinese = FALSE;
}
else if (compare(str1, EMACS_string, FALSE))
emacs_keys_mode = TRUE;
else if (compare(str1, NOEMACS_string, FALSE))
emacs_keys_mode = FALSE;
else if (compare(str1, chinese_cmd, FALSE))
{
ee_chinese = TRUE;
eightbit = TRUE;
}
else if (compare(str1, nochinese_cmd, FALSE))
ee_chinese = FALSE;
}
fclose(init_file);
}
}
free(string);
free(home);

string = getenv("LANG");
if (string != NULL)
{
if (strcmp(string, "zh_TW.big5") == 0)
{
ee_chinese = TRUE;
eightbit = TRUE;
}
}
}





void
dump_ee_conf()
{
FILE *init_file;
FILE *old_init_file = NULL;
char *file_name = ".init.ee";
char *home_dir = "~/.init.ee";
char buffer[512];
struct stat buf;
char *string;
int length;
int option = 0;

if (restrict_mode())
{
return;
}

option = menu_op(config_dump_menu);

werase(com_win);
wmove(com_win, 0, 0);

if (option == 0)
{
wprintw(com_win, conf_not_saved_msg);
wrefresh(com_win);
return;
}
else if (option == 2)
file_name = resolve_name(home_dir);





if (stat(file_name, &buf) != -1)
{
sprintf(buffer, "%s.old", file_name);
unlink(buffer);
link(file_name, buffer);
unlink(file_name);
old_init_file = fopen(buffer, "r");
}

init_file = fopen(file_name, "w");
if (init_file == NULL)
{
wprintw(com_win, conf_dump_err_msg);
wrefresh(com_win);
return;
}

if (old_init_file != NULL)
{



while ((string = fgets(buffer, 512, old_init_file)) != NULL)
{
length = strlen(string);
string[length - 1] = '\0';

if (unique_test(string, init_strings) == 1)
{
if (compare(string, Echo, FALSE))
{
fprintf(init_file, "%s\n", string);
}
}
else
fprintf(init_file, "%s\n", string);
}

fclose(old_init_file);
}

fprintf(init_file, "%s\n", case_sen ? CASE : NOCASE);
fprintf(init_file, "%s\n", expand_tabs ? EXPAND : NOEXPAND);
fprintf(init_file, "%s\n", info_window ? INFO : NOINFO );
fprintf(init_file, "%s\n", observ_margins ? MARGINS : NOMARGINS );
fprintf(init_file, "%s\n", auto_format ? AUTOFORMAT : NOAUTOFORMAT );
fprintf(init_file, "%s %s\n", PRINTCOMMAND, print_command);
fprintf(init_file, "%s %d\n", RIGHTMARGIN, right_margin);
fprintf(init_file, "%s\n", nohighlight ? NOHIGHLIGHT : HIGHLIGHT );
fprintf(init_file, "%s\n", eightbit ? EIGHTBIT : NOEIGHTBIT );
fprintf(init_file, "%s\n", emacs_keys_mode ? EMACS_string : NOEMACS_string );
fprintf(init_file, "%s\n", ee_chinese ? chinese_cmd : nochinese_cmd );

fclose(init_file);

wprintw(com_win, conf_dump_success_msg, file_name);
wrefresh(com_win);

if ((option == 2) && (file_name != home_dir))
{
free(file_name);
}
}

void
echo_string(string)
char *string;
{
char *temp;
int Counter;

temp = string;
while (*temp != '\0')
{
if (*temp == '\\')
{
temp++;
if (*temp == 'n')
putchar('\n');
else if (*temp == 't')
putchar('\t');
else if (*temp == 'b')
putchar('\b');
else if (*temp == 'r')
putchar('\r');
else if (*temp == 'f')
putchar('\f');
else if ((*temp == 'e') || (*temp == 'E'))
putchar('\033');
else if (*temp == '\\')
putchar('\\');
else if (*temp == '\'')
putchar('\'');
else if ((*temp >= '0') && (*temp <= '9'))
{
Counter = 0;
while ((*temp >= '0') && (*temp <= '9'))
{
Counter = (8 * Counter) + (*temp - '0');
temp++;
}
putchar(Counter);
temp--;
}
temp++;
}
else
{
putchar(*temp);
temp++;
}
}

fflush(stdout);
}

void
spell_op()
{
if (restrict_mode())
{
return;
}
top();
insert_line(FALSE);
insert_line(FALSE);
top();
command(shell_echo_msg);
adv_line();
wmove(com_win, 0, 0);
wprintw(com_win, spell_in_prog_msg);
wrefresh(com_win);
command("<>!spell");

}

void
ispell_op()
{
char template[128], *name;
char string[256];
int fd;

if (restrict_mode())
{
return;
}
(void)sprintf(template, "/tmp/ee.XXXXXXXX");
fd = mkstemp(template);
if (fd < 0) {
wmove(com_win, 0, 0);
wprintw(com_win, create_file_fail_msg, name);
wrefresh(com_win);
return;
}
close(fd);
if (write_file(name, 0))
{
sprintf(string, "ispell %s", name);
sh_command(string);
delete_text();
tmp_file = name;
recv_file = TRUE;
check_fp();
unlink(name);
}
}

int
first_word_len(test_line)
struct text *test_line;
{
int counter;
unsigned char *pnt;

if (test_line == NULL)
return(0);

pnt = test_line->line;
if ((pnt == NULL) || (*pnt == '\0') ||
(*pnt == '.') || (*pnt == '>'))
return(0);

if ((*pnt == ' ') || (*pnt == '\t'))
{
pnt = next_word(pnt);
}

if (*pnt == '\0')
return(0);

counter = 0;
while ((*pnt != '\0') && ((*pnt != ' ') && (*pnt != '\t')))
{
pnt++;
counter++;
}
while ((*pnt != '\0') && ((*pnt == ' ') || (*pnt == '\t')))
{
pnt++;
counter++;
}
return(counter);
}

void
Auto_Format()
{
int string_count;
int offset;
int temp_case;
int word_len;
int temp_dwl;
int tmp_d_line_length;
int leave_loop = FALSE;
int status;
int counter;
char not_blank;
unsigned char *line;
unsigned char *tmp_srchstr;
unsigned char *temp1, *temp2;
unsigned char *temp_dword;
unsigned char temp_d_char[3];
unsigned char *tmp_d_line;


temp_d_char[0] = d_char[0];
temp_d_char[1] = d_char[1];
temp_d_char[2] = d_char[2];






if ((!observ_margins) || (Blank_Line(curr_line)))
return;






tmp_d_line = d_line;
tmp_d_line_length = dlt_line->line_length;
d_line = NULL;
auto_format = FALSE;
offset = position;
if ((position != 1) && ((*point == ' ') || (*point == '\t') || (position == curr_line->line_length) || (*point == '\0')))
prev_word();
temp_dword = d_word;
temp_dwl = d_wrd_len;
d_wrd_len = 0;
d_word = NULL;
temp_case = case_sen;
case_sen = TRUE;
tmp_srchstr = srch_str;
temp2 = srch_str = (unsigned char *) malloc(1 + curr_line->line_length - position);
if ((*point == ' ') || (*point == '\t'))
adv_word();
offset -= position;
counter = position;
line = temp1 = point;
while ((*temp1 != '\0') && (*temp1 != ' ') && (*temp1 != '\t') && (counter < curr_line->line_length))
{
*temp2 = *temp1;
temp2++;
temp1++;
counter++;
}
*temp2 = '\0';
if (position != 1)
bol();
while (!Blank_Line(curr_line->prev_line))
bol();
string_count = 0;
status = TRUE;
while ((line != point) && (status))
{
status = search(FALSE);
string_count++;
}





if (position != 1)
bol();
while (!Blank_Line(curr_line->prev_line))
bol();








counter = 0;

while (!leave_loop)
{
if (position != curr_line->line_length)
eol();
left(TRUE);
if (*point != ' ')
{
right(TRUE);
insert(' ');
}
else
right(TRUE);

not_blank = FALSE;






while ((curr_line->next_line != NULL) &&
((word_len = first_word_len(curr_line->next_line)) > 0)
&& ((scr_pos + word_len) < right_margin))
{
adv_line();
if ((*point == ' ') || (*point == '\t'))
adv_word();
del_word();
if (position != 1)
bol();











if ((Blank_Line(curr_line)) &&
(curr_line->line[0] != '.') &&
(curr_line->line[0] != '>'))
{
del_line();
not_blank = FALSE;
}
else
not_blank = TRUE;




left(TRUE);
undel_word();
eol();



left(TRUE);
if (*point != ' ')
{
right(TRUE);
insert(' ');
}
else
right(TRUE);
}





while (right_margin <= scr_pos)
{
prev_word();
if (position != 1)
{
del_word();
if (Blank_Line(curr_line->next_line))
insert_line(TRUE);
else
adv_line();
if ((*point == ' ') || (*point == '\t'))
adv_word();
undel_word();
not_blank = TRUE;
if (position != 1)
bol();
left(TRUE);
}
}

if ((!Blank_Line(curr_line->next_line)) || (not_blank))
{
adv_line();
counter++;
}
else
leave_loop = TRUE;
}





if (position != 1)
bol();
while ((counter-- > 0) || (!Blank_Line(curr_line->prev_line)))
bol();





status = TRUE;
while ((status) && (string_count > 0))
{
status = search(FALSE);
string_count--;
}





while (offset > 0)
{
offset--;
right(TRUE);
}

if ((string_count > 0) && (offset < 0))
{
while (offset < 0)
{
offset++;
left(TRUE);
}
}





if (d_word != NULL)
free(d_word);
d_word = temp_dword;
d_wrd_len = temp_dwl;
case_sen = temp_case;
free(srch_str);
srch_str = tmp_srchstr;
d_char[0] = temp_d_char[0];
d_char[1] = temp_d_char[1];
d_char[2] = temp_d_char[2];
auto_format = TRUE;
dlt_line->line_length = tmp_d_line_length;
d_line = tmp_d_line;

formatted = TRUE;
midscreen(scr_vert, point);
}

void
modes_op()
{
int ret_value;
int counter;
char *string;

do
{
sprintf(modes_menu[1].item_string, "%s %s", mode_strings[1],
(expand_tabs ? ON : OFF));
sprintf(modes_menu[2].item_string, "%s %s", mode_strings[2],
(case_sen ? ON : OFF));
sprintf(modes_menu[3].item_string, "%s %s", mode_strings[3],
(observ_margins ? ON : OFF));
sprintf(modes_menu[4].item_string, "%s %s", mode_strings[4],
(auto_format ? ON : OFF));
sprintf(modes_menu[5].item_string, "%s %s", mode_strings[5],
(eightbit ? ON : OFF));
sprintf(modes_menu[6].item_string, "%s %s", mode_strings[6],
(info_window ? ON : OFF));
sprintf(modes_menu[7].item_string, "%s %s", mode_strings[7],
(emacs_keys_mode ? ON : OFF));
sprintf(modes_menu[8].item_string, "%s %d", mode_strings[8],
right_margin);
sprintf(modes_menu[9].item_string, "%s %s", mode_strings[9],
(ee_chinese ? ON : OFF));

ret_value = menu_op(modes_menu);

switch (ret_value)
{
case 1:
expand_tabs = !expand_tabs;
break;
case 2:
case_sen = !case_sen;
break;
case 3:
observ_margins = !observ_margins;
break;
case 4:
auto_format = !auto_format;
if (auto_format)
observ_margins = TRUE;
break;
case 5:
eightbit = !eightbit;
if (!eightbit)
ee_chinese = FALSE;
#if defined(NCURSE)
if (ee_chinese)
nc_setattrib(A_NC_BIG5);
else
nc_clearattrib(A_NC_BIG5);
#endif

redraw();
wnoutrefresh(text_win);
break;
case 6:
if (info_window)
no_info_window();
else
create_info_window();
break;
case 7:
emacs_keys_mode = !emacs_keys_mode;
if (info_window)
paint_info_win();
break;
case 8:
string = get_string(margin_prompt, TRUE);
if (string != NULL)
{
counter = atoi(string);
if (counter > 0)
right_margin = counter;
free(string);
}
break;
case 9:
ee_chinese = !ee_chinese;
if (ee_chinese != FALSE)
eightbit = TRUE;
#if defined(NCURSE)
if (ee_chinese)
nc_setattrib(A_NC_BIG5);
else
nc_clearattrib(A_NC_BIG5);
#endif
redraw();
break;
default:
break;
}
}
while (ret_value != 0);
}

char *
is_in_string(string, substring)

char * string, *substring;
{
char *full, *sub;

for (sub = substring; (sub != NULL) && (*sub != '\0'); sub++)
{
for (full = string; (full != NULL) && (*full != '\0');
full++)
{
if (*sub == *full)
return(full);
}
}
return(NULL);
}






char *
resolve_name(name)
char *name;
{
char long_buffer[1024];
char short_buffer[128];
char *buffer;
char *slash;
char *tmp;
char *start_of_var;
int offset;
int index;
int counter;
struct passwd *user;

if (name[0] == '~')
{
if (name[1] == '/')
{
index = getuid();
user = (struct passwd *) getpwuid(index);
slash = name + 1;
}
else
{
slash = strchr(name, '/');
if (slash == NULL)
return(name);
*slash = '\0';
user = (struct passwd *) getpwnam((name + 1));
*slash = '/';
}
if (user == NULL)
{
return(name);
}
buffer = malloc(strlen(user->pw_dir) + strlen(slash) + 1);
strcpy(buffer, user->pw_dir);
strcat(buffer, slash);
}
else
buffer = name;

if (is_in_string(buffer, "$"))
{
tmp = buffer;
index = 0;

while ((*tmp != '\0') && (index < 1024))
{

while ((*tmp != '\0') && (*tmp != '$') &&
(index < 1024))
{
long_buffer[index] = *tmp;
tmp++;
index++;
}

if ((*tmp == '$') && (index < 1024))
{
counter = 0;
start_of_var = tmp;
tmp++;
if (*tmp == '{')
{
tmp++;
while ((*tmp != '\0') &&
(*tmp != '}') &&
(counter < 128))
{
short_buffer[counter] = *tmp;
counter++;
tmp++;
}
if (*tmp == '}')
tmp++;
}
else
{
while ((*tmp != '\0') &&
(*tmp != '/') &&
(*tmp != '$') &&
(counter < 128))
{
short_buffer[counter] = *tmp;
counter++;
tmp++;
}
}
short_buffer[counter] = '\0';
if ((slash = getenv(short_buffer)) != NULL)
{
offset = strlen(slash);
if ((offset + index) < 1024)
strcpy(&long_buffer[index], slash);
index += offset;
}
else
{
while ((start_of_var != tmp) && (index < 1024))
{
long_buffer[index] = *start_of_var;
start_of_var++;
index++;
}
}
}
}

if (index == 1024)
return(buffer);
else
long_buffer[index] = '\0';

if (name != buffer)
free(buffer);
buffer = malloc(index + 1);
strcpy(buffer, long_buffer);
}

return(buffer);
}

int
restrict_mode()
{
if (!restricted)
return(FALSE);

wmove(com_win, 0, 0);
wprintw(com_win, restricted_msg);
wclrtoeol(com_win);
wrefresh(com_win);
clear_com_win = TRUE;
return(TRUE);
}







int
unique_test(string, list)
char *string;
char *list[];
{
int counter;
int num_match;
int result;

num_match = 0;
counter = 0;
while (list[counter] != NULL)
{
result = compare(string, list[counter], FALSE);
if (result)
num_match++;
counter++;
}
return(num_match);
}

#if !defined(NO_CATGETS)






char *
catgetlocal(number, string)
int number;
char *string;
{
char *temp1;
char *temp2;

temp1 = catgets(catalog, 1, number, string);
if (temp1 != string)
{
temp2 = malloc(strlen(temp1) + 1);
strcpy(temp2, temp1);
temp1 = temp2;
}
return(temp1);
}
#endif








void
strings_init()
{
int counter;

setlocale(LC_ALL, "");
#if !defined(NO_CATGETS)
catalog = catopen("ee", NL_CAT_LOCALE);
#endif

modes_menu[0].item_string = catgetlocal( 1, "modes menu");
mode_strings[1] = catgetlocal( 2, "tabs to spaces ");
mode_strings[2] = catgetlocal( 3, "case sensitive search");
mode_strings[3] = catgetlocal( 4, "margins observed ");
mode_strings[4] = catgetlocal( 5, "auto-paragraph format");
mode_strings[5] = catgetlocal( 6, "eightbit characters ");
mode_strings[6] = catgetlocal( 7, "info window ");
mode_strings[8] = catgetlocal( 8, "right margin ");
leave_menu[0].item_string = catgetlocal( 9, "leave menu");
leave_menu[1].item_string = catgetlocal( 10, "save changes");
leave_menu[2].item_string = catgetlocal( 11, "no save");
file_menu[0].item_string = catgetlocal( 12, "file menu");
file_menu[1].item_string = catgetlocal( 13, "read a file");
file_menu[2].item_string = catgetlocal( 14, "write a file");
file_menu[3].item_string = catgetlocal( 15, "save file");
file_menu[4].item_string = catgetlocal( 16, "print editor contents");
search_menu[0].item_string = catgetlocal( 17, "search menu");
search_menu[1].item_string = catgetlocal( 18, "search for ...");
search_menu[2].item_string = catgetlocal( 19, "search");
spell_menu[0].item_string = catgetlocal( 20, "spell menu");
spell_menu[1].item_string = catgetlocal( 21, "use 'spell'");
spell_menu[2].item_string = catgetlocal( 22, "use 'ispell'");
misc_menu[0].item_string = catgetlocal( 23, "miscellaneous menu");
misc_menu[1].item_string = catgetlocal( 24, "format paragraph");
misc_menu[2].item_string = catgetlocal( 25, "shell command");
misc_menu[3].item_string = catgetlocal( 26, "check spelling");
main_menu[0].item_string = catgetlocal( 27, "main menu");
main_menu[1].item_string = catgetlocal( 28, "leave editor");
main_menu[2].item_string = catgetlocal( 29, "help");
main_menu[3].item_string = catgetlocal( 30, "file operations");
main_menu[4].item_string = catgetlocal( 31, "redraw screen");
main_menu[5].item_string = catgetlocal( 32, "settings");
main_menu[6].item_string = catgetlocal( 33, "search");
main_menu[7].item_string = catgetlocal( 34, "miscellaneous");
help_text[0] = catgetlocal( 35, "Control keys: ");
help_text[1] = catgetlocal( 36, "^a ascii code ^i tab ^r right ");
help_text[2] = catgetlocal( 37, "^b bottom of text ^j newline ^t top of text ");
help_text[3] = catgetlocal( 38, "^c command ^k delete char ^u up ");
help_text[4] = catgetlocal( 39, "^d down ^l left ^v undelete word ");
help_text[5] = catgetlocal( 40, "^e search prompt ^m newline ^w delete word ");
help_text[6] = catgetlocal( 41, "^f undelete char ^n next page ^x search ");
help_text[7] = catgetlocal( 42, "^g begin of line ^o end of line ^y delete line ");
help_text[8] = catgetlocal( 43, "^h backspace ^p prev page ^z undelete line ");
help_text[9] = catgetlocal( 44, "^[ (escape) menu ESC-Enter: exit ee ");
help_text[10] = catgetlocal( 45, " ");
help_text[11] = catgetlocal( 46, "Commands: ");
help_text[12] = catgetlocal( 47, "help : get this info file : print file name ");
help_text[13] = catgetlocal( 48, "read : read a file char : ascii code of char ");
help_text[14] = catgetlocal( 49, "write : write a file case : case sensitive search ");
help_text[15] = catgetlocal( 50, "exit : leave and save nocase : case insensitive search ");
help_text[16] = catgetlocal( 51, "quit : leave, no save !cmd : execute \"cmd\" in shell ");
help_text[17] = catgetlocal( 52, "line : display line #0-9 : go to line \"#\" ");
help_text[18] = catgetlocal( 53, "expand : expand tabs noexpand: do not expand tabs ");
help_text[19] = catgetlocal( 54, " ");
help_text[20] = catgetlocal( 55, " ee [+#] [-i] [-e] [-h] [file(s)] ");
help_text[21] = catgetlocal( 56, "+#:go to line #-i :no info window -e : don't expand tabs -h :no highlight");
control_keys[0] = catgetlocal( 57, "^[ (escape) menu ^e search prompt ^y delete line ^u up ^p prev page ");
control_keys[1] = catgetlocal( 58, "^a ascii code ^x search ^z undelete line ^d down ^n next page ");
control_keys[2] = catgetlocal( 59, "^b bottom of text ^g begin of line ^w delete word ^l left ");
control_keys[3] = catgetlocal( 60, "^t top of text ^o end of line ^v undelete word ^r right ");
control_keys[4] = catgetlocal( 61, "^c command ^k delete char ^f undelete char ESC-Enter: exit ee ");
command_strings[0] = catgetlocal( 62, "help : get help info |file : print file name |line : print line #");
command_strings[1] = catgetlocal( 63, "read : read a file |char : ascii code of char |0-9 : go to line \"#\"");
command_strings[2] = catgetlocal( 64, "write: write a file |case : case sensitive search |exit : leave and save ");
command_strings[3] = catgetlocal( 65, "!cmd : shell \"cmd\" |nocase: ignore case in search |quit : leave, no save");
command_strings[4] = catgetlocal( 66, "expand: expand tabs |noexpand: do not expand tabs ");
com_win_message = catgetlocal( 67, " press Escape (^[) for menu");
no_file_string = catgetlocal( 68, "no file");
ascii_code_str = catgetlocal( 69, "ascii code: ");
printer_msg_str = catgetlocal( 70, "sending contents of buffer to \"%s\" ");
command_str = catgetlocal( 71, "command: ");
file_write_prompt_str = catgetlocal( 72, "name of file to write: ");
file_read_prompt_str = catgetlocal( 73, "name of file to read: ");
char_str = catgetlocal( 74, "character = %d");
unkn_cmd_str = catgetlocal( 75, "unknown command \"%s\"");
non_unique_cmd_msg = catgetlocal( 76, "entered command is not unique");
line_num_str = catgetlocal( 77, "line %d ");
line_len_str = catgetlocal( 78, "length = %d");
current_file_str = catgetlocal( 79, "current file is \"%s\" ");
usage0 = catgetlocal( 80, "usage: %s [-i] [-e] [-h] [+line_number] [file(s)]\n");
usage1 = catgetlocal( 81, " -i turn off info window\n");
usage2 = catgetlocal( 82, " -e do not convert tabs to spaces\n");
usage3 = catgetlocal( 83, " -h do not use highlighting\n");
file_is_dir_msg = catgetlocal( 84, "file \"%s\" is a directory");
new_file_msg = catgetlocal( 85, "new file \"%s\"");
cant_open_msg = catgetlocal( 86, "can't open \"%s\"");
open_file_msg = catgetlocal( 87, "file \"%s\", %d lines");
file_read_fin_msg = catgetlocal( 88, "finished reading file \"%s\"");
reading_file_msg = catgetlocal( 89, "reading file \"%s\"");
read_only_msg = catgetlocal( 90, ", read only");
file_read_lines_msg = catgetlocal( 91, "file \"%s\", %d lines");
save_file_name_prompt = catgetlocal( 92, "enter name of file: ");
file_not_saved_msg = catgetlocal( 93, "no filename entered: file not saved");
changes_made_prompt = catgetlocal( 94, "changes have been made, are you sure? (y/n [n]) ");
yes_char = catgetlocal( 95, "y");
file_exists_prompt = catgetlocal( 96, "file already exists, overwrite? (y/n) [n] ");
create_file_fail_msg = catgetlocal( 97, "unable to create file \"%s\"");
writing_file_msg = catgetlocal( 98, "writing file \"%s\"");
file_written_msg = catgetlocal( 99, "\"%s\" %d lines, %d characters");
searching_msg = catgetlocal( 100, " ...searching");
str_not_found_msg = catgetlocal( 101, "string \"%s\" not found");
search_prompt_str = catgetlocal( 102, "search for: ");
exec_err_msg = catgetlocal( 103, "could not exec %s\n");
continue_msg = catgetlocal( 104, "press return to continue ");
menu_cancel_msg = catgetlocal( 105, "press Esc to cancel");
menu_size_err_msg = catgetlocal( 106, "menu too large for window");
press_any_key_msg = catgetlocal( 107, "press any key to continue ");
shell_prompt = catgetlocal( 108, "shell command: ");
formatting_msg = catgetlocal( 109, "...formatting paragraph...");
shell_echo_msg = catgetlocal( 110, "<!echo 'list of unrecognized words'; echo -=-=-=-=-=-");
spell_in_prog_msg = catgetlocal( 111, "sending contents of edit buffer to 'spell'");
margin_prompt = catgetlocal( 112, "right margin is: ");
restricted_msg = catgetlocal( 113, "restricted mode: unable to perform requested operation");
ON = catgetlocal( 114, "ON");
OFF = catgetlocal( 115, "OFF");
HELP = catgetlocal( 116, "HELP");
WRITE = catgetlocal( 117, "WRITE");
READ = catgetlocal( 118, "READ");
LINE = catgetlocal( 119, "LINE");
FILE_str = catgetlocal( 120, "FILE");
CHARACTER = catgetlocal( 121, "CHARACTER");
REDRAW = catgetlocal( 122, "REDRAW");
RESEQUENCE = catgetlocal( 123, "RESEQUENCE");
AUTHOR = catgetlocal( 124, "AUTHOR");
VERSION = catgetlocal( 125, "VERSION");
CASE = catgetlocal( 126, "CASE");
NOCASE = catgetlocal( 127, "NOCASE");
EXPAND = catgetlocal( 128, "EXPAND");
NOEXPAND = catgetlocal( 129, "NOEXPAND");
Exit_string = catgetlocal( 130, "EXIT");
QUIT_string = catgetlocal( 131, "QUIT");
INFO = catgetlocal( 132, "INFO");
NOINFO = catgetlocal( 133, "NOINFO");
MARGINS = catgetlocal( 134, "MARGINS");
NOMARGINS = catgetlocal( 135, "NOMARGINS");
AUTOFORMAT = catgetlocal( 136, "AUTOFORMAT");
NOAUTOFORMAT = catgetlocal( 137, "NOAUTOFORMAT");
Echo = catgetlocal( 138, "ECHO");
PRINTCOMMAND = catgetlocal( 139, "PRINTCOMMAND");
RIGHTMARGIN = catgetlocal( 140, "RIGHTMARGIN");
HIGHLIGHT = catgetlocal( 141, "HIGHLIGHT");
NOHIGHLIGHT = catgetlocal( 142, "NOHIGHLIGHT");
EIGHTBIT = catgetlocal( 143, "EIGHTBIT");
NOEIGHTBIT = catgetlocal( 144, "NOEIGHTBIT");



mode_strings[7] = catgetlocal( 145, "emacs key bindings ");
emacs_help_text[0] = help_text[0];
emacs_help_text[1] = catgetlocal( 146, "^a beginning of line ^i tab ^r restore word ");
emacs_help_text[2] = catgetlocal( 147, "^b back 1 char ^j undel char ^t top of text ");
emacs_help_text[3] = catgetlocal( 148, "^c command ^k delete line ^u bottom of text ");
emacs_help_text[4] = catgetlocal( 149, "^d delete char ^l undelete line ^v next page ");
emacs_help_text[5] = catgetlocal( 150, "^e end of line ^m newline ^w delete word ");
emacs_help_text[6] = catgetlocal( 151, "^f forward 1 char ^n next line ^x search ");
emacs_help_text[7] = catgetlocal( 152, "^g go back 1 page ^o ascii char insert ^y search prompt ");
emacs_help_text[8] = catgetlocal( 153, "^h backspace ^p prev line ^z next word ");
emacs_help_text[9] = help_text[9];
emacs_help_text[10] = help_text[10];
emacs_help_text[11] = help_text[11];
emacs_help_text[12] = help_text[12];
emacs_help_text[13] = help_text[13];
emacs_help_text[14] = help_text[14];
emacs_help_text[15] = help_text[15];
emacs_help_text[16] = help_text[16];
emacs_help_text[17] = help_text[17];
emacs_help_text[18] = help_text[18];
emacs_help_text[19] = help_text[19];
emacs_help_text[20] = help_text[20];
emacs_help_text[21] = help_text[21];
emacs_control_keys[0] = catgetlocal( 154, "^[ (escape) menu ^y search prompt ^k delete line ^p prev li ^g prev page");
emacs_control_keys[1] = catgetlocal( 155, "^o ascii code ^x search ^l undelete line ^n next li ^v next page");
emacs_control_keys[2] = catgetlocal( 156, "^u end of file ^a begin of line ^w delete word ^b back 1 char ^z next word");
emacs_control_keys[3] = catgetlocal( 157, "^t top of text ^e end of line ^r restore word ^f forward char ");
emacs_control_keys[4] = catgetlocal( 158, "^c command ^d delete char ^j undelete char ESC-Enter: exit");
EMACS_string = catgetlocal( 159, "EMACS");
NOEMACS_string = catgetlocal( 160, "NOEMACS");
usage4 = catgetlocal( 161, " +#put cursor at line #\n");
conf_dump_err_msg = catgetlocal( 162, "unable to open .init.ee for writing, no configuration saved!");
conf_dump_success_msg = catgetlocal( 163, "ee configuration saved in file %s");
modes_menu[10].item_string = catgetlocal( 164, "save editor configuration");
config_dump_menu[0].item_string = catgetlocal( 165, "save ee configuration");
config_dump_menu[1].item_string = catgetlocal( 166, "save in current directory");
config_dump_menu[2].item_string = catgetlocal( 167, "save in home directory");
conf_not_saved_msg = catgetlocal( 168, "ee configuration not saved");
ree_no_file_msg = catgetlocal( 169, "must specify a file when invoking ree");
menu_too_lrg_msg = catgetlocal( 180, "menu too large for window");
more_above_str = catgetlocal( 181, "^^more^^");
more_below_str = catgetlocal( 182, "VVmoreVV");
mode_strings[9] = catgetlocal( 183, "16 bit characters ");
chinese_cmd = catgetlocal( 184, "16BIT");
nochinese_cmd = catgetlocal( 185, "NO16BIT");

commands[0] = HELP;
commands[1] = WRITE;
commands[2] = READ;
commands[3] = LINE;
commands[4] = FILE_str;
commands[5] = REDRAW;
commands[6] = RESEQUENCE;
commands[7] = AUTHOR;
commands[8] = VERSION;
commands[9] = CASE;
commands[10] = NOCASE;
commands[11] = EXPAND;
commands[12] = NOEXPAND;
commands[13] = Exit_string;
commands[14] = QUIT_string;
commands[15] = "<";
commands[16] = ">";
commands[17] = "!";
commands[18] = "0";
commands[19] = "1";
commands[20] = "2";
commands[21] = "3";
commands[22] = "4";
commands[23] = "5";
commands[24] = "6";
commands[25] = "7";
commands[26] = "8";
commands[27] = "9";
commands[28] = CHARACTER;
commands[29] = chinese_cmd;
commands[30] = nochinese_cmd;
commands[31] = NULL;
init_strings[0] = CASE;
init_strings[1] = NOCASE;
init_strings[2] = EXPAND;
init_strings[3] = NOEXPAND;
init_strings[4] = INFO;
init_strings[5] = NOINFO;
init_strings[6] = MARGINS;
init_strings[7] = NOMARGINS;
init_strings[8] = AUTOFORMAT;
init_strings[9] = NOAUTOFORMAT;
init_strings[10] = Echo;
init_strings[11] = PRINTCOMMAND;
init_strings[12] = RIGHTMARGIN;
init_strings[13] = HIGHLIGHT;
init_strings[14] = NOHIGHLIGHT;
init_strings[15] = EIGHTBIT;
init_strings[16] = NOEIGHTBIT;
init_strings[17] = EMACS_string;
init_strings[18] = NOEMACS_string;
init_strings[19] = chinese_cmd;
init_strings[20] = nochinese_cmd;
init_strings[21] = NULL;





for (counter = 1; counter < NUM_MODES_ITEMS; counter++)
{
modes_menu[counter].item_string = malloc(80);
}

#if !defined(NO_CATGETS)
catclose(catalog);
#endif
}

