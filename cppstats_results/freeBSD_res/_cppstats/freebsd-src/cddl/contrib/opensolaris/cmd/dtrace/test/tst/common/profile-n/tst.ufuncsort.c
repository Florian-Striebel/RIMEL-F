#include <sys/cdefs.h>
typedef void f(int x);
static void
f_a(int i __unused)
{
}
static void
f_b(int i __unused)
{
}
static void
f_c(int i __unused)
{
}
static void
f_d(int i __unused)
{
}
static void
f_e(int i __unused)
{
}
static void
fN(f func, int i)
{
func(i);
}
int
main(void)
{
fN(f_a, 1);
fN(f_b, 2);
fN(f_c, 3);
fN(f_d, 4);
fN(f_e, 5);
fN(f_a, 11);
fN(f_c, 13);
fN(f_d, 14);
fN(f_a, 101);
fN(f_c, 103);
fN(f_c, 1003);
return (0);
}
