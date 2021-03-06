






























#if 0
#if !defined(lint)
static char const copyright[] =
"@(#) Copyright (c) 1989, 1993\n\
The Regents of the University of California. All rights reserved.\n";
#endif

#if !defined(lint)
static char sccsid[] = "@(#)echo.c 8.1 (Berkeley) 5/31/93";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/uio.h>

#include <assert.h>
#include <capsicum_helpers.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
int nflag;
int veclen;
struct iovec *iov, *vp;
char space[] = " ";
char newline[] = "\n";

if (caph_limit_stdio() < 0 || caph_enter() < 0)
err(1, "capsicum");


if (*++argv && !strcmp(*argv, "-n")) {
++argv;
--argc;
nflag = 1;
} else
nflag = 0;

veclen = (argc >= 2) ? (argc - 2) * 2 + 1 : 0;

if ((vp = iov = malloc((veclen + 1) * sizeof(struct iovec))) == NULL)
err(1, "malloc");

while (argv[0] != NULL) {
size_t len;

len = strlen(argv[0]);





if (argv[1] == NULL) {

if (len >= 2 &&
argv[0][len - 2] == '\\' &&
argv[0][len - 1] == 'c') {

len -= 2;
nflag = 1;
}
}
vp->iov_base = *argv;
vp++->iov_len = len;
if (*++argv) {
vp->iov_base = space;
vp++->iov_len = 1;
}
}
if (!nflag) {
veclen++;
vp->iov_base = newline;
vp++->iov_len = 1;
}

while (veclen) {
int nwrite;

nwrite = (veclen > IOV_MAX) ? IOV_MAX : veclen;
if (writev(STDOUT_FILENO, iov, nwrite) == -1)
err(1, "write");
iov += nwrite;
veclen -= nwrite;
}
return 0;
}
