












#if !defined(MSAN_CHAINED_ORIGIN_DEPOT_H)
#define MSAN_CHAINED_ORIGIN_DEPOT_H

#include "sanitizer_common/sanitizer_common.h"

namespace __msan {


StackDepotStats *ChainedOriginDepotGetStats();





bool ChainedOriginDepotPut(u32 here_id, u32 prev_id, u32 *new_id);


u32 ChainedOriginDepotGet(u32 id, u32 *other);

void ChainedOriginDepotLockAll();
void ChainedOriginDepotUnlockAll();

}

#endif
