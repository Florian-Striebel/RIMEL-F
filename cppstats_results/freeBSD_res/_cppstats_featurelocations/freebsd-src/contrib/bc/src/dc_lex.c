


































#if DC_ENABLED

#include <ctype.h>

#include <dc.h>
#include <vm.h>

bool dc_lex_negCommand(BcLex *l) {
char c = l->buf[l->i];
return !BC_LEX_NUM_CHAR(c, false, false);
}






static void dc_lex_register(BcLex *l) {


if (DC_X && isspace(l->buf[l->i - 1])) {

char c;


bc_lex_whitespace(l);
c = l->buf[l->i];


if (BC_ERR(!isalpha(c) && c != '_'))
bc_lex_verr(l, BC_ERR_PARSE_CHAR, c);


l->i += 1;
bc_lex_name(l);
}
else {



if (BC_ERR(l->buf[l->i - 1] == '\n'))
bc_lex_verr(l, BC_ERR_PARSE_CHAR, l->buf[l->i - 1]);


bc_vec_popAll(&l->str);
bc_vec_pushByte(&l->str, (uchar) l->buf[l->i - 1]);
bc_vec_pushByte(&l->str, '\0');
l->t = BC_LEX_NAME;
}
}







static void dc_lex_string(BcLex *l) {

size_t depth, nls, i;
char c;
bool got_more;


l->t = BC_LEX_STR;
bc_vec_popAll(&l->str);

do {

depth = 1;
nls = 0;
got_more = false;

assert(!l->is_stdin || l->buf == vm.buffer.v);




for (i = l->i; (c = l->buf[i]) && depth; ++i) {


if (c == '\\') {
c = l->buf[++i];
if (!c) break;
}
else {
depth += (c == '[');
depth -= (c == ']');
}


nls += (c == '\n');

if (depth) bc_vec_push(&l->str, &c);
}

if (BC_ERR(c == '\0' && depth)) {
if (!vm.eof && l->is_stdin) got_more = bc_lex_readLine(l);
if (got_more) bc_vec_popAll(&l->str);
}

} while (got_more && depth);


if (BC_ERR(c == '\0' && depth)) {
l->i = i;
bc_lex_err(l, BC_ERR_PARSE_STRING);
}

bc_vec_pushByte(&l->str, '\0');

l->i = i;
l->line += nls;
}





void dc_lex_token(BcLex *l) {

char c = l->buf[l->i++], c2;
size_t i;

BC_SIG_ASSERT_LOCKED;



for (i = 0; i < dc_lex_regs_len; ++i) {


if (l->last == dc_lex_regs[i]) {
dc_lex_register(l);
return;
}
}



if (c >= '"' && c <= '~' &&
(l->t = dc_lex_tokens[(c - '"')]) != BC_LEX_INVALID)
{
return;
}



switch (c) {

case '\0':
case '\n':
case '\t':
case '\v':
case '\f':
case '\r':
case ' ':
{
bc_lex_commonTokens(l, c);
break;
}



case '!':
{
c2 = l->buf[l->i];

if (c2 == '=') l->t = BC_LEX_OP_REL_NE;
else if (c2 == '<') l->t = BC_LEX_OP_REL_LE;
else if (c2 == '>') l->t = BC_LEX_OP_REL_GE;
else bc_lex_invalidChar(l, c);

l->i += 1;

break;
}

case '#':
{
bc_lex_lineComment(l);
break;
}

case '.':
{
c2 = l->buf[l->i];



if (BC_NO_ERR(BC_LEX_NUM_CHAR(c2, true, false)))
bc_lex_number(l, c);
else bc_lex_invalidChar(l, c);

break;
}

case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case 'A':
case 'B':
case 'C':
case 'D':
case 'E':
case 'F':
{
bc_lex_number(l, c);
break;
}

case 'g':
{
c2 = l->buf[l->i];

if (c2 == 'l') l->t = BC_LEX_KW_LINE_LENGTH;
else if (c2 == 'z') l->t = BC_LEX_KW_LEADING_ZERO;
else bc_lex_invalidChar(l, c2);

l->i += 1;

break;
}

case '[':
{
dc_lex_string(l);
break;
}

default:
{
bc_lex_invalidChar(l, c);
}
}
}
#endif
