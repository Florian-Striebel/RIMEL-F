












#include "less.h"
#include "charset.h"

extern int utf_mode;




public int
cvt_length(len, ops)
int len;
int ops;
{
if (utf_mode)





len *= 4;
return (len + 1);
}




public int *
cvt_alloc_chpos(len)
int len;
{
int i;
int *chpos = (int *) ecalloc(sizeof(int), len);

for (i = 0; i < len; i++)
chpos[i] = -1;
return (chpos);
}






public void
cvt_text(odst, osrc, chpos, lenp, ops)
char *odst;
char *osrc;
int *chpos;
int *lenp;
int ops;
{
char *dst;
char *edst = odst;
char *src;
char *src_end;
LWCHAR ch;

if (lenp != NULL)
src_end = osrc + *lenp;
else
src_end = osrc + strlen(osrc);

for (src = osrc, dst = odst; src < src_end; )
{
int src_pos = (int) (src - osrc);
int dst_pos = (int) (dst - odst);
struct ansi_state *pansi;
ch = step_char(&src, +1, src_end);
if ((ops & CVT_BS) && ch == '\b' && dst > odst)
{

do {
dst--;
} while (dst > odst && utf_mode &&
!IS_ASCII_OCTET(*dst) && !IS_UTF8_LEAD(*dst));
} else if ((ops & CVT_ANSI) && (pansi = ansi_start(ch)) != NULL)
{

while (src < src_end)
{
if (ansi_step(pansi, ch) != ANSI_MID)
break;
ch = *src++;
}
ansi_done(pansi);
} else
{

if ((ops & CVT_TO_LC) && IS_UPPER(ch))
ch = TO_LOWER(ch);
put_wchar(&dst, ch);

if (chpos != NULL)
chpos[dst_pos] = src_pos;
}
if (dst > edst)
edst = dst;
}
if ((ops & CVT_CRLF) && edst > odst && edst[-1] == '\r')
edst--;
*edst = '\0';
if (lenp != NULL)
*lenp = (int) (edst - odst);

}
