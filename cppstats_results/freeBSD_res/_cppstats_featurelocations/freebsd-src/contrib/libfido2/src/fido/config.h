





#if !defined(_FIDO_CONFIG_H)
#define _FIDO_CONFIG_H

#if defined(_FIDO_INTERNAL)
#include "blob.h"
#include "fido/err.h"
#include "fido/param.h"
#include "fido/types.h"
#else
#include <fido.h>
#include <fido/err.h>
#include <fido/param.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

int fido_dev_enable_entattest(fido_dev_t *, const char *);
int fido_dev_force_pin_change(fido_dev_t *, const char *);
int fido_dev_toggle_always_uv(fido_dev_t *, const char *);
int fido_dev_set_pin_minlen(fido_dev_t *, size_t, const char *);

#if defined(__cplusplus)
}
#endif

#endif
