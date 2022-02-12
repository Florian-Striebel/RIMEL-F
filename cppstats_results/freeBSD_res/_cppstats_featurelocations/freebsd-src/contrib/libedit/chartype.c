






























#include "config.h"
#if !defined(lint) && !defined(SCCSID)
__RCSID("$NetBSD: chartype.c,v 1.35 2019/07/23 10:18:52 christos Exp $");
#endif

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "el.h"

#define CT_BUFSIZ ((size_t)1024)

static int ct_conv_cbuff_resize(ct_buffer_t *, size_t);
static int ct_conv_wbuff_resize(ct_buffer_t *, size_t);

static int
ct_conv_cbuff_resize(ct_buffer_t *conv, size_t csize)
{
void *p;

if (csize <= conv->csize)
return 0;

conv->csize = csize;

p = el_realloc(conv->cbuff, conv->csize * sizeof(*conv->cbuff));
if (p == NULL) {
conv->csize = 0;
el_free(conv->cbuff);
conv->cbuff = NULL;
return -1;
}
conv->cbuff = p;
return 0;
}

static int
ct_conv_wbuff_resize(ct_buffer_t *conv, size_t wsize)
{
void *p;

if (wsize <= conv->wsize)
return 0;

conv->wsize = wsize;

p = el_realloc(conv->wbuff, conv->wsize * sizeof(*conv->wbuff));
if (p == NULL) {
conv->wsize = 0;
el_free(conv->wbuff);
conv->wbuff = NULL;
return -1;
}
conv->wbuff = p;
return 0;
}


char *
ct_encode_string(const wchar_t *s, ct_buffer_t *conv)
{
char *dst;
ssize_t used;

if (!s)
return NULL;

dst = conv->cbuff;
for (;;) {
used = (ssize_t)(dst - conv->cbuff);
if ((conv->csize - (size_t)used) < 5) {
if (ct_conv_cbuff_resize(conv,
conv->csize + CT_BUFSIZ) == -1)
return NULL;
dst = conv->cbuff + used;
}
if (!*s)
break;
used = ct_encode_char(dst, (size_t)5, *s);
if (used == -1)
abort();
++s;
dst += used;
}
*dst = '\0';
return conv->cbuff;
}

wchar_t *
ct_decode_string(const char *s, ct_buffer_t *conv)
{
size_t len;

if (!s)
return NULL;

len = mbstowcs(NULL, s, (size_t)0);
if (len == (size_t)-1)
return NULL;

if (conv->wsize < ++len)
if (ct_conv_wbuff_resize(conv, len + CT_BUFSIZ) == -1)
return NULL;

mbstowcs(conv->wbuff, s, conv->wsize);
return conv->wbuff;
}


libedit_private wchar_t **
ct_decode_argv(int argc, const char *argv[], ct_buffer_t *conv)
{
size_t bufspace;
int i;
wchar_t *p;
wchar_t **wargv;
ssize_t bytes;



for (i = 0, bufspace = 0; i < argc; ++i)
bufspace += argv[i] ? strlen(argv[i]) + 1 : 0;
if (conv->wsize < ++bufspace)
if (ct_conv_wbuff_resize(conv, bufspace + CT_BUFSIZ) == -1)
return NULL;

wargv = el_calloc((size_t)(argc + 1), sizeof(*wargv));

for (i = 0, p = conv->wbuff; i < argc; ++i) {
if (!argv[i]) {
wargv[i] = NULL;
continue;
} else {
wargv[i] = p;
bytes = (ssize_t)mbstowcs(p, argv[i], bufspace);
}
if (bytes == -1) {
el_free(wargv);
return NULL;
} else
bytes++;
bufspace -= (size_t)bytes;
p += bytes;
}
wargv[i] = NULL;

return wargv;
}


libedit_private size_t
ct_enc_width(wchar_t c)
{
mbstate_t mbs;
char buf[MB_LEN_MAX];
size_t size;
memset(&mbs, 0, sizeof(mbs));

if ((size = wcrtomb(buf, c, &mbs)) == (size_t)-1)
return 0;
return size;
}

libedit_private ssize_t
ct_encode_char(char *dst, size_t len, wchar_t c)
{
ssize_t l = 0;
if (len < ct_enc_width(c))
return -1;
l = wctomb(dst, c);

if (l < 0) {
wctomb(NULL, L'\0');
l = 0;
}
return l;
}

libedit_private const wchar_t *
ct_visual_string(const wchar_t *s, ct_buffer_t *conv)
{
wchar_t *dst;
ssize_t used;

if (!s)
return NULL;

if (ct_conv_wbuff_resize(conv, CT_BUFSIZ) == -1)
return NULL;

used = 0;
dst = conv->wbuff;
while (*s) {
used = ct_visual_char(dst,
conv->wsize - (size_t)(dst - conv->wbuff), *s);
if (used != -1) {
++s;
dst += used;
continue;
}


used = dst - conv->wbuff;
if (ct_conv_wbuff_resize(conv, conv->wsize + CT_BUFSIZ) == -1)
return NULL;
dst = conv->wbuff + used;
}

if (dst >= (conv->wbuff + conv->wsize)) {
used = dst - conv->wbuff;
if (ct_conv_wbuff_resize(conv, conv->wsize + CT_BUFSIZ) == -1)
return NULL;
dst = conv->wbuff + used;
}

*dst = L'\0';
return conv->wbuff;
}



libedit_private int
ct_visual_width(wchar_t c)
{
int t = ct_chr_class(c);
switch (t) {
case CHTYPE_ASCIICTL:
return 2;
case CHTYPE_TAB:
return 1;
case CHTYPE_NL:
return 0;
case CHTYPE_PRINT:
return wcwidth(c);
case CHTYPE_NONPRINT:
if (c > 0xffff)
return 8;
else
return 7;
default:
return 0;
}
}


libedit_private ssize_t
ct_visual_char(wchar_t *dst, size_t len, wchar_t c)
{
int t = ct_chr_class(c);
switch (t) {
case CHTYPE_TAB:
case CHTYPE_NL:
case CHTYPE_ASCIICTL:
if (len < 2)
return -1;
*dst++ = '^';
if (c == '\177')
*dst = '?';
else
*dst = c | 0100;
return 2;
case CHTYPE_PRINT:
if (len < 1)
return -1;
*dst = c;
return 1;
case CHTYPE_NONPRINT:


if ((ssize_t)len < ct_visual_width(c))
return -1;
*dst++ = '\\';
*dst++ = 'U';
*dst++ = '+';
#define tohexdigit(v) "0123456789ABCDEF"[v]
if (c > 0xffff)
*dst++ = tohexdigit(((unsigned int) c >> 16) & 0xf);
*dst++ = tohexdigit(((unsigned int) c >> 12) & 0xf);
*dst++ = tohexdigit(((unsigned int) c >> 8) & 0xf);
*dst++ = tohexdigit(((unsigned int) c >> 4) & 0xf);
*dst = tohexdigit(((unsigned int) c ) & 0xf);
return c > 0xffff ? 8 : 7;


default:
return 0;
}
}




libedit_private int
ct_chr_class(wchar_t c)
{
if (c == '\t')
return CHTYPE_TAB;
else if (c == '\n')
return CHTYPE_NL;
else if (c < 0x100 && iswcntrl(c))
return CHTYPE_ASCIICTL;
else if (iswprint(c))
return CHTYPE_PRINT;
else
return CHTYPE_NONPRINT;
}
