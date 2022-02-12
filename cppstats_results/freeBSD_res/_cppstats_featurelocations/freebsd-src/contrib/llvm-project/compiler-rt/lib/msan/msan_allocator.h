











#if !defined(MSAN_ALLOCATOR_H)
#define MSAN_ALLOCATOR_H

#include "sanitizer_common/sanitizer_common.h"

namespace __msan {

struct MsanThreadLocalMallocStorage {

ALIGNED(8) uptr allocator_cache[96 * (512 * 8 + 16)];
void CommitBack();

private:

MsanThreadLocalMallocStorage() {}
};

}
#endif
