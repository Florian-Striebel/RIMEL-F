

























#include <sys/types.h>
#include <sys/sysctl.h>

#include <err.h>
#include <unistd.h>

int
main(void)
{
int val = 1;

while (1) {
if (sysctlbyname("debug.dtracetest.sdttest", NULL, NULL, &val,
sizeof(val)))
err(1, "sysctlbyname");

sleep(1);
}

return (0);
}
