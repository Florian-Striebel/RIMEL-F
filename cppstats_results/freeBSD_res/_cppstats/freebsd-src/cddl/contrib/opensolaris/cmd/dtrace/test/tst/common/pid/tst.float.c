#include <unistd.h>
extern volatile double c;
volatile double c = 1.2;
int
main(int argc, char **argv __unused)
{
double a = 1.56;
double b = (double)argc;
for (;;) {
c *= a;
c += b;
(void) usleep(1000);
}
return (0);
}
