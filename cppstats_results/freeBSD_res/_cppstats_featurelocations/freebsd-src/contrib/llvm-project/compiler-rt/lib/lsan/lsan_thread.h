












#if !defined(LSAN_THREAD_H)
#define LSAN_THREAD_H

#include "sanitizer_common/sanitizer_thread_registry.h"

namespace __lsan {

class ThreadContextLsanBase : public ThreadContextBase {
public:
explicit ThreadContextLsanBase(int tid);
void OnFinished() override;
uptr stack_begin() { return stack_begin_; }
uptr stack_end() { return stack_end_; }
uptr cache_begin() { return cache_begin_; }
uptr cache_end() { return cache_end_; }


static void ThreadStart(u32 tid, tid_t os_id, ThreadType thread_type,
void *onstarted_arg);

protected:
~ThreadContextLsanBase() {}
uptr stack_begin_ = 0;
uptr stack_end_ = 0;
uptr cache_begin_ = 0;
uptr cache_end_ = 0;
};


class ThreadContext;

void InitializeThreadRegistry();
void InitializeMainThread();

u32 ThreadCreate(u32 tid, uptr uid, bool detached, void *arg = nullptr);
void ThreadFinish();
void ThreadDetach(u32 tid);
void ThreadJoin(u32 tid);
u32 ThreadTid(uptr uid);

u32 GetCurrentThread();
void SetCurrentThread(u32 tid);
ThreadContext *CurrentThreadContext();
void EnsureMainThreadIDIsCorrect();

}

#endif
