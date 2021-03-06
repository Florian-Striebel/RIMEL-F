

























#include <dialog.h>

#include <dlg_keys.h>

#if defined(HAVE_COLOR)
#include <dlg_colors.h>
#include <dlg_internals.h>

#define L_PAREN '('
#define R_PAREN ')'

#define MIN_TOKEN 3
#if defined(HAVE_RC_FILE2)
#define MAX_TOKEN 5
#else
#define MAX_TOKEN MIN_TOKEN
#endif

#define UNKNOWN_COLOR -2




static const color_names_st color_names[] =
{
#if defined(HAVE_USE_DEFAULT_COLORS)
{"DEFAULT", -1},
#endif
{"BLACK", COLOR_BLACK},
{"RED", COLOR_RED},
{"GREEN", COLOR_GREEN},
{"YELLOW", COLOR_YELLOW},
{"BLUE", COLOR_BLUE},
{"MAGENTA", COLOR_MAGENTA},
{"CYAN", COLOR_CYAN},
{"WHITE", COLOR_WHITE},
};
#define COLOR_COUNT TableSize(color_names)
#endif

#define GLOBALRC "/etc/dialogrc"
#define DIALOGRC ".dialogrc"


#define VAL_INT 0
#define VAL_STR 1
#define VAL_BOOL 2


typedef enum {
LINE_ERROR = -1,
LINE_EQUALS,
LINE_EMPTY
} PARSE_LINE;


#define VAR_COUNT TableSize(vars)


#define isquote(c) ((c) == '"' || (c) == '\'')


#define lastch(str) str[strlen(str)-1]




typedef struct {
const char *name;
void *var;
int type;
const char *comment;
} vars_st;





static const vars_st vars[] =
{
{"aspect",
&dialog_state.aspect_ratio,
VAL_INT,
"Set aspect-ration."},

{"separate_widget",
&dialog_state.separate_str,
VAL_STR,
"Set separator (for multiple widgets output)."},

{"tab_len",
&dialog_state.tab_len,
VAL_INT,
"Set tab-length (for textbox tab-conversion)."},

{"visit_items",
&dialog_state.visit_items,
VAL_BOOL,
"Make tab-traversal for checklist, etc., include the list."},

#if defined(HAVE_COLOR)
{"use_shadow",
&dialog_state.use_shadow,
VAL_BOOL,
"Shadow dialog boxes? This also turns on color."},

{"use_colors",
&dialog_state.use_colors,
VAL_BOOL,
"Turn color support ON or OFF"},
#endif
};

static int
skip_whitespace(char *str, int n)
{
while (isblank(UCH(str[n])) && str[n] != '\0')
n++;
return n;
}

static int
skip_keyword(char *str, int n)
{
while (isalnum(UCH(str[n])) && str[n] != '\0')
n++;
return n;
}

static void
trim_token(char **tok)
{
char *tmp = *tok + skip_whitespace(*tok, 0);

*tok = tmp;

while (*tmp != '\0' && !isblank(UCH(*tmp)))
tmp++;

*tmp = '\0';
}

static int
from_boolean(const char *str)
{
int code = -1;

if (str != NULL && *str != '\0') {
if (!dlg_strcmp(str, "ON")) {
code = 1;
} else if (!dlg_strcmp(str, "OFF")) {
code = 0;
}
}
return code;
}

static int
from_color_name(const char *str)
{
int code = UNKNOWN_COLOR;

if (str != NULL && *str != '\0') {
size_t i;

for (i = 0; i < COLOR_COUNT; ++i) {
if (!dlg_strcmp(str, color_names[i].name)) {
code = color_names[i].value;
break;
}
}
}
return code;
}

static int
find_vars(char *name)
{
int result = -1;
unsigned i;

for (i = 0; i < VAR_COUNT; i++) {
if (dlg_strcmp(vars[i].name, name) == 0) {
result = (int) i;
break;
}
}
return result;
}

#if defined(HAVE_COLOR)
static int
find_color(char *name)
{
int result = -1;
int i;
int limit = dlg_color_count();

for (i = 0; i < limit; i++) {
if (dlg_strcmp(dlg_color_table[i].name, name) == 0) {
result = i;
break;
}
}
return result;
}

static const char *
to_color_name(int code)
{
const char *result = "?";
size_t n;
for (n = 0; n < TableSize(color_names); ++n) {
if (code == color_names[n].value) {
result = color_names[n].name;
break;
}
}
return result;
}

static const char *
to_boolean(int code)
{
return code ? "ON" : "OFF";
}










static int
str_to_attr(char *str, DIALOG_COLORS * result)
{
char *tokens[MAX_TOKEN + 1];
char tempstr[MAX_LEN + 1];
size_t have;
size_t i = 0;
size_t tok_count = 0;

memset(result, 0, sizeof(*result));
result->fg = -1;
result->bg = -1;
result->hilite = -1;

if (str[0] != L_PAREN || lastch(str) != R_PAREN) {
int ret;

if ((ret = find_color(str)) >= 0) {
*result = dlg_color_table[ret];
return 0;
}

return -1;
}


have = strlen(str);
if (have > MAX_LEN) {
have = MAX_LEN - 1;
} else {
have -= 2;
}
memcpy(tempstr, str + 1, have);
tempstr[have] = '\0';



while (tok_count < TableSize(tokens)) {

tokens[tok_count++] = &tempstr[i];

while (tempstr[i] != '\0' && tempstr[i] != ',')
i++;

if (tempstr[i] == '\0')
break;

tempstr[i++] = '\0';
}

if (tok_count < MIN_TOKEN || tok_count > MAX_TOKEN) {

return -1;
}

for (i = 0; i < tok_count; ++i)
trim_token(&tokens[i]);


if (UNKNOWN_COLOR == (result->fg = from_color_name(tokens[0]))
|| UNKNOWN_COLOR == (result->bg = from_color_name(tokens[1]))
|| UNKNOWN_COLOR == (result->hilite = from_boolean(tokens[2]))
#if defined(HAVE_RC_FILE2)
|| (tok_count >= 4 && (result->ul = from_boolean(tokens[3])) == -1)
|| (tok_count >= 5 && (result->rv = from_boolean(tokens[4])) == -1)
#endif
) {

return -1;
}

return 0;
}
#endif





static int
begins_with(char *line, const char *keyword, char **params)
{
int i = skip_whitespace(line, 0);
int j = skip_keyword(line, i);

if ((j - i) == (int) strlen(keyword)) {
char save = line[j];
line[j] = 0;
if (!dlg_strcmp(keyword, line + i)) {
*params = line + skip_whitespace(line, j + 1);
return 1;
}
line[j] = save;
}

return 0;
}













static PARSE_LINE
parse_line(char *line, char **var, char **value)
{
int i = 0;


i = skip_whitespace(line, i);

if (line[i] == '\0')
return LINE_EMPTY;
else if (line[i] == '#')
return LINE_EMPTY;
else if (line[i] == '=')
return LINE_ERROR;


*var = line + i++;


while (!isblank(UCH(line[i])) && line[i] != '=' && line[i] != '\0')
i++;

if (line[i] == '\0')
return LINE_ERROR;
else if (line[i] == '=')
line[i++] = '\0';
else {
line[i++] = '\0';


i = skip_whitespace(line, i);

if (line[i] != '=')
return LINE_ERROR;
else
i++;
}


i = skip_whitespace(line, i);

if (line[i] == '\0')
return LINE_ERROR;
else
*value = line + i;


i = (int) strlen(*value) - 1;
while (isblank(UCH((*value)[i])) && i > 0)
i--;
(*value)[i + 1] = '\0';

return LINE_EQUALS;
}




void
dlg_create_rc(const char *filename)
{
unsigned i;
FILE *rc_file;

if ((rc_file = fopen(filename, "wt")) == NULL)
dlg_exiterr("Error opening file for writing in dlg_create_rc().");

fprintf(rc_file, "#\n\
#Run-time configuration file for dialog\n#\n#Automatically generated by \"dialog --create-rc <file>\"\n#\n#\n#Types of values:\n#\n#Number - <number>\n#String - \"string\"\n#Boolean - <ON|OFF>\n"









#if defined(HAVE_COLOR)
#if defined(HAVE_RC_FILE2)
"\
#Attribute - (foreground,background,highlight?,underline?,reverse?)\n"
#else
"\
#Attribute - (foreground,background,highlight?)\n"
#endif
#endif
);


for (i = 0; i < VAR_COUNT; i++) {
fprintf(rc_file, "\n#%s\n", vars[i].comment);
switch (vars[i].type) {
case VAL_INT:
fprintf(rc_file, "%s = %d\n", vars[i].name,
*((int *) vars[i].var));
break;
case VAL_STR:
fprintf(rc_file, "%s = \"%s\"\n", vars[i].name,
(char *) vars[i].var);
break;
case VAL_BOOL:
fprintf(rc_file, "%s = %s\n", vars[i].name,
*((bool *) vars[i].var) ? "ON" : "OFF");
break;
}
}
#if defined(HAVE_COLOR)
for (i = 0; i < (unsigned) dlg_color_count(); ++i) {
unsigned j;
bool repeat = FALSE;

fprintf(rc_file, "\n#%s\n", dlg_color_table[i].comment);
for (j = 0; j != i; ++j) {
if (dlg_color_table[i].fg == dlg_color_table[j].fg
&& dlg_color_table[i].bg == dlg_color_table[j].bg
&& dlg_color_table[i].hilite == dlg_color_table[j].hilite) {
fprintf(rc_file, "%s = %s\n",
dlg_color_table[i].name,
dlg_color_table[j].name);
repeat = TRUE;
break;
}
}

if (!repeat) {
fprintf(rc_file, "%s = %c", dlg_color_table[i].name, L_PAREN);
fprintf(rc_file, "%s", to_color_name(dlg_color_table[i].fg));
fprintf(rc_file, ",%s", to_color_name(dlg_color_table[i].bg));
fprintf(rc_file, ",%s", to_boolean(dlg_color_table[i].hilite));
#if defined(HAVE_RC_FILE2)
if (dlg_color_table[i].ul || dlg_color_table[i].rv)
fprintf(rc_file, ",%s", to_boolean(dlg_color_table[i].ul));
if (dlg_color_table[i].rv)
fprintf(rc_file, ",%s", to_boolean(dlg_color_table[i].rv));
#endif
fprintf(rc_file, "%c\n", R_PAREN);
}
}
#endif
dlg_dump_keys(rc_file);

(void) fclose(rc_file);
}

static void
report_error(const char *filename, int line_no, const char *msg)
{
fprintf(stderr, "%s:%d: %s\n", filename, line_no, msg);
dlg_trace_msg("%s:%d: %s\n", filename, line_no, msg);
}




int
dlg_parse_rc(void)
{
int i;
int l = 1;
PARSE_LINE parse;
char str[MAX_LEN + 1];
char *var;
char *value;
char *filename;
int result = 0;
FILE *rc_file = 0;
char *params;

















if ((filename = dlg_getenv_str("DIALOGRC")) != NULL)
rc_file = fopen(filename, "rt");

if (rc_file == NULL) {

if ((filename = dlg_getenv_str("HOME")) != NULL
&& strlen(filename) < MAX_LEN - (sizeof(DIALOGRC) + 3)) {
if (filename[0] == '\0' || lastch(filename) == '/')
sprintf(str, "%s%s", filename, DIALOGRC);
else
sprintf(str, "%s/%s", filename, DIALOGRC);
rc_file = fopen(filename = str, "rt");
}
}

if (rc_file == NULL) {

strcpy(str, GLOBALRC);
if ((rc_file = fopen(filename = str, "rt")) == NULL)
return 0;
}

DLG_TRACE(("#opened rc file \"%s\"\n", filename));

while ((result == 0) && (fgets(str, MAX_LEN, rc_file) != NULL)) {
DLG_TRACE(("#\t%s", str));
if (*str == '\0' || lastch(str) != '\n') {

report_error(filename, l, "line too long");
result = -1;
break;
}

lastch(str) = '\0';
if (begins_with(str, "bindkey", &params)) {
if (!dlg_parse_bindkey(params)) {
report_error(filename, l, "invalid bindkey");
result = -1;
}
continue;
}
parse = parse_line(str, &var, &value);

switch (parse) {
case LINE_EMPTY:
break;
case LINE_EQUALS:

if ((i = find_vars(var)) >= 0) {
switch (vars[i].type) {
case VAL_INT:
*((int *) vars[i].var) = atoi(value);
break;
case VAL_STR:
if (!isquote(value[0]) || !isquote(lastch(value))
|| strlen(value) < 2) {
report_error(filename, l, "expected string value");
result = -1;
} else {

value++;
lastch(value) = '\0';
strcpy((char *) vars[i].var, value);
}
break;
case VAL_BOOL:
if (!dlg_strcmp(value, "ON"))
*((bool *) vars[i].var) = TRUE;
else if (!dlg_strcmp(value, "OFF"))
*((bool *) vars[i].var) = FALSE;
else {
report_error(filename, l, "expected boolean value");
result = -1;
}
break;
}
#if defined(HAVE_COLOR)
} else if ((i = find_color(var)) >= 0) {
DIALOG_COLORS temp;
if (str_to_attr(value, &temp) == -1) {
report_error(filename, l, "expected attribute value");
result = -1;
} else {
dlg_color_table[i].fg = temp.fg;
dlg_color_table[i].bg = temp.bg;
dlg_color_table[i].hilite = temp.hilite;
#if defined(HAVE_RC_FILE2)
dlg_color_table[i].ul = temp.ul;
dlg_color_table[i].rv = temp.rv;
#endif
}
} else {
#endif
report_error(filename, l, "unknown variable");
result = -1;
}
break;
case LINE_ERROR:
report_error(filename, l, "syntax error");
result = -1;
break;
}
l++;
}

(void) fclose(rc_file);
return result;
}
