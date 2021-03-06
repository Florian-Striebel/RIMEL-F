











#if !defined(TSAN_IGNORESET_H)
#define TSAN_IGNORESET_H

#include "tsan_defs.h"

namespace __tsan {

class IgnoreSet {
public:
static const uptr kMaxSize = 16;

IgnoreSet();
void Add(u32 stack_id);
void Reset();
uptr Size() const;
u32 At(uptr i) const;

private:
uptr size_;
u32 stacks_[kMaxSize];
};

}

#endif
