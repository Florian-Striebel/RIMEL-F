

































#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)popen.c 8.3 (Berkeley) 5/3/95";
#else
__RCSID("$NetBSD: popenve.c,v 1.2 2015/01/22 03:10:50 christos Exp $");
#endif
#endif

#include <sys/param.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include <assert.h>
#include <errno.h>
#include <paths.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#if defined(__weak_alias)
__weak_alias(popen,_popen)
__weak_alias(pclose,_pclose)
#endif

static struct pid {
struct pid *next;
FILE *fp;
#if defined(_REENTRANT)
int fd;
#endif
pid_t pid;
} *pidlist;

#if defined(_REENTRANT)
static rwlock_t pidlist_lock = RWLOCK_INITIALIZER;
#endif

static struct pid *
pdes_get(int *pdes, const char **type)
{
struct pid *cur;
int flags = strchr(*type, 'e') ? O_CLOEXEC : 0;
int serrno;

if (strchr(*type, '+')) {
#if !defined(SOCK_CLOEXEC)
#define SOCK_CLOEXEC 0
#endif
int stype = flags ? (SOCK_STREAM | SOCK_CLOEXEC) : SOCK_STREAM;
*type = "r+";
if (socketpair(AF_LOCAL, stype, 0, pdes) < 0)
return NULL;
#if SOCK_CLOEXEC == 0
fcntl(pdes[0], F_SETFD, FD_CLOEXEC);
fcntl(pdes[1], F_SETFD, FD_CLOEXEC);
#endif
} else {
*type = strrchr(*type, 'r') ? "r" : "w";
#if SOCK_CLOEXEC != 0
if (pipe2(pdes, flags) == -1)
return NULL;
#else
if (pipe(pdes) == -1)
return NULL;
fcntl(pdes[0], F_SETFL, fcntl(pdes[0], F_GETFL) | flags);
fcntl(pdes[1], F_SETFL, fcntl(pdes[1], F_GETFL) | flags);
#endif
}

if ((cur = malloc(sizeof(*cur))) != NULL)
return cur;
serrno = errno;
(void)close(pdes[0]);
(void)close(pdes[1]);
errno = serrno;
return NULL;
}

static void
pdes_child(int *pdes, const char *type)
{
struct pid *old;




for (old = pidlist; old; old = old->next)
#if defined(_REENTRANT)
(void)close(old->fd);
#else
(void)close(fileno(old->fp));
#endif

if (type[0] == 'r') {
(void)close(pdes[0]);
if (pdes[1] != STDOUT_FILENO) {
(void)dup2(pdes[1], STDOUT_FILENO);
(void)close(pdes[1]);
}
if (type[1] == '+')
(void)dup2(STDOUT_FILENO, STDIN_FILENO);
} else {
(void)close(pdes[1]);
if (pdes[0] != STDIN_FILENO) {
(void)dup2(pdes[0], STDIN_FILENO);
(void)close(pdes[0]);
}
}
}

static void
pdes_parent(int *pdes, struct pid *cur, pid_t pid, const char *type)
{
FILE *iop;


if (*type == 'r') {
iop = fdopen(pdes[0], type);
#if defined(_REENTRANT)
cur->fd = pdes[0];
#endif
(void)close(pdes[1]);
} else {
iop = fdopen(pdes[1], type);
#if defined(_REENTRANT)
cur->fd = pdes[1];
#endif
(void)close(pdes[0]);
}


cur->fp = iop;
cur->pid = pid;
cur->next = pidlist;
pidlist = cur;
}

static void
pdes_error(int *pdes, struct pid *cur)
{
free(cur);
(void)close(pdes[0]);
(void)close(pdes[1]);
}

FILE *
popenve(const char *cmd, char *const *argv, char *const *envp, const char *type)
{
struct pid *cur;
int pdes[2], serrno;
pid_t pid;

if ((cur = pdes_get(pdes, &type)) == NULL)
return NULL;

#if defined(_REENTRANT)
(void)rwlock_rdlock(&pidlist_lock);
#endif
switch (pid = vfork()) {
case -1:
serrno = errno;
#if defined(_REENTRANT)
(void)rwlock_unlock(&pidlist_lock);
#endif
pdes_error(pdes, cur);
errno = serrno;
return NULL;

case 0:
pdes_child(pdes, type);
execve(cmd, argv, envp);
_exit(127);

}

pdes_parent(pdes, cur, pid, type);

#if defined(_REENTRANT)
(void)rwlock_unlock(&pidlist_lock);
#endif

return cur->fp;
}






int
pcloseve(FILE *iop)
{
struct pid *cur, *last;
int pstat;
pid_t pid;

#if defined(_REENTRANT)
rwlock_wrlock(&pidlist_lock);
#endif


for (last = NULL, cur = pidlist; cur; last = cur, cur = cur->next)
if (cur->fp == iop)
break;
if (cur == NULL) {
#if defined(_REENTRANT)
(void)rwlock_unlock(&pidlist_lock);
#endif
errno = ESRCH;
return -1;
}

(void)fclose(iop);


if (last == NULL)
pidlist = cur->next;
else
last->next = cur->next;

#if defined(_REENTRANT)
(void)rwlock_unlock(&pidlist_lock);
#endif

do {
pid = waitpid(cur->pid, &pstat, 0);
} while (pid == -1 && errno == EINTR);

free(cur);

return pid == -1 ? -1 : pstat;
}
