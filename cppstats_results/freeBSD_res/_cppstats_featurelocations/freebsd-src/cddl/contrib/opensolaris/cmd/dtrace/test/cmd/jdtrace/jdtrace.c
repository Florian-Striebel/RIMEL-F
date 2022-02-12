

























#pragma ident "%Z%%M% %I% %E% SMI"

#include <alloca.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/systeminfo.h>

int
main(int argc, char **argv)
{
int i, ac, has64;
char **av, **p;

ac = argc + 3;
av = p = alloca(sizeof (char *) * ac);

*p++ = "java";
*p++ = "-jar";
*p++ = "/opt/SUNWdtrt/lib/java/jdtrace.jar";

argc--;
argv++;

for (i = 0; i < argc; i++) {
p[i] = argv[i];
}
p[i] = NULL;

(void) execvp(av[0], av);

perror("exec failed");

return (0);
}
