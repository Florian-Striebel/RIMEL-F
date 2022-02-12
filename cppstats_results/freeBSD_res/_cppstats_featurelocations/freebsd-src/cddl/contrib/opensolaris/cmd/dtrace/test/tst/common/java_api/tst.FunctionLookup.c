

























#include <stdio.h>

typedef void f(int x);

static void
f1(int i)
{
printf("%d\n", i);
}

static void
f2(f func, int i)
{
func(i);
}

int
main(void)
{
f2(f1, 3);

return (0);
}
