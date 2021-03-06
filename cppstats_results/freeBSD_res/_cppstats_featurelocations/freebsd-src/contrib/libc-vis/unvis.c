






























#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)unvis.c 8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: unvis.c,v 1.44 2014/09/26 15:43:36 roy Exp $");
#endif
#endif
__FBSDID("$FreeBSD$");

#include "namespace.h"
#include <sys/types.h>

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <vis.h>

#define _DIAGASSERT(x) assert(x)





#define __arraycount(__x) (sizeof(__x) / sizeof(__x[0]))

#if defined(__weak_alias)
__weak_alias(strnunvisx,_strnunvisx)
#endif

#if !HAVE_VIS



#define S_GROUND 0
#define S_START 1
#define S_META 2
#define S_META1 3
#define S_CTRL 4
#define S_OCTAL2 5
#define S_OCTAL3 6
#define S_HEX 7
#define S_HEX1 8
#define S_HEX2 9
#define S_MIME1 10
#define S_MIME2 11
#define S_EATCRNL 12
#define S_AMP 13
#define S_NUMBER 14
#define S_STRING 15

#define isoctal(c) (((u_char)(c)) >= '0' && ((u_char)(c)) <= '7')
#define xtod(c) (isdigit(c) ? (c - '0') : ((tolower(c) - 'a') + 10))
#define XTOD(c) (isdigit(c) ? (c - '0') : ((c - 'A') + 10))




static const struct nv {
char name[7];
uint8_t value;
} nv[] = {
{ "AElig", 198 },
{ "Aacute", 193 },
{ "Acirc", 194 },
{ "Agrave", 192 },
{ "Aring", 197 },
{ "Atilde", 195 },
{ "Auml", 196 },
{ "Ccedil", 199 },
{ "ETH", 208 },
{ "Eacute", 201 },
{ "Ecirc", 202 },
{ "Egrave", 200 },
{ "Euml", 203 },
{ "Iacute", 205 },
{ "Icirc", 206 },
{ "Igrave", 204 },
{ "Iuml", 207 },
{ "Ntilde", 209 },
{ "Oacute", 211 },
{ "Ocirc", 212 },
{ "Ograve", 210 },
{ "Oslash", 216 },
{ "Otilde", 213 },
{ "Ouml", 214 },
{ "THORN", 222 },
{ "Uacute", 218 },
{ "Ucirc", 219 },
{ "Ugrave", 217 },
{ "Uuml", 220 },
{ "Yacute", 221 },
{ "aacute", 225 },
{ "acirc", 226 },
{ "acute", 180 },
{ "aelig", 230 },
{ "agrave", 224 },
{ "amp", 38 },
{ "aring", 229 },
{ "atilde", 227 },
{ "auml", 228 },
{ "brvbar", 166 },
{ "ccedil", 231 },
{ "cedil", 184 },
{ "cent", 162 },
{ "copy", 169 },
{ "curren", 164 },
{ "deg", 176 },
{ "divide", 247 },
{ "eacute", 233 },
{ "ecirc", 234 },
{ "egrave", 232 },
{ "eth", 240 },
{ "euml", 235 },
{ "frac12", 189 },
{ "frac14", 188 },
{ "frac34", 190 },
{ "gt", 62 },
{ "iacute", 237 },
{ "icirc", 238 },
{ "iexcl", 161 },
{ "igrave", 236 },
{ "iquest", 191 },
{ "iuml", 239 },
{ "laquo", 171 },
{ "lt", 60 },
{ "macr", 175 },
{ "micro", 181 },
{ "middot", 183 },
{ "nbsp", 160 },
{ "not", 172 },
{ "ntilde", 241 },
{ "oacute", 243 },
{ "ocirc", 244 },
{ "ograve", 242 },
{ "ordf", 170 },
{ "ordm", 186 },
{ "oslash", 248 },
{ "otilde", 245 },
{ "ouml", 246 },
{ "para", 182 },
{ "plusmn", 177 },
{ "pound", 163 },
{ "quot", 34 },
{ "raquo", 187 },
{ "reg", 174 },
{ "sect", 167 },
{ "shy", 173 },
{ "sup1", 185 },
{ "sup2", 178 },
{ "sup3", 179 },
{ "szlig", 223 },
{ "thorn", 254 },
{ "times", 215 },
{ "uacute", 250 },
{ "ucirc", 251 },
{ "ugrave", 249 },
{ "uml", 168 },
{ "uuml", 252 },
{ "yacute", 253 },
{ "yen", 165 },
{ "yuml", 255 },
};




int
unvis(char *cp, int c, int *astate, int flag)
{
unsigned char uc = (unsigned char)c;
unsigned char st, ia, is, lc;





#define GS(a) ((a) & 0xff)
#define SS(a, b) (((uint32_t)(a) << 24) | (b))
#define GI(a) ((uint32_t)(a) >> 24)

_DIAGASSERT(cp != NULL);
_DIAGASSERT(astate != NULL);
st = GS(*astate);

if (flag & UNVIS_END) {
switch (st) {
case S_OCTAL2:
case S_OCTAL3:
case S_HEX2:
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case S_GROUND:
return UNVIS_NOCHAR;
default:
return UNVIS_SYNBAD;
}
}

switch (st) {

case S_GROUND:
*cp = 0;
if ((flag & VIS_NOESCAPE) == 0 && c == '\\') {
*astate = SS(0, S_START);
return UNVIS_NOCHAR;
}
if ((flag & VIS_HTTP1808) && c == '%') {
*astate = SS(0, S_HEX1);
return UNVIS_NOCHAR;
}
if ((flag & VIS_HTTP1866) && c == '&') {
*astate = SS(0, S_AMP);
return UNVIS_NOCHAR;
}
if ((flag & VIS_MIMESTYLE) && c == '=') {
*astate = SS(0, S_MIME1);
return UNVIS_NOCHAR;
}
*cp = c;
return UNVIS_VALID;

case S_START:
switch(c) {
case '\\':
*cp = c;
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case '0': case '1': case '2': case '3':
case '4': case '5': case '6': case '7':
*cp = (c - '0');
*astate = SS(0, S_OCTAL2);
return UNVIS_NOCHAR;
case 'M':
*cp = (char)0200;
*astate = SS(0, S_META);
return UNVIS_NOCHAR;
case '^':
*astate = SS(0, S_CTRL);
return UNVIS_NOCHAR;
case 'n':
*cp = '\n';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 'r':
*cp = '\r';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 'b':
*cp = '\b';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 'a':
*cp = '\007';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 'v':
*cp = '\v';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 't':
*cp = '\t';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 'f':
*cp = '\f';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 's':
*cp = ' ';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 'E':
*cp = '\033';
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
case 'x':
*astate = SS(0, S_HEX);
return UNVIS_NOCHAR;
case '\n':



*astate = SS(0, S_GROUND);
return UNVIS_NOCHAR;
case '$':



*astate = SS(0, S_GROUND);
return UNVIS_NOCHAR;
default:
if (isgraph(c)) {
*cp = c;
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
}
}
goto bad;

case S_META:
if (c == '-')
*astate = SS(0, S_META1);
else if (c == '^')
*astate = SS(0, S_CTRL);
else
goto bad;
return UNVIS_NOCHAR;

case S_META1:
*astate = SS(0, S_GROUND);
*cp |= c;
return UNVIS_VALID;

case S_CTRL:
if (c == '?')
*cp |= 0177;
else
*cp |= c & 037;
*astate = SS(0, S_GROUND);
return UNVIS_VALID;

case S_OCTAL2:
if (isoctal(uc)) {



*cp = (*cp << 3) + (c - '0');
*astate = SS(0, S_OCTAL3);
return UNVIS_NOCHAR;
}



*astate = SS(0, S_GROUND);
return UNVIS_VALIDPUSH;

case S_OCTAL3:
*astate = SS(0, S_GROUND);
if (isoctal(uc)) {
*cp = (*cp << 3) + (c - '0');
return UNVIS_VALID;
}



return UNVIS_VALIDPUSH;

case S_HEX:
if (!isxdigit(uc))
goto bad;

case S_HEX1:
if (isxdigit(uc)) {
*cp = xtod(uc);
*astate = SS(0, S_HEX2);
return UNVIS_NOCHAR;
}



*astate = SS(0, S_GROUND);
return UNVIS_VALIDPUSH;

case S_HEX2:
*astate = S_GROUND;
if (isxdigit(uc)) {
*cp = xtod(uc) | (*cp << 4);
return UNVIS_VALID;
}
return UNVIS_VALIDPUSH;

case S_MIME1:
if (uc == '\n' || uc == '\r') {
*astate = SS(0, S_EATCRNL);
return UNVIS_NOCHAR;
}
if (isxdigit(uc) && (isdigit(uc) || isupper(uc))) {
*cp = XTOD(uc);
*astate = SS(0, S_MIME2);
return UNVIS_NOCHAR;
}
goto bad;

case S_MIME2:
if (isxdigit(uc) && (isdigit(uc) || isupper(uc))) {
*astate = SS(0, S_GROUND);
*cp = XTOD(uc) | (*cp << 4);
return UNVIS_VALID;
}
goto bad;

case S_EATCRNL:
switch (uc) {
case '\r':
case '\n':
return UNVIS_NOCHAR;
case '=':
*astate = SS(0, S_MIME1);
return UNVIS_NOCHAR;
default:
*cp = uc;
*astate = SS(0, S_GROUND);
return UNVIS_VALID;
}

case S_AMP:
*cp = 0;
if (uc == '#') {
*astate = SS(0, S_NUMBER);
return UNVIS_NOCHAR;
}
*astate = SS(0, S_STRING);


case S_STRING:
ia = *cp;
is = GI(*astate);
lc = is == 0 ? 0 : nv[ia].name[is - 1];

if (uc == ';')
uc = '\0';

for (; ia < __arraycount(nv); ia++) {
if (is != 0 && nv[ia].name[is - 1] != lc)
goto bad;
if (nv[ia].name[is] == uc)
break;
}

if (ia == __arraycount(nv))
goto bad;

if (uc != 0) {
*cp = ia;
*astate = SS(is + 1, S_STRING);
return UNVIS_NOCHAR;
}

*cp = nv[ia].value;
*astate = SS(0, S_GROUND);
return UNVIS_VALID;

case S_NUMBER:
if (uc == ';')
return UNVIS_VALID;
if (!isdigit(uc))
goto bad;
*cp += (*cp * 10) + uc - '0';
return UNVIS_NOCHAR;

default:
bad:



*astate = SS(0, S_GROUND);
return UNVIS_SYNBAD;
}
}








int
strnunvisx(char *dst, size_t dlen, const char *src, int flag)
{
char c;
char t = '\0', *start = dst;
int state = 0;

_DIAGASSERT(src != NULL);
_DIAGASSERT(dst != NULL);
#define CHECKSPACE() do { if (dlen-- == 0) { errno = ENOSPC; return -1; } } while (0)







while ((c = *src++) != '\0') {
again:
switch (unvis(&t, c, &state, flag)) {
case UNVIS_VALID:
CHECKSPACE();
*dst++ = t;
break;
case UNVIS_VALIDPUSH:
CHECKSPACE();
*dst++ = t;
goto again;
case 0:
case UNVIS_NOCHAR:
break;
case UNVIS_SYNBAD:
errno = EINVAL;
return -1;
default:
_DIAGASSERT(0);
errno = EINVAL;
return -1;
}
}
if (unvis(&t, c, &state, UNVIS_END) == UNVIS_VALID) {
CHECKSPACE();
*dst++ = t;
}
CHECKSPACE();
*dst = '\0';
return (int)(dst - start);
}

int
strunvisx(char *dst, const char *src, int flag)
{
return strnunvisx(dst, (size_t)~0, src, flag);
}

int
strunvis(char *dst, const char *src)
{
return strnunvisx(dst, (size_t)~0, src, 0);
}

int
strnunvis(char *dst, size_t dlen, const char *src)
{
return strnunvisx(dst, dlen, src, 0);
}
#endif
