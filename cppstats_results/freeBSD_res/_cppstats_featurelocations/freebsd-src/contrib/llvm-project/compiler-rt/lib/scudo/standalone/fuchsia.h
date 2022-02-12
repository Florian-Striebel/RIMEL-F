







#if !defined(SCUDO_FUCHSIA_H_)
#define SCUDO_FUCHSIA_H_

#include "platform.h"

#if SCUDO_FUCHSIA

#include <zircon/process.h>

namespace scudo {

struct MapPlatformData {
zx_handle_t Vmar;
zx_handle_t Vmo;
uintptr_t VmarBase;
uint64_t VmoSize;
};

}

#endif

#endif
