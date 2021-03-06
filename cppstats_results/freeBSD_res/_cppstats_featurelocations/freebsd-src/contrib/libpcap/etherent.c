




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap-types.h>

#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

#include "pcap-int.h"

#include <pcap/namedb.h>

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

static inline int skip_space(FILE *);
static inline int skip_line(FILE *);


static inline u_char
xdtoi(u_char c)
{
if (isdigit(c))
return (u_char)(c - '0');
else if (islower(c))
return (u_char)(c - 'a' + 10);
else
return (u_char)(c - 'A' + 10);
}

static inline int
skip_space(FILE *f)
{
int c;

do {
c = getc(f);
} while (isspace(c) && c != '\n');

return c;
}

static inline int
skip_line(FILE *f)
{
int c;

do
c = getc(f);
while (c != '\n' && c != EOF);

return c;
}

struct pcap_etherent *
pcap_next_etherent(FILE *fp)
{
register int c, i;
u_char d;
char *bp;
size_t namesize;
static struct pcap_etherent e;

memset((char *)&e, 0, sizeof(e));
for (;;) {

c = skip_space(fp);
if (c == EOF)
return (NULL);
if (c == '\n')
continue;



if (!isxdigit(c)) {
c = skip_line(fp);
if (c == EOF)
return (NULL);
continue;
}


for (i = 0; i < 6; i += 1) {
d = xdtoi((u_char)c);
c = getc(fp);
if (c == EOF)
return (NULL);
if (isxdigit(c)) {
d <<= 4;
d |= xdtoi((u_char)c);
c = getc(fp);
if (c == EOF)
return (NULL);
}
e.addr[i] = d;
if (c != ':')
break;
c = getc(fp);
if (c == EOF)
return (NULL);
}


if (!isspace(c)) {
c = skip_line(fp);
if (c == EOF)
return (NULL);
continue;
}
c = skip_space(fp);
if (c == EOF)
return (NULL);


if (c == '\n')
continue;

if (c == '#') {
c = skip_line(fp);
if (c == EOF)
return (NULL);
continue;
}


bp = e.name;

namesize = sizeof(e.name) - 1;
do {
*bp++ = (u_char)c;
c = getc(fp);
if (c == EOF)
return (NULL);
} while (!isspace(c) && --namesize != 0);
*bp = '\0';


if (c != '\n')
(void)skip_line(fp);

return &e;
}
}
