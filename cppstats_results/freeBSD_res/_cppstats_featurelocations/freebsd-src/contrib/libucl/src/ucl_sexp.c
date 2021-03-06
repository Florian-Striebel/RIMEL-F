























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <ucl.h>
#include "ucl.h"
#include "ucl_internal.h"
#include "utlist.h"

#define NEXT_STATE do { if (p >= end) { if (state != read_ebrace) { ucl_create_err (&parser->err,"extra data");state = parse_err; } } else { switch (*p) { case '(': state = read_obrace; break; case ')': state = read_ebrace; break; default: len = 0; mult = 1; state = read_length; break; } } } while(0)
























bool
ucl_parse_csexp (struct ucl_parser *parser)
{
const unsigned char *p, *end;
ucl_object_t *obj;
struct ucl_stack *st;
uint64_t len = 0, mult = 1;
enum {
start_parse,
read_obrace,
read_length,
read_value,
read_ebrace,
parse_err
} state = start_parse;

assert (parser != NULL);
assert (parser->chunks != NULL);
assert (parser->chunks->begin != NULL);
assert (parser->chunks->remain != 0);

p = parser->chunks->begin;
end = p + parser->chunks->remain;

while (p < end) {
switch (state) {
case start_parse:

if (*p == '(') {
state = read_obrace;
}
else {
ucl_create_err (&parser->err, "bad starting character for "
"sexp block: %x", (int)*p);
state = parse_err;
}
break;

case read_obrace:
st = calloc (1, sizeof (*st));

if (st == NULL) {
ucl_create_err (&parser->err, "no memory");
state = parse_err;
continue;
}

st->obj = ucl_object_typed_new (UCL_ARRAY);

if (st->obj == NULL) {
ucl_create_err (&parser->err, "no memory");
state = parse_err;
free (st);
continue;
}

if (parser->stack == NULL) {

parser->stack = st;

if (parser->top_obj == NULL) {
parser->top_obj = st->obj;
}
}
else {

LL_PREPEND (parser->stack, st);
}

p ++;
NEXT_STATE;

break;

case read_length:
if (*p == ':') {
if (len == 0) {
ucl_create_err (&parser->err, "zero length element");
state = parse_err;
continue;
}

state = read_value;
}
else if (*p >= '0' && *p <= '9') {
len += (*p - '0') * mult;
mult *= 10;

if (len > UINT32_MAX) {
ucl_create_err (&parser->err, "too big length of an "
"element");
state = parse_err;
continue;
}
}
else {
ucl_create_err (&parser->err, "bad length character: %x",
(int)*p);
state = parse_err;
continue;
}

p ++;
break;

case read_value:
if ((uint64_t)(end - p) > len || len == 0) {
ucl_create_err (&parser->err, "invalid length: %llu, %ld "
"remain", (long long unsigned)len, (long)(end - p));
state = parse_err;
continue;
}
obj = ucl_object_typed_new (UCL_STRING);

obj->value.sv = (const char*)p;
obj->len = len;
obj->flags |= UCL_OBJECT_BINARY;

if (!(parser->flags & UCL_PARSER_ZEROCOPY)) {
ucl_copy_value_trash (obj);
}

ucl_array_append (parser->stack->obj, obj);
p += len;
NEXT_STATE;
break;

case read_ebrace:
if (parser->stack == NULL) {

ucl_create_err (&parser->err, "invalid length: %llu, %ld "
"remain", (long long unsigned)len, (long)(end - p));
state = parse_err;
continue;
}

st = parser->stack;
parser->stack = st->next;

if (parser->stack->obj->type == UCL_ARRAY) {
ucl_array_append (parser->stack->obj, st->obj);
}
else {
ucl_create_err (&parser->err, "bad container object, array "
"expected");
state = parse_err;
continue;
}

free (st);
st = NULL;
p++;
NEXT_STATE;
break;

case parse_err:
default:
return false;
}
}

if (state != read_ebrace) {
ucl_create_err (&parser->err, "invalid finishing state: %d", state);
return false;
}

return true;
}
