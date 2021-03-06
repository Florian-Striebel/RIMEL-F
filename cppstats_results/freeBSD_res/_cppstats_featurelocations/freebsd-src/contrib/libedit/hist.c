

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)hist.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: hist.c,v 1.34 2019/07/23 10:19:35 christos Exp $");
#endif
#endif




#include <stdlib.h>
#include <string.h>
#include <vis.h>

#include "el.h"




libedit_private int
hist_init(EditLine *el)
{

el->el_history.fun = NULL;
el->el_history.ref = NULL;
el->el_history.buf = el_calloc(EL_BUFSIZ, sizeof(*el->el_history.buf));
if (el->el_history.buf == NULL)
return -1;
el->el_history.sz = EL_BUFSIZ;
el->el_history.last = el->el_history.buf;
return 0;
}





libedit_private void
hist_end(EditLine *el)
{

el_free(el->el_history.buf);
el->el_history.buf = NULL;
}





libedit_private int
hist_set(EditLine *el, hist_fun_t fun, void *ptr)
{

el->el_history.ref = ptr;
el->el_history.fun = fun;
return 0;
}






libedit_private el_action_t
hist_get(EditLine *el)
{
const wchar_t *hp;
int h;
size_t blen, hlen;

if (el->el_history.eventno == 0) {
(void) wcsncpy(el->el_line.buffer, el->el_history.buf,
el->el_history.sz);
el->el_line.lastchar = el->el_line.buffer +
(el->el_history.last - el->el_history.buf);

#if defined(KSHVI)
if (el->el_map.type == MAP_VI)
el->el_line.cursor = el->el_line.buffer;
else
#endif
el->el_line.cursor = el->el_line.lastchar;

return CC_REFRESH;
}
if (el->el_history.ref == NULL)
return CC_ERROR;

hp = HIST_FIRST(el);

if (hp == NULL)
return CC_ERROR;

for (h = 1; h < el->el_history.eventno; h++)
if ((hp = HIST_NEXT(el)) == NULL)
goto out;

hlen = wcslen(hp) + 1;
blen = (size_t)(el->el_line.limit - el->el_line.buffer);
if (hlen > blen && !ch_enlargebufs(el, hlen))
goto out;

memcpy(el->el_line.buffer, hp, hlen * sizeof(*hp));
el->el_line.lastchar = el->el_line.buffer + hlen - 1;

if (el->el_line.lastchar > el->el_line.buffer
&& el->el_line.lastchar[-1] == '\n')
el->el_line.lastchar--;
if (el->el_line.lastchar > el->el_line.buffer
&& el->el_line.lastchar[-1] == ' ')
el->el_line.lastchar--;
#if defined(KSHVI)
if (el->el_map.type == MAP_VI)
el->el_line.cursor = el->el_line.buffer;
else
#endif
el->el_line.cursor = el->el_line.lastchar;

return CC_REFRESH;
out:
el->el_history.eventno = h;
return CC_ERROR;

}





libedit_private int
hist_command(EditLine *el, int argc, const wchar_t **argv)
{
const wchar_t *str;
int num;
HistEventW ev;

if (el->el_history.ref == NULL)
return -1;

if (argc == 1 || wcscmp(argv[1], L"list") == 0) {
size_t maxlen = 0;
char *buf = NULL;
int hno = 1;


for (str = HIST_LAST(el); str != NULL; str = HIST_PREV(el)) {
char *ptr =
ct_encode_string(str, &el->el_scratch);
size_t len = strlen(ptr);
if (len > 0 && ptr[len - 1] == '\n')
ptr[--len] = '\0';
len = len * 4 + 1;
if (len >= maxlen) {
maxlen = len + 1024;
char *nbuf = el_realloc(buf, maxlen);
if (nbuf == NULL) {
el_free(buf);
return -1;
}
buf = nbuf;
}
strvis(buf, ptr, VIS_NL);
(void) fprintf(el->el_outfile, "%d\t%s\n",
hno++, buf);
}
el_free(buf);
return 0;
}

if (argc != 3)
return -1;

num = (int)wcstol(argv[2], NULL, 0);

if (wcscmp(argv[1], L"size") == 0)
return history_w(el->el_history.ref, &ev, H_SETSIZE, num);

if (wcscmp(argv[1], L"unique") == 0)
return history_w(el->el_history.ref, &ev, H_SETUNIQUE, num);

return -1;
}





libedit_private int

hist_enlargebuf(EditLine *el, size_t oldsz, size_t newsz)
{
wchar_t *newbuf;

newbuf = el_realloc(el->el_history.buf, newsz * sizeof(*newbuf));
if (!newbuf)
return 0;

(void) memset(&newbuf[oldsz], '\0', (newsz - oldsz) * sizeof(*newbuf));

el->el_history.last = newbuf +
(el->el_history.last - el->el_history.buf);
el->el_history.buf = newbuf;
el->el_history.sz = newsz;

return 1;
}

libedit_private wchar_t *
hist_convert(EditLine *el, int fn, void *arg)
{
HistEventW ev;
if ((*(el)->el_history.fun)((el)->el_history.ref, &ev, fn, arg) == -1)
return NULL;
return ct_decode_string((const char *)(const void *)ev.str,
&el->el_scratch);
}
