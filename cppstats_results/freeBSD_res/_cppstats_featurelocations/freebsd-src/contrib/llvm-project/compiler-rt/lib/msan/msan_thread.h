











#if !defined(MSAN_THREAD_H)
#define MSAN_THREAD_H

#include "msan_allocator.h"
#include "sanitizer_common/sanitizer_common.h"

namespace __msan {

class MsanThread {
public:
static MsanThread *Create(thread_callback_t start_routine, void *arg);
static void TSDDtor(void *tsd);
void Destroy();

void Init();
thread_return_t ThreadStart();

uptr stack_top();
uptr stack_bottom();
uptr tls_begin() { return tls_begin_; }
uptr tls_end() { return tls_end_; }
bool IsMainThread() { return start_routine_ == nullptr; }

bool AddrIsInStack(uptr addr);

bool InSignalHandler() { return in_signal_handler_; }
void EnterSignalHandler() { in_signal_handler_++; }
void LeaveSignalHandler() { in_signal_handler_--; }

void StartSwitchFiber(uptr bottom, uptr size);
void FinishSwitchFiber(uptr *bottom_old, uptr *size_old);

MsanThreadLocalMallocStorage &malloc_storage() { return malloc_storage_; }

int destructor_iterations_;

private:


void SetThreadStackAndTls();
void ClearShadowForThreadStackAndTLS();
struct StackBounds {
uptr bottom;
uptr top;
};
StackBounds GetStackBounds() const;
thread_callback_t start_routine_;
void *arg_;

bool stack_switching_;

StackBounds stack_;
StackBounds next_stack_;

uptr tls_begin_;
uptr tls_end_;

unsigned in_signal_handler_;

MsanThreadLocalMallocStorage malloc_storage_;
};

MsanThread *GetCurrentThread();
void SetCurrentThread(MsanThread *t);

}

#endif
