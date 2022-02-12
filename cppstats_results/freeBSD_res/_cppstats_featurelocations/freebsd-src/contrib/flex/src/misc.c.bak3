































#include "flexdef.h"
#include "tables.h"

#define CMD_IF_TABLES_SER "%if-tables-serialization"
#define CMD_TABLES_YYDMAP "%tables-yydmap"
#define CMD_DEFINE_YYTABLES "%define-yytables"
#define CMD_IF_CPP_ONLY "%if-c++-only"
#define CMD_IF_C_ONLY "%if-c-only"
#define CMD_IF_C_OR_CPP "%if-c-or-c++"
#define CMD_NOT_FOR_HEADER "%not-for-header"
#define CMD_OK_FOR_HEADER "%ok-for-header"
#define CMD_PUSH "%push"
#define CMD_POP "%pop"
#define CMD_IF_REENTRANT "%if-reentrant"
#define CMD_IF_NOT_REENTRANT "%if-not-reentrant"
#define CMD_IF_BISON_BRIDGE "%if-bison-bridge"
#define CMD_IF_NOT_BISON_BRIDGE "%if-not-bison-bridge"
#define CMD_ENDIF "%endif"


struct sko_state {
bool dc;
};
static struct sko_state *sko_stack=0;
static int sko_len=0,sko_sz=0;
static void sko_push(bool dc)
{
if(!sko_stack){
sko_sz = 1;
sko_stack = malloc(sizeof(struct sko_state) * (size_t) sko_sz);
if (!sko_stack)
flexfatal(_("allocation of sko_stack failed"));
sko_len = 0;
}
if(sko_len >= sko_sz){
sko_sz *= 2;
sko_stack = realloc(sko_stack,
sizeof(struct sko_state) * (size_t) sko_sz);
}


sko_stack[sko_len].dc = dc;
sko_len++;
}
static void sko_peek(bool *dc)
{
if(sko_len <= 0)
flex_die("peek attempt when sko stack is empty");
if(dc)
*dc = sko_stack[sko_len-1].dc;
}
static void sko_pop(bool* dc)
{
sko_peek(dc);
sko_len--;
if(sko_len < 0)
flex_die("popped too many times in skeleton.");
}


void action_define (const char *defname, int value)
{
char buf[MAXLINE];
char *cpy;

if ((int) strlen (defname) > MAXLINE / 2) {
format_pinpoint_message (_
("name \"%s\" ridiculously long"),
defname);
return;
}

snprintf (buf, sizeof(buf), "#define %s %d\n", defname, value);
add_action (buf);


cpy = xstrdup(defname);
buf_append (&defs_buf, &cpy, 1);
}


void add_action (const char *new_text)
{
int len = (int) strlen (new_text);

while (len + action_index >= action_size - 10 ) {
int new_size = action_size * 2;

if (new_size <= 0)



action_size += action_size / 8;
else
action_size = new_size;

action_array =
reallocate_character_array (action_array,
action_size);
}

strcpy (&action_array[action_index], new_text);

action_index += len;
}




void *allocate_array (int size, size_t element_size)
{
void *mem;
#if HAVE_REALLOCARRAY

mem = reallocarray(NULL, (size_t) size, element_size);
#else
size_t num_bytes = (size_t) size * element_size;
mem = (size && SIZE_MAX / (size_t) size < element_size) ? NULL :
malloc(num_bytes);
#endif
if (!mem)
flexfatal (_
("memory allocation failed in allocate_array()"));

return mem;
}




int all_lower (char *str)
{
while (*str) {
if (!isascii ((unsigned char) * str) || !islower ((unsigned char) * str))
return 0;
++str;
}

return 1;
}




int all_upper (char *str)
{
while (*str) {
if (!isascii ((unsigned char) * str) || !isupper ((unsigned char) * str))
return 0;
++str;
}

return 1;
}




int intcmp (const void *a, const void *b)
{
return *(const int *) a - *(const int *) b;
}







void check_char (int c)
{
if (c >= CSIZE)
lerr (_("bad character '%s' detected in check_char()"),
readable_form (c));

if (c >= csize)
lerr (_
("scanner requires -8 flag to use the character %s"),
readable_form (c));
}





unsigned char clower (int c)
{
return (unsigned char) ((isascii (c) && isupper (c)) ? tolower (c) : c);
}


char *xstrdup(const char *s)
{
char *s2;

if ((s2 = strdup(s)) == NULL)
flexfatal (_("memory allocation failure in xstrdup()"));

return s2;
}




int cclcmp (const void *a, const void *b)
{
if (!*(const unsigned char *) a)
return 1;
else
if (!*(const unsigned char *) b)
return - 1;
else
return *(const unsigned char *) a - *(const unsigned char *) b;
}




void dataend (void)
{

if (gentables) {

if (datapos > 0)
dataflush ();


outn (" } ;\n");
}
dataline = 0;
datapos = 0;
}




void dataflush (void)
{

if (!gentables)
return;

outc ('\n');

if (++dataline >= NUMDATALINES) {



outc ('\n');
dataline = 0;
}


datapos = 0;
}




void flexerror (const char *msg)
{
fprintf (stderr, "%s: %s\n", program_name, msg);
flexend (1);
}




void flexfatal (const char *msg)
{
fprintf (stderr, _("%s: fatal internal error, %s\n"),
program_name, msg);
FLEX_EXIT (1);
}




void lerr (const char *msg, ...)
{
char errmsg[MAXLINE];
va_list args;

va_start(args, msg);
vsnprintf (errmsg, sizeof(errmsg), msg, args);
va_end(args);
flexerror (errmsg);
}




void lerr_fatal (const char *msg, ...)
{
char errmsg[MAXLINE];
va_list args;
va_start(args, msg);

vsnprintf (errmsg, sizeof(errmsg), msg, args);
va_end(args);
flexfatal (errmsg);
}




void line_directive_out (FILE *output_file, int do_infile)
{
char directive[MAXLINE], filename[MAXLINE];
char *s1, *s2, *s3;
static const char line_fmt[] = "#line %d \"%s\"\n";

if (!gen_line_dirs)
return;

s1 = do_infile ? infilename : "M4_YY_OUTFILE_NAME";

if (do_infile && !s1)
s1 = "<stdin>";

s2 = filename;
s3 = &filename[sizeof (filename) - 2];

while (s2 < s3 && *s1) {
if (*s1 == '\\' || *s1 == '"')

*s2++ = '\\';

*s2++ = *s1++;
}

*s2 = '\0';

if (do_infile)
snprintf (directive, sizeof(directive), line_fmt, linenum, filename);
else {
snprintf (directive, sizeof(directive), line_fmt, 0, filename);
}




if (output_file) {
fputs (directive, output_file);
}
else
add_action (directive);
}






void mark_defs1 (void)
{
defs1_offset = 0;
action_array[action_index++] = '\0';
action_offset = prolog_offset = action_index;
action_array[action_index] = '\0';
}





void mark_prolog (void)
{
action_array[action_index++] = '\0';
action_offset = action_index;
action_array[action_index] = '\0';
}






void mk2data (int value)
{

if (!gentables)
return;

if (datapos >= NUMDATAITEMS) {
outc (',');
dataflush ();
}

if (datapos == 0)

out (" ");

else
outc (',');

++datapos;

out_dec ("%5d", value);
}







void mkdata (int value)
{

if (!gentables)
return;

if (datapos >= NUMDATAITEMS) {
outc (',');
dataflush ();
}

if (datapos == 0)

out (" ");
else
outc (',');

++datapos;

out_dec ("%5d", value);
}




int myctoi (const char *array)
{
int val = 0;

(void) sscanf (array, "%d", &val);

return val;
}




unsigned char myesc (unsigned char array[])
{
unsigned char c, esc_char;

switch (array[1]) {
case 'b':
return '\b';
case 'f':
return '\f';
case 'n':
return '\n';
case 'r':
return '\r';
case 't':
return '\t';
case 'a':
return '\a';
case 'v':
return '\v';
case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
{
int sptr = 1;

while (sptr <= 3 &&
array[sptr] >= '0' && array[sptr] <= '7') {
++sptr;
}

c = array[sptr];
array[sptr] = '\0';

esc_char = (unsigned char) strtoul (array + 1, NULL, 8);

array[sptr] = c;

return esc_char;
}

case 'x':
{
int sptr = 2;

while (sptr <= 3 && isxdigit (array[sptr])) {




++sptr;
}

c = array[sptr];
array[sptr] = '\0';

esc_char = (unsigned char) strtoul (array + 2, NULL, 16);

array[sptr] = c;

return esc_char;
}

default:
return array[1];
}
}






void out (const char *str)
{
fputs (str, stdout);
}

void out_dec (const char *fmt, int n)
{
fprintf (stdout, fmt, n);
}

void out_dec2 (const char *fmt, int n1, int n2)
{
fprintf (stdout, fmt, n1, n2);
}

void out_hex (const char *fmt, unsigned int x)
{
fprintf (stdout, fmt, x);
}

void out_str (const char *fmt, const char str[])
{
fprintf (stdout,fmt, str);
}

void out_str3 (const char *fmt, const char s1[], const char s2[], const char s3[])
{
fprintf (stdout,fmt, s1, s2, s3);
}

void out_str_dec (const char *fmt, const char str[], int n)
{
fprintf (stdout,fmt, str, n);
}

void outc (int c)
{
fputc (c, stdout);
}

void outn (const char *str)
{
fputs (str,stdout);
fputc('\n',stdout);
}





void out_m4_define (const char* def, const char* val)
{
const char * fmt = "m4_define( [[%s]], [[%s]])m4_dnl\n";
fprintf(stdout, fmt, def, val?val:"");
}







char *readable_form (int c)
{
static char rform[20];

if ((c >= 0 && c < 32) || c >= 127) {
switch (c) {
case '\b':
return "\\b";
case '\f':
return "\\f";
case '\n':
return "\\n";
case '\r':
return "\\r";
case '\t':
return "\\t";
case '\a':
return "\\a";
case '\v':
return "\\v";
default:
if(trace_hex)
snprintf (rform, sizeof(rform), "\\x%.2x", (unsigned int) c);
else
snprintf (rform, sizeof(rform), "\\%.3o", (unsigned int) c);
return rform;
}
}

else if (c == ' ')
return "' '";

else {
rform[0] = (char) c;
rform[1] = '\0';

return rform;
}
}




void *reallocate_array (void *array, int size, size_t element_size)
{
void *new_array;
#if HAVE_REALLOCARRAY

new_array = reallocarray(array, (size_t) size, element_size);
#else
size_t num_bytes = (size_t) size * element_size;
new_array = (size && SIZE_MAX / (size_t) size < element_size) ? NULL :
realloc(array, num_bytes);
#endif
if (!new_array)
flexfatal (_("attempt to increase array size failed"));

return new_array;
}








void skelout (void)
{
char buf_storage[MAXLINE];
char *buf = buf_storage;
bool do_copy = true;


if(sko_len > 0)
sko_peek(&do_copy);
sko_len = 0;
sko_push(do_copy=true);





while (skelfile ?
(fgets (buf, MAXLINE, skelfile) != NULL) :
((buf = (char *) skel[skel_ind++]) != 0)) {

if (skelfile)
chomp (buf);


if (buf[0] == '%') {

if (ddebug && buf[1] != '#') {
if (buf[strlen (buf) - 1] == '\\')
out_str ("/* %s */\\\n", buf);
else
out_str ("/* %s */\n", buf);
}






#define cmd_match(s) (strncmp(buf,(s),strlen(s))==0)

if (buf[1] == '%') {

return;
}
else if (cmd_match (CMD_PUSH)){
sko_push(do_copy);
if(ddebug){
out_str("/*(state = (%s) */",do_copy?"true":"false");
}
out_str("%s\n", buf[strlen (buf) - 1] =='\\' ? "\\" : "");
}
else if (cmd_match (CMD_POP)){
sko_pop(&do_copy);
if(ddebug){
out_str("/*(state = (%s) */",do_copy?"true":"false");
}
out_str("%s\n", buf[strlen (buf) - 1] =='\\' ? "\\" : "");
}
else if (cmd_match (CMD_IF_REENTRANT)){
sko_push(do_copy);
do_copy = reentrant && do_copy;
}
else if (cmd_match (CMD_IF_NOT_REENTRANT)){
sko_push(do_copy);
do_copy = !reentrant && do_copy;
}
else if (cmd_match(CMD_IF_BISON_BRIDGE)){
sko_push(do_copy);
do_copy = bison_bridge_lval && do_copy;
}
else if (cmd_match(CMD_IF_NOT_BISON_BRIDGE)){
sko_push(do_copy);
do_copy = !bison_bridge_lval && do_copy;
}
else if (cmd_match (CMD_ENDIF)){
sko_pop(&do_copy);
}
else if (cmd_match (CMD_IF_TABLES_SER)) {
do_copy = do_copy && tablesext;
}
else if (cmd_match (CMD_TABLES_YYDMAP)) {
if (tablesext && yydmap_buf.elts)
outn ((char *) (yydmap_buf.elts));
}
else if (cmd_match (CMD_DEFINE_YYTABLES)) {
out_str("#define YYTABLES_NAME \"%s\"\n",
tablesname?tablesname:"yytables");
}
else if (cmd_match (CMD_IF_CPP_ONLY)) {

sko_push(do_copy);
do_copy = C_plus_plus;
}
else if (cmd_match (CMD_IF_C_ONLY)) {

sko_push(do_copy);
do_copy = !C_plus_plus;
}
else if (cmd_match (CMD_IF_C_OR_CPP)) {

sko_push(do_copy);
do_copy = true;
}
else if (cmd_match (CMD_NOT_FOR_HEADER)) {

OUT_BEGIN_CODE ();
}
else if (cmd_match (CMD_OK_FOR_HEADER)) {

OUT_END_CODE ();
}
else {
flexfatal (_("bad line in skeleton file"));
}
}

else if (do_copy)
outn (buf);
}
}








void transition_struct_out (int element_v, int element_n)
{


if (!gentables)
return;

out_dec2 (" {%4d,%4d },", element_v, element_n);

datapos += TRANS_STRUCT_PRINT_LENGTH;

if (datapos >= 79 - TRANS_STRUCT_PRINT_LENGTH) {
outc ('\n');

if (++dataline % 10 == 0)
outc ('\n');

datapos = 0;
}
}







void *yy_flex_xmalloc (int size)
{
void *result;

result = malloc((size_t) size);
if (!result)
flexfatal (_
("memory allocation failed in yy_flex_xmalloc()"));

return result;
}





char *chomp (char *str)
{
char *p = str;

if (!str || !*str)
return str;


while (*p)
++p;
--p;


while (p >= str && (*p == '\r' || *p == '\n'))
*p-- = 0;
return str;
}
