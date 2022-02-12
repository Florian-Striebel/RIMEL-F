#include <unistd.h>
int waiting(volatile int *);
int go(void);
int
waiting(volatile int *a)
{
return (*a);
}
int
go(void)
{
int i, j, total = 0;
for (i = 0; i < 10; i++) {
for (j = 0; j < 10; j++) {
total += i * j;
}
}
return (total);
}
int
main(void)
{
volatile int a = 0;
while (waiting(&a) == 0)
continue;
(void) fork();
(void) go();
return (0);
}
