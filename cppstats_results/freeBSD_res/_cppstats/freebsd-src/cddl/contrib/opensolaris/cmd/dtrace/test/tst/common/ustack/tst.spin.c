#include <unistd.h>
extern volatile long long count;
volatile long long count = 0;
int baz(int);
int bar(int);
int foo(int, int);
int
baz(int a)
{
(void) getpid();
while (count != -1) {
count++;
a++;
}
return (a + 1);
}
int
bar(int a)
{
return (baz(a + 1) - 1);
}
int
foo(int a, int b)
{
return (bar(a) - b);
}
int
main(int argc, char **argv)
{
return (foo(argc, (int)(uintptr_t)argv) == 0);
}
