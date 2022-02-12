









#if !defined(MSAN_ORIGIN_H)
#define MSAN_ORIGIN_H

#include "sanitizer_common/sanitizer_stackdepot.h"
#include "msan_chained_origin_depot.h"

namespace __msan {




































class Origin {
public:
static bool isValidId(u32 id) { return id != 0 && id != (u32)-1; }

u32 raw_id() const { return raw_id_; }
bool isHeapOrigin() const {

return raw_id_ >> kHeapShift == 0;
}
bool isStackOrigin() const {

return (raw_id_ >> kDepthShift) == (1 << kDepthBits);
}
bool isChainedOrigin() const {

return (raw_id_ >> kDepthShift) > (1 << kDepthBits);
}
u32 getChainedId() const {
CHECK(isChainedOrigin());
return raw_id_ & kChainedIdMask;
}
u32 getStackId() const {
CHECK(isStackOrigin());
return raw_id_ & kChainedIdMask;
}
u32 getHeapId() const {
CHECK(isHeapOrigin());
return raw_id_ & kHeapIdMask;
}


Origin getNextChainedOrigin(StackTrace *stack) const {
CHECK(isChainedOrigin());
u32 prev_id;
u32 stack_id = ChainedOriginDepotGet(getChainedId(), &prev_id);
if (stack) *stack = StackDepotGet(stack_id);
return Origin(prev_id);
}

StackTrace getStackTraceForHeapOrigin() const {
return StackDepotGet(getHeapId());
}

static Origin CreateStackOrigin(u32 id) {
CHECK((id & kStackIdMask) == id);
return Origin((1 << kHeapShift) | id);
}

static Origin CreateHeapOrigin(StackTrace *stack) {
u32 stack_id = StackDepotPut(*stack);
CHECK(stack_id);
CHECK((stack_id & kHeapIdMask) == stack_id);
return Origin(stack_id);
}

static Origin CreateChainedOrigin(Origin prev, StackTrace *stack) {
int depth = prev.isChainedOrigin() ? prev.depth() : 0;


if (flags()->origin_history_size > 0) {
if (depth + 1 >= flags()->origin_history_size) {
return prev;
} else {
++depth;
CHECK(depth < (1 << kDepthBits));
}
}

StackDepotHandle h = StackDepotPut_WithHandle(*stack);
if (!h.valid()) return prev;

if (flags()->origin_history_per_stack_limit > 0) {
int use_count = h.use_count();
if (use_count > flags()->origin_history_per_stack_limit) return prev;
}

u32 chained_id;
bool inserted = ChainedOriginDepotPut(h.id(), prev.raw_id(), &chained_id);
CHECK((chained_id & kChainedIdMask) == chained_id);

if (inserted && flags()->origin_history_per_stack_limit > 0)
h.inc_use_count_unsafe();

return Origin((1 << kHeapShift) | (depth << kDepthShift) | chained_id);
}

static Origin FromRawId(u32 id) {
return Origin(id);
}

private:
static const int kDepthBits = 3;
static const int kDepthShift = 32 - kDepthBits - 1;

static const int kHeapShift = 31;
static const u32 kChainedIdMask = ((u32)-1) >> (32 - kDepthShift);
static const u32 kStackIdMask = ((u32)-1) >> (32 - kDepthShift);
static const u32 kHeapIdMask = ((u32)-1) >> (32 - kHeapShift);

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
