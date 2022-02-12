






























#include "config.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <histedit.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "filecomplete.h"
#include "el.h"

typedef struct {
const wchar_t *user_typed_text;
const char *completion_function_input ;
const char *expanded_text[2];
const wchar_t *escaped_output;
} test_input;

static test_input inputs[] = {
{

L"ls ang",
"ang",
{"ang<ular>test", NULL},
L"ls ang\\<ular\\>test "
},
{

L"ls \"dq_ang",
"dq_ang",
{"dq_ang<ular>test", NULL},
L"ls \"dq_ang<ular>test\""
},
{

L"ls 'sq_ang",
"sq_ang",
{"sq_ang<ular>test", NULL},
L"ls 'sq_ang<ular>test'"
},
{

L"ls back",
"back",
{"backslash\\test", NULL},
L"ls backslash\\\\test "
},
{

L"ls 'sback",
"sback",
{"sbackslash\\test", NULL},
L"ls 'sbackslash\\test'"
},
{

L"ls \"dback",
"dback",
{"dbackslash\\test", NULL},
L"ls \"dbackslash\\\\test\""
},
{

L"ls br",
"br",
{"braces{test}", NULL},
L"ls braces\\{test\\} "
},
{

L"ls 'sbr",
"sbr",
{"sbraces{test}", NULL},
L"ls 'sbraces{test}'"
},
{

L"ls \"dbr",
"dbr",
{"dbraces{test}", NULL},
L"ls \"dbraces{test}\""
},
{

L"ls doll",
"doll",
{"doll$artest", NULL},
L"ls doll\\$artest "
},
{

L"ls 'sdoll",
"sdoll",
{"sdoll$artest", NULL},
L"ls 'sdoll$artest'"
},
{

L"ls \"ddoll",
"ddoll",
{"ddoll$artest", NULL},
L"ls \"ddoll\\$artest\""
},
{

L"ls eq",
"eq",
{"equals==test", NULL},
L"ls equals\\=\\=test "
},
{

L"ls 'seq",
"seq",
{"sequals==test", NULL},
L"ls 'sequals==test'"
},
{

L"ls \"deq",
"deq",
{"dequals==test", NULL},
L"ls \"dequals==test\""
},
{

L"ls new",
"new",
{"new\\nline", NULL},
L"ls new\\\\nline "
},
{

L"ls 'snew",
"snew",
{"snew\nline", NULL},
L"ls 'snew\nline'"
},
{

L"ls \"dnew",
"dnew",
{"dnew\nline", NULL},
L"ls \"dnew\nline\""
},
{

L"ls spac",
"spac",
{"space test", NULL},
L"ls space\\ test "
},
{

L"ls 's_spac",
"s_spac",
{"s_space test", NULL},
L"ls 's_space test'"
},
{

L"ls \"d_spac",
"d_spac",
{"d_space test", NULL},
L"ls \"d_space test\""
},
{

L"ls multi",
"multi",
{"multi space test", NULL},
L"ls multi\\ space\\ \\ test "
},
{

L"ls 's_multi",
"s_multi",
{"s_multi space test", NULL},
L"ls 's_multi space test'"
},
{

L"ls \"d_multi",
"d_multi",
{"d_multi space test", NULL},
L"ls \"d_multi space test\""
},
{

L"ls doub",
"doub",
{"doub\"quotes", NULL},
L"ls doub\\\"quotes "
},
{

L"ls 's_doub",
"s_doub",
{"s_doub\"quotes", NULL},
L"ls 's_doub\"quotes'"
},
{

L"ls \"d_doub",
"d_doub",
{"d_doub\"quotes", NULL},
L"ls \"d_doub\\\"quotes\""
},
{

L"ls mud",
"mud",
{"mud\"qu\"otes\"", NULL},
L"ls mud\\\"qu\\\"otes\\\" "
},
{

L"ls 'smud",
"smud",
{"smud\"qu\"otes\"", NULL},
L"ls 'smud\"qu\"otes\"'"
},
{

L"ls \"dmud",
"dmud",
{"dmud\"qu\"otes\"", NULL},
L"ls \"dmud\\\"qu\\\"otes\\\"\""
},
{

L"ls sing",
"sing",
{"single'quote", NULL},
L"ls single\\'quote "
},
{

L"ls 'ssing",
"ssing",
{"ssingle'quote", NULL},
L"ls 'ssingle'\\''quote'"
},
{

L"ls \"dsing",
"dsing",
{"dsingle'quote", NULL},
L"ls \"dsingle'quote\""
},
{

L"ls mu_sing",
"mu_sing",
{"mu_single''quotes''", NULL},
L"ls mu_single\\'\\'quotes\\'\\' "
},
{

L"ls 'smu_sing",
"smu_sing",
{"smu_single''quotes''", NULL},
L"ls 'smu_single'\\'''\\''quotes'\\\'''\\'''"
},
{

L"ls \"dmu_sing",
"dmu_sing",
{"dmu_single''quotes''", NULL},
L"ls \"dmu_single''quotes''\""
},
{

L"ls paren",
"paren",
{"paren(test)", NULL},
L"ls paren\\(test\\) "
},
{

L"ls 'sparen",
"sparen",
{"sparen(test)", NULL},
L"ls 'sparen(test)'"
},
{

L"ls \"dparen",
"dparen",
{"dparen(test)", NULL},
L"ls \"dparen(test)\""
},
{

L"ls pip",
"pip",
{"pipe|test", NULL},
L"ls pipe\\|test "
},
{

L"ls 'spip",
"spip",
{"spipe|test", NULL},
L"ls 'spipe|test'",
},
{

L"ls \"dpip",
"dpip",
{"dpipe|test", NULL},
L"ls \"dpipe|test\""
},
{

L"ls ta",
"ta",
{"tab\ttest", NULL},
L"ls tab\\\ttest "
},
{

L"ls 'sta",
"sta",
{"stab\ttest", NULL},
L"ls 'stab\ttest'"
},
{

L"ls \"dta",
"dta",
{"dtab\ttest", NULL},
L"ls \"dtab\ttest\""
},
{

L"ls tic",
"tic",
{"tick`test`", NULL},
L"ls tick\\`test\\` "
},
{

L"ls 'stic",
"stic",
{"stick`test`", NULL},
L"ls 'stick`test`'"
},
{

L"ls \"dtic",
"dtic",
{"dtick`test`", NULL},
L"ls \"dtick\\`test\\`\""
},
{

L"ls at",
"at",
{"atthe@rate", NULL},
L"ls atthe\\@rate "
},
{

L"ls 'sat",
"sat",
{"satthe@rate", NULL},
L"ls 'satthe@rate'"
},
{

L"ls \"dat",
"dat",
{"datthe@rate", NULL},
L"ls \"datthe@rate\""
},
{

L"ls semi",
"semi",
{"semi;colon;test", NULL},
L"ls semi\\;colon\\;test "
},
{

L"ls 'ssemi",
"ssemi",
{"ssemi;colon;test", NULL},
L"ls 'ssemi;colon;test'"
},
{

L"ls \"dsemi",
"dsemi",
{"dsemi;colon;test", NULL},
L"ls \"dsemi;colon;test\""
},
{

L"ls amp",
"amp",
{"ampers&and", NULL},
L"ls ampers\\&and "
},
{

L"ls 'samp",
"samp",
{"sampers&and", NULL},
L"ls 'sampers&and'"
},
{

L"ls \"damp",
"damp",
{"dampers&and", NULL},
L"ls \"dampers&and\""
},
{

L"ls foo\\",
"foo",
{"foo bar", NULL},
L"ls foo\\ bar "
},
{

L"ls foo'",
"foo'",
{"foo bar", NULL},
L"ls foo\\ bar "
},
{

L"ls foo\"",
"foo\"",
{"foo bar", NULL},
L"ls foo\\ bar "
},
{

L"ls fo",
"fo",
{"foo bar", "foo baz"},
L"ls foo\\ ba"
},
{
L"ls ba",
"ba",
{"bar <bar>", "bar <baz>"},
L"ls bar\\ \\<ba"
}
};

static const wchar_t break_chars[] = L" \t\n\"\\'`@$><=;|&{(";






static char *
mycomplet_func(const char *text, int index)
{
static int last_index = 0;
size_t i = 0;
if (last_index == 2) {
last_index = 0;
return NULL;
}

for (i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
if (strcmp(text, inputs[i].completion_function_input) == 0) {
if (inputs[i].expanded_text[last_index] != NULL)
return strdup(inputs[i].expanded_text[last_index++]);
else {
last_index = 0;
return NULL;
}
}
}

return NULL;
}

int
main(int argc, char **argv)
{
EditLine *el = el_init(argv[0], stdin, stdout, stderr);
size_t i;
size_t input_len;
el_line_t line;
wchar_t *buffer = malloc(64 * sizeof(*buffer));
if (buffer == NULL)
err(EXIT_FAILURE, "malloc failed");

for (i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
memset(buffer, 0, 64 * sizeof(*buffer));
input_len = wcslen(inputs[i].user_typed_text);
wmemcpy(buffer, inputs[i].user_typed_text, input_len);
buffer[input_len] = 0;
line.buffer = buffer;
line.cursor = line.buffer + input_len ;
line.lastchar = line.cursor - 1;
line.limit = line.buffer + 64 * sizeof(*buffer);
el->el_line = line;
fn_complete(el, mycomplet_func, NULL, break_chars, NULL, NULL, 10, NULL, NULL, NULL, NULL);





printf("User input: %ls\t Expected output: %ls\t Generated output: %ls\n",
inputs[i].user_typed_text, inputs[i].escaped_output, el->el_line.buffer);
assert(wcscmp(el->el_line.buffer, inputs[i].escaped_output) == 0);
}
el_end(el);
return 0;

}
