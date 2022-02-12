









#include <fido.h>
#include <stdio.h>
#include <stdlib.h>

#include "../openbsd-compat/openbsd-compat.h"

int
main(int argc, char **argv)
{
fido_dev_t *dev;
int n;
int r;

if (argc != 2) {
fprintf(stderr, "usage: retries <device>\n");
exit(EXIT_FAILURE);
}

fido_init(0);

if ((dev = fido_dev_new()) == NULL)
errx(1, "fido_dev_new");

if ((r = fido_dev_open(dev, argv[1])) != FIDO_OK)
errx(1, "fido_open: %s (0x%x)", fido_strerr(r), r);

if ((r = fido_dev_get_retry_count(dev, &n)) != FIDO_OK)
errx(1, "fido_get_retries: %s (0x%x)", fido_strerr(r), r);

if ((r = fido_dev_close(dev)) != FIDO_OK)
errx(1, "fido_close: %s (0x%x)", fido_strerr(r), r);

fido_dev_free(&dev);

printf("%d\n", n);

exit(0);
}
