












#if !defined(ASAN_FAKE_STACK_H)
#define ASAN_FAKE_STACK_H

#include "sanitizer_common/sanitizer_common.h"

namespace __asan {


struct FakeFrame {
uptr magic;
uptr descr;
uptr pc;
uptr real_stack;
};




























class FakeStack {
static const uptr kMinStackFrameSizeLog = 6;
static const uptr kMaxStackFrameSizeLog = 16;

public:
static const uptr kNumberOfSizeClasses =
kMaxStackFrameSizeLog - kMinStackFrameSizeLog + 1;


static FakeStack *Create(uptr stack_size_log);

void Destroy(int tid);


static uptr SizeRequiredForFlags(uptr stack_size_log) {
return ((uptr)1) << (stack_size_log + 1 - kMinStackFrameSizeLog);
}


static uptr SizeRequiredForFrames(uptr stack_size_log) {
return (((uptr)1) << stack_size_log) * kNumberOfSizeClasses;
}


static uptr RequiredSize(uptr stack_size_log) {
return kFlagsOffset + SizeRequiredForFlags(stack_size_log) +
SizeRequiredForFrames(stack_size_log);
}







static uptr FlagsOffset(uptr stack_size_log, uptr class_id) {
uptr t = kNumberOfSizeClasses - 1 - class_id;
const uptr all_ones = (((uptr)1) << (kNumberOfSizeClasses - 1)) - 1;
return ((all_ones >> t) << t) << (stack_size_log - 15);
}

static uptr NumberOfFrames(uptr stack_size_log, uptr class_id) {
return ((uptr)1) << (stack_size_log - kMinStackFrameSizeLog - class_id);
}


static uptr ModuloNumberOfFrames(uptr stack_size_log, uptr class_id, uptr n) {
return n & (NumberOfFrames(stack_size_log, class_id) - 1);
}


u8 *GetFlags(uptr stack_size_log, uptr class_id) {
return reinterpret_cast<u8 *>(this) + kFlagsOffset +
FlagsOffset(stack_size_log, class_id);
}


u8 *GetFrame(uptr stack_size_log, uptr class_id, uptr pos) {
return reinterpret_cast<u8 *>(this) + kFlagsOffset +
SizeRequiredForFlags(stack_size_log) +
(((uptr)1) << stack_size_log) * class_id +
BytesInSizeClass(class_id) * pos;
}


FakeFrame *Allocate(uptr stack_size_log, uptr class_id, uptr real_stack);


static void Deallocate(uptr x, uptr class_id) {
**SavedFlagPtr(x, class_id) = 0;
}


void PoisonAll(u8 magic);


uptr AddrIsInFakeStack(uptr addr, uptr *frame_beg, uptr *frame_end);
USED uptr AddrIsInFakeStack(uptr addr) {
uptr t1, t2;
return AddrIsInFakeStack(addr, &t1, &t2);
}


static uptr BytesInSizeClass(uptr class_id) {
return ((uptr)1) << (class_id + kMinStackFrameSizeLog);
}




static u8 **SavedFlagPtr(uptr x, uptr class_id) {
return reinterpret_cast<u8 **>(x + BytesInSizeClass(class_id) - sizeof(x));
}

uptr stack_size_log() const { return stack_size_log_; }

void HandleNoReturn();
void GC(uptr real_stack);

void ForEachFakeFrame(RangeIteratorCallback callback, void *arg);

private:
FakeStack() { }
static const uptr kFlagsOffset = 4096;

COMPILER_CHECK(kNumberOfSizeClasses == 11);
static const uptr kMaxStackMallocSize = ((uptr)1) << kMaxStackFrameSizeLog;

uptr hint_position_[kNumberOfSizeClasses];
uptr stack_size_log_;

bool needs_gc_;
};

FakeStack *GetTLSFakeStack();
void SetTLSFakeStack(FakeStack *fs);

}

#endif
