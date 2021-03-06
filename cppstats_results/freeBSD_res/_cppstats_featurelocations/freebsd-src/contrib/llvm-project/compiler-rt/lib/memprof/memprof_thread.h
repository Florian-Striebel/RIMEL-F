












#if !defined(MEMPROF_THREAD_H)
#define MEMPROF_THREAD_H

#include "memprof_allocator.h"
#include "memprof_internal.h"
#include "memprof_stats.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_libc.h"
#include "sanitizer_common/sanitizer_thread_registry.h"

namespace __sanitizer {
struct DTLS;
}

namespace __memprof {

class MemprofThread;



struct MemprofThreadContext final : public ThreadContextBase {
explicit MemprofThreadContext(int tid)
: ThreadContextBase(tid), announced(false),
destructor_iterations(GetPthreadDestructorIterations()), stack_id(0),
thread(nullptr) {}
bool announced;
u8 destructor_iterations;
u32 stack_id;
MemprofThread *thread;

void OnCreated(void *arg) override;
void OnFinished() override;

struct CreateThreadContextArgs {
MemprofThread *thread;
StackTrace *stack;
};
};


COMPILER_CHECK(sizeof(MemprofThreadContext) <= 256);


class MemprofThread {
public:
static MemprofThread *Create(thread_callback_t start_routine, void *arg,
u32 parent_tid, StackTrace *stack,
bool detached);
static void TSDDtor(void *tsd);
void Destroy();

struct InitOptions;
void Init(const InitOptions *options = nullptr);

thread_return_t ThreadStart(tid_t os_id,
atomic_uintptr_t *signal_thread_is_registered);

uptr stack_top();
uptr stack_bottom();
uptr stack_size();
uptr tls_begin() { return tls_begin_; }
uptr tls_end() { return tls_end_; }
DTLS *dtls() { return dtls_; }
u32 tid() { return context_->tid; }
MemprofThreadContext *context() { return context_; }
void set_context(MemprofThreadContext *context) { context_ = context; }

bool AddrIsInStack(uptr addr);




bool isUnwinding() const { return unwinding_; }
void setUnwinding(bool b) { unwinding_ = b; }

MemprofThreadLocalMallocStorage &malloc_storage() { return malloc_storage_; }
MemprofStats &stats() { return stats_; }

private:



void SetThreadStackAndTls(const InitOptions *options);

struct StackBounds {
uptr bottom;
uptr top;
};
StackBounds GetStackBounds() const;

MemprofThreadContext *context_;
thread_callback_t start_routine_;
void *arg_;

uptr stack_top_;
uptr stack_bottom_;

uptr tls_begin_;
uptr tls_end_;
DTLS *dtls_;

MemprofThreadLocalMallocStorage malloc_storage_;
MemprofStats stats_;
bool unwinding_;
};


ThreadRegistry &memprofThreadRegistry();


MemprofThreadContext *GetThreadContextByTidLocked(u32 tid);


MemprofThread *GetCurrentThread();
void SetCurrentThread(MemprofThread *t);
u32 GetCurrentTidOrInvalid();


void EnsureMainThreadIDIsCorrect();
}

#endif
