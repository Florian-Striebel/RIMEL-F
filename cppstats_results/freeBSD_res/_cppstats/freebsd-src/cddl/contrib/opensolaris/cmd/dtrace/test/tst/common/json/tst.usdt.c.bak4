














#include <sys/sdt.h>
#include <stdio.h>
#include <stdlib.h>
#include "usdt.h"

#define FMT "{" " \"sizes\": [ \"first\", 2, %f ]," " \"index\": %d," " \"facts\": {" " \"odd\": \"%s\"," " \"even\": \"%s\"" " }," " \"action\": \"%s\"" "}\n"









int waiting(volatile int *);

int
waiting(volatile int *a)
{
return (*a);
}

int
main(void)
{
volatile int a = 0;
int idx;
double size = 250.5;

while (waiting(&a) == 0)
continue;

for (idx = 0; idx < 10; idx++) {
const char *odd, *even, *action;
char *json;

size *= 1.78;
odd = idx % 2 == 1 ? "true" : "false";
even = idx % 2 == 0 ? "true" : "false";
action = idx == 7 ? "ignore" : "print";

asprintf(&json, FMT, size, idx, odd, even, action);
BUNYAN_FAKE_LOG_DEBUG(json);
free(json);
}

BUNYAN_FAKE_LOG_DEBUG(__DECONST(char *, "{\"finished\": true}"));

return (0);
}
