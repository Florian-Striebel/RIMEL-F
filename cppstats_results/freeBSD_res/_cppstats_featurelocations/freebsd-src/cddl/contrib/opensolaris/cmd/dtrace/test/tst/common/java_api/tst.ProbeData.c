

























#include <sys/cdefs.h>

typedef void f(char *);

static void
f_a(char *a __unused)
{
}

static void
f_b(char *a __unused)
{
}

static void
f_c(char *a __unused)
{
}

static void
f_d(char *a __unused)
{
}

static void
f_e(char *a __unused)
{
}

static void
fN(f func, char *a, int i __unused)
{
func(a);
}

static void
fN2(f func, char *a, int i __unused)
{
func(a);
}

int
main(void)
{




char a[] = {(char)-7, (char)201, (char)0, (char)0, (char)28, (char)1};
char b[] = {(char)84, (char)69, (char)0, (char)0, (char)28, (char)0};
char c[] = {(char)84, (char)69, (char)0, (char)0, (char)28, (char)1};
char d[] = {(char)-7, (char)201, (char)0, (char)0, (char)29, (char)0};
char e[] = {(char)84, (char)69, (char)0, (char)0, (char)28, (char)0};

fN(f_a, a, 1);
fN(f_b, b, 0);
fN(f_d, d, 102);
fN2(f_e, e, -2);
fN(f_c, c, 0);
fN(f_a, a, -1);
fN(f_d, d, 101);
fN(f_e, e, -2);
fN(f_e, e, 2);
fN2(f_e, e, 2);

return (0);
}
