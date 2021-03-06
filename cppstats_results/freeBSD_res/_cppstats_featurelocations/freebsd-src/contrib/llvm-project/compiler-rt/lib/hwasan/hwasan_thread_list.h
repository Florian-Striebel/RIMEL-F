












































#include "hwasan.h"
#include "hwasan_allocator.h"
#include "hwasan_flags.h"
#include "hwasan_thread.h"

#include "sanitizer_common/sanitizer_placement_new.h"

namespace __hwasan {

static uptr RingBufferSize() {
uptr desired_bytes = flags()->stack_history_size * sizeof(uptr);


for (int shift = 1; shift < 7; ++shift) {
uptr size = 4096 * (1ULL << shift);
if (size >= desired_bytes)
return size;
}
Printf("stack history size too large: %d\n", flags()->stack_history_size);
CHECK(0);
return 0;
}

struct ThreadStats {
uptr n_live_threads;
uptr total_stack_size;
};

class HwasanThreadList {
public:
HwasanThreadList(uptr storage, uptr size)
: free_space_(storage), free_space_end_(storage + size) {





ring_buffer_size_ = RingBufferSize();
thread_alloc_size_ =
RoundUpTo(ring_buffer_size_ + sizeof(Thread), ring_buffer_size_ * 2);
}

Thread *CreateCurrentThread(const Thread::InitState *state = nullptr) {
Thread *t = nullptr;
{
SpinMutexLock l(&free_list_mutex_);
if (!free_list_.empty()) {
t = free_list_.back();
free_list_.pop_back();
}
}
if (t) {
uptr start = (uptr)t - ring_buffer_size_;
internal_memset((void *)start, 0, ring_buffer_size_ + sizeof(Thread));
} else {
t = AllocThread();
}
{
SpinMutexLock l(&live_list_mutex_);
live_list_.push_back(t);
}
t->Init((uptr)t - ring_buffer_size_, ring_buffer_size_, state);
AddThreadStats(t);
return t;
}

void DontNeedThread(Thread *t) {
uptr start = (uptr)t - ring_buffer_size_;
ReleaseMemoryPagesToOS(start, start + thread_alloc_size_);
}

void RemoveThreadFromLiveList(Thread *t) {
SpinMutexLock l(&live_list_mutex_);
for (Thread *&t2 : live_list_)
if (t2 == t) {


t2 = live_list_.back();
live_list_.pop_back();
return;
}
CHECK(0 && "thread not found in live list");
}

void ReleaseThread(Thread *t) {
RemoveThreadStats(t);
t->Destroy();
DontNeedThread(t);
RemoveThreadFromLiveList(t);
SpinMutexLock l(&free_list_mutex_);
free_list_.push_back(t);
}

Thread *GetThreadByBufferAddress(uptr p) {
return (Thread *)(RoundDownTo(p, ring_buffer_size_ * 2) +
ring_buffer_size_);
}

uptr MemoryUsedPerThread() {
uptr res = sizeof(Thread) + ring_buffer_size_;
if (auto sz = flags()->heap_history_size)
res += HeapAllocationsRingBuffer::SizeInBytes(sz);
return res;
}

template <class CB>
void VisitAllLiveThreads(CB cb) {
SpinMutexLock l(&live_list_mutex_);
for (Thread *t : live_list_) cb(t);
}

void AddThreadStats(Thread *t) {
SpinMutexLock l(&stats_mutex_);
stats_.n_live_threads++;
stats_.total_stack_size += t->stack_size();
}

void RemoveThreadStats(Thread *t) {
SpinMutexLock l(&stats_mutex_);
stats_.n_live_threads--;
stats_.total_stack_size -= t->stack_size();
}

ThreadStats GetThreadStats() {
SpinMutexLock l(&stats_mutex_);
return stats_;
}

uptr GetRingBufferSize() const { return ring_buffer_size_; }

private:
Thread *AllocThread() {
SpinMutexLock l(&free_space_mutex_);
uptr align = ring_buffer_size_ * 2;
CHECK(IsAligned(free_space_, align));
Thread *t = (Thread *)(free_space_ + ring_buffer_size_);
free_space_ += thread_alloc_size_;
CHECK(free_space_ <= free_space_end_ && "out of thread memory");
return t;
}

SpinMutex free_space_mutex_;
uptr free_space_;
uptr free_space_end_;
uptr ring_buffer_size_;
uptr thread_alloc_size_;

SpinMutex free_list_mutex_;
InternalMmapVector<Thread *> free_list_;
SpinMutex live_list_mutex_;
InternalMmapVector<Thread *> live_list_;

ThreadStats stats_;
SpinMutex stats_mutex_;
};

void InitThreadList(uptr storage, uptr size);
HwasanThreadList &hwasanThreadList();

}
