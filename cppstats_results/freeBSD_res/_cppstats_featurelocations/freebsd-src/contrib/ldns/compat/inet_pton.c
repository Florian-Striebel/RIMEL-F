

















#include <ldns/config.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>






static int inet_pton4 (const char *src, uint8_t *dst);
static int inet_pton6 (const char *src, uint8_t *dst);






#if !defined(NS_INT16SZ)
#define NS_INT16SZ 2
#endif

#if !defined(NS_IN6ADDRSZ)
#define NS_IN6ADDRSZ 16
#endif

#if !defined(NS_INADDRSZ)
#define NS_INADDRSZ 4
#endif












int
inet_pton(af, src, dst)
int af;
const char *src;
void *dst;
{
switch (af) {
case AF_INET:
return (inet_pton4(src, dst));
case AF_INET6:
return (inet_pton6(src, dst));
default:
#if defined(EAFNOSUPPORT)
errno = EAFNOSUPPORT;
#else
errno = ENOSYS;
#endif
return (-1);
}

}











static int
inet_pton4(src, dst)
const char *src;
uint8_t *dst;
{
static const char digits[] = "0123456789";
int saw_digit, octets, ch;
uint8_t tmp[NS_INADDRSZ], *tp;

saw_digit = 0;
octets = 0;
*(tp = tmp) = 0;
while ((ch = *src++) != '\0') {
const char *pch;

if ((pch = strchr(digits, ch)) != NULL) {
uint32_t new = *tp * 10 + (pch - digits);

if (new > 255)
return (0);
*tp = new;
if (! saw_digit) {
if (++octets > 4)
return (0);
saw_digit = 1;
}
} else if (ch == '.' && saw_digit) {
if (octets == 4)
return (0);
*++tp = 0;
saw_digit = 0;
} else
return (0);
}
if (octets < 4)
return (0);

memcpy(dst, tmp, NS_INADDRSZ);
return (1);
}














static int
inet_pton6(src, dst)
const char *src;
uint8_t *dst;
{
static const char xdigits_l[] = "0123456789abcdef",
xdigits_u[] = "0123456789ABCDEF";
uint8_t tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
const char *xdigits, *curtok;
int ch, saw_xdigit;
uint32_t val;

memset((tp = tmp), '\0', NS_IN6ADDRSZ);
endp = tp + NS_IN6ADDRSZ;
colonp = NULL;

if (*src == ':')
if (*++src != ':')
return (0);
curtok = src;
saw_xdigit = 0;
val = 0;
while ((ch = *src++) != '\0') {
const char *pch;

if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
pch = strchr((xdigits = xdigits_u), ch);
if (pch != NULL) {
val <<= 4;
val |= (pch - xdigits);
if (val > 0xffff)
return (0);
saw_xdigit = 1;
continue;
}
if (ch == ':') {
curtok = src;
if (!saw_xdigit) {
if (colonp)
return (0);
colonp = tp;
continue;
}
if (tp + NS_INT16SZ > endp)
return (0);
*tp++ = (uint8_t) (val >> 8) & 0xff;
*tp++ = (uint8_t) val & 0xff;
saw_xdigit = 0;
val = 0;
continue;
}
if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
inet_pton4(curtok, tp) > 0) {
tp += NS_INADDRSZ;
saw_xdigit = 0;
break;
}
return (0);
}
if (saw_xdigit) {
if (tp + NS_INT16SZ > endp)
return (0);
*tp++ = (uint8_t) (val >> 8) & 0xff;
*tp++ = (uint8_t) val & 0xff;
}
if (colonp != NULL) {




const int n = tp - colonp;
int i;

for (i = 1; i <= n; i++) {
endp[- i] = colonp[n - i];
colonp[n - i] = 0;
}
tp = endp;
}
if (tp != endp)
return (0);
memcpy(dst, tmp, NS_IN6ADDRSZ);
return (1);
}
