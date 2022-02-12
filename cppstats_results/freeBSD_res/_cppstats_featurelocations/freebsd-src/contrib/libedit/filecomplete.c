






























#include "config.h"
#if !defined(lint) && !defined(SCCSID)
__RCSID("$NetBSD: filecomplete.c,v 1.68 2021/05/05 14:49:59 christos Exp $");
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "el.h"
#include "filecomplete.h"

static const wchar_t break_chars[] = L" \t\n\"\\'`@$><=;|&{(";











char *
fn_tilde_expand(const char *txt)
{
#if defined(HAVE_GETPW_R_POSIX) || defined(HAVE_GETPW_R_DRAFT)
struct passwd pwres;
char pwbuf[1024];
#endif
struct passwd *pass;
char *temp;
size_t len = 0;

if (txt[0] != '~')
return strdup(txt);

temp = strchr(txt + 1, '/');
if (temp == NULL) {
temp = strdup(txt + 1);
if (temp == NULL)
return NULL;
} else {

len = (size_t)(temp - txt + 1);
temp = el_calloc(len, sizeof(*temp));
if (temp == NULL)
return NULL;
(void)strlcpy(temp, txt + 1, len - 1);
}
if (temp[0] == 0) {
#if defined(HAVE_GETPW_R_POSIX)
if (getpwuid_r(getuid(), &pwres, pwbuf, sizeof(pwbuf),
&pass) != 0)
pass = NULL;
#elif HAVE_GETPW_R_DRAFT
pass = getpwuid_r(getuid(), &pwres, pwbuf, sizeof(pwbuf));
#else
pass = getpwuid(getuid());
#endif
} else {
#if defined(HAVE_GETPW_R_POSIX)
if (getpwnam_r(temp, &pwres, pwbuf, sizeof(pwbuf), &pass) != 0)
pass = NULL;
#elif HAVE_GETPW_R_DRAFT
pass = getpwnam_r(temp, &pwres, pwbuf, sizeof(pwbuf));
#else
pass = getpwnam(temp);
#endif
}
el_free(temp);
if (pass == NULL)
return strdup(txt);



txt += len;

len = strlen(pass->pw_dir) + 1 + strlen(txt) + 1;
temp = el_calloc(len, sizeof(*temp));
if (temp == NULL)
return NULL;
(void)snprintf(temp, len, "%s/%s", pass->pw_dir, txt);

return temp;
}

static int
needs_escaping(char c)
{
switch (c) {
case '\'':
case '"':
case '(':
case ')':
case '\\':
case '<':
case '>':
case '$':
case '#':
case ' ':
case '\n':
case '\t':
case '?':
case ';':
case '`':
case '@':
case '=':
case '|':
case '{':
case '}':
case '&':
case '*':
case '[':
return 1;
default:
return 0;
}
}

static int
needs_dquote_escaping(char c)
{
switch (c) {
case '"':
case '\\':
case '`':
case '$':
return 1;
default:
return 0;
}
}


static wchar_t *
unescape_string(const wchar_t *string, size_t length)
{
size_t i;
size_t j = 0;
wchar_t *unescaped = el_calloc(length + 1, sizeof(*string));
if (unescaped == NULL)
return NULL;
for (i = 0; i < length ; i++) {
if (string[i] == '\\')
continue;
unescaped[j++] = string[i];
}
unescaped[j] = 0;
return unescaped;
}

static char *
escape_filename(EditLine * el, const char *filename, int single_match,
const char *(*app_func)(const char *))
{
size_t original_len = 0;
size_t escaped_character_count = 0;
size_t offset = 0;
size_t newlen;
const char *s;
char c;
size_t s_quoted = 0;
size_t d_quoted = 0;
char *escaped_str;
wchar_t *temp = el->el_line.buffer;
const char *append_char = NULL;

if (filename == NULL)
return NULL;

while (temp != el->el_line.cursor) {




if (temp[0] == '\'' && !d_quoted)
s_quoted = !s_quoted;



else if (temp[0] == '"' && !s_quoted)
d_quoted = !d_quoted;
temp++;
}




for (s = filename; *s; s++, original_len++) {
c = *s;

if (s_quoted && c == '\'') {
escaped_character_count += 3;
continue;
}

if (d_quoted && needs_dquote_escaping(c)) {
escaped_character_count++;
continue;
}
if (!s_quoted && !d_quoted && needs_escaping(c))
escaped_character_count++;
}

newlen = original_len + escaped_character_count + 1;
if (s_quoted || d_quoted)
newlen++;

if (single_match && app_func)
newlen++;

if ((escaped_str = el_malloc(newlen)) == NULL)
return NULL;

for (s = filename; *s; s++) {
c = *s;
if (!needs_escaping(c)) {

escaped_str[offset++] = c;
continue;
}


if (c == '\'' && s_quoted) {
escaped_str[offset++] = '\'';
escaped_str[offset++] = '\\';
escaped_str[offset++] = '\'';
escaped_str[offset++] = '\'';
continue;
}


if (s_quoted) {
escaped_str[offset++] = c;
continue;
}




if (d_quoted && !needs_dquote_escaping(c)) {
escaped_str[offset++] = c;
continue;
}


escaped_str[offset++] = '\\';
escaped_str[offset++] = c;
}

if (single_match && app_func) {
escaped_str[offset] = 0;
append_char = app_func(filename);

if (append_char[0] == ' ') {
if (!s_quoted && !d_quoted)
escaped_str[offset++] = append_char[0];
} else
escaped_str[offset++] = append_char[0];
}


if (single_match && (append_char && append_char[0] == ' ')) {
if (s_quoted)
escaped_str[offset++] = '\'';
else if (d_quoted)
escaped_str[offset++] = '"';
}

escaped_str[offset] = 0;
return escaped_str;
}








char *
fn_filename_completion_function(const char *text, int state)
{
static DIR *dir = NULL;
static char *filename = NULL, *dirname = NULL, *dirpath = NULL;
static size_t filename_len = 0;
struct dirent *entry;
char *temp;
size_t len;

if (state == 0 || dir == NULL) {
temp = strrchr(text, '/');
if (temp) {
char *nptr;
temp++;
nptr = el_realloc(filename, (strlen(temp) + 1) *
sizeof(*nptr));
if (nptr == NULL) {
el_free(filename);
filename = NULL;
return NULL;
}
filename = nptr;
(void)strcpy(filename, temp);
len = (size_t)(temp - text);

nptr = el_realloc(dirname, (len + 1) *
sizeof(*nptr));
if (nptr == NULL) {
el_free(dirname);
dirname = NULL;
return NULL;
}
dirname = nptr;
(void)strlcpy(dirname, text, len + 1);
} else {
el_free(filename);
if (*text == 0)
filename = NULL;
else {
filename = strdup(text);
if (filename == NULL)
return NULL;
}
el_free(dirname);
dirname = NULL;
}

if (dir != NULL) {
(void)closedir(dir);
dir = NULL;
}



el_free(dirpath);
dirpath = NULL;
if (dirname == NULL) {
if ((dirname = strdup("")) == NULL)
return NULL;
dirpath = strdup("./");
} else if (*dirname == '~')
dirpath = fn_tilde_expand(dirname);
else
dirpath = strdup(dirname);

if (dirpath == NULL)
return NULL;

dir = opendir(dirpath);
if (!dir)
return NULL;


filename_len = filename ? strlen(filename) : 0;
}


while ((entry = readdir(dir)) != NULL) {

if (entry->d_name[0] == '.' && (!entry->d_name[1]
|| (entry->d_name[1] == '.' && !entry->d_name[2])))
continue;
if (filename_len == 0)
break;


if (entry->d_name[0] == filename[0]
#if HAVE_STRUCT_DIRENT_D_NAMLEN
&& entry->d_namlen >= filename_len
#else
&& strlen(entry->d_name) >= filename_len
#endif
&& strncmp(entry->d_name, filename,
filename_len) == 0)
break;
}

if (entry) {

#if HAVE_STRUCT_DIRENT_D_NAMLEN
len = entry->d_namlen;
#else
len = strlen(entry->d_name);
#endif

len = strlen(dirname) + len + 1;
temp = el_calloc(len, sizeof(*temp));
if (temp == NULL)
return NULL;
(void)snprintf(temp, len, "%s%s", dirname, entry->d_name);
} else {
(void)closedir(dir);
dir = NULL;
temp = NULL;
}

return temp;
}


static const char *
append_char_function(const char *name)
{
struct stat stbuf;
char *expname = *name == '~' ? fn_tilde_expand(name) : NULL;
const char *rs = " ";

if (stat(expname ? expname : name, &stbuf) == -1)
goto out;
if (S_ISDIR(stbuf.st_mode))
rs = "/";
out:
if (expname)
el_free(expname);
return rs;
}




char ** completion_matches(const char *, char *(*)(const char *, int));
char **
completion_matches(const char *text, char *(*genfunc)(const char *, int))
{
char **match_list = NULL, *retstr, *prevstr;
size_t match_list_len, max_equal, which, i;
size_t matches;

matches = 0;
match_list_len = 1;
while ((retstr = (*genfunc) (text, (int)matches)) != NULL) {

if (matches + 3 >= match_list_len) {
char **nmatch_list;
while (matches + 3 >= match_list_len)
match_list_len <<= 1;
nmatch_list = el_realloc(match_list,
match_list_len * sizeof(*nmatch_list));
if (nmatch_list == NULL) {
el_free(match_list);
return NULL;
}
match_list = nmatch_list;

}
match_list[++matches] = retstr;
}

if (!match_list)
return NULL;


which = 2;
prevstr = match_list[1];
max_equal = strlen(prevstr);
for (; which <= matches; which++) {
for (i = 0; i < max_equal &&
prevstr[i] == match_list[which][i]; i++)
continue;
max_equal = i;
}

retstr = el_calloc(max_equal + 1, sizeof(*retstr));
if (retstr == NULL) {
el_free(match_list);
return NULL;
}
(void)strlcpy(retstr, match_list[1], max_equal + 1);
match_list[0] = retstr;


match_list[matches + 1] = NULL;

return match_list;
}




static int
_fn_qsort_string_compare(const void *i1, const void *i2)
{
const char *s1 = ((const char * const *)i1)[0];
const char *s2 = ((const char * const *)i2)[0];

return strcasecmp(s1, s2);
}









void
fn_display_match_list(EditLine * el, char **matches, size_t num, size_t width,
const char *(*app_func) (const char *))
{
size_t line, lines, col, cols, thisguy;
int screenwidth = el->el_terminal.t_size.h;
if (app_func == NULL)
app_func = append_char_function;


matches++;
num--;





cols = (size_t)screenwidth / (width + 2);
if (cols == 0)
cols = 1;


lines = (num + cols - 1) / cols;


qsort(matches, num, sizeof(char *), _fn_qsort_string_compare);




for (line = 0; line < lines; line++) {
for (col = 0; col < cols; col++) {
thisguy = line + col * lines;
if (thisguy >= num)
break;
(void)fprintf(el->el_outfile, "%s%s%s",
col == 0 ? "" : " ", matches[thisguy],
(*app_func)(matches[thisguy]));
(void)fprintf(el->el_outfile, "%-*s",
(int) (width - strlen(matches[thisguy])), "");
}
(void)fprintf(el->el_outfile, "\n");
}
}

static wchar_t *
find_word_to_complete(const wchar_t * cursor, const wchar_t * buffer,
const wchar_t * word_break, const wchar_t * special_prefixes, size_t * length,
int do_unescape)
{

const wchar_t *ctemp = cursor;
wchar_t *temp;
size_t len;




if (ctemp > buffer) {
switch (ctemp[-1]) {
case '\\':
case '\'':
case '"':
ctemp--;
break;
default:
break;
}
}

for (;;) {
if (ctemp <= buffer)
break;
if (wcschr(word_break, ctemp[-1])) {
if (ctemp - buffer >= 2 && ctemp[-2] == '\\') {
ctemp -= 2;
continue;
}
break;
}
if (special_prefixes && wcschr(special_prefixes, ctemp[-1]))
break;
ctemp--;
}

len = (size_t) (cursor - ctemp);
if (len == 1 && (ctemp[0] == '\'' || ctemp[0] == '"')) {
len = 0;
ctemp++;
}
*length = len;
if (do_unescape) {
wchar_t *unescaped_word = unescape_string(ctemp, len);
if (unescaped_word == NULL)
return NULL;
return unescaped_word;
}
temp = el_malloc((len + 1) * sizeof(*temp));
(void) wcsncpy(temp, ctemp, len);
temp[len] = '\0';
return temp;
}













int
fn_complete2(EditLine *el,
char *(*complete_func)(const char *, int),
char **(*attempted_completion_function)(const char *, int, int),
const wchar_t *word_break, const wchar_t *special_prefixes,
const char *(*app_func)(const char *), size_t query_items,
int *completion_type, int *over, int *point, int *end,
unsigned int flags)
{
const LineInfoW *li;
wchar_t *temp;
char **matches;
char *completion;
size_t len;
int what_to_do = '\t';
int retval = CC_NORM;
int do_unescape = flags & FN_QUOTE_MATCH;

if (el->el_state.lastcmd == el->el_state.thiscmd)
what_to_do = '?';


if (completion_type != NULL)
*completion_type = what_to_do;

if (!complete_func)
complete_func = fn_filename_completion_function;
if (!app_func)
app_func = append_char_function;

li = el_wline(el);
temp = find_word_to_complete(li->cursor,
li->buffer, word_break, special_prefixes, &len, do_unescape);
if (temp == NULL)
goto out;



if (point != NULL)
*point = (int)(li->cursor - li->buffer);
if (end != NULL)
*end = (int)(li->lastchar - li->buffer);

if (attempted_completion_function) {
int cur_off = (int)(li->cursor - li->buffer);
matches = (*attempted_completion_function)(
ct_encode_string(temp, &el->el_scratch),
cur_off - (int)len, cur_off);
} else
matches = NULL;
if (!attempted_completion_function ||
(over != NULL && !*over && !matches))
matches = completion_matches(
ct_encode_string(temp, &el->el_scratch), complete_func);

if (over != NULL)
*over = 0;

if (matches == NULL) {
goto out;
}
int i;
size_t matches_num, maxlen, match_len, match_display=1;
int single_match = matches[2] == NULL &&
(matches[1] == NULL || strcmp(matches[0], matches[1]) == 0);

retval = CC_REFRESH;

if (matches[0][0] != '\0') {
el_deletestr(el, (int)len);
if (flags & FN_QUOTE_MATCH)
completion = escape_filename(el, matches[0],
single_match, app_func);
else
completion = strdup(matches[0]);
if (completion == NULL)
goto out2;





el_winsertstr(el,
ct_decode_string(completion, &el->el_scratch));

if (single_match && attempted_completion_function &&
!(flags & FN_QUOTE_MATCH))
{






el_winsertstr(el, ct_decode_string(
(*app_func)(completion), &el->el_scratch));
}
free(completion);
}


if (!single_match && (what_to_do == '!' || what_to_do == '?')) {





for(i = 1, maxlen = 0; matches[i]; i++) {
match_len = strlen(matches[i]);
if (match_len > maxlen)
maxlen = match_len;
}

matches_num = (size_t)(i - 1);


(void)fprintf(el->el_outfile, "\n");





if (matches_num > query_items) {
(void)fprintf(el->el_outfile,
"Display all %zu possibilities? (y or n) ",
matches_num);
(void)fflush(el->el_outfile);
if (getc(stdin) != 'y')
match_display = 0;
(void)fprintf(el->el_outfile, "\n");
}

if (match_display) {







fn_display_match_list(el, matches,
matches_num+1, maxlen, app_func);
}
retval = CC_REDISPLAY;
} else if (matches[0][0]) {





el_beep(el);
} else {


el_beep(el);
retval = CC_NORM;
}


out2:
for (i = 0; matches[i]; i++)
el_free(matches[i]);
el_free(matches);
matches = NULL;

out:
el_free(temp);
return retval;
}

int
fn_complete(EditLine *el,
char *(*complete_func)(const char *, int),
char **(*attempted_completion_function)(const char *, int, int),
const wchar_t *word_break, const wchar_t *special_prefixes,
const char *(*app_func)(const char *), size_t query_items,
int *completion_type, int *over, int *point, int *end)
{
return fn_complete2(el, complete_func, attempted_completion_function,
word_break, special_prefixes, app_func, query_items,
completion_type, over, point, end,
attempted_completion_function ? 0 : FN_QUOTE_MATCH);
}





unsigned char
_el_fn_complete(EditLine *el, int ch __attribute__((__unused__)))
{
return (unsigned char)fn_complete(el, NULL, NULL,
break_chars, NULL, NULL, (size_t)100,
NULL, NULL, NULL, NULL);
}





unsigned char
_el_fn_sh_complete(EditLine *el, int ch)
{
return _el_fn_complete(el, ch);
}
