

















#include <fido.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../openbsd-compat/openbsd-compat.h"
#include "extern.h"

void
usage(void)
{
fprintf(stderr,
"usage: fido2-assert -G [-bdhpruv] [-t option] [-i input_file] [-o output_file] device\n"
" fido2-assert -V [-dhpv] [-i input_file] key_file [type]\n"
);

exit(1);
}

int
main(int argc, char **argv)
{
if (argc < 2 || strlen(argv[1]) != 2 || argv[1][0] != '-')
usage();

switch (argv[1][1]) {
case 'G':
return (assert_get(--argc, ++argv));
case 'V':
return (assert_verify(--argc, ++argv));
}

usage();


}
