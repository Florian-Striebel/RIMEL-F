












#if !defined(DFSAN_CHAINED_ORIGIN_DEPOT_H)
#define DFSAN_CHAINED_ORIGIN_DEPOT_H

#include "sanitizer_common/sanitizer_chained_origin_depot.h"
#include "sanitizer_common/sanitizer_common.h"

namespace __dfsan {

ChainedOriginDepot* GetChainedOriginDepot();

}

#endif
