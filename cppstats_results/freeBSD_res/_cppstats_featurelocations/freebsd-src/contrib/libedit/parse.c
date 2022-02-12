

































#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)parse.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: parse.c,v 1.42 2019/07/23 10:18:52 christos Exp $");
#endif
#endif














#include <stdlib.h>
#include <string.h>

#include "el.h"
#include "parse.h"

static const struct {
const wchar_t *name;
int (*func)(EditLine *, int, const wchar_t **);
} cmds[] = {
{ L"bind", map_bind },
{ L"echotc", terminal_echotc },
{ L"edit", el_editmode },
{ L"history", hist_command },
{ L"telltc", terminal_telltc },
{ L"settc", terminal_settc },
{ L"setty", tty_stty },
{ NULL, NULL }
};





libedit_private int
parse_line(EditLine *el, const wchar_t *line)
{
const wchar_t **argv;
int argc;
TokenizerW *tok;

tok = tok_winit(NULL);
tok_wstr(tok, line, &argc, &argv);
argc = el_wparse(el, argc, argv);
tok_wend(tok);
return argc;
}





int
el_wparse(EditLine *el, int argc, const wchar_t *argv[])
{
const wchar_t *ptr;
int i;

if (argc < 1)
return -1;
ptr = wcschr(argv[0], L':');
if (ptr != NULL) {
wchar_t *tprog;
size_t l;

if (ptr == argv[0])
return 0;
l = (size_t)(ptr - argv[0]);
tprog = el_calloc(l + 1, sizeof(*tprog));
if (tprog == NULL)
return 0;
(void) wcsncpy(tprog, argv[0], l);
tprog[l] = '\0';
ptr++;
l = (size_t)el_match(el->el_prog, tprog);
el_free(tprog);
if (!l)
return 0;
} else
ptr = argv[0];

for (i = 0; cmds[i].name != NULL; i++)
if (wcscmp(cmds[i].name, ptr) == 0) {
i = (*cmds[i].func) (el, argc, argv);
return -i;
}
return -1;
}






libedit_private int
parse__escape(const wchar_t **ptr)
{
const wchar_t *p;
wint_t c;

p = *ptr;

if (p[1] == 0)
return -1;

if (*p == '\\') {
p++;
switch (*p) {
case 'a':
c = '\007';
break;
case 'b':
c = '\010';
break;
case 't':
c = '\011';
break;
case 'n':
c = '\012';
break;
case 'v':
c = '\013';
break;
case 'f':
c = '\014';
break;
case 'r':
c = '\015';
break;
case 'e':
c = '\033';
break;
case 'U':
{
int i;
const wchar_t hex[] = L"0123456789ABCDEF";
const wchar_t *h;
++p;
if (*p++ != '+')
return -1;
c = 0;
for (i = 0; i < 5; ++i) {
h = wcschr(hex, *p++);
if (!h && i < 4)
return -1;
else if (h)
c = (c << 4) | ((int)(h - hex));
else
--p;
}
if (c > 0x10FFFF)
return -1;
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
{
int cnt, ch;

for (cnt = 0, c = 0; cnt < 3; cnt++) {
ch = *p++;
if (ch < '0' || ch > '7') {
p--;
break;
}
c = (c << 3) | (ch - '0');
}
if ((c & (wint_t)0xffffff00) != (wint_t)0)
return -1;
--p;
break;
}
default:
c = *p;
break;
}
} else if (*p == '^') {
p++;
c = (*p == '?') ? '\177' : (*p & 0237);
} else
c = *p;
*ptr = ++p;
return c;
}




libedit_private wchar_t *
parse__string(wchar_t *out, const wchar_t *in)
{
wchar_t *rv = out;
int n;

for (;;)
switch (*in) {
case '\0':
*out = '\0';
return rv;

case '\\':
case '^':
if ((n = parse__escape(&in)) == -1)
return NULL;
*out++ = (wchar_t)n;
break;

case 'M':
if (in[1] == '-' && in[2] != '\0') {
*out++ = '\033';
in += 2;
break;
}


default:
*out++ = *in++;
break;
}
}






libedit_private int
parse_cmd(EditLine *el, const wchar_t *cmd)
{
el_bindings_t *b = el->el_map.help;
size_t i;

for (i = 0; i < el->el_map.nfunc; i++)
if (wcscmp(b[i].name, cmd) == 0)
return b[i].func;
return -1;
}
