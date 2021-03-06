











#include "sanitizer_common/sanitizer_hash.h"
#include "hwasan.h"

namespace __hwasan {

static u32 malloc_hash(StackTrace *stack, uptr orig_size) {
uptr len = Min(stack->size, (unsigned)7);
MurMur2HashBuilder H(len);
H.add(orig_size);





for (uptr i = 1; i < len; ++i) H.add(((u32)stack->trace[i]) & 0xFFF);
return H.get();
}

static inline bool malloc_bisect(StackTrace *stack, uptr orig_size) {
uptr left = flags()->malloc_bisect_left;
uptr right = flags()->malloc_bisect_right;
if (LIKELY(left == 0 && right == 0))
return true;
if (!stack)
return true;


uptr h = (uptr)malloc_hash(stack, orig_size);
if (h < left || h > right)
return false;
if (flags()->malloc_bisect_dump) {
Printf("[alloc] %u %zu\n", h, orig_size);
stack->Print();
}
return true;
}

}
