











#if !defined(DFSAN_ALLOCATOR_H)
#define DFSAN_ALLOCATOR_H

#include "sanitizer_common/sanitizer_common.h"

namespace __dfsan {

struct DFsanThreadLocalMallocStorage {
ALIGNED(8) uptr allocator_cache[96 * (512 * 8 + 16)];
void CommitBack();

private:

DFsanThreadLocalMallocStorage() {}
};

}
#endif
