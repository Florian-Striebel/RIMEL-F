


























#include "config.h"
#if !defined(lint) && !defined(SCCSID)
__RCSID("$NetBSD: eln.c,v 1.36 2021/08/15 10:08:41 christos Exp $");
#endif

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "el.h"

int
el_getc(EditLine *el, char *cp)
{
int num_read;
wchar_t wc = 0;

num_read = el_wgetc(el, &wc);
*cp = '\0';
if (num_read <= 0)
return num_read;
num_read = wctob(wc);
if (num_read == EOF) {
errno = ERANGE;
return -1;
} else {
*cp = (char)num_read;
return 1;
}
}


void
el_push(EditLine *el, const char *str)
{


el_wpush(el, ct_decode_string(str, &el->el_lgcyconv));
}


const char *
el_gets(EditLine *el, int *nread)
{
const wchar_t *tmp;

tmp = el_wgets(el, nread);
if (tmp != NULL) {
int i;
size_t nwread = 0;

for (i = 0; i < *nread; i++)
nwread += ct_enc_width(tmp[i]);
*nread = (int)nwread;
}
return ct_encode_string(tmp, &el->el_lgcyconv);
}


int
el_parse(EditLine *el, int argc, const char *argv[])
{
int ret;
const wchar_t **wargv;

wargv = (void *)ct_decode_argv(argc, argv, &el->el_lgcyconv);
if (!wargv)
return -1;
ret = el_wparse(el, argc, wargv);
el_free(wargv);

return ret;
}


int
el_set(EditLine *el, int op, ...)
{
va_list ap;
int ret;

if (!el)
return -1;
va_start(ap, op);

switch (op) {
case EL_PROMPT:
case EL_RPROMPT: {
el_pfunc_t p = va_arg(ap, el_pfunc_t);
ret = prompt_set(el, p, 0, op, 0);
break;
}

case EL_RESIZE: {
el_zfunc_t p = va_arg(ap, el_zfunc_t);
void *arg = va_arg(ap, void *);
ret = ch_resizefun(el, p, arg);
break;
}

case EL_ALIAS_TEXT: {
el_afunc_t p = va_arg(ap, el_afunc_t);
void *arg = va_arg(ap, void *);
ret = ch_aliasfun(el, p, arg);
break;
}

case EL_PROMPT_ESC:
case EL_RPROMPT_ESC: {
el_pfunc_t p = va_arg(ap, el_pfunc_t);
int c = va_arg(ap, int);

ret = prompt_set(el, p, c, op, 0);
break;
}

case EL_TERMINAL:
ret = el_wset(el, op, va_arg(ap, char *));
break;

case EL_EDITOR:
ret = el_wset(el, op, ct_decode_string(va_arg(ap, char *),
&el->el_lgcyconv));
break;

case EL_SIGNAL:
case EL_EDITMODE:
case EL_SAFEREAD:
case EL_UNBUFFERED:
case EL_PREP_TERM:
ret = el_wset(el, op, va_arg(ap, int));
break;

case EL_BIND:
case EL_TELLTC:
case EL_SETTC:
case EL_ECHOTC:
case EL_SETTY: {
const char *argv[20];
int i;
const wchar_t **wargv;
for (i = 1; i < (int)__arraycount(argv) - 1; ++i)
if ((argv[i] = va_arg(ap, const char *)) == NULL)
break;
argv[0] = argv[i] = NULL;
wargv = (void *)ct_decode_argv(i + 1, argv, &el->el_lgcyconv);
if (!wargv) {
ret = -1;
goto out;
}





switch (op) {
case EL_BIND:
wargv[0] = L"bind";
ret = map_bind(el, i, wargv);
break;
case EL_TELLTC:
wargv[0] = L"telltc";
ret = terminal_telltc(el, i, wargv);
break;
case EL_SETTC:
wargv[0] = L"settc";
ret = terminal_settc(el, i, wargv);
break;
case EL_ECHOTC:
wargv[0] = L"echotc";
ret = terminal_echotc(el, i, wargv);
break;
case EL_SETTY:
wargv[0] = L"setty";
ret = tty_stty(el, i, wargv);
break;
default:
ret = -1;
}
el_free(wargv);
break;
}


case EL_ADDFN: {
const char *args[2];
el_func_t func;
wchar_t **wargv;

args[0] = va_arg(ap, const char *);
args[1] = va_arg(ap, const char *);
func = va_arg(ap, el_func_t);

wargv = ct_decode_argv(2, args, &el->el_lgcyconv);
if (!wargv) {
ret = -1;
goto out;
}

ret = map_addfunc(el, wcsdup(wargv[0]), wcsdup(wargv[1]),
func);
el_free(wargv);
break;
}
case EL_HIST: {
hist_fun_t fun = va_arg(ap, hist_fun_t);
void *ptr = va_arg(ap, void *);
ret = hist_set(el, fun, ptr);
el->el_flags |= NARROW_HISTORY;
break;
}

case EL_GETCFN:
ret = el_wset(el, op, va_arg(ap, el_rfunc_t));
break;

case EL_CLIENTDATA:
ret = el_wset(el, op, va_arg(ap, void *));
break;

case EL_SETFP: {
int what = va_arg(ap, int);
FILE *fp = va_arg(ap, FILE *);
ret = el_wset(el, op, what, fp);
break;
}

case EL_REFRESH:
re_clear_display(el);
re_refresh(el);
terminal__flush(el);
ret = 0;
break;

default:
ret = -1;
break;
}

out:
va_end(ap);
return ret;
}


int
el_get(EditLine *el, int op, ...)
{
va_list ap;
int ret;

if (!el)
return -1;

va_start(ap, op);

switch (op) {
case EL_PROMPT:
case EL_RPROMPT: {
el_pfunc_t *p = va_arg(ap, el_pfunc_t *);
ret = prompt_get(el, p, 0, op);
break;
}

case EL_PROMPT_ESC:
case EL_RPROMPT_ESC: {
el_pfunc_t *p = va_arg(ap, el_pfunc_t *);
char *c = va_arg(ap, char *);
wchar_t wc = 0;
ret = prompt_get(el, p, &wc, op);
*c = (char)wc;
break;
}

case EL_EDITOR: {
const char **p = va_arg(ap, const char **);
const wchar_t *pw;
ret = el_wget(el, op, &pw);
*p = ct_encode_string(pw, &el->el_lgcyconv);
if (!el->el_lgcyconv.csize)
ret = -1;
break;
}

case EL_TERMINAL:
ret = el_wget(el, op, va_arg(ap, const char **));
break;

case EL_SIGNAL:
case EL_EDITMODE:
case EL_SAFEREAD:
case EL_UNBUFFERED:
case EL_PREP_TERM:
ret = el_wget(el, op, va_arg(ap, int *));
break;

case EL_GETTC: {
char *argv[3];
static char gettc[] = "gettc";
argv[0] = gettc;
argv[1] = va_arg(ap, char *);
argv[2] = va_arg(ap, void *);
ret = terminal_gettc(el, 3, argv);
break;
}

case EL_GETCFN:
ret = el_wget(el, op, va_arg(ap, el_rfunc_t *));
break;

case EL_CLIENTDATA:
ret = el_wget(el, op, va_arg(ap, void **));
break;

case EL_GETFP: {
int what = va_arg(ap, int);
FILE **fpp = va_arg(ap, FILE **);
ret = el_wget(el, op, what, fpp);
break;
}

default:
ret = -1;
break;
}

va_end(ap);
return ret;
}


const LineInfo *
el_line(EditLine *el)
{
const LineInfoW *winfo = el_wline(el);
LineInfo *info = &el->el_lgcylinfo;
size_t offset;
const wchar_t *p;

info->buffer = ct_encode_string(winfo->buffer, &el->el_lgcyconv);

offset = 0;
for (p = winfo->buffer; p < winfo->cursor; p++)
offset += ct_enc_width(*p);
info->cursor = info->buffer + offset;

offset = 0;
for (p = winfo->buffer; p < winfo->lastchar; p++)
offset += ct_enc_width(*p);
info->lastchar = info->buffer + offset;

return info;
}


int
el_insertstr(EditLine *el, const char *str)
{
return el_winsertstr(el, ct_decode_string(str, &el->el_lgcyconv));
}
