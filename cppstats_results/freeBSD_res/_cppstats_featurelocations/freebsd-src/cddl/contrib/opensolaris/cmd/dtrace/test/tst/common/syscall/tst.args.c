

























#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>


int
main(void)
{
for (;;) {
(void) __syscall(SYS_mmap, NULL, (size_t)1, 2, 3, -1,
(off_t)0x12345678);
}

return (0);
}
