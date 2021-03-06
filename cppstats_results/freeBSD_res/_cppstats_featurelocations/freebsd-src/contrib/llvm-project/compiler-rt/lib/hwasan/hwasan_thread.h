











#if !defined(HWASAN_THREAD_H)
#define HWASAN_THREAD_H

#include "hwasan_allocator.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_ring_buffer.h"

namespace __hwasan {

typedef __sanitizer::CompactRingBuffer<uptr> StackAllocationsRingBuffer;

class Thread {
public:

struct InitState;

void Init(uptr stack_buffer_start, uptr stack_buffer_size,
const InitState *state = nullptr);
void InitRandomState();
void InitStackAndTls(const InitState *state = nullptr);


void InitStackRingBuffer(uptr stack_buffer_start, uptr stack_buffer_size);

void Destroy();

uptr stack_top() { return stack_top_; }
uptr stack_bottom() { return stack_bottom_; }
uptr stack_size() { return stack_top() - stack_bottom(); }
uptr tls_begin() { return tls_begin_; }
uptr tls_end() { return tls_end_; }
bool IsMainThread() { return unique_id_ == 0; }

bool AddrIsInStack(uptr addr) {
return addr >= stack_bottom_ && addr < stack_top_;
}

AllocatorCache *allocator_cache() { return &allocator_cache_; }
HeapAllocationsRingBuffer *heap_allocations() { return heap_allocations_; }
StackAllocationsRingBuffer *stack_allocations() { return stack_allocations_; }

tag_t GenerateRandomTag(uptr num_bits = kTagBits);

void DisableTagging() { tagging_disabled_++; }
void EnableTagging() { tagging_disabled_--; }

u64 unique_id() const { return unique_id_; }
void Announce() {
if (announced_) return;
announced_ = true;
Print("Thread: ");
}

uptr &vfork_spill() { return vfork_spill_; }

private:


void ClearShadowForThreadStackAndTLS();
void Print(const char *prefix);
uptr vfork_spill_;
uptr stack_top_;
uptr stack_bottom_;
uptr tls_begin_;
uptr tls_end_;

u32 random_state_;
u32 random_buffer_;

AllocatorCache allocator_cache_;
HeapAllocationsRingBuffer *heap_allocations_;
StackAllocationsRingBuffer *stack_allocations_;

u64 unique_id_;

u32 tagging_disabled_;

bool announced_;

friend struct ThreadListHead;
};

Thread *GetCurrentThread();
uptr *GetCurrentThreadLongPtr();

struct ScopedTaggingDisabler {
ScopedTaggingDisabler() { GetCurrentThread()->DisableTagging(); }
~ScopedTaggingDisabler() { GetCurrentThread()->EnableTagging(); }
};

}

#endif
