#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include "forker.h"
int
main(void)
{
int i;
for (i = 0; i < 10000; i++) {
FORKER_FIRE();
if (fork() == 0)
exit(0);
(void) wait(NULL);
}
return (0);
}
