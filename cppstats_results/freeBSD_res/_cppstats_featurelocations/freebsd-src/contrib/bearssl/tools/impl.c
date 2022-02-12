























#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "brssl.h"
#include "bearssl.h"


int
do_impl(int argc, char *argv[])
{
const br_config_option *opt;

(void)argc;
(void)argv;

for (opt = br_get_config(); opt->name != NULL; opt ++) {
printf("%-25s %8ld\n", opt->name, opt->value);
}

return 0;
}
