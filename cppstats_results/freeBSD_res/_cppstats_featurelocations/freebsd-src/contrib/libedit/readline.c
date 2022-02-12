






























#include "config.h"
#if !defined(lint) && !defined(SCCSID)
__RCSID("$NetBSD: readline.c,v 1.168 2021/09/10 18:51:36 rillig Exp $");
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vis.h>

#include "readline/readline.h"
#include "el.h"
#include "fcns.h"
#include "filecomplete.h"

void rl_prep_terminal(int);
void rl_deprep_terminal(void);


#define TAB '\r'






const char *rl_library_version = "EditLine wrapper";
int rl_readline_version = RL_READLINE_VERSION;
static char empty[] = { '\0' };
static char expand_chars[] = { ' ', '\t', '\n', '=', '(', '\0' };
static char break_chars[] = { ' ', '\t', '\n', '"', '\\', '\'', '`', '@', '$',
'>', '<', '=', ';', '|', '&', '{', '(', '\0' };
const char *rl_readline_name = empty;
FILE *rl_instream = NULL;
FILE *rl_outstream = NULL;
int rl_point = 0;
int rl_end = 0;
char *rl_line_buffer = NULL;
rl_vcpfunc_t *rl_linefunc = NULL;
int rl_done = 0;
rl_hook_func_t *rl_event_hook = NULL;
KEYMAP_ENTRY_ARRAY emacs_standard_keymap,
emacs_meta_keymap,
emacs_ctlx_keymap;






int rl_catch_signals = 1;
int rl_catch_sigwinch = 1;

int history_base = 1;
int history_length = 0;
int history_offset = 0;
int max_input_history = 0;
char history_expansion_char = '!';
char history_subst_char = '^';
char *history_no_expand_chars = expand_chars;
Function *history_inhibit_expansion_function = NULL;
char *history_arg_extract(int start, int end, const char *str);

int rl_inhibit_completion = 0;
int rl_attempted_completion_over = 0;
const char *rl_basic_word_break_characters = break_chars;
char *rl_completer_word_break_characters = NULL;
const char *rl_completer_quote_characters = NULL;
const char *rl_basic_quote_characters = "\"'";
rl_compentry_func_t *rl_completion_entry_function = NULL;
char *(*rl_completion_word_break_hook)(void) = NULL;
rl_completion_func_t *rl_attempted_completion_function = NULL;
Function *rl_pre_input_hook = NULL;
Function *rl_startup1_hook = NULL;
int (*rl_getc_function)(FILE *) = NULL;
char *rl_terminal_name = NULL;
int rl_already_prompted = 0;
int rl_filename_completion_desired = 0;
int rl_ignore_completion_duplicates = 0;
int readline_echoing_p = 1;
int _rl_print_completions_horizontally = 0;
VFunction *rl_redisplay_function = NULL;
Function *rl_startup_hook = NULL;
VFunction *rl_completion_display_matches_hook = NULL;
VFunction *rl_prep_term_function = (VFunction *)rl_prep_terminal;
VFunction *rl_deprep_term_function = (VFunction *)rl_deprep_terminal;
KEYMAP_ENTRY_ARRAY emacs_meta_keymap;
unsigned long rl_readline_state;
int _rl_complete_mark_directories;
rl_icppfunc_t *rl_directory_completion_hook;
int rl_completion_suppress_append;
int rl_sort_completion_matches;
int _rl_completion_prefix_display_length;
int _rl_echoing_p;
int history_max_entries;
char *rl_display_prompt;




char *rl_prompt = NULL;





int rl_completion_type = 0;





int rl_completion_query_items = 100;






const char *rl_special_prefixes = NULL;





int rl_completion_append_character = ' ';



static History *h = NULL;
static EditLine *e = NULL;
static rl_command_func_t *map[256];
static jmp_buf topbuf;


static unsigned char _el_rl_complete(EditLine *, int);
static unsigned char _el_rl_tstp(EditLine *, int);
static char *_get_prompt(EditLine *);
static int _getc_function(EditLine *, wchar_t *);
static int _history_expand_command(const char *, size_t, size_t,
char **);
static char *_rl_compat_sub(const char *, const char *,
const char *, int);
static int _rl_event_read_char(EditLine *, wchar_t *);
static void _rl_update_pos(void);

static HIST_ENTRY rl_he;


static char *
_get_prompt(EditLine *el __attribute__((__unused__)))
{
rl_already_prompted = 1;
return rl_prompt;
}





static int

_getc_function(EditLine *el __attribute__((__unused__)), wchar_t *c)
{
int i;

i = (*rl_getc_function)(rl_instream);
if (i == -1)
return 0;
*c = (wchar_t)i;
return 1;
}

static void
_resize_fun(EditLine *el, void *a)
{
const LineInfo *li;
const char **ap = a;

li = el_line(el);
*ap = li->buffer;
}

static const char *
_default_history_file(void)
{
struct passwd *p;
static char *path;
size_t len;

if (path)
return path;

if ((p = getpwuid(getuid())) == NULL)
return NULL;

len = strlen(p->pw_dir) + sizeof("/.history");
if ((path = malloc(len)) == NULL)
return NULL;

(void)snprintf(path, len, "%s/.history", p->pw_dir);
return path;
}








int
rl_set_prompt(const char *prompt)
{
char *p;

if (!prompt)
prompt = "";
if (rl_prompt != NULL && strcmp(rl_prompt, prompt) == 0)
return 0;
if (rl_prompt)
el_free(rl_prompt);
rl_prompt = strdup(prompt);
if (rl_prompt == NULL)
return -1;

while ((p = strchr(rl_prompt, RL_PROMPT_END_IGNORE)) != NULL) {

if (p[1] == RL_PROMPT_START_IGNORE) {
memmove(p, p + 2, 1 + strlen(p + 2));
} else {
*p = RL_PROMPT_START_IGNORE;
}
}

return 0;
}




int
rl_initialize(void)
{
HistEvent ev;
int editmode = 1;
struct termios t;

if (e != NULL)
el_end(e);
if (h != NULL)
history_end(h);

if (!rl_instream)
rl_instream = stdin;
if (!rl_outstream)
rl_outstream = stdout;




if (tcgetattr(fileno(rl_instream), &t) != -1 && (t.c_lflag & ECHO) == 0)
editmode = 0;

e = el_init_internal(rl_readline_name, rl_instream, rl_outstream,
stderr, fileno(rl_instream), fileno(rl_outstream), fileno(stderr),
NO_RESET);

if (!editmode)
el_set(e, EL_EDITMODE, 0);

h = history_init();
if (!e || !h)
return -1;

history(h, &ev, H_SETSIZE, INT_MAX);
history_length = 0;
max_input_history = INT_MAX;
el_set(e, EL_HIST, history, h);


el_set(e, EL_RESIZE, _resize_fun, &rl_line_buffer);


if (rl_getc_function)
el_set(e, EL_GETCFN, _getc_function);


if (rl_set_prompt("") == -1) {
history_end(h);
el_end(e);
return -1;
}
el_set(e, EL_PROMPT_ESC, _get_prompt, RL_PROMPT_START_IGNORE);
el_set(e, EL_SIGNAL, rl_catch_signals);



el_set(e, EL_EDITOR, "emacs");
if (rl_terminal_name != NULL)
el_set(e, EL_TERMINAL, rl_terminal_name);
else
el_get(e, EL_TERMINAL, &rl_terminal_name);





el_set(e, EL_ADDFN, "rl_complete",
"ReadLine compatible completion function",
_el_rl_complete);
el_set(e, EL_BIND, "^I", "rl_complete", NULL);




el_set(e, EL_ADDFN, "rl_tstp",
"ReadLine compatible suspend function",
_el_rl_tstp);
el_set(e, EL_BIND, "^Z", "rl_tstp", NULL);




el_set(e, EL_BIND, "^R", "em-inc-search-prev", NULL);




el_set(e, EL_BIND, "\\e[1~", "ed-move-to-beg", NULL);
el_set(e, EL_BIND, "\\e[4~", "ed-move-to-end", NULL);
el_set(e, EL_BIND, "\\e[7~", "ed-move-to-beg", NULL);
el_set(e, EL_BIND, "\\e[8~", "ed-move-to-end", NULL);
el_set(e, EL_BIND, "\\e[H", "ed-move-to-beg", NULL);
el_set(e, EL_BIND, "\\e[F", "ed-move-to-end", NULL);




el_set(e, EL_BIND, "\\e[3~", "ed-delete-next-char", NULL);
el_set(e, EL_BIND, "\\e[2~", "ed-quoted-insert", NULL);




el_set(e, EL_BIND, "\\e[1;5C", "em-next-word", NULL);
el_set(e, EL_BIND, "\\e[1;5D", "ed-prev-word", NULL);
el_set(e, EL_BIND, "\\e[5C", "em-next-word", NULL);
el_set(e, EL_BIND, "\\e[5D", "ed-prev-word", NULL);
el_set(e, EL_BIND, "\\e\\e[C", "em-next-word", NULL);
el_set(e, EL_BIND, "\\e\\e[D", "ed-prev-word", NULL);


el_source(e, NULL);





_resize_fun(e, &rl_line_buffer);
_rl_update_pos();

tty_end(e, TCSADRAIN);

return 0;
}






char *
readline(const char *p)
{
HistEvent ev;
const char * volatile prompt = p;
int count;
const char *ret;
char *buf;
static int used_event_hook;

if (e == NULL || h == NULL)
rl_initialize();
if (rl_startup_hook) {
(*rl_startup_hook)(NULL, 0);
}
tty_init(e);


rl_done = 0;

(void)setjmp(topbuf);
buf = NULL;


if (rl_set_prompt(prompt) == -1)
goto out;

if (rl_pre_input_hook)
(*rl_pre_input_hook)(NULL, 0);

if (rl_event_hook && !(e->el_flags & NO_TTY)) {
el_set(e, EL_GETCFN, _rl_event_read_char);
used_event_hook = 1;
}

if (!rl_event_hook && used_event_hook) {
el_set(e, EL_GETCFN, EL_BUILTIN_GETCFN);
used_event_hook = 0;
}

rl_already_prompted = 0;


ret = el_gets(e, &count);

if (ret && count > 0) {
buf = strdup(ret);
if (buf == NULL)
goto out;
buf[strcspn(buf, "\n")] = '\0';
} else
buf = NULL;

history(h, &ev, H_GETSIZE);
history_length = ev.num;

out:
tty_end(e, TCSADRAIN);
return buf;
}









void
using_history(void)
{
if (h == NULL || e == NULL)
rl_initialize();
history_offset = history_length;
}







static char *
_rl_compat_sub(const char *str, const char *what, const char *with,
int globally)
{
const char *s;
char *r, *result;
size_t len, with_len, what_len;

len = strlen(str);
with_len = strlen(with);
what_len = strlen(what);


s = str;
while (*s) {
if (*s == *what && !strncmp(s, what, what_len)) {
len += with_len - what_len;
if (!globally)
break;
s += what_len;
} else
s++;
}
r = result = el_calloc(len + 1, sizeof(*r));
if (result == NULL)
return NULL;
s = str;
while (*s) {
if (*s == *what && !strncmp(s, what, what_len)) {
memcpy(r, with, with_len);
r += with_len;
s += what_len;
if (!globally) {
(void)strcpy(r, s);
return result;
}
} else
*r++ = *s++;
}
*r = '\0';
return result;
}

static char *last_search_pat;
static char *last_search_match;

const char *
get_history_event(const char *cmd, int *cindex, int qchar)
{
int idx, sign, sub, num, begin, ret;
size_t len;
char *pat;
const char *rptr;
HistEvent ev;

idx = *cindex;
if (cmd[idx++] != history_expansion_char)
return NULL;


if (cmd[idx] == history_expansion_char || cmd[idx] == '\0') {
if (history(h, &ev, H_FIRST) != 0)
return NULL;
*cindex = cmd[idx]? (idx + 1):idx;
return ev.str;
}
sign = 0;
if (cmd[idx] == '-') {
sign = 1;
idx++;
}

if ('0' <= cmd[idx] && cmd[idx] <= '9') {
HIST_ENTRY *he;

num = 0;
while (cmd[idx] && '0' <= cmd[idx] && cmd[idx] <= '9') {
num = num * 10 + cmd[idx] - '0';
idx++;
}
if (sign)
num = history_length - num + history_base;

if (!(he = history_get(num)))
return NULL;

*cindex = idx;
return he->line;
}
sub = 0;
if (cmd[idx] == '?') {
sub = 1;
idx++;
}
begin = idx;
while (cmd[idx]) {
if (cmd[idx] == '\n')
break;
if (sub && cmd[idx] == '?')
break;
if (!sub && (cmd[idx] == ':' || cmd[idx] == ' '
|| cmd[idx] == '\t' || cmd[idx] == qchar))
break;
idx++;
}
len = (size_t)idx - (size_t)begin;
if (sub && cmd[idx] == '?')
idx++;
if (sub && len == 0 && last_search_pat && *last_search_pat)
pat = last_search_pat;
else if (len == 0)
return NULL;
else {
if ((pat = el_calloc(len + 1, sizeof(*pat))) == NULL)
return NULL;
(void)strlcpy(pat, cmd + begin, len + 1);
}

if (history(h, &ev, H_CURR) != 0) {
if (pat != last_search_pat)
el_free(pat);
return NULL;
}
num = ev.num;

if (sub) {
if (pat != last_search_pat) {
el_free(last_search_pat);
last_search_pat = pat;
}
ret = history_search(pat, -1);
} else
ret = history_search_prefix(pat, -1);

if (ret == -1) {

history(h, &ev, H_FIRST);
(void)fprintf(rl_outstream, "%s: Event not found\n", pat);
if (pat != last_search_pat)
el_free(pat);
return NULL;
}

if (sub && len) {
el_free(last_search_match);
last_search_match = strdup(pat);
}

if (pat != last_search_pat)
el_free(pat);

if (history(h, &ev, H_CURR) != 0)
return NULL;
*cindex = idx;
rptr = ev.str;


(void)history(h, &ev, H_SET, num);

return rptr;
}

static int
getfrom(const char **cmdp, char **fromp, const char *search, int delim)
{
size_t size = 16;
size_t len = 0;
const char *cmd = *cmdp;
char *what = el_realloc(*fromp, size * sizeof(*what));
if (what == NULL){
el_free(*fromp);
*fromp = NULL;
return 0;
}
for (; *cmd && *cmd != delim; cmd++) {
if (*cmd == '\\' && cmd[1] == delim)
cmd++;
if (len - 1 >= size) {
char *nwhat;
nwhat = el_realloc(what, (size <<= 1) * sizeof(*nwhat));
if (nwhat == NULL) {
el_free(what);
el_free(*fromp);
*cmdp = cmd;
*fromp = NULL;
return 0;
}
what = nwhat;
}
what[len++] = *cmd;
}
what[len] = '\0';
*fromp = what;
*cmdp = cmd;
if (*what == '\0') {
el_free(what);
if (search) {
*fromp = strdup(search);
if (*fromp == NULL) {
return 0;
}
} else {
*fromp = NULL;
return -1;
}
}
if (!*cmd) {
el_free(what);
*fromp = NULL;
return -1;
}

cmd++;
*cmdp = cmd;

if (!*cmd) {
el_free(what);
*fromp = NULL;
return -1;
}
return 1;
}

static int
getto(const char **cmdp, char **top, const char *from, int delim)
{
size_t size = 16;
size_t len = 0;
size_t from_len = strlen(from);
const char *cmd = *cmdp;
char *with = el_realloc(*top, size * sizeof(*with));
*top = NULL;
if (with == NULL)
goto out;

for (; *cmd && *cmd != delim; cmd++) {
if (len + from_len + 1 >= size) {
char *nwith;
size += from_len + 1;
nwith = el_realloc(with, size * sizeof(*nwith));
if (nwith == NULL)
goto out;
with = nwith;
}
if (*cmd == '&') {

strcpy(&with[len], from);
len += from_len;
continue;
}
if (*cmd == '\\' && (*(cmd + 1) == delim || *(cmd + 1) == '&'))
cmd++;
with[len++] = *cmd;
}
if (!*cmd)
goto out;
with[len] = '\0';
*top = with;
*cmdp = cmd;
return 1;
out:
el_free(with);
el_free(*top);
*top = NULL;
*cmdp = cmd;
return -1;
}

static void
replace(char **tmp, int c)
{
char *aptr;
if ((aptr = strrchr(*tmp, c)) == NULL)
return;
aptr = strdup(aptr + 1);
el_free(*tmp);
*tmp = aptr;
}











static int
_history_expand_command(const char *command, size_t offs, size_t cmdlen,
char **result)
{
char *tmp, *search = NULL, *aptr, delim;
const char *ptr, *cmd;
static char *from = NULL, *to = NULL;
int start, end, idx, has_mods = 0;
int p_on = 0, g_on = 0, ev;

*result = NULL;
aptr = NULL;
ptr = NULL;


idx = 0;

if (strchr(":^*$", command[offs + 1])) {
char str[4];





str[0] = str[1] = '!';
str[2] = '0';
ptr = get_history_event(str, &idx, 0);
idx = (command[offs + 1] == ':')? 1:0;
has_mods = 1;
} else {
if (command[offs + 1] == '#') {

if ((aptr = el_calloc(offs + 1, sizeof(*aptr)))
== NULL)
return -1;
(void)strlcpy(aptr, command, offs + 1);
idx = 1;
} else {
int qchar;

qchar = (offs > 0 && command[offs - 1] == '"')
? '"' : '\0';
ptr = get_history_event(command + offs, &idx, qchar);
}
has_mods = command[offs + (size_t)idx] == ':';
}

if (ptr == NULL && aptr == NULL)
return -1;

if (!has_mods) {
*result = strdup(aptr ? aptr : ptr);
if (aptr)
el_free(aptr);
if (*result == NULL)
return -1;
return 1;
}

cmd = command + offs + idx + 1;



if (*cmd == '%')
tmp = strdup(last_search_match ? last_search_match : "");
else if (strchr("^*$-0123456789", *cmd)) {
start = end = -1;
if (*cmd == '^')
start = end = 1, cmd++;
else if (*cmd == '$')
start = -1, cmd++;
else if (*cmd == '*')
start = 1, cmd++;
else if (*cmd == '-' || isdigit((unsigned char) *cmd)) {
start = 0;
while (*cmd && '0' <= *cmd && *cmd <= '9')
start = start * 10 + *cmd++ - '0';

if (*cmd == '-') {
if (isdigit((unsigned char) cmd[1])) {
cmd++;
end = 0;
while (*cmd && '0' <= *cmd && *cmd <= '9')
end = end * 10 + *cmd++ - '0';
} else if (cmd[1] == '$') {
cmd += 2;
end = -1;
} else {
cmd++;
end = -2;
}
} else if (*cmd == '*')
end = -1, cmd++;
else
end = start;
}
tmp = history_arg_extract(start, end, aptr? aptr:ptr);
if (tmp == NULL) {
(void)fprintf(rl_outstream, "%s: Bad word specifier",
command + offs + idx);
if (aptr)
el_free(aptr);
return -1;
}
} else
tmp = strdup(aptr? aptr:ptr);

if (aptr)
el_free(aptr);

if (*cmd == '\0' || ((size_t)(cmd - (command + offs)) >= cmdlen)) {
*result = tmp;
return 1;
}

for (; *cmd; cmd++) {
switch (*cmd) {
case ':':
continue;
case 'h':
if ((aptr = strrchr(tmp, '/')) != NULL)
*aptr = '\0';
continue;
case 't':
replace(&tmp, '/');
continue;
case 'r':
if ((aptr = strrchr(tmp, '.')) != NULL)
*aptr = '\0';
continue;
case 'e':
replace(&tmp, '.');
continue;
case 'p':
p_on = 1;
continue;
case 'g':
g_on = 2;
continue;
case '&':
if (from == NULL || to == NULL)
continue;

case 's':
ev = -1;
delim = *++cmd;
if (delim == '\0' || *++cmd == '\0')
goto out;
if ((ev = getfrom(&cmd, &from, search, delim)) != 1)
goto out;
if ((ev = getto(&cmd, &to, from, delim)) != 1)
goto out;
aptr = _rl_compat_sub(tmp, from, to, g_on);
if (aptr) {
el_free(tmp);
tmp = aptr;
}
g_on = 0;
cmd--;
continue;
}
}
*result = tmp;
return p_on ? 2 : 1;
out:
el_free(tmp);
return ev;

}





int
history_expand(char *str, char **output)
{
int ret = 0;
size_t idx, i, size;
char *tmp, *result;

if (h == NULL || e == NULL)
rl_initialize();

if (history_expansion_char == 0) {
*output = strdup(str);
return 0;
}

*output = NULL;
if (str[0] == history_subst_char) {

*output = el_calloc(strlen(str) + 4 + 1, sizeof(**output));
if (*output == NULL)
return 0;
(*output)[0] = (*output)[1] = history_expansion_char;
(*output)[2] = ':';
(*output)[3] = 's';
(void)strcpy((*output) + 4, str);
str = *output;
} else {
*output = strdup(str);
if (*output == NULL)
return 0;
}

#define ADD_STRING(what, len, fr) { if (idx + len + 1 > size) { char *nresult = el_realloc(result, (size += len + 1) * sizeof(*nresult)); if (nresult == NULL) { el_free(*output); el_free(fr); return 0; } result = nresult; } (void)strlcpy(&result[idx], what, len + 1); idx += len; }















result = NULL;
size = idx = 0;
tmp = NULL;
for (i = 0; str[i];) {
int qchar, loop_again;
size_t len, start, j;

qchar = 0;
loop_again = 1;
start = j = i;
loop:
for (; str[j]; j++) {
if (str[j] == '\\' &&
str[j + 1] == history_expansion_char) {
len = strlen(&str[j + 1]) + 1;
memmove(&str[j], &str[j + 1], len);
continue;
}
if (!loop_again) {
if (isspace((unsigned char) str[j])
|| str[j] == qchar)
break;
}
if (str[j] == history_expansion_char
&& !strchr(history_no_expand_chars, str[j + 1])
&& (!history_inhibit_expansion_function ||
(*history_inhibit_expansion_function)(str,
(int)j) == 0))
break;
}

if (str[j] && loop_again) {
i = j;
qchar = (j > 0 && str[j - 1] == '"' )? '"':0;
j++;
if (str[j] == history_expansion_char)
j++;
loop_again = 0;
goto loop;
}
len = i - start;
ADD_STRING(&str[start], len, NULL);

if (str[i] == '\0' || str[i] != history_expansion_char) {
len = j - i;
ADD_STRING(&str[i], len, NULL);
if (start == 0)
ret = 0;
else
ret = 1;
break;
}
ret = _history_expand_command (str, i, (j - i), &tmp);
if (ret > 0 && tmp) {
len = strlen(tmp);
ADD_STRING(tmp, len, tmp);
}
if (tmp) {
el_free(tmp);
tmp = NULL;
}
i = j;
}


if (ret == 2) {
add_history(result);
#if defined(GDB_411_HACK)




ret = -1;
#endif
}
el_free(*output);
*output = result;

return ret;
}




char *
history_arg_extract(int start, int end, const char *str)
{
size_t i, len, max;
char **arr, *result = NULL;

arr = history_tokenize(str);
if (!arr)
return NULL;
if (arr && *arr == NULL)
goto out;

for (max = 0; arr[max]; max++)
continue;
max--;

if (start == '$')
start = (int)max;
if (end == '$')
end = (int)max;
if (end < 0)
end = (int)max + end + 1;
if (start < 0)
start = end;

if (start < 0 || end < 0 || (size_t)start > max ||
(size_t)end > max || start > end)
goto out;

for (i = (size_t)start, len = 0; i <= (size_t)end; i++)
len += strlen(arr[i]) + 1;
len++;
result = el_calloc(len, sizeof(*result));
if (result == NULL)
goto out;

for (i = (size_t)start, len = 0; i <= (size_t)end; i++) {
(void)strcpy(result + len, arr[i]);
len += strlen(arr[i]);
if (i < (size_t)end)
result[len++] = ' ';
}
result[len] = '\0';

out:
for (i = 0; arr[i]; i++)
el_free(arr[i]);
el_free(arr);

return result;
}





char **
history_tokenize(const char *str)
{
int size = 1, idx = 0, i, start;
size_t len;
char **result = NULL, *temp, delim = '\0';

for (i = 0; str[i];) {
while (isspace((unsigned char) str[i]))
i++;
start = i;
for (; str[i];) {
if (str[i] == '\\') {
if (str[i+1] != '\0')
i++;
} else if (str[i] == delim)
delim = '\0';
else if (!delim &&
(isspace((unsigned char) str[i]) ||
strchr("()<>;&|$", str[i])))
break;
else if (!delim && strchr("'`\"", str[i]))
delim = str[i];
if (str[i])
i++;
}

if (idx + 2 >= size) {
char **nresult;
size <<= 1;
nresult = el_realloc(result, (size_t)size * sizeof(*nresult));
if (nresult == NULL) {
el_free(result);
return NULL;
}
result = nresult;
}
len = (size_t)i - (size_t)start;
temp = el_calloc(len + 1, sizeof(*temp));
if (temp == NULL) {
for (i = 0; i < idx; i++)
el_free(result[i]);
el_free(result);
return NULL;
}
(void)strlcpy(temp, &str[start], len + 1);
result[idx++] = temp;
result[idx] = NULL;
if (str[i])
i++;
}
return result;
}





void
stifle_history(int max)
{
HistEvent ev;
HIST_ENTRY *he;

if (h == NULL || e == NULL)
rl_initialize();

if (history(h, &ev, H_SETSIZE, max) == 0) {
max_input_history = max;
if (history_length > max)
history_base = history_length - max;
while (history_length > max) {
he = remove_history(0);
el_free(he->data);
el_free((void *)(unsigned long)he->line);
el_free(he);
}
}
}





int
unstifle_history(void)
{
HistEvent ev;
int omax;

history(h, &ev, H_SETSIZE, INT_MAX);
omax = max_input_history;
max_input_history = INT_MAX;
return omax;
}


int
history_is_stifled(void)
{


return max_input_history != INT_MAX;
}

static const char _history_tmp_template[] = "/tmp/.historyXXXXXX";

int
history_truncate_file (const char *filename, int nlines)
{
int ret = 0;
FILE *fp, *tp;
char template[sizeof(_history_tmp_template)];
char buf[4096];
int fd;
char *cp;
off_t off;
int count = 0;
ssize_t left = 0;

if (filename == NULL && (filename = _default_history_file()) == NULL)
return errno;
if ((fp = fopen(filename, "r+")) == NULL)
return errno;
strcpy(template, _history_tmp_template);
if ((fd = mkstemp(template)) == -1) {
ret = errno;
goto out1;
}

if ((tp = fdopen(fd, "r+")) == NULL) {
close(fd);
ret = errno;
goto out2;
}

for(;;) {
if (fread(buf, sizeof(buf), (size_t)1, fp) != 1) {
if (ferror(fp)) {
ret = errno;
break;
}
if (fseeko(fp, (off_t)sizeof(buf) * count, SEEK_SET) ==
(off_t)-1) {
ret = errno;
break;
}
left = (ssize_t)fread(buf, (size_t)1, sizeof(buf), fp);
if (ferror(fp)) {
ret = errno;
break;
}
if (left == 0) {
count--;
left = sizeof(buf);
} else if (fwrite(buf, (size_t)left, (size_t)1, tp)
!= 1) {
ret = errno;
break;
}
fflush(tp);
break;
}
if (fwrite(buf, sizeof(buf), (size_t)1, tp) != 1) {
ret = errno;
break;
}
count++;
}
if (ret)
goto out3;
cp = buf + left - 1;
if(*cp != '\n')
cp++;
for(;;) {
while (--cp >= buf) {
if (*cp == '\n') {
if (--nlines == 0) {
if (++cp >= buf + sizeof(buf)) {
count++;
cp = buf;
}
break;
}
}
}
if (nlines <= 0 || count == 0)
break;
count--;
if (fseeko(tp, (off_t)sizeof(buf) * count, SEEK_SET) < 0) {
ret = errno;
break;
}
if (fread(buf, sizeof(buf), (size_t)1, tp) != 1) {
if (ferror(tp)) {
ret = errno;
break;
}
ret = EAGAIN;
break;
}
cp = buf + sizeof(buf);
}

if (ret || nlines > 0)
goto out3;

if (fseeko(fp, (off_t)0, SEEK_SET) == (off_t)-1) {
ret = errno;
goto out3;
}

if (fseeko(tp, (off_t)sizeof(buf) * count + (cp - buf), SEEK_SET) ==
(off_t)-1) {
ret = errno;
goto out3;
}

for(;;) {
if ((left = (ssize_t)fread(buf, (size_t)1, sizeof(buf), tp)) == 0) {
if (ferror(fp))
ret = errno;
break;
}
if (fwrite(buf, (size_t)left, (size_t)1, fp) != 1) {
ret = errno;
break;
}
}
fflush(fp);
if((off = ftello(fp)) > 0)
(void)ftruncate(fileno(fp), off);
out3:
fclose(tp);
out2:
unlink(template);
out1:
fclose(fp);

return ret;
}





int
read_history(const char *filename)
{
HistEvent ev;

if (h == NULL || e == NULL)
rl_initialize();
if (filename == NULL && (filename = _default_history_file()) == NULL)
return errno;
errno = 0;
if (history(h, &ev, H_LOAD, filename) == -1)
return errno ? errno : EINVAL;
if (history(h, &ev, H_GETSIZE) == 0)
history_length = ev.num;
if (history_length < 0)
return EINVAL;
return 0;
}





int
write_history(const char *filename)
{
HistEvent ev;

if (h == NULL || e == NULL)
rl_initialize();
if (filename == NULL && (filename = _default_history_file()) == NULL)
return errno;
return history(h, &ev, H_SAVE, filename) == -1 ?
(errno ? errno : EINVAL) : 0;
}

int
append_history(int n, const char *filename)
{
HistEvent ev;
FILE *fp;

if (h == NULL || e == NULL)
rl_initialize();
if (filename == NULL && (filename = _default_history_file()) == NULL)
return errno;

if ((fp = fopen(filename, "a")) == NULL)
return errno;

if (history(h, &ev, H_NSAVE_FP, (size_t)n, fp) == -1) {
int serrno = errno ? errno : EINVAL;
fclose(fp);
return serrno;
}
fclose(fp);
return 0;
}






HIST_ENTRY *
history_get(int num)
{
static HIST_ENTRY she;
HistEvent ev;
int curr_num;

if (h == NULL || e == NULL)
rl_initialize();

if (num < history_base)
return NULL;


if (history(h, &ev, H_CURR) != 0)
return NULL;
curr_num = ev.num;





if (history(h, &ev, H_DELDATA, num - history_base, (void **)-1) != 0)
goto out;


if (history(h, &ev, H_CURR) != 0)
goto out;
if (history(h, &ev, H_NEXT_EVDATA, ev.num, &she.data) != 0)
goto out;
she.line = ev.str;


(void)history(h, &ev, H_SET, curr_num);

return &she;

out:

(void)history(h, &ev, H_SET, curr_num);
return NULL;
}





int
add_history(const char *line)
{
HistEvent ev;

if (h == NULL || e == NULL)
rl_initialize();

if (history(h, &ev, H_ENTER, line) == -1)
return 0;

(void)history(h, &ev, H_GETSIZE);
if (ev.num == history_length)
history_base++;
else {
history_offset++;
history_length = ev.num;
}
return 0;
}





HIST_ENTRY *
remove_history(int num)
{
HIST_ENTRY *he;
HistEvent ev;

if (h == NULL || e == NULL)
rl_initialize();

if ((he = el_malloc(sizeof(*he))) == NULL)
return NULL;

if (history(h, &ev, H_DELDATA, num, &he->data) != 0) {
el_free(he);
return NULL;
}

he->line = ev.str;
if (history(h, &ev, H_GETSIZE) == 0)
history_length = ev.num;

return he;
}





HIST_ENTRY *
replace_history_entry(int num, const char *line, histdata_t data)
{
HIST_ENTRY *he;
HistEvent ev;
int curr_num;

if (h == NULL || e == NULL)
rl_initialize();


if (history(h, &ev, H_CURR) != 0)
return NULL;
curr_num = ev.num;


if (history(h, &ev, H_LAST) != 0)
return NULL;

if ((he = el_malloc(sizeof(*he))) == NULL)
return NULL;


if (history(h, &ev, H_NEXT_EVDATA, num, &he->data))
goto out;

he->line = strdup(ev.str);
if (he->line == NULL)
goto out;

if (history(h, &ev, H_REPLACE, line, data))
goto out;


if (history(h, &ev, H_SET, curr_num))
goto out;

return he;
out:
el_free(he);
return NULL;
}




void
clear_history(void)
{
HistEvent ev;

if (h == NULL || e == NULL)
rl_initialize();

(void)history(h, &ev, H_CLEAR);
history_offset = history_length = 0;
}





int
where_history(void)
{
return history_offset;
}

static HIST_ENTRY **_history_listp;
static HIST_ENTRY *_history_list;

HIST_ENTRY **
history_list(void)
{
HistEvent ev;
HIST_ENTRY **nlp, *nl;
int i;

if (history(h, &ev, H_LAST) != 0)
return NULL;

if ((nlp = el_realloc(_history_listp,
((size_t)history_length + 1) * sizeof(*nlp))) == NULL)
return NULL;
_history_listp = nlp;

if ((nl = el_realloc(_history_list,
(size_t)history_length * sizeof(*nl))) == NULL)
return NULL;
_history_list = nl;

i = 0;
do {
_history_listp[i] = &_history_list[i];
_history_list[i].line = ev.str;
_history_list[i].data = NULL;
if (i++ == history_length)
abort();
} while (history(h, &ev, H_PREV) == 0);
_history_listp[i] = NULL;
return _history_listp;
}




HIST_ENTRY *
current_history(void)
{
HistEvent ev;

if (history(h, &ev, H_PREV_EVENT, history_offset + 1) != 0)
return NULL;

rl_he.line = ev.str;
rl_he.data = NULL;
return &rl_he;
}





int
history_total_bytes(void)
{
HistEvent ev;
int curr_num;
size_t size;

if (history(h, &ev, H_CURR) != 0)
return -1;
curr_num = ev.num;

(void)history(h, &ev, H_FIRST);
size = 0;
do
size += strlen(ev.str) * sizeof(*ev.str);
while (history(h, &ev, H_NEXT) == 0);


history(h, &ev, H_PREV_EVENT, curr_num);

return (int)size;
}





int
history_set_pos(int pos)
{
if (pos >= history_length || pos < 0)
return 0;

history_offset = pos;
return 1;
}






HIST_ENTRY *
previous_history(void)
{
HistEvent ev;

if (history_offset == 0)
return NULL;

if (history(h, &ev, H_LAST) != 0)
return NULL;

history_offset--;
return current_history();
}





HIST_ENTRY *
next_history(void)
{
HistEvent ev;

if (history_offset >= history_length)
return NULL;

if (history(h, &ev, H_LAST) != 0)
return NULL;

history_offset++;
return current_history();
}





int
history_search(const char *str, int direction)
{
HistEvent ev;
const char *strp;
int curr_num;

if (history(h, &ev, H_CURR) != 0)
return -1;
curr_num = ev.num;

for (;;) {
if ((strp = strstr(ev.str, str)) != NULL)
return (int)(strp - ev.str);
if (history(h, &ev, direction < 0 ? H_NEXT:H_PREV) != 0)
break;
}
(void)history(h, &ev, H_SET, curr_num);
return -1;
}





int
history_search_prefix(const char *str, int direction)
{
HistEvent ev;

return (history(h, &ev, direction < 0 ?
H_PREV_STR : H_NEXT_STR, str));
}







int
history_search_pos(const char *str,
int direction __attribute__((__unused__)), int pos)
{
HistEvent ev;
int curr_num, off;

off = (pos > 0) ? pos : -pos;
pos = (pos > 0) ? 1 : -1;

if (history(h, &ev, H_CURR) != 0)
return -1;
curr_num = ev.num;

if (!history_set_pos(off) || history(h, &ev, H_CURR) != 0)
return -1;

for (;;) {
if (strstr(ev.str, str))
return off;
if (history(h, &ev, (pos < 0) ? H_PREV : H_NEXT) != 0)
break;
}


(void)history(h, &ev,
pos < 0 ? H_NEXT_EVENT : H_PREV_EVENT, curr_num);

return -1;
}





char *
tilde_expand(char *name)
{
return fn_tilde_expand(name);
}

char *
filename_completion_function(const char *name, int state)
{
return fn_filename_completion_function(name, state);
}








char *
username_completion_function(const char *text, int state)
{
#if defined(HAVE_GETPW_R_POSIX) || defined(HAVE_GETPW_R_DRAFT)
struct passwd pwres;
char pwbuf[1024];
#endif
struct passwd *pass = NULL;

if (text[0] == '\0')
return NULL;

if (*text == '~')
text++;

if (state == 0)
setpwent();

while (
#if defined(HAVE_GETPW_R_POSIX) || defined(HAVE_GETPW_R_DRAFT)
getpwent_r(&pwres, pwbuf, sizeof(pwbuf), &pass) == 0 && pass != NULL
#else
(pass = getpwent()) != NULL
#endif
&& text[0] == pass->pw_name[0]
&& strcmp(text, pass->pw_name) == 0)
continue;

if (pass == NULL) {
endpwent();
return NULL;
}
return strdup(pass->pw_name);
}






static unsigned char
_el_rl_tstp(EditLine *el __attribute__((__unused__)), int ch __attribute__((__unused__)))
{
(void)kill(0, SIGTSTP);
return CC_NORM;
}

static const char *

_rl_completion_append_character_function(const char *dummy
__attribute__((__unused__)))
{
static char buf[2];
buf[0] = (char)rl_completion_append_character;
buf[1] = '\0';
return buf;
}







void
rl_display_match_list(char **matches, int len, int max)
{

fn_display_match_list(e, matches, (size_t)len, (size_t)max,
_rl_completion_append_character_function);
}





int
rl_complete(int ignore __attribute__((__unused__)), int invoking_key)
{
static ct_buffer_t wbreak_conv, sprefix_conv;
const char *breakchars;

if (h == NULL || e == NULL)
rl_initialize();

if (rl_inhibit_completion) {
char arr[2];
arr[0] = (char)invoking_key;
arr[1] = '\0';
el_insertstr(e, arr);
return CC_REFRESH;
}

if (rl_completion_word_break_hook != NULL)
breakchars = (*rl_completion_word_break_hook)();
else
breakchars = rl_basic_word_break_characters;

_rl_update_pos();


return fn_complete(e,
(rl_compentry_func_t *)rl_completion_entry_function,
rl_attempted_completion_function,
ct_decode_string(rl_basic_word_break_characters, &wbreak_conv),
ct_decode_string(breakchars, &sprefix_conv),
_rl_completion_append_character_function,
(size_t)rl_completion_query_items,
&rl_completion_type, &rl_attempted_completion_over,
&rl_point, &rl_end);


}



static unsigned char
_el_rl_complete(EditLine *el __attribute__((__unused__)), int ch)
{
return (unsigned char)rl_complete(0, ch);
}








int
rl_bind_key(int c, rl_command_func_t *func)
{
int retval = -1;

if (h == NULL || e == NULL)
rl_initialize();

if (func == rl_insert) {

e->el_map.key[c] = ED_INSERT;
retval = 0;
}
return retval;
}






int
rl_read_key(void)
{
char fooarr[2 * sizeof(int)];

if (e == NULL || h == NULL)
rl_initialize();

return el_getc(e, fooarr);
}






int
rl_reset_terminal(const char *p __attribute__((__unused__)))
{

if (h == NULL || e == NULL)
rl_initialize();
el_reset(e);
return 0;
}





int
rl_insert(int count, int c)
{
char arr[2];

if (h == NULL || e == NULL)
rl_initialize();


arr[0] = (char)c;
arr[1] = '\0';

for (; count > 0; count--)
el_push(e, arr);

return 0;
}

int
rl_insert_text(const char *text)
{
if (!text || *text == 0)
return 0;

if (h == NULL || e == NULL)
rl_initialize();

if (el_insertstr(e, text) < 0)
return 0;
return (int)strlen(text);
}


int
rl_newline(int count __attribute__((__unused__)),
int c __attribute__((__unused__)))
{



return rl_insert(1, '\n');
}


static unsigned char
rl_bind_wrapper(EditLine *el __attribute__((__unused__)), unsigned char c)
{
if (map[c] == NULL)
return CC_ERROR;

_rl_update_pos();

(*map[c])(1, c);


if (rl_done)
return CC_EOF;

return CC_NORM;
}

int
rl_add_defun(const char *name, rl_command_func_t *fun, int c)
{
char dest[8];
if ((size_t)c >= sizeof(map) / sizeof(map[0]) || c < 0)
return -1;
map[(unsigned char)c] = fun;
el_set(e, EL_ADDFN, name, name, rl_bind_wrapper);
vis(dest, c, VIS_WHITE|VIS_NOSLASH, 0);
el_set(e, EL_BIND, dest, name, NULL);
return 0;
}

void
rl_callback_read_char(void)
{
int count = 0, done = 0;
const char *buf = el_gets(e, &count);
char *wbuf;

el_set(e, EL_UNBUFFERED, 1);
if (buf == NULL || count-- <= 0)
return;
if (count == 0 && buf[0] == e->el_tty.t_c[TS_IO][C_EOF])
done = 1;
if (buf[count] == '\n' || buf[count] == '\r')
done = 2;

if (done && rl_linefunc != NULL) {
el_set(e, EL_UNBUFFERED, 0);
if (done == 2) {
if ((wbuf = strdup(buf)) != NULL)
wbuf[count] = '\0';
} else
wbuf = NULL;
(*(void (*)(const char *))rl_linefunc)(wbuf);
}
}

void
rl_callback_handler_install(const char *prompt, rl_vcpfunc_t *linefunc)
{
if (e == NULL) {
rl_initialize();
}
(void)rl_set_prompt(prompt);
rl_linefunc = linefunc;
el_set(e, EL_UNBUFFERED, 1);
}

void
rl_callback_handler_remove(void)
{
el_set(e, EL_UNBUFFERED, 0);
rl_linefunc = NULL;
}

void
rl_redisplay(void)
{
char a[2];
a[0] = (char)e->el_tty.t_c[TS_IO][C_REPRINT];
a[1] = '\0';
el_push(e, a);
}

int
rl_get_previous_history(int count, int key)
{
char a[2];
a[0] = (char)key;
a[1] = '\0';
while (count--)
el_push(e, a);
return 0;
}

void

rl_prep_terminal(int meta_flag __attribute__((__unused__)))
{
el_set(e, EL_PREP_TERM, 1);
}

void
rl_deprep_terminal(void)
{
el_set(e, EL_PREP_TERM, 0);
}

int
rl_read_init_file(const char *s)
{
return el_source(e, s);
}

int
rl_parse_and_bind(const char *line)
{
const char **argv;
int argc;
Tokenizer *tok;

tok = tok_init(NULL);
tok_str(tok, line, &argc, &argv);
argc = el_parse(e, argc, argv);
tok_end(tok);
return argc ? 1 : 0;
}

int
rl_variable_bind(const char *var, const char *value)
{




return el_set(e, EL_BIND, "", var, value, NULL) == -1 ? 1 : 0;
}

int
rl_stuff_char(int c)
{
char buf[2];

buf[0] = (char)c;
buf[1] = '\0';
el_insertstr(e, buf);
return 1;
}

static int
_rl_event_read_char(EditLine *el, wchar_t *wc)
{
char ch;
int n;
ssize_t num_read = 0;

ch = '\0';
*wc = L'\0';
while (rl_event_hook) {

(*rl_event_hook)();

#if defined(FIONREAD)
if (ioctl(el->el_infd, FIONREAD, &n) < 0)
return -1;
if (n)
num_read = read(el->el_infd, &ch, (size_t)1);
else
num_read = 0;
#elif defined(F_SETFL) && defined(O_NDELAY)
if ((n = fcntl(el->el_infd, F_GETFL, 0)) < 0)
return -1;
if (fcntl(el->el_infd, F_SETFL, n|O_NDELAY) < 0)
return -1;
num_read = read(el->el_infd, &ch, 1);
if (fcntl(el->el_infd, F_SETFL, n))
return -1;
#else

num_read = read(el->el_infd, &ch, 1);
return -1;
#endif

if (num_read < 0 && errno == EAGAIN)
continue;
if (num_read == 0)
continue;
break;
}
if (!rl_event_hook)
el_set(el, EL_GETCFN, EL_BUILTIN_GETCFN);
*wc = (wchar_t)ch;
return (int)num_read;
}

static void
_rl_update_pos(void)
{
const LineInfo *li = el_line(e);

rl_point = (int)(li->cursor - li->buffer);
rl_end = (int)(li->lastchar - li->buffer);
rl_line_buffer[rl_end] = '\0';
}

void
rl_get_screen_size(int *rows, int *cols)
{
if (rows)
el_get(e, EL_GETTC, "li", rows);
if (cols)
el_get(e, EL_GETTC, "co", cols);
}

void
rl_set_screen_size(int rows, int cols)
{
char buf[64];
(void)snprintf(buf, sizeof(buf), "%d", rows);
el_set(e, EL_SETTC, "li", buf, NULL);
(void)snprintf(buf, sizeof(buf), "%d", cols);
el_set(e, EL_SETTC, "co", buf, NULL);
}

char **
rl_completion_matches(const char *str, rl_compentry_func_t *fun)
{
size_t len, max, i, j, min;
char **list, *match, *a, *b;

len = 1;
max = 10;
if ((list = el_calloc(max, sizeof(*list))) == NULL)
return NULL;

while ((match = (*fun)(str, (int)(len - 1))) != NULL) {
list[len++] = match;
if (len == max) {
char **nl;
max += 10;
if ((nl = el_realloc(list, max * sizeof(*nl))) == NULL)
goto out;
list = nl;
}
}
if (len == 1)
goto out;
list[len] = NULL;
if (len == 2) {
if ((list[0] = strdup(list[1])) == NULL)
goto out;
return list;
}
qsort(&list[1], len - 1, sizeof(*list),
(int (*)(const void *, const void *)) strcmp);
min = SIZE_MAX;
for (i = 1, a = list[i]; i < len - 1; i++, a = b) {
b = list[i + 1];
for (j = 0; a[j] && a[j] == b[j]; j++)
continue;
if (min > j)
min = j;
}
if (min == 0 && *str) {
if ((list[0] = strdup(str)) == NULL)
goto out;
} else {
if ((list[0] = el_calloc(min + 1, sizeof(*list[0]))) == NULL)
goto out;
(void)memcpy(list[0], list[1], min);
list[0][min] = '\0';
}
return list;

out:
el_free(list);
return NULL;
}

char *
rl_filename_completion_function (const char *text, int state)
{
return fn_filename_completion_function(text, state);
}

void
rl_forced_update_display(void)
{
el_set(e, EL_REFRESH);
}

int
_rl_abort_internal(void)
{
el_beep(e);
longjmp(topbuf, 1);

}

int
_rl_qsort_string_compare(char **s1, char **s2)
{
return strcoll(*s1, *s2);
}

HISTORY_STATE *
history_get_history_state(void)
{
HISTORY_STATE *hs;

if ((hs = el_malloc(sizeof(*hs))) == NULL)
return NULL;
hs->length = history_length;
return hs;
}

int

rl_kill_text(int from __attribute__((__unused__)),
int to __attribute__((__unused__)))
{
return 0;
}

Keymap
rl_make_bare_keymap(void)
{
return NULL;
}

Keymap
rl_get_keymap(void)
{
return NULL;
}

void

rl_set_keymap(Keymap k __attribute__((__unused__)))
{
}

int

rl_generic_bind(int type __attribute__((__unused__)),
const char * keyseq __attribute__((__unused__)),
const char * data __attribute__((__unused__)),
Keymap k __attribute__((__unused__)))
{
return 0;
}

int

rl_bind_key_in_map(int key __attribute__((__unused__)),
rl_command_func_t *fun __attribute__((__unused__)),
Keymap k __attribute__((__unused__)))
{
return 0;
}


void
rl_cleanup_after_signal(void)
{
}

int
rl_on_new_line(void)
{
return 0;
}

void
rl_free_line_state(void)
{
}

int

rl_set_keyboard_input_timeout(int u __attribute__((__unused__)))
{
return 0;
}

void
rl_resize_terminal(void)
{
el_resize(e);
}

void
rl_reset_after_signal(void)
{
if (rl_prep_term_function)
(*rl_prep_term_function)();
}

void
rl_echo_signal_char(int sig)
{
int c = tty_get_signal_character(e, sig);
if (c == -1)
return;
re_putc(e, c, 0);
}

int
rl_crlf(void)
{
re_putc(e, '\n', 0);
return 0;
}

int
rl_ding(void)
{
re_putc(e, '\a', 0);
return 0;
}

int
rl_abort(int count, int key)
{
return count && key ? 0 : 0;
}

int
rl_set_keymap_name(const char *name, Keymap k)
{
return name && k ? 0 : 0;
}

histdata_t
free_history_entry(HIST_ENTRY *he)
{
return he ? NULL : NULL;
}

void
_rl_erase_entire_line(void)
{
}
