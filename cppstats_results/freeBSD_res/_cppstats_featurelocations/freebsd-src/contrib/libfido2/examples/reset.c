









#include <fido.h>
#include <stdio.h>
#include <stdlib.h>

#include "../openbsd-compat/openbsd-compat.h"
#include "extern.h"

int
main(int argc, char **argv)
{
fido_dev_t *dev;
int r;

if (argc != 2) {
fprintf(stderr, "usage: reset <device>\n");
exit(EXIT_FAILURE);
}

fido_init(0);

if ((dev = fido_dev_new()) == NULL)
errx(1, "fido_dev_new");

if ((r = fido_dev_open(dev, argv[1])) != FIDO_OK)
errx(1, "fido_dev_open: %s (0x%x)", fido_strerr(r), r);

#if defined(SIGNAL_EXAMPLE)
prepare_signal_handler(SIGINT);
#endif

if ((r = fido_dev_reset(dev)) != FIDO_OK) {
#if defined(SIGNAL_EXAMPLE)
if (got_signal)
fido_dev_cancel(dev);
#endif
errx(1, "fido_reset: %s (0x%x)", fido_strerr(r), r);
}

if ((r = fido_dev_close(dev)) != FIDO_OK)
errx(1, "fido_dev_close: %s (0x%x)", fido_strerr(r), r);

fido_dev_free(&dev);

exit(0);
}
