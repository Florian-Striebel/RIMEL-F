





#include "fido.h"

static int
fido_dev_reset_tx(fido_dev_t *dev)
{
const unsigned char cbor[] = { CTAP_CBOR_RESET };

if (fido_tx(dev, CTAP_CMD_CBOR, cbor, sizeof(cbor)) < 0) {
fido_log_debug("%s: fido_tx", __func__);
return (FIDO_ERR_TX);
}

return (FIDO_OK);
}

static int
fido_dev_reset_wait(fido_dev_t *dev, int ms)
{
int r;

if ((r = fido_dev_reset_tx(dev)) != FIDO_OK ||
(r = fido_rx_cbor_status(dev, ms)) != FIDO_OK)
return (r);

if (dev->flags & FIDO_DEV_PIN_SET) {
dev->flags &= ~FIDO_DEV_PIN_SET;
dev->flags |= FIDO_DEV_PIN_UNSET;
}

return (FIDO_OK);
}

int
fido_dev_reset(fido_dev_t *dev)
{
return (fido_dev_reset_wait(dev, -1));
}
