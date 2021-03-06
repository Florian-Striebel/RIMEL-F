






























#include "config.h"
#if !defined(lint) && !defined(SCCSID)
__RCSID("$NetBSD: literal.c,v 1.5 2019/07/23 13:10:11 christos Exp $");
#endif




#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "el.h"

libedit_private void
literal_init(EditLine *el)
{
el_literal_t *l = &el->el_literal;

memset(l, 0, sizeof(*l));
}

libedit_private void
literal_end(EditLine *el)
{
literal_clear(el);
}

libedit_private void
literal_clear(EditLine *el)
{
el_literal_t *l = &el->el_literal;
size_t i;

if (l->l_len == 0)
return;

for (i = 0; i < l->l_idx; i++)
el_free(l->l_buf[i]);
el_free(l->l_buf);
l->l_buf = NULL;
l->l_len = 0;
l->l_idx = 0;
}

libedit_private wint_t
literal_add(EditLine *el, const wchar_t *buf, const wchar_t *end, int *wp)
{
el_literal_t *l = &el->el_literal;
size_t i, len;
ssize_t w, n;
char *b;

w = wcwidth(end[1]);
*wp = (int)w;

if (w <= 0)
return 0;

len = (size_t)(end - buf);
for (w = 0, i = 0; i < len; i++)
w += ct_enc_width(buf[i]);
w += ct_enc_width(end[1]);

b = el_malloc((size_t)(w + 1));
if (b == NULL)
return 0;

for (n = 0, i = 0; i < len; i++)
n += ct_encode_char(b + n, (size_t)(w - n), buf[i]);
n += ct_encode_char(b + n, (size_t)(w - n), end[1]);
b[n] = '\0';







if (l->l_idx == l->l_len) {
char **bp;

l->l_len += 4;
bp = el_realloc(l->l_buf, sizeof(*l->l_buf) * l->l_len);
if (bp == NULL) {
free(b);
l->l_len -= 4;
return 0;
}
l->l_buf = bp;
}
l->l_buf[l->l_idx++] = b;
return EL_LITERAL | (wint_t)(l->l_idx - 1);
}

libedit_private const char *
literal_get(EditLine *el, wint_t idx)
{
el_literal_t *l = &el->el_literal;

assert(idx & EL_LITERAL);
idx &= ~EL_LITERAL;
assert(l->l_idx > (size_t)idx);
return l->l_buf[idx];
}
