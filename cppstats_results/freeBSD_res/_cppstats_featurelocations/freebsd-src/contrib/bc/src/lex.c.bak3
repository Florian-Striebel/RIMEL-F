


































#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include <lex.h>
#include <vm.h>
#include <bc.h>

void bc_lex_invalidChar(BcLex *l, char c) {
l->t = BC_LEX_INVALID;
bc_lex_verr(l, BC_ERR_PARSE_CHAR, c);
}

void bc_lex_lineComment(BcLex *l) {
l->t = BC_LEX_WHITESPACE;
while (l->i < l->len && l->buf[l->i] != '\n') l->i += 1;
}

void bc_lex_comment(BcLex *l) {

size_t i, nlines = 0;
const char *buf;
bool end = false, got_more;
char c;

l->i += 1;
l->t = BC_LEX_WHITESPACE;




do {

buf = l->buf;
got_more = false;


assert(!vm.is_stdin || buf == vm.buffer.v);


for (i = l->i; !end; i += !end) {


for (; (c = buf[i]) && c != '*'; ++i) nlines += (c == '\n');


if (BC_ERR(!c || buf[i + 1] == '\0')) {


if (!vm.eof && l->is_stdin) got_more = bc_lex_readLine(l);

break;
}


end = (buf[i + 1] == '/');
}

} while (got_more && !end);


if (!end) {
l->i = i;
bc_lex_err(l, BC_ERR_PARSE_COMMENT);
}

l->i = i + 2;
l->line += nlines;
}

void bc_lex_whitespace(BcLex *l) {

char c;

l->t = BC_LEX_WHITESPACE;


for (c = l->buf[l->i]; c != '\n' && isspace(c); c = l->buf[++l->i]);
}

void bc_lex_commonTokens(BcLex *l, char c) {
if (!c) l->t = BC_LEX_EOF;
else if (c == '\n') l->t = BC_LEX_NLINE;
else bc_lex_whitespace(l);
}








static size_t bc_lex_num(BcLex *l, char start, bool int_only) {

const char *buf = l->buf + l->i;
size_t i;
char c;
bool last_pt, pt = (start == '.');





for (i = 0; (c = buf[i]) && (BC_LEX_NUM_CHAR(c, pt, int_only) ||
(c == '\\' && buf[i + 1] == '\n')); ++i)
{


if (c == '\\') {

i += 2;


while(isspace(buf[i]) && buf[i] != '\n') i += 1;

c = buf[i];


if (!BC_LEX_NUM_CHAR(c, pt, int_only)) break;
}


last_pt = (c == '.');



if (pt && last_pt) break;


pt = pt || last_pt;

bc_vec_push(&l->str, &c);
}

return i;
}

void bc_lex_number(BcLex *l, char start) {

l->t = BC_LEX_NUMBER;


bc_vec_popAll(&l->str);
bc_vec_push(&l->str, &start);


l->i += bc_lex_num(l, start, false);

#if BC_ENABLE_EXTRA_MATH
{
char c = l->buf[l->i];


if (c == 'e') {

#if BC_ENABLED

if (BC_IS_POSIX) bc_lex_err(l, BC_ERR_POSIX_EXP_NUM);
#endif


bc_vec_push(&l->str, &c);
l->i += 1;
c = l->buf[l->i];


if (c == BC_LEX_NEG_CHAR) {
bc_vec_push(&l->str, &c);
l->i += 1;
c = l->buf[l->i];
}


if (BC_ERR(!BC_LEX_NUM_CHAR(c, false, true)))
bc_lex_verr(l, BC_ERR_PARSE_CHAR, c);


l->i += bc_lex_num(l, 0, true);
}
}
#endif

bc_vec_pushByte(&l->str, '\0');
}

void bc_lex_name(BcLex *l) {

size_t i = 0;
const char *buf = l->buf + l->i - 1;
char c = buf[i];

l->t = BC_LEX_NAME;


while ((c >= 'a' && c <= 'z') || isdigit(c) || c == '_') c = buf[++i];


bc_vec_string(&l->str, i, buf);


l->i += i - 1;
}

void bc_lex_init(BcLex *l) {
BC_SIG_ASSERT_LOCKED;
assert(l != NULL);
bc_vec_init(&l->str, sizeof(char), BC_DTOR_NONE);
}

void bc_lex_free(BcLex *l) {
BC_SIG_ASSERT_LOCKED;
assert(l != NULL);
bc_vec_free(&l->str);
}

void bc_lex_file(BcLex *l, const char *file) {
assert(l != NULL && file != NULL);
l->line = 1;
vm.file = file;
}

void bc_lex_next(BcLex *l) {

BC_SIG_ASSERT_LOCKED;

assert(l != NULL);

l->last = l->t;


l->line += (l->i != 0 && l->buf[l->i - 1] == '\n');


if (BC_ERR(l->last == BC_LEX_EOF)) bc_lex_err(l, BC_ERR_PARSE_EOF);

l->t = BC_LEX_EOF;


if (l->i == l->len) return;



do {
vm.next(l);
} while (l->t == BC_LEX_WHITESPACE);
}








static void bc_lex_fixText(BcLex *l, const char *text, size_t len) {
l->buf = text;
l->len = len;
}

bool bc_lex_readLine(BcLex *l) {

bool good;



BC_SIG_UNLOCK;

good = bc_vm_readLine(false);

BC_SIG_LOCK;

bc_lex_fixText(l, vm.buffer.v, vm.buffer.len - 1);

return good;
}

void bc_lex_text(BcLex *l, const char *text, bool is_stdin) {

BC_SIG_ASSERT_LOCKED;

assert(l != NULL && text != NULL);

bc_lex_fixText(l, text, strlen(text));
l->i = 0;
l->t = l->last = BC_LEX_INVALID;
l->is_stdin = is_stdin;

bc_lex_next(l);
}
