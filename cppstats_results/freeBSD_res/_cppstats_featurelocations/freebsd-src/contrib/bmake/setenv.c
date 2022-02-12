
































#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#if !defined(HAVE_SETENV)

#if defined(LIBC_SCCS) && !defined(lint)

static char *rcsid = "$Id: setenv.c,v 1.5 1996/09/04 22:10:42 sjg Exp $";
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>










static char *
__findenv(name, offset)
register char *name;
int *offset;
{
extern char **environ;
register int len;
register char **P, *C;

for (C = name, len = 0; *C && *C != '='; ++C, ++len);
for (P = environ; *P; ++P)
if (!strncmp(*P, name, len))
if (*(C = *P + len) == '=') {
*offset = P - environ;
return(++C);
}
return(NULL);
}






setenv(name, value, rewrite)
register const char *name;
register const char *value;
int rewrite;
{
extern char **environ;
static int alloced;
register char *C;
int l_value, offset;
char *__findenv();

if (*value == '=')
++value;
l_value = strlen(value);
if ((C = __findenv(name, &offset))) {
if (!rewrite)
return (0);
if (strlen(C) >= l_value) {
while (*C++ = *value++);
return (0);
}
} else {
register int cnt;
register char **P;

for (P = environ, cnt = 0; *P; ++P, ++cnt);
if (alloced) {
environ = (char **)realloc((char *)environ,
(size_t)(sizeof(char *) * (cnt + 2)));
if (!environ)
return (-1);
}
else {
alloced = 1;
P = (char **)malloc((size_t)(sizeof(char *) *
(cnt + 2)));
if (!P)
return (-1);
bcopy(environ, P, cnt * sizeof(char *));
environ = P;
}
environ[cnt + 1] = NULL;
offset = cnt;
}
for (C = (char *)name; *C && *C != '='; ++C);
if (!(environ[offset] =
malloc((size_t)((int)(C - name) + l_value + 2))))
return (-1);
for (C = environ[offset]; (*C = *name++) && *C != '='; ++C)
;
for (*C++ = '='; *C++ = *value++; )
;
return (0);
}





void
unsetenv(name)
const char *name;
{
extern char **environ;
register char **P;
int offset;
char *__findenv();

while (__findenv(name, &offset))
for (P = &environ[offset];; ++P)
if (!(*P = *(P + 1)))
break;
}
#endif
