

































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)memalloc.c 8.3 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include "shell.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "mystring.h"
#include "expand.h"
#include <stdlib.h>
#include <unistd.h>

static void
badalloc(const char *message)
{
write(2, message, strlen(message));
abort();
}





pointer
ckmalloc(size_t nbytes)
{
pointer p;

if (!is_int_on())
badalloc("Unsafe ckmalloc() call\n");
p = malloc(nbytes);
if (p == NULL)
error("Out of space");
return p;
}






pointer
ckrealloc(pointer p, int nbytes)
{
if (!is_int_on())
badalloc("Unsafe ckrealloc() call\n");
p = realloc(p, nbytes);
if (p == NULL)
error("Out of space");
return p;
}

void
ckfree(pointer p)
{
if (!is_int_on())
badalloc("Unsafe ckfree() call\n");
free(p);
}






char *
savestr(const char *s)
{
char *p;
size_t len;

len = strlen(s);
p = ckmalloc(len + 1);
memcpy(p, s, len + 1);
return p;
}











#define MINSIZE 496


struct stack_block {
struct stack_block *prev;

};
#define SPACE(sp) ((char*)(sp) + ALIGN(sizeof(struct stack_block)))

static struct stack_block *stackp;
char *stacknxt;
int stacknleft;
char *sstrend;


static void
stnewblock(int nbytes)
{
struct stack_block *sp;
int allocsize;

if (nbytes < MINSIZE)
nbytes = MINSIZE;

allocsize = ALIGN(sizeof(struct stack_block)) + ALIGN(nbytes);

INTOFF;
sp = ckmalloc(allocsize);
sp->prev = stackp;
stacknxt = SPACE(sp);
stacknleft = allocsize - (stacknxt - (char*)sp);
sstrend = stacknxt + stacknleft;
stackp = sp;
INTON;
}


pointer
stalloc(int nbytes)
{
char *p;

nbytes = ALIGN(nbytes);
if (nbytes > stacknleft)
stnewblock(nbytes);
p = stacknxt;
stacknxt += nbytes;
stacknleft -= nbytes;
return p;
}


void
stunalloc(pointer p)
{
if (p == NULL) {
write(STDERR_FILENO, "stunalloc\n", 10);
abort();
}
stacknleft += stacknxt - (char *)p;
stacknxt = p;
}


char *
stsavestr(const char *s)
{
char *p;
size_t len;

len = strlen(s);
p = stalloc(len + 1);
memcpy(p, s, len + 1);
return p;
}


void
setstackmark(struct stackmark *mark)
{
mark->stackp = stackp;
mark->stacknxt = stacknxt;
mark->stacknleft = stacknleft;

if (stackp != NULL && stacknxt == SPACE(stackp))
stalloc(1);
}


void
popstackmark(struct stackmark *mark)
{
struct stack_block *sp;

INTOFF;
while (stackp != mark->stackp) {
sp = stackp;
stackp = sp->prev;
ckfree(sp);
}
stacknxt = mark->stacknxt;
stacknleft = mark->stacknleft;
if (stacknleft != 0)
sstrend = stacknxt + stacknleft;
else
sstrend = stacknxt;
INTON;
}












static void
growstackblock(int min)
{
char *p;
int newlen;
char *oldspace;
int oldlen;
struct stack_block *sp;
struct stack_block *oldstackp;

if (min < stacknleft)
min = stacknleft;
if ((unsigned int)min >=
INT_MAX / 2 - ALIGN(sizeof(struct stack_block)))
error("Out of space");
min += stacknleft;
min += ALIGN(sizeof(struct stack_block));
newlen = 512;
while (newlen < min)
newlen <<= 1;
oldspace = stacknxt;
oldlen = stacknleft;

if (stackp != NULL && stacknxt == SPACE(stackp)) {
INTOFF;
oldstackp = stackp;
stackp = oldstackp->prev;
sp = ckrealloc((pointer)oldstackp, newlen);
sp->prev = stackp;
stackp = sp;
stacknxt = SPACE(sp);
stacknleft = newlen - (stacknxt - (char*)sp);
sstrend = stacknxt + stacknleft;
INTON;
} else {
newlen -= ALIGN(sizeof(struct stack_block));
p = stalloc(newlen);
if (oldlen != 0)
memcpy(p, oldspace, oldlen);
stunalloc(p);
}
}





















static char *
growstrstackblock(int n, int min)
{
growstackblock(min);
return stackblock() + n;
}

char *
growstackstr(void)
{
int len;

len = stackblocksize();
return (growstrstackblock(len, 0));
}






char *
makestrspace(int min, char *p)
{
int len;

len = p - stackblock();
return (growstrstackblock(len, min));
}


char *
stputbin(const char *data, size_t len, char *p)
{
CHECKSTRSPACE(len, p);
memcpy(p, data, len);
return (p + len);
}

char *
stputs(const char *data, char *p)
{
return (stputbin(data, strlen(data), p));
}
