






























#include <sys/cdefs.h>
#if !defined(lint)
__RCSID("$NetBSD: rl1.c,v 1.2 2016/02/29 00:54:19 christos Exp $");
#endif




#include <stdio.h>
#include <readline/readline.h>

int
main(int argc, char *argv[])
{
char *p;
while ((p = readline("hi$")) != NULL) {
add_history(p);
printf("%d %s\n", history_length, p);
}
return 0;
}
