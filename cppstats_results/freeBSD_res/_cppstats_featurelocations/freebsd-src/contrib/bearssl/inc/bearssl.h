























#if !defined(BR_BEARSSL_H__)
#define BR_BEARSSL_H__

#include <stddef.h>
#include <stdint.h>

































































































#include "bearssl_hash.h"
#include "bearssl_hmac.h"
#include "bearssl_kdf.h"
#include "bearssl_rand.h"
#include "bearssl_prf.h"
#include "bearssl_block.h"
#include "bearssl_aead.h"
#include "bearssl_rsa.h"
#include "bearssl_ec.h"
#include "bearssl_ssl.h"
#include "bearssl_x509.h"
#include "bearssl_pem.h"










typedef struct {

const char *name;

long value;
} br_config_option;













const br_config_option *br_get_config(void);

#endif
