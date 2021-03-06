












#if !defined(DFSAN_ORIGIN_H)
#define DFSAN_ORIGIN_H

#include "dfsan_chained_origin_depot.h"
#include "dfsan_flags.h"
#include "sanitizer_common/sanitizer_stackdepot.h"

namespace __dfsan {

























class Origin {
public:
static bool isValidId(u32 id) { return id != 0; }

u32 raw_id() const { return raw_id_; }

bool isChainedOrigin() const { return Origin::isValidId(raw_id_); }

u32 getChainedId() const {
CHECK(Origin::isValidId(raw_id_));
return raw_id_ & kChainedIdMask;
}





Origin getNextChainedOrigin(StackTrace *stack) const {
CHECK(Origin::isValidId(raw_id_));
u32 prev_id;
u32 stack_id = GetChainedOriginDepot()->Get(getChainedId(), &prev_id);
if (stack)
*stack = StackDepotGet(stack_id);
return Origin(prev_id);
}

static Origin CreateChainedOrigin(Origin prev, StackTrace *stack) {
int depth = prev.isChainedOrigin() ? prev.depth() : -1;


if (flags().origin_history_size > 0) {
++depth;
if (depth >= flags().origin_history_size || depth > kMaxDepth)
return prev;
}

StackDepotHandle h = StackDepotPut_WithHandle(*stack);
if (!h.valid())
return prev;

if (flags().origin_history_per_stack_limit > 0) {
int use_count = h.use_count();
if (use_count > flags().origin_history_per_stack_limit)
return prev;
}

u32 chained_id;
bool inserted =
GetChainedOriginDepot()->Put(h.id(), prev.raw_id(), &chained_id);
CHECK((chained_id & kChainedIdMask) == chained_id);

if (inserted && flags().origin_history_per_stack_limit > 0)
h.inc_use_count_unsafe();

return Origin((depth << kDepthShift) | chained_id);
}

static Origin FromRawId(u32 id) { return Origin(id); }

private:
static const int kDepthBits = 4;
static const int kDepthShift = 32 - kDepthBits;

static const u32 kChainedIdMask = ((u32)-1) >> kDepthBits;

u32 raw_id_;

explicit Origin(u32 raw_id) : raw_id_(raw_id) {}

int depth() const {
CHECK(isChainedOrigin());
return (raw_id_ >> kDepthShift) & ((1 << kDepthBits) - 1);
}

public:
static const int kMaxDepth = (1 << kDepthBits) - 1;
};

}

#endif
