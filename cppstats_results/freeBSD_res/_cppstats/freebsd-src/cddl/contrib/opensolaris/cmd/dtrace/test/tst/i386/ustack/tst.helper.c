#pragma ident "%Z%%M% %I% %E% SMI"
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
int
baz(void)
{
return (8);
}
static int
foo(void)
{
uint8_t instr[] = {
0x55,
0x8b, 0xec,
0xe8, 0x0, 0x0, 0x0, 0x0,
0x8b, 0xe5,
0x5d,
0xc3
};
uint8_t *fp = malloc(sizeof (instr));
*((int *)&instr[4]) = (uintptr_t)baz - (uintptr_t)&fp[8];
bcopy(instr, fp, sizeof (instr));
(*(int (*)(void))fp)();
free(fp);
return (0);
}
int
main(int argc, char **argv)
{
for (;;) {
foo();
}
return (0);
}
