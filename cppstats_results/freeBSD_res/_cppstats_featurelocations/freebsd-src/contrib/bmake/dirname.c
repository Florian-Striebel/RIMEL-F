






























#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif
#if !defined(HAVE_DIRNAME)

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: dirname.c,v 1.14 2018/09/27 00:45:34 kre Exp $");
#endif

#include <sys/param.h>
#if defined(HAVE_LIBGEN_H)
#include <libgen.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if !defined(PATH_MAX)
#define PATH_MAX 1024
#endif
#if !defined(MIN)
#define MIN(a, b) ((a < b) ? a : b)
#endif


static size_t
xdirname_r(const char *path, char *buf, size_t buflen)
{
const char *endp;
size_t len;





if (path == NULL || *path == '\0') {
path = ".";
len = 1;
goto out;
}


endp = path + strlen(path) - 1;
while (endp != path && *endp == '/')
endp--;


while (endp > path && *endp != '/')
endp--;

if (endp == path) {
path = *endp == '/' ? "/" : ".";
len = 1;
goto out;
}

do
endp--;
while (endp > path && *endp == '/');

len = endp - path + 1;
out:
if (buf != NULL && buflen != 0) {
buflen = MIN(len, buflen - 1);
if (buf != path)
memcpy(buf, path, buflen);
buf[buflen] = '\0';
}
return len;
}

char *
dirname(char *path)
{
static char result[PATH_MAX];
(void)xdirname_r(path, result, sizeof(result));
return result;
}

#if defined(MAIN)
#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
printf("%s\n", dirname(argv[1]));
exit(0);
}
#endif
#endif
