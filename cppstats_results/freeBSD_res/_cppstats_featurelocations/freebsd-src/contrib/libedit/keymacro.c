

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)key.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: keymacro.c,v 1.24 2019/07/23 10:18:52 christos Exp $");
#endif
#endif























#include <stdlib.h>
#include <string.h>

#include "el.h"
#include "fcns.h"





struct keymacro_node_t {
wchar_t ch;
int type;
keymacro_value_t val;

struct keymacro_node_t *next;
struct keymacro_node_t *sibling;
};

static int node_trav(EditLine *, keymacro_node_t *, wchar_t *,
keymacro_value_t *);
static int node__try(EditLine *, keymacro_node_t *,
const wchar_t *, keymacro_value_t *, int);
static keymacro_node_t *node__get(wint_t);
static void node__free(keymacro_node_t *);
static void node__put(EditLine *, keymacro_node_t *);
static int node__delete(EditLine *, keymacro_node_t **,
const wchar_t *);
static int node_lookup(EditLine *, const wchar_t *,
keymacro_node_t *, size_t);
static int node_enum(EditLine *, keymacro_node_t *, size_t);

#define KEY_BUFSIZ EL_BUFSIZ





libedit_private int
keymacro_init(EditLine *el)
{

el->el_keymacro.buf = el_calloc(KEY_BUFSIZ,
sizeof(*el->el_keymacro.buf));
if (el->el_keymacro.buf == NULL)
return -1;
el->el_keymacro.map = NULL;
keymacro_reset(el);
return 0;
}




libedit_private void
keymacro_end(EditLine *el)
{

el_free(el->el_keymacro.buf);
el->el_keymacro.buf = NULL;
node__free(el->el_keymacro.map);
}





libedit_private keymacro_value_t *
keymacro_map_cmd(EditLine *el, int cmd)
{

el->el_keymacro.val.cmd = (el_action_t) cmd;
return &el->el_keymacro.val;
}





libedit_private keymacro_value_t *
keymacro_map_str(EditLine *el, wchar_t *str)
{

el->el_keymacro.val.str = str;
return &el->el_keymacro.val;
}







libedit_private void
keymacro_reset(EditLine *el)
{

node__put(el, el->el_keymacro.map);
el->el_keymacro.map = NULL;
return;
}











libedit_private int
keymacro_get(EditLine *el, wchar_t *ch, keymacro_value_t *val)
{

return node_trav(el, el->el_keymacro.map, ch, val);
}








libedit_private void
keymacro_add(EditLine *el, const wchar_t *key, keymacro_value_t *val,
int ntype)
{

if (key[0] == '\0') {
(void) fprintf(el->el_errfile,
"keymacro_add: Null extended-key not allowed.\n");
return;
}
if (ntype == XK_CMD && val->cmd == ED_SEQUENCE_LEAD_IN) {
(void) fprintf(el->el_errfile,
"keymacro_add: sequence-lead-in command not allowed\n");
return;
}
if (el->el_keymacro.map == NULL)

el->el_keymacro.map = node__get(key[0]);



(void) node__try(el, el->el_keymacro.map, key, val, ntype);
return;
}





libedit_private void
keymacro_clear(EditLine *el, el_action_t *map, const wchar_t *in)
{
if (*in > N_KEYS)
return;
if ((map[(unsigned char)*in] == ED_SEQUENCE_LEAD_IN) &&
((map == el->el_map.key &&
el->el_map.alt[(unsigned char)*in] != ED_SEQUENCE_LEAD_IN) ||
(map == el->el_map.alt &&
el->el_map.key[(unsigned char)*in] != ED_SEQUENCE_LEAD_IN)))
(void) keymacro_delete(el, in);
}






libedit_private int
keymacro_delete(EditLine *el, const wchar_t *key)
{

if (key[0] == '\0') {
(void) fprintf(el->el_errfile,
"keymacro_delete: Null extended-key not allowed.\n");
return -1;
}
if (el->el_keymacro.map == NULL)
return 0;

(void) node__delete(el, &el->el_keymacro.map, key);
return 0;
}






libedit_private void
keymacro_print(EditLine *el, const wchar_t *key)
{


if (el->el_keymacro.map == NULL && *key == 0)
return;

el->el_keymacro.buf[0] = '"';
if (node_lookup(el, key, el->el_keymacro.map, (size_t)1) <= -1)

(void) fprintf(el->el_errfile, "Unbound extended key \"%ls"
"\"\n", key);
return;
}






static int
node_trav(EditLine *el, keymacro_node_t *ptr, wchar_t *ch,
keymacro_value_t *val)
{

if (ptr->ch == *ch) {

if (ptr->next) {

if (el_wgetc(el, ch) != 1)
return XK_NOD;
return node_trav(el, ptr->next, ch, val);
} else {
*val = ptr->val;
if (ptr->type != XK_CMD)
*ch = '\0';
return ptr->type;
}
} else {

if (ptr->sibling) {

return node_trav(el, ptr->sibling, ch, val);
} else {

val->str = NULL;
return XK_STR;
}
}
}





static int
node__try(EditLine *el, keymacro_node_t *ptr, const wchar_t *str,
keymacro_value_t *val, int ntype)
{

if (ptr->ch != *str) {
keymacro_node_t *xm;

for (xm = ptr; xm->sibling != NULL; xm = xm->sibling)
if (xm->sibling->ch == *str)
break;
if (xm->sibling == NULL)
xm->sibling = node__get(*str);
ptr = xm->sibling;
}
if (*++str == '\0') {

if (ptr->next != NULL) {
node__put(el, ptr->next);

ptr->next = NULL;
}
switch (ptr->type) {
case XK_CMD:
case XK_NOD:
break;
case XK_STR:
if (ptr->val.str)
el_free(ptr->val.str);
break;
default:
EL_ABORT((el->el_errfile, "Bad XK_ type %d\n",
ptr->type));
break;
}

switch (ptr->type = ntype) {
case XK_CMD:
ptr->val = *val;
break;
case XK_STR:
if ((ptr->val.str = wcsdup(val->str)) == NULL)
return -1;
break;
default:
EL_ABORT((el->el_errfile, "Bad XK_ type %d\n", ntype));
break;
}
} else {

if (ptr->next == NULL)
ptr->next = node__get(*str);
(void) node__try(el, ptr->next, str, val, ntype);
}
return 0;
}





static int
node__delete(EditLine *el, keymacro_node_t **inptr, const wchar_t *str)
{
keymacro_node_t *ptr;
keymacro_node_t *prev_ptr = NULL;

ptr = *inptr;

if (ptr->ch != *str) {
keymacro_node_t *xm;

for (xm = ptr; xm->sibling != NULL; xm = xm->sibling)
if (xm->sibling->ch == *str)
break;
if (xm->sibling == NULL)
return 0;
prev_ptr = xm;
ptr = xm->sibling;
}
if (*++str == '\0') {

if (prev_ptr == NULL)
*inptr = ptr->sibling;
else
prev_ptr->sibling = ptr->sibling;
ptr->sibling = NULL;
node__put(el, ptr);
return 1;
} else if (ptr->next != NULL &&
node__delete(el, &ptr->next, str) == 1) {
if (ptr->next != NULL)
return 0;
if (prev_ptr == NULL)
*inptr = ptr->sibling;
else
prev_ptr->sibling = ptr->sibling;
ptr->sibling = NULL;
node__put(el, ptr);
return 1;
} else {
return 0;
}
}





static void
node__put(EditLine *el, keymacro_node_t *ptr)
{
if (ptr == NULL)
return;

if (ptr->next != NULL) {
node__put(el, ptr->next);
ptr->next = NULL;
}
node__put(el, ptr->sibling);

switch (ptr->type) {
case XK_CMD:
case XK_NOD:
break;
case XK_STR:
if (ptr->val.str != NULL)
el_free(ptr->val.str);
break;
default:
EL_ABORT((el->el_errfile, "Bad XK_ type %d\n", ptr->type));
break;
}
el_free(ptr);
}





static keymacro_node_t *
node__get(wint_t ch)
{
keymacro_node_t *ptr;

ptr = el_malloc(sizeof(*ptr));
if (ptr == NULL)
return NULL;
ptr->ch = ch;
ptr->type = XK_NOD;
ptr->val.str = NULL;
ptr->next = NULL;
ptr->sibling = NULL;
return ptr;
}

static void
node__free(keymacro_node_t *k)
{
if (k == NULL)
return;
node__free(k->sibling);
node__free(k->next);
el_free(k);
}





static int
node_lookup(EditLine *el, const wchar_t *str, keymacro_node_t *ptr,
size_t cnt)
{
ssize_t used;

if (ptr == NULL)
return -1;

if (!str || *str == 0) {

(void) node_enum(el, ptr, cnt);
return 0;
} else {

if (ptr->ch == *str) {

used = ct_visual_char(el->el_keymacro.buf + cnt,
KEY_BUFSIZ - cnt, ptr->ch);
if (used == -1)
return -1;
if (ptr->next != NULL)

return (node_lookup(el, str + 1, ptr->next,
(size_t)used + cnt));
else {

if (str[1] == 0) {
size_t px = cnt + (size_t)used;
el->el_keymacro.buf[px] = '"';
el->el_keymacro.buf[px + 1] = '\0';
keymacro_kprint(el, el->el_keymacro.buf,
&ptr->val, ptr->type);
return 0;
} else
return -1;

}
} else {

if (ptr->sibling)
return (node_lookup(el, str, ptr->sibling,
cnt));
else
return -1;
}
}
}





static int
node_enum(EditLine *el, keymacro_node_t *ptr, size_t cnt)
{
ssize_t used;

if (cnt >= KEY_BUFSIZ - 5) {
el->el_keymacro.buf[++cnt] = '"';
el->el_keymacro.buf[++cnt] = '\0';
(void) fprintf(el->el_errfile,
"Some extended keys too long for internal print buffer");
(void) fprintf(el->el_errfile, " \"%ls...\"\n",
el->el_keymacro.buf);
return 0;
}
if (ptr == NULL) {
#if defined(DEBUG_EDIT)
(void) fprintf(el->el_errfile,
"node_enum: BUG!! Null ptr passed\n!");
#endif
return -1;
}

used = ct_visual_char(el->el_keymacro.buf + cnt, KEY_BUFSIZ - cnt,
ptr->ch);
if (ptr->next == NULL) {

el->el_keymacro.buf[cnt + (size_t)used ] = '"';
el->el_keymacro.buf[cnt + (size_t)used + 1] = '\0';
keymacro_kprint(el, el->el_keymacro.buf, &ptr->val, ptr->type);
} else
(void) node_enum(el, ptr->next, cnt + (size_t)used);


if (ptr->sibling)
(void) node_enum(el, ptr->sibling, cnt);
return 0;
}






libedit_private void
keymacro_kprint(EditLine *el, const wchar_t *key, keymacro_value_t *val,
int ntype)
{
el_bindings_t *fp;
char unparsbuf[EL_BUFSIZ];
static const char fmt[] = "%-15s-> %s\n";

if (val != NULL)
switch (ntype) {
case XK_STR:
(void) keymacro__decode_str(val->str, unparsbuf,
sizeof(unparsbuf),
ntype == XK_STR ? "\"\"" : "[]");
(void) fprintf(el->el_outfile, fmt,
ct_encode_string(key, &el->el_scratch), unparsbuf);
break;
case XK_CMD:
for (fp = el->el_map.help; fp->name; fp++)
if (val->cmd == fp->func) {
wcstombs(unparsbuf, fp->name, sizeof(unparsbuf));
unparsbuf[sizeof(unparsbuf) -1] = '\0';
(void) fprintf(el->el_outfile, fmt,
ct_encode_string(key, &el->el_scratch), unparsbuf);
break;
}
#if defined(DEBUG_KEY)
if (fp->name == NULL)
(void) fprintf(el->el_outfile,
"BUG! Command not found.\n");
#endif

break;
default:
EL_ABORT((el->el_errfile, "Bad XK_ type %d\n", ntype));
break;
}
else
(void) fprintf(el->el_outfile, fmt, ct_encode_string(key,
&el->el_scratch), "no input");
}


#define ADDC(c) if (b < eb) *b++ = c; else b++







libedit_private size_t
keymacro__decode_str(const wchar_t *str, char *buf, size_t len,
const char *sep)
{
char *b = buf, *eb = b + len;
const wchar_t *p;

b = buf;
if (sep[0] != '\0') {
ADDC(sep[0]);
}
if (*str == '\0') {
ADDC('^');
ADDC('@');
goto add_endsep;
}
for (p = str; *p != 0; p++) {
wchar_t dbuf[VISUAL_WIDTH_MAX];
wchar_t *p2 = dbuf;
ssize_t l = ct_visual_char(dbuf, VISUAL_WIDTH_MAX, *p);
while (l-- > 0) {
ssize_t n = ct_encode_char(b, (size_t)(eb - b), *p2++);
if (n == -1)
goto add_endsep;
else
b += n;
}
}
add_endsep:
if (sep[0] != '\0' && sep[1] != '\0') {
ADDC(sep[1]);
}
ADDC('\0');
if ((size_t)(b - buf) >= len)
buf[len - 1] = '\0';
return (size_t)(b - buf);
}
