
























































#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: vis.c,v 1.74 2017/11/27 16:37:21 christos Exp $");
#endif
#if defined(__FBSDID)
__FBSDID("$FreeBSD$");
#define _DIAGASSERT(x) assert(x)
#endif

#include "namespace.h"
#include <sys/types.h>
#include <sys/param.h>

#include <assert.h>
#include <vis.h>
#include <errno.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

#if defined(__weak_alias)
__weak_alias(strvisx,_strvisx)
#endif

#if !HAVE_VIS || !HAVE_SVIS
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>









static wchar_t *do_svis(wchar_t *, wint_t, int, wint_t, const wchar_t *);

#undef BELL
#define BELL L'\a'

#if defined(LC_C_LOCALE)
#define iscgraph(c) isgraph_l(c, LC_C_LOCALE)
#else

#define iscgraph(c) isgraph(c)
#if defined(notyet)
#include <locale.h>
static int
iscgraph(int c) {
int rv;
char *ol;

ol = setlocale(LC_CTYPE, "C");
rv = isgraph(c);
if (ol)
setlocale(LC_CTYPE, ol);
return rv;
}
#endif
#endif

#define ISGRAPH(flags, c) (((flags) & VIS_NOLOCALE) ? iscgraph(c) : iswgraph(c))


#define iswoctal(c) (((u_char)(c)) >= L'0' && ((u_char)(c)) <= L'7')
#define iswwhite(c) (c == L' ' || c == L'\t' || c == L'\n')
#define iswsafe(c) (c == L'\b' || c == BELL || c == L'\r')
#define xtoa(c) L"0123456789abcdef"[c]
#define XTOA(c) L"0123456789ABCDEF"[c]

#define MAXEXTRAS 30

static const wchar_t char_shell[] = L"'`\";&<>()|{}]\\$!^~";
static const wchar_t char_glob[] = L"*?[#";

#if !HAVE_NBTOOL_CONFIG_H
#if !defined(__NetBSD__)







#if defined(__FreeBSD__)




#if !defined(CTASSERT)
#define CTASSERT(x) _CTASSERT(x, __LINE__)
#define _CTASSERT(x, y) __CTASSERT(x, y)
#define __CTASSERT(x, y) typedef char __assert ##y[(x) ? 1 : -1]
#endif
#endif
CTASSERT(MB_LEN_MAX <= sizeof(uint64_t));
#endif
#endif




static wchar_t *
do_hvis(wchar_t *dst, wint_t c, int flags, wint_t nextc, const wchar_t *extra)
{
if (iswalnum(c)

|| c == L'$' || c == L'-' || c == L'_' || c == L'.' || c == L'+'

|| c == L'!' || c == L'*' || c == L'\'' || c == L'(' || c == L')'
|| c == L',')
dst = do_svis(dst, c, flags, nextc, extra);
else {
*dst++ = L'%';
*dst++ = xtoa(((unsigned int)c >> 4) & 0xf);
*dst++ = xtoa((unsigned int)c & 0xf);
}

return dst;
}





static wchar_t *
do_mvis(wchar_t *dst, wint_t c, int flags, wint_t nextc, const wchar_t *extra)
{
if ((c != L'\n') &&

((iswspace(c) && (nextc == L'\r' || nextc == L'\n')) ||

(!iswspace(c) && (c < 33 || (c > 60 && c < 62) || c > 126)) ||

wcschr(L"#$@[\\]^`{|}~", c) != NULL)) {
*dst++ = L'=';
*dst++ = XTOA(((unsigned int)c >> 4) & 0xf);
*dst++ = XTOA((unsigned int)c & 0xf);
} else
dst = do_svis(dst, c, flags, nextc, extra);
return dst;
}




static wchar_t *
do_mbyte(wchar_t *dst, wint_t c, int flags, wint_t nextc, int iswextra)
{
if (flags & VIS_CSTYLE) {
switch (c) {
case L'\n':
*dst++ = L'\\'; *dst++ = L'n';
return dst;
case L'\r':
*dst++ = L'\\'; *dst++ = L'r';
return dst;
case L'\b':
*dst++ = L'\\'; *dst++ = L'b';
return dst;
case BELL:
*dst++ = L'\\'; *dst++ = L'a';
return dst;
case L'\v':
*dst++ = L'\\'; *dst++ = L'v';
return dst;
case L'\t':
*dst++ = L'\\'; *dst++ = L't';
return dst;
case L'\f':
*dst++ = L'\\'; *dst++ = L'f';
return dst;
case L' ':
*dst++ = L'\\'; *dst++ = L's';
return dst;
case L'\0':
*dst++ = L'\\'; *dst++ = L'0';
if (iswoctal(nextc)) {
*dst++ = L'0';
*dst++ = L'0';
}
return dst;


case L'n':
case L'r':
case L'b':
case L'a':
case L'v':
case L't':
case L'f':
case L's':
case L'0':
case L'M':
case L'^':
case L'$':
break;
default:
if (ISGRAPH(flags, c) && !iswoctal(c)) {
*dst++ = L'\\';
*dst++ = c;
return dst;
}
}
}
if (iswextra || ((c & 0177) == L' ') || (flags & VIS_OCTAL)) {
*dst++ = L'\\';
*dst++ = (u_char)(((u_int32_t)(u_char)c >> 6) & 03) + L'0';
*dst++ = (u_char)(((u_int32_t)(u_char)c >> 3) & 07) + L'0';
*dst++ = (c & 07) + L'0';
} else {
if ((flags & VIS_NOSLASH) == 0)
*dst++ = L'\\';

if (c & 0200) {
c &= 0177;
*dst++ = L'M';
}

if (iswcntrl(c)) {
*dst++ = L'^';
if (c == 0177)
*dst++ = L'?';
else
*dst++ = c + L'@';
} else {
*dst++ = L'-';
*dst++ = c;
}
}

return dst;
}










static wchar_t *
do_svis(wchar_t *dst, wint_t c, int flags, wint_t nextc, const wchar_t *extra)
{
int iswextra, i, shft;
uint64_t bmsk, wmsk;

iswextra = wcschr(extra, c) != NULL;
if (!iswextra && (ISGRAPH(flags, c) || iswwhite(c) ||
((flags & VIS_SAFE) && iswsafe(c)))) {
*dst++ = c;
return dst;
}


wmsk = 0;
for (i = sizeof(wmsk) - 1; i >= 0; i--) {
shft = i * NBBY;
bmsk = (uint64_t)0xffLL << shft;
wmsk |= bmsk;
if ((c & wmsk) || i == 0)
dst = do_mbyte(dst, (wint_t)(
(uint64_t)(c & bmsk) >> shft),
flags, nextc, iswextra);
}

return dst;
}

typedef wchar_t *(*visfun_t)(wchar_t *, wint_t, int, wint_t, const wchar_t *);




static visfun_t
getvisfun(int flags)
{
if (flags & VIS_HTTPSTYLE)
return do_hvis;
if (flags & VIS_MIMESTYLE)
return do_mvis;
return do_svis;
}




static wchar_t *
makeextralist(int flags, const char *src)
{
wchar_t *dst, *d;
size_t len;
const wchar_t *s;
mbstate_t mbstate;

bzero(&mbstate, sizeof(mbstate));
len = strlen(src);
if ((dst = calloc(len + MAXEXTRAS, sizeof(*dst))) == NULL)
return NULL;

if ((flags & VIS_NOLOCALE) || mbsrtowcs(dst, &src, len, &mbstate) == (size_t)-1) {
size_t i;
for (i = 0; i < len; i++)
dst[i] = (wchar_t)(u_char)src[i];
d = dst + len;
} else
d = dst + wcslen(dst);

if (flags & VIS_GLOB)
for (s = char_glob; *s; *d++ = *s++)
continue;

if (flags & VIS_SHELL)
for (s = char_shell; *s; *d++ = *s++)
continue;

if (flags & VIS_SP) *d++ = L' ';
if (flags & VIS_TAB) *d++ = L'\t';
if (flags & VIS_NL) *d++ = L'\n';
if (flags & VIS_DQ) *d++ = L'"';
if ((flags & VIS_NOSLASH) == 0) *d++ = L'\\';
*d = L'\0';

return dst;
}






static int
istrsenvisx(char **mbdstp, size_t *dlen, const char *mbsrc, size_t mblength,
int flags, const char *mbextra, int *cerr_ptr)
{
wchar_t *dst, *src, *pdst, *psrc, *start, *extra;
size_t len, olen;
uint64_t bmsk, wmsk;
wint_t c;
visfun_t f;
int clen = 0, cerr, error = -1, i, shft;
char *mbdst, *mdst;
ssize_t mbslength, maxolen;
mbstate_t mbstate;

_DIAGASSERT(mbdstp != NULL);
_DIAGASSERT(mbsrc != NULL || mblength == 0);
_DIAGASSERT(mbextra != NULL);

mbslength = (ssize_t)mblength;




if (mbslength == 1)
mbslength++;















psrc = pdst = extra = NULL;
mdst = NULL;
if ((psrc = calloc(mbslength + 1, sizeof(*psrc))) == NULL)
return -1;
if ((pdst = calloc((16 * mbslength) + 1, sizeof(*pdst))) == NULL)
goto out;
if (*mbdstp == NULL) {
if ((mdst = calloc((16 * mbslength) + 1, sizeof(*mdst))) == NULL)
goto out;
*mbdstp = mdst;
}

mbdst = *mbdstp;
dst = pdst;
src = psrc;

if (flags & VIS_NOLOCALE) {

cerr = 1;
} else {

cerr = cerr_ptr ? *cerr_ptr : 0;
}







bzero(&mbstate, sizeof(mbstate));
while (mbslength > 0) {

if (!cerr)
clen = mbrtowc(src, mbsrc, MIN(mbslength, MB_LEN_MAX),
&mbstate);
if (cerr || clen < 0) {

*src = (wint_t)(u_char)*mbsrc;
clen = 1;
cerr = 1;
}
if (clen == 0) {




clen = 1;
}

src++;

mbsrc += clen;

mbslength -= clen;
}
len = src - psrc;
src = psrc;






if (mblength < len)
len = mblength;


extra = makeextralist(flags, mbextra);
if (!extra) {
if (dlen && *dlen == 0) {
errno = ENOSPC;
goto out;
}
*mbdst = '\0';
error = 0;
goto out;
}


f = getvisfun(flags);






for (start = dst; len > 0; len--) {
c = *src++;
dst = (*f)(dst, c, flags, len >= 1 ? *src : L'\0', extra);
if (dst == NULL) {
errno = ENOSPC;
goto out;
}
}


*dst = L'\0';







len = wcslen(start);
maxolen = dlen ? *dlen : (wcslen(start) * MB_LEN_MAX + 1);
olen = 0;
bzero(&mbstate, sizeof(mbstate));
for (dst = start; len > 0; len--) {
if (!cerr)
clen = wcrtomb(mbdst, *dst, &mbstate);
if (cerr || clen < 0) {







clen = 0;
wmsk = 0;
for (i = sizeof(wmsk) - 1; i >= 0; i--) {
shft = i * NBBY;
bmsk = (uint64_t)0xffLL << shft;
wmsk |= bmsk;
if ((*dst & wmsk) || i == 0)
mbdst[clen++] = (char)(
(uint64_t)(*dst & bmsk) >>
shft);
}
cerr = 1;
}

if (olen + clen > (size_t)maxolen)
break;

mbdst += clen;

dst++;

olen += clen;
}


*mbdst = '\0';

if (flags & VIS_NOLOCALE) {

if (cerr_ptr)
*cerr_ptr = cerr;
}

free(extra);
free(pdst);
free(psrc);

return (int)olen;
out:
free(extra);
free(pdst);
free(psrc);
free(mdst);
return error;
}

static int
istrsenvisxl(char **mbdstp, size_t *dlen, const char *mbsrc,
int flags, const char *mbextra, int *cerr_ptr)
{
return istrsenvisx(mbdstp, dlen, mbsrc,
mbsrc != NULL ? strlen(mbsrc) : 0, flags, mbextra, cerr_ptr);
}

#endif

#if !HAVE_SVIS







char *
svis(char *mbdst, int c, int flags, int nextc, const char *mbextra)
{
char cc[2];
int ret;

cc[0] = c;
cc[1] = nextc;

ret = istrsenvisx(&mbdst, NULL, cc, 1, flags, mbextra, NULL);
if (ret < 0)
return NULL;
return mbdst + ret;
}

char *
snvis(char *mbdst, size_t dlen, int c, int flags, int nextc, const char *mbextra)
{
char cc[2];
int ret;

cc[0] = c;
cc[1] = nextc;

ret = istrsenvisx(&mbdst, &dlen, cc, 1, flags, mbextra, NULL);
if (ret < 0)
return NULL;
return mbdst + ret;
}

int
strsvis(char *mbdst, const char *mbsrc, int flags, const char *mbextra)
{
return istrsenvisxl(&mbdst, NULL, mbsrc, flags, mbextra, NULL);
}

int
strsnvis(char *mbdst, size_t dlen, const char *mbsrc, int flags, const char *mbextra)
{
return istrsenvisxl(&mbdst, &dlen, mbsrc, flags, mbextra, NULL);
}

int
strsvisx(char *mbdst, const char *mbsrc, size_t len, int flags, const char *mbextra)
{
return istrsenvisx(&mbdst, NULL, mbsrc, len, flags, mbextra, NULL);
}

int
strsnvisx(char *mbdst, size_t dlen, const char *mbsrc, size_t len, int flags,
const char *mbextra)
{
return istrsenvisx(&mbdst, &dlen, mbsrc, len, flags, mbextra, NULL);
}

int
strsenvisx(char *mbdst, size_t dlen, const char *mbsrc, size_t len, int flags,
const char *mbextra, int *cerr_ptr)
{
return istrsenvisx(&mbdst, &dlen, mbsrc, len, flags, mbextra, cerr_ptr);
}
#endif

#if !HAVE_VIS



char *
vis(char *mbdst, int c, int flags, int nextc)
{
char cc[2];
int ret;

cc[0] = c;
cc[1] = nextc;

ret = istrsenvisx(&mbdst, NULL, cc, 1, flags, "", NULL);
if (ret < 0)
return NULL;
return mbdst + ret;
}

char *
nvis(char *mbdst, size_t dlen, int c, int flags, int nextc)
{
char cc[2];
int ret;

cc[0] = c;
cc[1] = nextc;

ret = istrsenvisx(&mbdst, &dlen, cc, 1, flags, "", NULL);
if (ret < 0)
return NULL;
return mbdst + ret;
}









int
strvis(char *mbdst, const char *mbsrc, int flags)
{
return istrsenvisxl(&mbdst, NULL, mbsrc, flags, "", NULL);
}

int
strnvis(char *mbdst, size_t dlen, const char *mbsrc, int flags)
{
return istrsenvisxl(&mbdst, &dlen, mbsrc, flags, "", NULL);
}

int
stravis(char **mbdstp, const char *mbsrc, int flags)
{
*mbdstp = NULL;
return istrsenvisxl(mbdstp, NULL, mbsrc, flags, "", NULL);
}












int
strvisx(char *mbdst, const char *mbsrc, size_t len, int flags)
{
return istrsenvisx(&mbdst, NULL, mbsrc, len, flags, "", NULL);
}

int
strnvisx(char *mbdst, size_t dlen, const char *mbsrc, size_t len, int flags)
{
return istrsenvisx(&mbdst, &dlen, mbsrc, len, flags, "", NULL);
}

int
strenvisx(char *mbdst, size_t dlen, const char *mbsrc, size_t len, int flags,
int *cerr_ptr)
{
return istrsenvisx(&mbdst, &dlen, mbsrc, len, flags, "", cerr_ptr);
}
#endif
