



















































#include <ldns/config.h>

#if !defined(HAVE_INET_ATON)

#include <sys/types.h>
#include <sys/param.h>
#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif
#if defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif
#include <ctype.h>

#if 0




in_addr_t
inet_addr(const char *cp)
{
struct in_addr val;

if (inet_aton(cp, &val))
return (val.s_addr);
return (INADDR_NONE);
}
#endif








int
inet_aton(const char *cp, struct in_addr *addr)
{
uint32_t val;
int base, n;
char c;
unsigned int parts[4];
unsigned int *pp = parts;

c = *cp;
for (;;) {





if (!isdigit((int) c))
return (0);
val = 0; base = 10;
if (c == '0') {
c = *++cp;
if (c == 'x' || c == 'X')
base = 16, c = *++cp;
else
base = 8;
}
for (;;) {
if (isascii((int) c) && isdigit((int) c)) {
val = (val * base) + (c - '0');
c = *++cp;
} else if (base == 16 && isascii((int) c) && isxdigit((int) c)) {
val = (val << 4) |
(c + 10 - (islower((int) c) ? 'a' : 'A'));
c = *++cp;
} else
break;
}
if (c == '.') {






if (pp >= parts + 3)
return (0);
*pp++ = val;
c = *++cp;
} else
break;
}



if (c != '\0' && (!isascii((int) c) || !isspace((int) c)))
return (0);




n = pp - parts + 1;
switch (n) {

case 0:
return (0);

case 1:
break;

case 2:
if ((val > 0xffffff) || (parts[0] > 0xff))
return (0);
val |= parts[0] << 24;
break;

case 3:
if ((val > 0xffff) || (parts[0] > 0xff) || (parts[1] > 0xff))
return (0);
val |= (parts[0] << 24) | (parts[1] << 16);
break;

case 4:
if ((val > 0xff) || (parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff))
return (0);
val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
break;
}
if (addr)
addr->s_addr = htonl(val);
return (1);
}

#endif
