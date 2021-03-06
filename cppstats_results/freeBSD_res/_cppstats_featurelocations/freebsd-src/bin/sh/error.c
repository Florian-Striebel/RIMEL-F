































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)error.c 8.2 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");





#include "shell.h"
#include "eval.h"
#include "main.h"
#include "options.h"
#include "output.h"
#include "error.h"
#include "nodes.h"
#include "show.h"
#include "trap.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>






struct jmploc *handler;
volatile sig_atomic_t exception;
volatile sig_atomic_t suppressint;
volatile sig_atomic_t intpending;


static void verrorwithstatus(int, const char *, va_list) __printf0like(2, 0) __dead2;










void
exraise(int e)
{
INTOFF;
if (handler == NULL)
abort();
exception = e;
longjmp(handler->loc, 1);
}













void
onint(void)
{
sigset_t sigs;

intpending = 0;
sigemptyset(&sigs);
sigprocmask(SIG_SETMASK, &sigs, NULL);




#if 0
if (tcgetpgrp(0) == getpid())
write(STDERR_FILENO, "\n", 1);
#endif
if (rootshell && iflag)
exraise(EXINT);
else {
signal(SIGINT, SIG_DFL);
kill(getpid(), SIGINT);
_exit(128 + SIGINT);
}
}


static void
vwarning(const char *msg, va_list ap)
{
if (commandname)
outfmt(out2, "%s: ", commandname);
else if (arg0)
outfmt(out2, "%s: ", arg0);
doformat(out2, msg, ap);
out2fmt_flush("\n");
}


void
warning(const char *msg, ...)
{
va_list ap;
va_start(ap, msg);
vwarning(msg, ap);
va_end(ap);
}







static void
verrorwithstatus(int status, const char *msg, va_list ap)
{








FORCEINTON;

#if defined(DEBUG)
if (msg)
TRACE(("verrorwithstatus(%d, \"%s\") pid=%d\n",
status, msg, getpid()));
else
TRACE(("verrorwithstatus(%d, NULL) pid=%d\n",
status, getpid()));
#endif
if (msg)
vwarning(msg, ap);
flushall();
exitstatus = status;
exraise(EXERROR);
}


void
error(const char *msg, ...)
{
va_list ap;
va_start(ap, msg);
verrorwithstatus(2, msg, ap);
va_end(ap);
}


void
errorwithstatus(int status, const char *msg, ...)
{
va_list ap;
va_start(ap, msg);
verrorwithstatus(status, msg, ap);
va_end(ap);
}
