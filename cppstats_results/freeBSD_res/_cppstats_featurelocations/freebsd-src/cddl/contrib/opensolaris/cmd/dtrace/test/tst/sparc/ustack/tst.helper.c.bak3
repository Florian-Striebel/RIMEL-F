

























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




uint32_t instr[] = {
0x9de3bfa0,
0x40000000,
0x01000000,
0x81c7e008,
0x81e80000
};
uint32_t *fp = malloc(sizeof (instr));




instr[1] |= ((uintptr_t)baz - (uintptr_t)&fp[1]) >> 2;





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
