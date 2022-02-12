






















#include <math.h>
#include "ucl.h"
#include "ucl_internal.h"
#include "ucl_chartable.h"






struct ucl_parser_saved_state {
unsigned int line;
unsigned int column;
size_t remain;
const unsigned char *pos;
};








#define ucl_chunk_skipc(chunk, p) do { if (*(p) == '\n') { (chunk)->line ++; (chunk)->column = 0; } else (chunk)->column ++; (p++); (chunk)->pos ++; (chunk)->remain --; } while (0)











static inline void
ucl_set_err (struct ucl_parser *parser, int code, const char *str, UT_string **err)
{
const char *fmt_string, *filename;
struct ucl_chunk *chunk = parser->chunks;

if (parser->cur_file) {
filename = parser->cur_file;
}
else {
filename = "<unknown>";
}

if (chunk->pos < chunk->end) {
if (isgraph (*chunk->pos)) {
fmt_string = "error while parsing %s: "
"line: %d, column: %d - '%s', character: '%c'";
}
else {
fmt_string = "error while parsing %s: "
"line: %d, column: %d - '%s', character: '0x%02x'";
}
ucl_create_err (err, fmt_string,
filename, chunk->line, chunk->column,
str, *chunk->pos);
}
else {
ucl_create_err (err, "error while parsing %s: at the end of chunk: %s",
filename, str);
}

parser->err_code = code;
parser->state = UCL_STATE_ERROR;
}

static void
ucl_save_comment (struct ucl_parser *parser, const char *begin, size_t len)
{
ucl_object_t *nobj;

if (len > 0 && begin != NULL) {
nobj = ucl_object_fromstring_common (begin, len, 0);

if (parser->last_comment) {

DL_APPEND (parser->last_comment, nobj);
}
else {
parser->last_comment = nobj;
}
}
}

static void
ucl_attach_comment (struct ucl_parser *parser, ucl_object_t *obj, bool before)
{
if (parser->last_comment) {
ucl_object_insert_key (parser->comments, parser->last_comment,
(const char *)&obj, sizeof (void *), true);

if (before) {
parser->last_comment->flags |= UCL_OBJECT_INHERITED;
}

parser->last_comment = NULL;
}
}






static bool
ucl_skip_comments (struct ucl_parser *parser)
{
struct ucl_chunk *chunk = parser->chunks;
const unsigned char *p, *beg = NULL;
int comments_nested = 0;
bool quoted = false;

p = chunk->pos;

start:
if (chunk->remain > 0 && *p == '#') {
if (parser->state != UCL_STATE_SCOMMENT &&
parser->state != UCL_STATE_MCOMMENT) {
beg = p;

while (p < chunk->end) {
if (*p == '\n') {
if (parser->flags & UCL_PARSER_SAVE_COMMENTS) {
ucl_save_comment (parser, beg, p - beg);
beg = NULL;
}

ucl_chunk_skipc (chunk, p);

goto start;
}
ucl_chunk_skipc (chunk, p);
}
}
}
else if (chunk->remain >= 2 && *p == '/') {
if (p[1] == '*') {
beg = p;
ucl_chunk_skipc (chunk, p);
comments_nested ++;
ucl_chunk_skipc (chunk, p);

while (p < chunk->end) {
if (*p == '"' && *(p - 1) != '\\') {
quoted = !quoted;
}

if (!quoted) {
if (*p == '*') {
ucl_chunk_skipc (chunk, p);
if (*p == '/') {
comments_nested --;
if (comments_nested == 0) {
if (parser->flags & UCL_PARSER_SAVE_COMMENTS) {
ucl_save_comment (parser, beg, p - beg + 1);
beg = NULL;
}

ucl_chunk_skipc (chunk, p);
goto start;
}
}
ucl_chunk_skipc (chunk, p);
}
else if (p[0] == '/' && chunk->remain >= 2 && p[1] == '*') {
comments_nested ++;
ucl_chunk_skipc (chunk, p);
ucl_chunk_skipc (chunk, p);
continue;
}
}

ucl_chunk_skipc (chunk, p);
}
if (comments_nested != 0) {
ucl_set_err (parser, UCL_ENESTED,
"unfinished multiline comment", &parser->err);
return false;
}
}
}

if (beg && p > beg && (parser->flags & UCL_PARSER_SAVE_COMMENTS)) {
ucl_save_comment (parser, beg, p - beg);
}

return true;
}







static inline unsigned long
ucl_lex_num_multiplier (const unsigned char c, bool is_bytes) {
const struct {
char c;
long mult_normal;
long mult_bytes;
} multipliers[] = {
{'m', 1000 * 1000, 1024 * 1024},
{'k', 1000, 1024},
{'g', 1000 * 1000 * 1000, 1024 * 1024 * 1024}
};
int i;

for (i = 0; i < 3; i ++) {
if (tolower (c) == multipliers[i].c) {
if (is_bytes) {
return multipliers[i].mult_bytes;
}
return multipliers[i].mult_normal;
}
}

return 1;
}







static inline double
ucl_lex_time_multiplier (const unsigned char c) {
const struct {
char c;
double mult;
} multipliers[] = {
{'m', 60},
{'h', 60 * 60},
{'d', 60 * 60 * 24},
{'w', 60 * 60 * 24 * 7},
{'y', 60 * 60 * 24 * 365}
};
int i;

for (i = 0; i < 5; i ++) {
if (tolower (c) == multipliers[i].c) {
return multipliers[i].mult;
}
}

return 1;
}






static inline bool
ucl_lex_is_atom_end (const unsigned char c)
{
return ucl_test_character (c, UCL_CHARACTER_VALUE_END);
}

static inline bool
ucl_lex_is_comment (const unsigned char c1, const unsigned char c2)
{
if (c1 == '/') {
if (c2 == '*') {
return true;
}
}
else if (c1 == '#') {
return true;
}
return false;
}











static inline const char *
ucl_check_variable_safe (struct ucl_parser *parser, const char *ptr, size_t remain,
size_t *out_len, bool strict, bool *found)
{
struct ucl_variable *var;
unsigned char *dst;
size_t dstlen;
bool need_free = false;

LL_FOREACH (parser->variables, var) {
if (strict) {
if (remain == var->var_len) {
if (memcmp (ptr, var->var, var->var_len) == 0) {
*out_len += var->value_len;
*found = true;
return (ptr + var->var_len);
}
}
}
else {
if (remain >= var->var_len) {
if (memcmp (ptr, var->var, var->var_len) == 0) {
*out_len += var->value_len;
*found = true;
return (ptr + var->var_len);
}
}
}
}


if (!(*found) && parser->var_handler != NULL && strict) {

if (parser->var_handler (ptr, remain, &dst, &dstlen, &need_free,
parser->var_data)) {
*out_len = dstlen;
*found = true;
if (need_free) {
free (dst);
}
return (ptr + remain);
}
}

return ptr;
}










static const char *
ucl_check_variable (struct ucl_parser *parser, const char *ptr,
size_t remain, size_t *out_len, bool *vars_found)
{
const char *p, *end, *ret = ptr;
bool found = false;

if (*ptr == '{') {

p = ptr + 1;
end = ptr + remain;
while (p < end) {
if (*p == '}') {
ret = ucl_check_variable_safe (parser, ptr + 1, p - ptr - 1,
out_len, true, &found);
if (found) {

ret ++;
if (!*vars_found) {
*vars_found = true;
}
}
else {
*out_len += 2;
}
break;
}
p ++;
}
}
else if (*ptr != '$') {

ret = ucl_check_variable_safe (parser, ptr, remain, out_len, false, &found);
if (found && !*vars_found) {
*vars_found = true;
}
if (!found) {
(*out_len) ++;
}
}
else {
ret ++;
(*out_len) ++;
}

return ret;
}









static const char *
ucl_expand_single_variable (struct ucl_parser *parser, const char *ptr,
size_t remain, unsigned char **dest)
{
unsigned char *d = *dest, *dst;
const char *p = ptr + 1, *ret;
struct ucl_variable *var;
size_t dstlen;
bool need_free = false;
bool found = false;
bool strict = false;

ret = ptr + 1;
remain --;

if (*p == '$') {
*d++ = *p++;
*dest = d;
return p;
}
else if (*p == '{') {
p ++;
strict = true;
ret += 2;
remain -= 2;
}

LL_FOREACH (parser->variables, var) {
if (remain >= var->var_len) {
if (memcmp (p, var->var, var->var_len) == 0) {
memcpy (d, var->value, var->value_len);
ret += var->var_len;
d += var->value_len;
found = true;
break;
}
}
}
if (!found) {
if (strict && parser->var_handler != NULL) {
if (parser->var_handler (p, remain, &dst, &dstlen, &need_free,
parser->var_data)) {
memcpy (d, dst, dstlen);
ret += remain;
d += dstlen;
found = true;
if (need_free) {
free (dst);
}
}
}


if (!found) {
if (strict) {

memcpy (d, ptr, 2);
d += 2;
ret --;
}
else {
memcpy (d, ptr, 1);
d ++;
}
}
}

*dest = d;
return ret;
}









static ssize_t
ucl_expand_variable (struct ucl_parser *parser, unsigned char **dst,
const char *src, size_t in_len)
{
const char *p, *end = src + in_len;
unsigned char *d;
size_t out_len = 0;
bool vars_found = false;

if (parser->flags & UCL_PARSER_DISABLE_MACRO) {
*dst = NULL;
return in_len;
}

p = src;
while (p != end) {
if (*p == '$') {
p = ucl_check_variable (parser, p + 1, end - p - 1, &out_len, &vars_found);
}
else {
p ++;
out_len ++;
}
}

if (!vars_found) {

*dst = NULL;
return in_len;
}

*dst = UCL_ALLOC (out_len + 1);
if (*dst == NULL) {
return in_len;
}

d = *dst;
p = src;
while (p != end) {
if (*p == '$') {
p = ucl_expand_single_variable (parser, p, end - p, &d);
}
else {
*d++ = *p++;
}
}

*d = '\0';

return out_len;
}














static inline ssize_t
ucl_copy_or_store_ptr (struct ucl_parser *parser,
const unsigned char *src, unsigned char **dst,
const char **dst_const, size_t in_len,
bool need_unescape, bool need_lowercase, bool need_expand,
bool unescape_squote)
{
ssize_t ret = -1, tret;
unsigned char *tmp;

if (need_unescape || need_lowercase ||
(need_expand && parser->variables != NULL) ||
!(parser->flags & UCL_PARSER_ZEROCOPY)) {

*dst = UCL_ALLOC (in_len + 1);
if (*dst == NULL) {
ucl_set_err (parser, UCL_EINTERNAL, "cannot allocate memory for a string",
&parser->err);
return false;
}
if (need_lowercase) {
ret = ucl_strlcpy_tolower (*dst, src, in_len + 1);
}
else {
ret = ucl_strlcpy_unsafe (*dst, src, in_len + 1);
}

if (need_unescape) {
if (!unescape_squote) {
ret = ucl_unescape_json_string (*dst, ret);
}
else {
ret = ucl_unescape_squoted_string (*dst, ret);
}
}

if (need_expand) {
tmp = *dst;
tret = ret;
ret = ucl_expand_variable (parser, dst, tmp, ret);
if (*dst == NULL) {

*dst = tmp;
ret = tret;
}
else {

UCL_FREE (in_len + 1, tmp);
}
}
*dst_const = *dst;
}
else {
*dst_const = src;
ret = in_len;
}

return ret;
}








static inline ucl_object_t *
ucl_parser_add_container (ucl_object_t *obj, struct ucl_parser *parser,
bool is_array, uint32_t level, bool has_obrace)
{
struct ucl_stack *st;
ucl_object_t *nobj;

if (obj == NULL) {
nobj = ucl_object_new_full (is_array ? UCL_ARRAY : UCL_OBJECT, parser->chunks->priority);
if (nobj == NULL) {
goto enomem0;
}
} else {
if (obj->type == (is_array ? UCL_OBJECT : UCL_ARRAY)) {

ucl_set_err (parser, UCL_EMERGE,
"cannot merge an object with an array",
&parser->err);

return NULL;
}
nobj = obj;
nobj->type = is_array ? UCL_ARRAY : UCL_OBJECT;
}

if (!is_array) {
if (nobj->value.ov == NULL) {
nobj->value.ov = ucl_hash_create (parser->flags & UCL_PARSER_KEY_LOWERCASE);
if (nobj->value.ov == NULL) {
goto enomem1;
}
}
parser->state = UCL_STATE_KEY;
} else {
parser->state = UCL_STATE_VALUE;
}

st = UCL_ALLOC (sizeof (struct ucl_stack));

if (st == NULL) {
goto enomem1;
}

st->obj = nobj;

if (level >= UINT16_MAX) {
ucl_set_err (parser, UCL_ENESTED,
"objects are nesting too deep (over 65535 limit)",
&parser->err);
if (nobj != obj) {
ucl_object_unref (obj);
}

return NULL;
}


st->e.params.level = level;
st->e.params.line = parser->chunks->line;
st->chunk = parser->chunks;

if (has_obrace) {
st->e.params.flags = UCL_STACK_HAS_OBRACE;
}
else {
st->e.params.flags = 0;
}

LL_PREPEND (parser->stack, st);
parser->cur_obj = nobj;

return nobj;
enomem1:
if (nobj != obj)
ucl_object_unref (nobj);
enomem0:
ucl_set_err (parser, UCL_EINTERNAL, "cannot allocate memory for an object",
&parser->err);
return NULL;
}

int
ucl_maybe_parse_number (ucl_object_t *obj,
const char *start, const char *end, const char **pos,
bool allow_double, bool number_bytes, bool allow_time)
{
const char *p = start, *c = start;
char *endptr;
bool got_dot = false, got_exp = false, need_double = false,
is_time = false, valid_start = false, is_hex = false,
is_neg = false;
double dv = 0;
int64_t lv = 0;

if (*p == '-') {
is_neg = true;
c ++;
p ++;
}
while (p < end) {
if (is_hex && isxdigit (*p)) {
p ++;
}
else if (isdigit (*p)) {
valid_start = true;
p ++;
}
else if (!is_hex && (*p == 'x' || *p == 'X')) {
is_hex = true;
allow_double = false;
c = p + 1;
}
else if (allow_double) {
if (p == c) {

*pos = start;
return EINVAL;
}
else if (*p == '.') {
if (got_dot) {

*pos = start;
return EINVAL;
}
else {
got_dot = true;
need_double = true;
p ++;
}
}
else if (*p == 'e' || *p == 'E') {
if (got_exp) {

*pos = start;
return EINVAL;
}
else {
got_exp = true;
need_double = true;
p ++;
if (p >= end) {
*pos = start;
return EINVAL;
}
if (!isdigit (*p) && *p != '+' && *p != '-') {

*pos = start;
return EINVAL;
}
else {
p ++;
}
}
}
else {

break;
}
}
else {
break;
}
}

if (!valid_start) {
*pos = start;
return EINVAL;
}

errno = 0;
if (need_double) {
dv = strtod (c, &endptr);
}
else {
if (is_hex) {
lv = strtoimax (c, &endptr, 16);
}
else {
lv = strtoimax (c, &endptr, 10);
}
}
if (errno == ERANGE) {
*pos = start;
return ERANGE;
}


if (endptr == NULL || ucl_lex_is_atom_end (*endptr) || *endptr == '\0') {
p = endptr;
goto set_obj;
}

if (endptr < end && endptr != start) {
p = endptr;
switch (*p) {
case 'm':
case 'M':
case 'g':
case 'G':
case 'k':
case 'K':
if (end - p >= 2) {
if (p[1] == 's' || p[1] == 'S') {

if (!need_double) {
need_double = true;
dv = lv;
}
is_time = true;
if (p[0] == 'm' || p[0] == 'M') {
dv /= 1000.;
}
else {
dv *= ucl_lex_num_multiplier (*p, false);
}
p += 2;
goto set_obj;
}
else if (number_bytes || (p[1] == 'b' || p[1] == 'B')) {

if (need_double) {
need_double = false;
lv = dv;
}
lv *= ucl_lex_num_multiplier (*p, true);
p += 2;
goto set_obj;
}
else if (ucl_lex_is_atom_end (p[1])) {
if (need_double) {
dv *= ucl_lex_num_multiplier (*p, false);
}
else {
lv *= ucl_lex_num_multiplier (*p, number_bytes);
}
p ++;
goto set_obj;
}
else if (allow_time && end - p >= 3) {
if (tolower (p[0]) == 'm' &&
tolower (p[1]) == 'i' &&
tolower (p[2]) == 'n') {

if (!need_double) {
need_double = true;
dv = lv;
}
is_time = true;
dv *= 60.;
p += 3;
goto set_obj;
}
}
}
else {
if (need_double) {
dv *= ucl_lex_num_multiplier (*p, false);
}
else {
lv *= ucl_lex_num_multiplier (*p, number_bytes);
}
p ++;
goto set_obj;
}
break;
case 'S':
case 's':
if (allow_time &&
(p == end - 1 || ucl_lex_is_atom_end (p[1]))) {
if (!need_double) {
need_double = true;
dv = lv;
}
p ++;
is_time = true;
goto set_obj;
}
break;
case 'h':
case 'H':
case 'd':
case 'D':
case 'w':
case 'W':
case 'Y':
case 'y':
if (allow_time &&
(p == end - 1 || ucl_lex_is_atom_end (p[1]))) {
if (!need_double) {
need_double = true;
dv = lv;
}
is_time = true;
dv *= ucl_lex_time_multiplier (*p);
p ++;
goto set_obj;
}
break;
case '\t':
case ' ':
while (p < end && ucl_test_character(*p, UCL_CHARACTER_WHITESPACE)) {
p++;
}
if (ucl_lex_is_atom_end(*p))
goto set_obj;
break;
}
}
else if (endptr == end) {

p = endptr;
goto set_obj;
}

*pos = c;
return EINVAL;

set_obj:
if (obj != NULL) {
if (allow_double && (need_double || is_time)) {
if (!is_time) {
obj->type = UCL_FLOAT;
}
else {
obj->type = UCL_TIME;
}
obj->value.dv = is_neg ? (-dv) : dv;
}
else {
obj->type = UCL_INT;
obj->value.iv = is_neg ? (-lv) : lv;
}
}
*pos = p;
return 0;
}








static bool
ucl_lex_number (struct ucl_parser *parser,
struct ucl_chunk *chunk, ucl_object_t *obj)
{
const unsigned char *pos;
int ret;

ret = ucl_maybe_parse_number (obj, chunk->pos, chunk->end, (const char **)&pos,
true, false, ((parser->flags & UCL_PARSER_NO_TIME) == 0));

if (ret == 0) {
chunk->remain -= pos - chunk->pos;
chunk->column += pos - chunk->pos;
chunk->pos = pos;
return true;
}
else if (ret == ERANGE) {
ucl_set_err (parser, UCL_ESYNTAX, "numeric value out of range",
&parser->err);
}

return false;
}










static bool
ucl_lex_json_string (struct ucl_parser *parser,
struct ucl_chunk *chunk,
bool *need_unescape,
bool *ucl_escape,
bool *var_expand)
{
const unsigned char *p = chunk->pos;
unsigned char c;
int i;

while (p < chunk->end) {
c = *p;
if (c < 0x1F) {

if (c == '\n') {
ucl_set_err (parser, UCL_ESYNTAX, "unexpected newline",
&parser->err);
}
else {
ucl_set_err (parser, UCL_ESYNTAX, "unexpected control character",
&parser->err);
}
return false;
}
else if (c == '\\') {
ucl_chunk_skipc (chunk, p);
c = *p;
if (p >= chunk->end) {
ucl_set_err (parser, UCL_ESYNTAX, "unfinished escape character",
&parser->err);
return false;
}
else if (ucl_test_character (c, UCL_CHARACTER_ESCAPE)) {
if (c == 'u') {
ucl_chunk_skipc (chunk, p);
for (i = 0; i < 4 && p < chunk->end; i ++) {
if (!isxdigit (*p)) {
ucl_set_err (parser, UCL_ESYNTAX, "invalid utf escape",
&parser->err);
return false;
}
ucl_chunk_skipc (chunk, p);
}
if (p >= chunk->end) {
ucl_set_err (parser, UCL_ESYNTAX,
"unfinished escape character",
&parser->err);
return false;
}
}
else {
ucl_chunk_skipc (chunk, p);
}
}
*need_unescape = true;
*ucl_escape = true;
continue;
}
else if (c == '"') {
ucl_chunk_skipc (chunk, p);
return true;
}
else if (ucl_test_character (c, UCL_CHARACTER_UCL_UNSAFE)) {
*ucl_escape = true;
}
else if (c == '$') {
*var_expand = true;
}
ucl_chunk_skipc (chunk, p);
}

ucl_set_err (parser, UCL_ESYNTAX,
"no quote at the end of json string",
&parser->err);
return false;
}








static bool
ucl_lex_squoted_string (struct ucl_parser *parser,
struct ucl_chunk *chunk, bool *need_unescape)
{
const unsigned char *p = chunk->pos;
unsigned char c;

while (p < chunk->end) {
c = *p;
if (c == '\\') {
ucl_chunk_skipc (chunk, p);

if (p >= chunk->end) {
ucl_set_err (parser, UCL_ESYNTAX,
"unfinished escape character",
&parser->err);
return false;
}
else {
ucl_chunk_skipc (chunk, p);
}

*need_unescape = true;
continue;
}
else if (c == '\'') {
ucl_chunk_skipc (chunk, p);
return true;
}

ucl_chunk_skipc (chunk, p);
}

ucl_set_err (parser, UCL_ESYNTAX,
"no quote at the end of single quoted string",
&parser->err);
return false;
}

static void
ucl_parser_append_elt (struct ucl_parser *parser, ucl_hash_t *cont,
ucl_object_t *top,
ucl_object_t *elt)
{
ucl_object_t *nobj;

if ((parser->flags & UCL_PARSER_NO_IMPLICIT_ARRAYS) == 0) {

top->flags |= UCL_OBJECT_MULTIVALUE;
DL_APPEND (top, elt);
parser->stack->obj->len ++;
}
else {
if ((top->flags & UCL_OBJECT_MULTIVALUE) != 0) {

ucl_array_append (top, elt);
}
else {

nobj = ucl_object_typed_new (UCL_ARRAY);
nobj->key = top->key;
nobj->keylen = top->keylen;
nobj->flags |= UCL_OBJECT_MULTIVALUE;
ucl_array_append (nobj, top);
ucl_array_append (nobj, elt);
ucl_hash_replace (cont, top, nobj);
}
}
}

bool
ucl_parser_process_object_element (struct ucl_parser *parser, ucl_object_t *nobj)
{
ucl_hash_t *container;
ucl_object_t *tobj = NULL, *cur;
char errmsg[256];

container = parser->stack->obj->value.ov;

DL_FOREACH (parser->stack->obj, cur) {
tobj = __DECONST (ucl_object_t *, ucl_hash_search_obj (cur->value.ov, nobj));

if (tobj != NULL) {
break;
}
}


if (tobj == NULL) {
container = ucl_hash_insert_object (container, nobj,
parser->flags & UCL_PARSER_KEY_LOWERCASE);
if (container == NULL) {
return false;
}
nobj->prev = nobj;
nobj->next = NULL;
parser->stack->obj->len ++;
}
else {
unsigned priold = ucl_object_get_priority (tobj),
prinew = ucl_object_get_priority (nobj);
switch (parser->chunks->strategy) {

case UCL_DUPLICATE_APPEND:









if (tobj->flags & UCL_OBJECT_INHERITED) {
prinew = priold + 1;
}

if (priold == prinew) {
ucl_parser_append_elt (parser, container, tobj, nobj);
}
else if (priold > prinew) {





DL_APPEND (parser->trash_objs, nobj);
}
else {
ucl_hash_replace (container, tobj, nobj);
ucl_object_unref (tobj);
}

break;

case UCL_DUPLICATE_REWRITE:

ucl_hash_replace (container, tobj, nobj);
ucl_object_unref (tobj);

break;

case UCL_DUPLICATE_ERROR:
snprintf(errmsg, sizeof(errmsg),
"duplicate element for key '%s' found",
nobj->key);
ucl_set_err (parser, UCL_EMERGE, errmsg, &parser->err);
return false;

case UCL_DUPLICATE_MERGE:




if (tobj->type == UCL_OBJECT || tobj->type == UCL_ARRAY) {
ucl_object_unref (nobj);
nobj = tobj;
}
else if (priold == prinew) {
ucl_parser_append_elt (parser, container, tobj, nobj);
}
else if (priold > prinew) {





DL_APPEND (parser->trash_objs, nobj);
}
else {
ucl_hash_replace (container, tobj, nobj);
ucl_object_unref (tobj);
}
break;
}
}

parser->stack->obj->value.ov = container;
parser->cur_obj = nobj;
ucl_attach_comment (parser, nobj, false);

return true;
}









static bool
ucl_parse_key (struct ucl_parser *parser, struct ucl_chunk *chunk,
bool *next_key, bool *end_of_object)
{
const unsigned char *p, *c = NULL, *end, *t;
const char *key = NULL;
bool got_quote = false, got_eq = false, got_semicolon = false,
need_unescape = false, ucl_escape = false, var_expand = false,
got_content = false, got_sep = false;
ucl_object_t *nobj;
ssize_t keylen;

p = chunk->pos;

if (*p == '.') {

if (!(parser->flags & UCL_PARSER_DISABLE_MACRO)) {
ucl_chunk_skipc (chunk, p);
}

parser->prev_state = parser->state;
parser->state = UCL_STATE_MACRO_NAME;
*end_of_object = false;
return true;
}
while (p < chunk->end) {



if (c == NULL) {
if (chunk->remain >= 2 && ucl_lex_is_comment (p[0], p[1])) {
if (!ucl_skip_comments (parser)) {
return false;
}
p = chunk->pos;
}
else if (ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE)) {
ucl_chunk_skipc (chunk, p);
}
else if (ucl_test_character (*p, UCL_CHARACTER_KEY_START)) {

c = p;
ucl_chunk_skipc (chunk, p);
got_content = true;
}
else if (*p == '"') {

c = p + 1;
got_quote = true;
got_content = true;
ucl_chunk_skipc (chunk, p);
}
else if (*p == '}') {

*end_of_object = true;
return true;
}
else if (*p == '.') {
ucl_chunk_skipc (chunk, p);
parser->prev_state = parser->state;
parser->state = UCL_STATE_MACRO_NAME;
return true;
}
else {

ucl_set_err (parser, UCL_ESYNTAX, "key must begin with a letter",
&parser->err);
return false;
}
}
else {

if (!got_quote) {
if (ucl_test_character (*p, UCL_CHARACTER_KEY)) {
got_content = true;
ucl_chunk_skipc (chunk, p);
}
else if (ucl_test_character (*p, UCL_CHARACTER_KEY_SEP)) {
end = p;
break;
}
else {
ucl_set_err (parser, UCL_ESYNTAX, "invalid character in a key",
&parser->err);
return false;
}
}
else {

if (!ucl_lex_json_string (parser, chunk, &need_unescape, &ucl_escape, &var_expand)) {
return false;
}

end = chunk->pos - 1;
p = chunk->pos;
break;
}
}
}

if (p >= chunk->end && got_content) {
ucl_set_err (parser, UCL_ESYNTAX, "unfinished key", &parser->err);
return false;
}
else if (!got_content) {
return true;
}
*end_of_object = false;

while (p < chunk->end) {
if (ucl_test_character (*p, UCL_CHARACTER_WHITESPACE)) {
ucl_chunk_skipc (chunk, p);
}
else if (*p == '=') {
if (!got_eq && !got_semicolon) {
ucl_chunk_skipc (chunk, p);
got_eq = true;
}
else {
ucl_set_err (parser, UCL_ESYNTAX, "unexpected '=' character",
&parser->err);
return false;
}
}
else if (*p == ':') {
if (!got_eq && !got_semicolon) {
ucl_chunk_skipc (chunk, p);
got_semicolon = true;
}
else {
ucl_set_err (parser, UCL_ESYNTAX, "unexpected ':' character",
&parser->err);
return false;
}
}
else if (chunk->remain >= 2 && ucl_lex_is_comment (p[0], p[1])) {

if (!ucl_skip_comments (parser)) {
return false;
}
p = chunk->pos;
}
else {

break;
}
}

if (p >= chunk->end && got_content) {
ucl_set_err (parser, UCL_ESYNTAX, "unfinished key", &parser->err);
return false;
}

got_sep = got_semicolon || got_eq;

if (!got_sep) {







t = p;
*next_key = false;
while (ucl_test_character (*t, UCL_CHARACTER_WHITESPACE)) {
t ++;
}

if (*t != '{' && *t != '[') {
while (t < chunk->end) {
if (*t == ',' || *t == ';' || *t == '\n' || *t == '\r') {
break;
}
else if (*t == '{' || *t == '[') {
*next_key = true;
break;
}
t ++;
}
}
}


nobj = ucl_object_new_full (UCL_NULL, parser->chunks->priority);
if (nobj == NULL) {
return false;
}
keylen = ucl_copy_or_store_ptr (parser, c, &nobj->trash_stack[UCL_TRASH_KEY],
&key, end - c, need_unescape, parser->flags & UCL_PARSER_KEY_LOWERCASE,
false, false);
if (keylen == -1) {
ucl_object_unref (nobj);
return false;
}
else if (keylen == 0) {
ucl_set_err (parser, UCL_ESYNTAX, "empty keys are not allowed", &parser->err);
ucl_object_unref (nobj);
return false;
}

nobj->key = key;
nobj->keylen = keylen;

if (!ucl_parser_process_object_element (parser, nobj)) {
return false;
}

if (ucl_escape) {
nobj->flags |= UCL_OBJECT_NEED_KEY_ESCAPE;
}


return true;
}









static bool
ucl_parse_string_value (struct ucl_parser *parser,
struct ucl_chunk *chunk, bool *var_expand, bool *need_unescape)
{
const unsigned char *p;
enum {
UCL_BRACE_ROUND = 0,
UCL_BRACE_SQUARE,
UCL_BRACE_FIGURE
};
int braces[3][2] = {{0, 0}, {0, 0}, {0, 0}};

p = chunk->pos;

while (p < chunk->end) {


if (*p == '{') {
braces[UCL_BRACE_FIGURE][0] ++;
}
else if (*p == '}') {
braces[UCL_BRACE_FIGURE][1] ++;
if (braces[UCL_BRACE_FIGURE][1] <= braces[UCL_BRACE_FIGURE][0]) {

ucl_chunk_skipc (chunk, p);
continue;
}
}

else if (*p == '[') {
braces[UCL_BRACE_SQUARE][0] ++;
}
else if (*p == ']') {
braces[UCL_BRACE_SQUARE][1] ++;
if (braces[UCL_BRACE_SQUARE][1] <= braces[UCL_BRACE_SQUARE][0]) {

ucl_chunk_skipc (chunk, p);
continue;
}
}
else if (*p == '$') {
*var_expand = true;
}
else if (*p == '\\') {
*need_unescape = true;
ucl_chunk_skipc (chunk, p);
if (p < chunk->end) {
ucl_chunk_skipc (chunk, p);
}
continue;
}

if (ucl_lex_is_atom_end (*p) || (chunk->remain >= 2 && ucl_lex_is_comment (p[0], p[1]))) {
break;
}
ucl_chunk_skipc (chunk, p);
}

return true;
}











static int
ucl_parse_multiline_string (struct ucl_parser *parser,
struct ucl_chunk *chunk, const unsigned char *term,
int term_len, unsigned char const **beg,
bool *var_expand)
{
const unsigned char *p, *c, *tend;
bool newline = false;
int len = 0;

p = chunk->pos;

c = p;

while (p < chunk->end) {
if (newline) {
if (chunk->end - p < term_len) {
return 0;
}
else if (memcmp (p, term, term_len) == 0) {
tend = p + term_len;
if (*tend != '\n' && *tend != ';' && *tend != ',') {

ucl_chunk_skipc (chunk, p);
continue;
}
len = p - c;
chunk->remain -= term_len;
chunk->pos = p + term_len;
chunk->column = term_len;
*beg = c;
break;
}
}
if (*p == '\n') {
newline = true;
}
else {
if (*p == '$') {
*var_expand = true;
}
newline = false;
}
ucl_chunk_skipc (chunk, p);
}

return len;
}

static inline ucl_object_t*
ucl_parser_get_container (struct ucl_parser *parser)
{
ucl_object_t *t, *obj = NULL;

if (parser == NULL || parser->stack == NULL || parser->stack->obj == NULL) {
return NULL;
}

if (parser->stack->obj->type == UCL_ARRAY) {

obj = ucl_object_new_full (UCL_NULL, parser->chunks->priority);
t = parser->stack->obj;

if (!ucl_array_append (t, obj)) {
ucl_object_unref (obj);
return NULL;
}

parser->cur_obj = obj;
ucl_attach_comment (parser, obj, false);
}
else {

obj = parser->cur_obj;
}

return obj;
}







static bool
ucl_parse_value (struct ucl_parser *parser, struct ucl_chunk *chunk)
{
const unsigned char *p, *c;
ucl_object_t *obj = NULL;
unsigned int stripped_spaces;
ssize_t str_len;
bool need_unescape = false, ucl_escape = false, var_expand = false;

p = chunk->pos;


if (ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE) ||
(chunk->remain >= 2 && ucl_lex_is_comment (p[0], p[1]))) {
while (p < chunk->end && ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE)) {
ucl_chunk_skipc (chunk, p);
}
if (!ucl_skip_comments (parser)) {
return false;
}
p = chunk->pos;
}

while (p < chunk->end) {
c = p;
switch (*p) {
case '"':
ucl_chunk_skipc (chunk, p);

if (!ucl_lex_json_string (parser, chunk, &need_unescape, &ucl_escape,
&var_expand)) {
return false;
}

obj = ucl_parser_get_container (parser);
if (!obj) {
return false;
}

str_len = chunk->pos - c - 2;
obj->type = UCL_STRING;
if ((str_len = ucl_copy_or_store_ptr (parser, c + 1,
&obj->trash_stack[UCL_TRASH_VALUE],
&obj->value.sv, str_len, need_unescape, false,
var_expand, false)) == -1) {
return false;
}

obj->len = str_len;
parser->state = UCL_STATE_AFTER_VALUE;

return true;
break;
case '\'':
ucl_chunk_skipc (chunk, p);

if (!ucl_lex_squoted_string (parser, chunk, &need_unescape)) {
return false;
}

obj = ucl_parser_get_container (parser);
if (!obj) {
return false;
}

str_len = chunk->pos - c - 2;
obj->type = UCL_STRING;
obj->flags |= UCL_OBJECT_SQUOTED;

if ((str_len = ucl_copy_or_store_ptr (parser, c + 1,
&obj->trash_stack[UCL_TRASH_VALUE],
&obj->value.sv, str_len, need_unescape, false,
var_expand, true)) == -1) {
return false;
}

obj->len = str_len;

parser->state = UCL_STATE_AFTER_VALUE;

return true;
break;
case '{':
obj = ucl_parser_get_container (parser);
if (obj == NULL) {
return false;
}

if (parser->stack) {
obj = ucl_parser_add_container (obj, parser, false,
parser->stack->e.params.level, true);
}
else {
return false;
}
if (obj == NULL) {
return false;
}

ucl_chunk_skipc (chunk, p);

return true;
break;
case '[':
obj = ucl_parser_get_container (parser);
if (obj == NULL) {
return false;
}

if (parser->stack) {
obj = ucl_parser_add_container (obj, parser, true,
parser->stack->e.params.level, true);
}
else {
return false;
}

if (obj == NULL) {
return false;
}

ucl_chunk_skipc (chunk, p);

return true;
break;
case ']':

if (parser->stack && parser->stack->obj->type == UCL_ARRAY) {
parser->state = UCL_STATE_AFTER_VALUE;
return true;
}
else {
goto parse_string;
}
break;
case '<':
obj = ucl_parser_get_container (parser);

if (chunk->end - p > 3) {
if (memcmp (p, "<<", 2) == 0) {
p += 2;

while (p < chunk->end && *p >= 'A' && *p <= 'Z') {
p ++;
}
if (*p =='\n') {

chunk->remain -= p - c + 1;
c += 2;
chunk->pos = p + 1;
chunk->column = 0;
chunk->line ++;
if ((str_len = ucl_parse_multiline_string (parser, chunk, c,
p - c, &c, &var_expand)) == 0) {
ucl_set_err (parser, UCL_ESYNTAX,
"unterminated multiline value", &parser->err);
return false;
}

obj->type = UCL_STRING;
obj->flags |= UCL_OBJECT_MULTILINE;
if ((str_len = ucl_copy_or_store_ptr (parser, c,
&obj->trash_stack[UCL_TRASH_VALUE],
&obj->value.sv, str_len - 1, false,
false, var_expand, false)) == -1) {
return false;
}
obj->len = str_len;

parser->state = UCL_STATE_AFTER_VALUE;

return true;
}
}
}


default:
parse_string:
if (obj == NULL) {
obj = ucl_parser_get_container (parser);
}


if (ucl_test_character (*p, UCL_CHARACTER_VALUE_DIGIT_START)) {
if (!ucl_lex_number (parser, chunk, obj)) {
if (parser->state == UCL_STATE_ERROR) {
return false;
}
}
else {
parser->state = UCL_STATE_AFTER_VALUE;
return true;
}

}

if (!ucl_parse_string_value (parser, chunk, &var_expand,
&need_unescape)) {
return false;
}

stripped_spaces = 0;
while (ucl_test_character (*(chunk->pos - 1 - stripped_spaces),
UCL_CHARACTER_WHITESPACE)) {
stripped_spaces ++;
}
str_len = chunk->pos - c - stripped_spaces;
if (str_len <= 0) {
ucl_set_err (parser, UCL_ESYNTAX, "string value must not be empty",
&parser->err);
return false;
}
else if (str_len == 4 && memcmp (c, "null", 4) == 0) {
obj->len = 0;
obj->type = UCL_NULL;
}
else if (str_len == 3 && memcmp (c, "nan", 3) == 0) {
obj->len = 0;
obj->type = UCL_FLOAT;
obj->value.dv = NAN;
}
else if (str_len == 3 && memcmp (c, "inf", 3) == 0) {
obj->len = 0;
obj->type = UCL_FLOAT;
obj->value.dv = INFINITY;
}
else if (!ucl_maybe_parse_boolean (obj, c, str_len)) {
obj->type = UCL_STRING;
if ((str_len = ucl_copy_or_store_ptr (parser, c,
&obj->trash_stack[UCL_TRASH_VALUE],
&obj->value.sv, str_len, need_unescape,
false, var_expand, false)) == -1) {
return false;
}
obj->len = str_len;
}

parser->state = UCL_STATE_AFTER_VALUE;

return true;
break;
}
}

return true;
}







static bool
ucl_parse_after_value (struct ucl_parser *parser, struct ucl_chunk *chunk)
{
const unsigned char *p;
bool got_sep = false;
struct ucl_stack *st;

p = chunk->pos;

while (p < chunk->end) {
if (ucl_test_character (*p, UCL_CHARACTER_WHITESPACE)) {

ucl_chunk_skipc (chunk, p);
}
else if (chunk->remain >= 2 && ucl_lex_is_comment (p[0], p[1])) {

if (!ucl_skip_comments (parser)) {
return false;
}

got_sep = true;
p = chunk->pos;
}
else if (ucl_test_character (*p, UCL_CHARACTER_VALUE_END)) {
if (*p == '}' || *p == ']') {
if (parser->stack == NULL) {
ucl_set_err (parser, UCL_ESYNTAX,
"end of array or object detected without corresponding start",
&parser->err);
return false;
}
if ((*p == '}' && parser->stack->obj->type == UCL_OBJECT) ||
(*p == ']' && parser->stack->obj->type == UCL_ARRAY)) {


st = parser->stack;

if (!(st->e.params.flags & UCL_STACK_HAS_OBRACE)) {
parser->err_code = UCL_EUNPAIRED;
ucl_create_err (&parser->err,
"%s:%d object closed with } is not opened with { at line %d",
chunk->fname ? chunk->fname : "memory",
parser->chunks->line, st->e.params.line);

return false;
}

parser->stack = st->next;
UCL_FREE (sizeof (struct ucl_stack), st);

if (parser->cur_obj) {
ucl_attach_comment (parser, parser->cur_obj, true);
}

while (parser->stack != NULL) {
st = parser->stack;

if (st->next == NULL) {
break;
}
else if (st->next->e.params.level == st->e.params.level) {
break;
}


parser->stack = st->next;
parser->cur_obj = st->obj;
UCL_FREE (sizeof (struct ucl_stack), st);
}
}
else {
ucl_set_err (parser, UCL_ESYNTAX,
"unexpected terminating symbol detected",
&parser->err);
return false;
}

if (parser->stack == NULL) {

return true;
}
else {
ucl_chunk_skipc (chunk, p);
}
got_sep = true;
}
else {

got_sep = true;
ucl_chunk_skipc (chunk, p);
}
}
else {

if (!got_sep) {
ucl_set_err (parser, UCL_ESYNTAX, "delimiter is missing",
&parser->err);
return false;
}
return true;
}
}

return true;
}

static bool
ucl_skip_macro_as_comment (struct ucl_parser *parser,
struct ucl_chunk *chunk)
{
const unsigned char *p, *c;
enum {
macro_skip_start = 0,
macro_has_symbols,
macro_has_obrace,
macro_has_quote,
macro_has_backslash,
macro_has_sqbrace,
macro_save
} state = macro_skip_start, prev_state = macro_skip_start;

p = chunk->pos;
c = chunk->pos;

while (p < chunk->end) {
switch (state) {
case macro_skip_start:
if (!ucl_test_character (*p, UCL_CHARACTER_WHITESPACE)) {
state = macro_has_symbols;
}
else if (ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE)) {
state = macro_save;
continue;
}

ucl_chunk_skipc (chunk, p);
break;

case macro_has_symbols:
if (*p == '{') {
state = macro_has_sqbrace;
}
else if (*p == '(') {
state = macro_has_obrace;
}
else if (*p == '"') {
state = macro_has_quote;
}
else if (*p == '\n') {
state = macro_save;
continue;
}

ucl_chunk_skipc (chunk, p);
break;

case macro_has_obrace:
if (*p == '\\') {
prev_state = state;
state = macro_has_backslash;
}
else if (*p == ')') {
state = macro_has_symbols;
}

ucl_chunk_skipc (chunk, p);
break;

case macro_has_sqbrace:
if (*p == '\\') {
prev_state = state;
state = macro_has_backslash;
}
else if (*p == '}') {
state = macro_save;
}

ucl_chunk_skipc (chunk, p);
break;

case macro_has_quote:
if (*p == '\\') {
prev_state = state;
state = macro_has_backslash;
}
else if (*p == '"') {
state = macro_save;
}

ucl_chunk_skipc (chunk, p);
break;

case macro_has_backslash:
state = prev_state;
ucl_chunk_skipc (chunk, p);
break;

case macro_save:
if (parser->flags & UCL_PARSER_SAVE_COMMENTS) {
ucl_save_comment (parser, c, p - c);
}

return true;
}
}

return false;
}










static bool
ucl_parse_macro_value (struct ucl_parser *parser,
struct ucl_chunk *chunk, struct ucl_macro *macro,
unsigned char const **macro_start, size_t *macro_len)
{
const unsigned char *p, *c;
bool need_unescape = false, ucl_escape = false, var_expand = false;

p = chunk->pos;

switch (*p) {
case '"':

c = p;
ucl_chunk_skipc (chunk, p);
if (!ucl_lex_json_string (parser, chunk, &need_unescape, &ucl_escape, &var_expand)) {
return false;
}

*macro_start = c + 1;
*macro_len = chunk->pos - c - 2;
p = chunk->pos;
break;
case '{':

ucl_chunk_skipc (chunk, p);

while (p < chunk->end) {
if (ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE)) {
ucl_chunk_skipc (chunk, p);
}
else {
break;
}
}
c = p;
while (p < chunk->end) {
if (*p == '}') {
break;
}
ucl_chunk_skipc (chunk, p);
}
*macro_start = c;
*macro_len = p - c;
ucl_chunk_skipc (chunk, p);
break;
default:

c = p;
while (p < chunk->end) {
if (ucl_lex_is_atom_end (*p)) {
break;
}
ucl_chunk_skipc (chunk, p);
}
*macro_start = c;
*macro_len = p - c;
break;
}



while (p < chunk->end) {
if (!ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE) && *p != ';') {
break;
}
ucl_chunk_skipc (chunk, p);
}
return true;
}







static ucl_object_t *
ucl_parse_macro_arguments (struct ucl_parser *parser,
struct ucl_chunk *chunk)
{
ucl_object_t *res = NULL;
struct ucl_parser *params_parser;
int obraces = 1, ebraces = 0, state = 0;
const unsigned char *p, *c;
size_t args_len = 0;
struct ucl_parser_saved_state saved;

saved.column = chunk->column;
saved.line = chunk->line;
saved.pos = chunk->pos;
saved.remain = chunk->remain;
p = chunk->pos;

if (*p != '(' || chunk->remain < 2) {
return NULL;
}


ucl_chunk_skipc (chunk, p);
c = p;

while ((p) < (chunk)->end) {
switch (state) {
case 0:

if (*p == '(') {
obraces ++;
}
else if (*p == ')') {
ebraces ++;
}
else if (*p == '"') {
state = 1;
}

if (obraces == ebraces) {
state = 99;
}
else {
args_len ++;
}

if (chunk->remain == 0) {
goto restore_chunk;
}
ucl_chunk_skipc (chunk, p);
break;
case 1:

if (*p == '"' && *(p - 1) != '\\') {
state = 0;
}
if (chunk->remain == 0) {
goto restore_chunk;
}
args_len ++;
ucl_chunk_skipc (chunk, p);
break;
case 99:




params_parser = ucl_parser_new (parser->flags);
if (!ucl_parser_add_chunk (params_parser, c, args_len)) {
ucl_set_err (parser, UCL_ESYNTAX, "macro arguments parsing error",
&parser->err);
}
else {
res = ucl_parser_get_object (params_parser);
}
ucl_parser_free (params_parser);

return res;

break;
}
}

return res;

restore_chunk:
chunk->column = saved.column;
chunk->line = saved.line;
chunk->pos = saved.pos;
chunk->remain = saved.remain;

return NULL;
}

#define SKIP_SPACES_COMMENTS(parser, chunk, p) do { while ((p) < (chunk)->end) { if (!ucl_test_character (*(p), UCL_CHARACTER_WHITESPACE_UNSAFE)) { if ((chunk)->remain >= 2 && ucl_lex_is_comment ((p)[0], (p)[1])) { if (!ucl_skip_comments (parser)) { return false; } p = (chunk)->pos; } break; } ucl_chunk_skipc (chunk, p); } } while(0)



















static bool
ucl_state_machine (struct ucl_parser *parser)
{
ucl_object_t *obj, *macro_args;
struct ucl_chunk *chunk = parser->chunks;
const unsigned char *p, *c = NULL, *macro_start = NULL;
unsigned char *macro_escaped;
size_t macro_len = 0;
struct ucl_macro *macro = NULL;
bool next_key = false, end_of_object = false, ret;

if (parser->top_obj == NULL) {
parser->state = UCL_STATE_INIT;
}

p = chunk->pos;
while (chunk->pos < chunk->end) {
switch (parser->state) {
case UCL_STATE_INIT:





if (!ucl_skip_comments (parser)) {
parser->prev_state = parser->state;
parser->state = UCL_STATE_ERROR;
return false;
}
else {
bool seen_obrace = false;


while (p < chunk->end && ucl_test_character (*p,
UCL_CHARACTER_WHITESPACE_UNSAFE)) {
ucl_chunk_skipc (chunk, p);
}

p = chunk->pos;

if (p < chunk->end) {
if (*p == '[') {
parser->state = UCL_STATE_VALUE;
ucl_chunk_skipc (chunk, p);
seen_obrace = true;
}
else {

if (*p == '{') {
ucl_chunk_skipc (chunk, p);
parser->state = UCL_STATE_KEY_OBRACE;
seen_obrace = true;
}
else {
parser->state = UCL_STATE_KEY;
}
}
}

if (parser->top_obj == NULL) {
if (parser->state == UCL_STATE_VALUE) {
obj = ucl_parser_add_container (NULL, parser, true, 0,
seen_obrace);
}
else {
obj = ucl_parser_add_container (NULL, parser, false, 0,
seen_obrace);
}

if (obj == NULL) {
return false;
}

parser->top_obj = obj;
parser->cur_obj = obj;
}

}
break;
case UCL_STATE_KEY:
case UCL_STATE_KEY_OBRACE:

while (p < chunk->end && ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE)) {
ucl_chunk_skipc (chunk, p);
}
if (p == chunk->end || *p == '}') {

parser->state = UCL_STATE_AFTER_VALUE;
continue;
}
if (parser->stack == NULL) {

ucl_set_err (parser, UCL_ESYNTAX, "top object is finished but the parser "
"expects a key", &parser->err);
parser->prev_state = parser->state;
parser->state = UCL_STATE_ERROR;
return false;
}
if (!ucl_parse_key (parser, chunk, &next_key, &end_of_object)) {
parser->prev_state = parser->state;
parser->state = UCL_STATE_ERROR;
return false;
}

if (end_of_object) {
p = chunk->pos;
parser->state = UCL_STATE_AFTER_VALUE;
continue;
}
else if (parser->state != UCL_STATE_MACRO_NAME) {
if (next_key && parser->stack->obj->type == UCL_OBJECT) {

obj = ucl_parser_add_container (parser->cur_obj,
parser,
false,
parser->stack->e.params.level + 1,
parser->state == UCL_STATE_KEY_OBRACE);
if (obj == NULL) {
return false;
}
}
else {
parser->state = UCL_STATE_VALUE;
}
}
else {
c = chunk->pos;
}
p = chunk->pos;
break;
case UCL_STATE_VALUE:

if (!parser->cur_obj || !ucl_parse_value (parser, chunk)) {
parser->prev_state = parser->state;
parser->state = UCL_STATE_ERROR;
return false;
}

p = chunk->pos;
break;
case UCL_STATE_AFTER_VALUE:
if (!ucl_parse_after_value (parser, chunk)) {
parser->prev_state = parser->state;
parser->state = UCL_STATE_ERROR;
return false;
}

if (parser->stack != NULL) {
if (parser->stack->obj->type == UCL_OBJECT) {
parser->state = UCL_STATE_KEY;
}
else {

parser->state = UCL_STATE_VALUE;
}
}
else {

return true;
}

p = chunk->pos;
break;
case UCL_STATE_MACRO_NAME:
if (parser->flags & UCL_PARSER_DISABLE_MACRO) {
if (!ucl_skip_macro_as_comment (parser, chunk)) {

ucl_create_err (&parser->err,
"error at %s:%d at column %d: invalid macro",
chunk->fname ? chunk->fname : "memory",
chunk->line,
chunk->column);
parser->state = UCL_STATE_ERROR;
return false;
}
else {
p = chunk->pos;
parser->state = parser->prev_state;
}
}
else {
if (!ucl_test_character (*p, UCL_CHARACTER_WHITESPACE_UNSAFE) &&
*p != '(') {
ucl_chunk_skipc (chunk, p);
}
else {
if (c != NULL && p - c > 0) {

macro_len = (size_t) (p - c);
HASH_FIND (hh, parser->macroes, c, macro_len, macro);
if (macro == NULL) {
ucl_create_err (&parser->err,
"error at %s:%d at column %d: "
"unknown macro: '%.*s', character: '%c'",
chunk->fname ? chunk->fname : "memory",
chunk->line,
chunk->column,
(int) (p - c),
c,
*chunk->pos);
parser->state = UCL_STATE_ERROR;
return false;
}

SKIP_SPACES_COMMENTS(parser, chunk, p);
parser->state = UCL_STATE_MACRO;
}
else {

ucl_create_err (&parser->err,
"error at %s:%d at column %d: invalid macro name",
chunk->fname ? chunk->fname : "memory",
chunk->line,
chunk->column);
parser->state = UCL_STATE_ERROR;
return false;
}
}
}
break;
case UCL_STATE_MACRO:
if (*chunk->pos == '(') {
macro_args = ucl_parse_macro_arguments (parser, chunk);
p = chunk->pos;
if (macro_args) {
SKIP_SPACES_COMMENTS(parser, chunk, p);
}
}
else {
macro_args = NULL;
}
if (!ucl_parse_macro_value (parser, chunk, macro,
&macro_start, &macro_len)) {
parser->prev_state = parser->state;
parser->state = UCL_STATE_ERROR;
return false;
}
macro_len = ucl_expand_variable (parser, &macro_escaped,
macro_start, macro_len);
parser->state = parser->prev_state;

if (macro_escaped == NULL && macro != NULL) {
if (macro->is_context) {
ret = macro->h.context_handler (macro_start, macro_len,
macro_args,
parser->top_obj,
macro->ud);
}
else {
ret = macro->h.handler (macro_start, macro_len, macro_args,
macro->ud);
}
}
else if (macro != NULL) {
if (macro->is_context) {
ret = macro->h.context_handler (macro_escaped, macro_len,
macro_args,
parser->top_obj,
macro->ud);
}
else {
ret = macro->h.handler (macro_escaped, macro_len, macro_args,
macro->ud);
}

UCL_FREE (macro_len + 1, macro_escaped);
}
else {
ret = false;
ucl_set_err (parser, UCL_EINTERNAL,
"internal error: parser has macro undefined", &parser->err);
}




chunk = parser->chunks;
p = chunk->pos;

if (macro_args) {
ucl_object_unref (macro_args);
}

if (!ret) {
return false;
}
break;
default:
ucl_set_err (parser, UCL_EINTERNAL,
"internal error: parser is in an unknown state", &parser->err);
parser->state = UCL_STATE_ERROR;
return false;
}
}

if (parser->last_comment) {
if (parser->cur_obj) {
ucl_attach_comment (parser, parser->cur_obj, true);
}
else if (parser->stack && parser->stack->obj) {
ucl_attach_comment (parser, parser->stack->obj, true);
}
else if (parser->top_obj) {
ucl_attach_comment (parser, parser->top_obj, true);
}
else {
ucl_object_unref (parser->last_comment);
}
}

if (parser->stack != NULL && parser->state != UCL_STATE_ERROR) {
struct ucl_stack *st;
bool has_error = false;

LL_FOREACH (parser->stack, st) {
if (st->chunk != parser->chunks) {
break;
}
if (st->e.params.flags & UCL_STACK_HAS_OBRACE) {
if (parser->err == NULL) {
utstring_new (parser->err);
}

utstring_printf (parser->err, "%s:%d unmatched open brace at %d; ",
chunk->fname ? chunk->fname : "memory",
parser->chunks->line,
st->e.params.line);

has_error = true;
}
}

if (has_error) {
parser->err_code = UCL_EUNPAIRED;

return false;
}
}

return true;
}

#define UPRM_SAFE(fn, a, b, c, el) do { if (!fn(a, b, c, a)) goto el; } while (0)




struct ucl_parser*
ucl_parser_new (int flags)
{
struct ucl_parser *parser;

parser = UCL_ALLOC (sizeof (struct ucl_parser));
if (parser == NULL) {
return NULL;
}

memset (parser, 0, sizeof (struct ucl_parser));

UPRM_SAFE(ucl_parser_register_macro, parser, "include", ucl_include_handler, e0);
UPRM_SAFE(ucl_parser_register_macro, parser, "try_include", ucl_try_include_handler, e0);
UPRM_SAFE(ucl_parser_register_macro, parser, "includes", ucl_includes_handler, e0);
UPRM_SAFE(ucl_parser_register_macro, parser, "priority", ucl_priority_handler, e0);
UPRM_SAFE(ucl_parser_register_macro, parser, "load", ucl_load_handler, e0);
UPRM_SAFE(ucl_parser_register_context_macro, parser, "inherit", ucl_inherit_handler, e0);

parser->flags = flags;
parser->includepaths = NULL;

if (flags & UCL_PARSER_SAVE_COMMENTS) {
parser->comments = ucl_object_typed_new (UCL_OBJECT);
}

if (!(flags & UCL_PARSER_NO_FILEVARS)) {

ucl_parser_set_filevars (parser, NULL, false);
}

return parser;
e0:
ucl_parser_free(parser);
return NULL;
}

bool
ucl_parser_set_default_priority (struct ucl_parser *parser, unsigned prio)
{
if (parser == NULL) {
return false;
}

parser->default_priority = prio;

return true;
}

int
ucl_parser_get_default_priority (struct ucl_parser *parser)
{
if (parser == NULL) {
return -1;
}

return parser->default_priority;
}

bool
ucl_parser_register_macro (struct ucl_parser *parser, const char *macro,
ucl_macro_handler handler, void* ud)
{
struct ucl_macro *new;

if (macro == NULL || handler == NULL) {
return false;
}

new = UCL_ALLOC (sizeof (struct ucl_macro));
if (new == NULL) {
return false;
}

memset (new, 0, sizeof (struct ucl_macro));
new->h.handler = handler;
new->name = strdup (macro);
if (new->name == NULL) {
UCL_FREE (sizeof (struct ucl_macro), new);
return false;
}
new->ud = ud;
HASH_ADD_KEYPTR (hh, parser->macroes, new->name, strlen (new->name), new);
return true;
}

bool
ucl_parser_register_context_macro (struct ucl_parser *parser, const char *macro,
ucl_context_macro_handler handler, void* ud)
{
struct ucl_macro *new;

if (macro == NULL || handler == NULL) {
return false;
}

new = UCL_ALLOC (sizeof (struct ucl_macro));
if (new == NULL) {
return false;
}

memset (new, 0, sizeof (struct ucl_macro));
new->h.context_handler = handler;
new->name = strdup (macro);
if (new->name == NULL) {
UCL_FREE (sizeof (struct ucl_macro), new);
return false;
}
new->ud = ud;
new->is_context = true;
HASH_ADD_KEYPTR (hh, parser->macroes, new->name, strlen (new->name), new);
return true;
}

void
ucl_parser_register_variable (struct ucl_parser *parser, const char *var,
const char *value)
{
struct ucl_variable *new = NULL, *cur;

if (var == NULL) {
return;
}


LL_FOREACH (parser->variables, cur) {
if (strcmp (cur->var, var) == 0) {
new = cur;
break;
}
}

if (value == NULL) {

if (new != NULL) {

DL_DELETE (parser->variables, new);
free (new->var);
free (new->value);
UCL_FREE (sizeof (struct ucl_variable), new);
}
else {

return;
}
}
else {
if (new == NULL) {
new = UCL_ALLOC (sizeof (struct ucl_variable));
if (new == NULL) {
return;
}
memset (new, 0, sizeof (struct ucl_variable));
new->var = strdup (var);
new->var_len = strlen (var);
new->value = strdup (value);
new->value_len = strlen (value);

DL_APPEND (parser->variables, new);
}
else {
free (new->value);
new->value = strdup (value);
new->value_len = strlen (value);
}
}
}

void
ucl_parser_set_variables_handler (struct ucl_parser *parser,
ucl_variable_handler handler, void *ud)
{
parser->var_handler = handler;
parser->var_data = ud;
}

bool
ucl_parser_add_chunk_full (struct ucl_parser *parser, const unsigned char *data,
size_t len, unsigned priority, enum ucl_duplicate_strategy strat,
enum ucl_parse_type parse_type)
{
struct ucl_chunk *chunk;
struct ucl_parser_special_handler *special_handler;

if (parser == NULL) {
return false;
}

if (data == NULL && len != 0) {
ucl_create_err (&parser->err, "invalid chunk added");
return false;
}

if (parser->state != UCL_STATE_ERROR) {
chunk = UCL_ALLOC (sizeof (struct ucl_chunk));
if (chunk == NULL) {
ucl_create_err (&parser->err, "cannot allocate chunk structure");
return false;
}

memset (chunk, 0, sizeof (*chunk));


LL_FOREACH (parser->special_handlers, special_handler) {
if ((special_handler->flags & UCL_SPECIAL_HANDLER_PREPROCESS_ALL) ||
(len >= special_handler->magic_len &&
memcmp (data, special_handler->magic, special_handler->magic_len) == 0)) {
unsigned char *ndata = NULL;
size_t nlen = 0;

if (!special_handler->handler (parser, data, len, &ndata, &nlen,
special_handler->user_data)) {
ucl_create_err (&parser->err, "call for external handler failed");
return false;
}

struct ucl_parser_special_handler_chain *nchain;
nchain = UCL_ALLOC (sizeof (*nchain));
nchain->begin = ndata;
nchain->len = nlen;
nchain->special_handler = special_handler;


LL_PREPEND (chunk->special_handlers, nchain);

data = ndata;
len = nlen;
}
}

if (parse_type == UCL_PARSE_AUTO && len > 0) {

if ((*data & 0x80) == 0x80 && (*data >= 0xdc && *data <= 0xdf)) {
parse_type = UCL_PARSE_MSGPACK;
}
else if (*data == '(') {
parse_type = UCL_PARSE_CSEXP;
}
else {
parse_type = UCL_PARSE_UCL;
}
}

chunk->begin = data;
chunk->remain = len;
chunk->pos = chunk->begin;
chunk->end = chunk->begin + len;
chunk->line = 1;
chunk->column = 0;
chunk->priority = priority;
chunk->strategy = strat;
chunk->parse_type = parse_type;

if (parser->cur_file) {
chunk->fname = strdup (parser->cur_file);
}

LL_PREPEND (parser->chunks, chunk);
parser->recursion ++;

if (parser->recursion > UCL_MAX_RECURSION) {
ucl_create_err (&parser->err, "maximum include nesting limit is reached: %d",
parser->recursion);
return false;
}

if (len > 0) {

switch (parse_type) {
default:
case UCL_PARSE_UCL:
return ucl_state_machine (parser);
case UCL_PARSE_MSGPACK:
return ucl_parse_msgpack (parser);
case UCL_PARSE_CSEXP:
return ucl_parse_csexp (parser);
}
}
else {

if (parser->top_obj == NULL) {




parser->top_obj = ucl_object_new_full (UCL_OBJECT, priority);
}

return true;
}
}

ucl_create_err (&parser->err, "a parser is in an invalid state");

return false;
}

bool
ucl_parser_add_chunk_priority (struct ucl_parser *parser,
const unsigned char *data, size_t len, unsigned priority)
{

if (parser == NULL) {
return false;
}

return ucl_parser_add_chunk_full (parser, data, len,
priority, UCL_DUPLICATE_APPEND, UCL_PARSE_UCL);
}

bool
ucl_parser_add_chunk (struct ucl_parser *parser, const unsigned char *data,
size_t len)
{
if (parser == NULL) {
return false;
}

return ucl_parser_add_chunk_full (parser, data, len,
parser->default_priority, UCL_DUPLICATE_APPEND, UCL_PARSE_UCL);
}

bool
ucl_parser_insert_chunk (struct ucl_parser *parser, const unsigned char *data,
size_t len)
{
if (parser == NULL || parser->top_obj == NULL) {
return false;
}

bool res;
struct ucl_chunk *chunk;

int state = parser->state;
parser->state = UCL_STATE_INIT;


if (parser->stack != NULL && parser->stack->next != NULL) {
parser->stack->e.params.level = parser->stack->next->e.params.level;
}

res = ucl_parser_add_chunk_full (parser, data, len, parser->chunks->priority,
parser->chunks->strategy, parser->chunks->parse_type);


chunk = parser->chunks;
if (chunk != NULL) {
parser->chunks = chunk->next;
ucl_chunk_free (chunk);
parser->recursion --;
}

parser->state = state;

return res;
}

bool
ucl_parser_add_string_priority (struct ucl_parser *parser, const char *data,
size_t len, unsigned priority)
{
if (data == NULL) {
ucl_create_err (&parser->err, "invalid string added");
return false;
}
if (len == 0) {
len = strlen (data);
}

return ucl_parser_add_chunk_priority (parser,
(const unsigned char *)data, len, priority);
}

bool
ucl_parser_add_string (struct ucl_parser *parser, const char *data,
size_t len)
{
if (parser == NULL) {
return false;
}

return ucl_parser_add_string_priority (parser,
(const unsigned char *)data, len, parser->default_priority);
}

bool
ucl_set_include_path (struct ucl_parser *parser, ucl_object_t *paths)
{
if (parser == NULL || paths == NULL) {
return false;
}

if (parser->includepaths == NULL) {
parser->includepaths = ucl_object_copy (paths);
}
else {
ucl_object_unref (parser->includepaths);
parser->includepaths = ucl_object_copy (paths);
}

if (parser->includepaths == NULL) {
return false;
}

return true;
}

unsigned char ucl_parser_chunk_peek (struct ucl_parser *parser)
{
if (parser == NULL || parser->chunks == NULL || parser->chunks->pos == NULL || parser->chunks->end == NULL ||
parser->chunks->pos == parser->chunks->end) {
return 0;
}

return( *parser->chunks->pos );
}

bool ucl_parser_chunk_skip (struct ucl_parser *parser)
{
if (parser == NULL || parser->chunks == NULL || parser->chunks->pos == NULL || parser->chunks->end == NULL ||
parser->chunks->pos == parser->chunks->end) {
return false;
}

const unsigned char *p = parser->chunks->pos;
ucl_chunk_skipc( parser->chunks, p );
if( parser->chunks->pos != NULL ) return true;
return false;
}

ucl_object_t*
ucl_parser_get_current_stack_object (struct ucl_parser *parser, unsigned int depth)
{
ucl_object_t *obj;

if (parser == NULL || parser->stack == NULL) {
return NULL;
}

struct ucl_stack *stack = parser->stack;
if(stack == NULL || stack->obj == NULL || ucl_object_type (stack->obj) != UCL_OBJECT)
{
return NULL;
}

for( unsigned int i = 0; i < depth; ++i )
{
stack = stack->next;
if(stack == NULL || stack->obj == NULL || ucl_object_type (stack->obj) != UCL_OBJECT)
{
return NULL;
}
}

obj = ucl_object_ref (stack->obj);
return obj;
}

