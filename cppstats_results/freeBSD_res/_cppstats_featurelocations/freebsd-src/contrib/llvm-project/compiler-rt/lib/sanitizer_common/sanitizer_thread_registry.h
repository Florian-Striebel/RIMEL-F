












#if !defined(SANITIZER_THREAD_REGISTRY_H)
#define SANITIZER_THREAD_REGISTRY_H

#include "sanitizer_common.h"
#include "sanitizer_list.h"
#include "sanitizer_mutex.h"

namespace __sanitizer {

enum ThreadStatus {
ThreadStatusInvalid,
ThreadStatusCreated,
ThreadStatusRunning,
ThreadStatusFinished,
ThreadStatusDead
};

enum class ThreadType {
Regular,
Worker,
Fiber,
};



class ThreadContextBase {
public:
explicit ThreadContextBase(u32 tid);
const u32 tid;
u64 unique_id;
u32 reuse_count;
tid_t os_id;
uptr user_id;
char name[64];

ThreadStatus status;
bool detached;
ThreadType thread_type;

u32 parent_tid;
ThreadContextBase *next;

atomic_uint32_t thread_destroyed;

void SetName(const char *new_name);

void SetDead();
void SetJoined(void *arg);
void SetFinished();
void SetStarted(tid_t _os_id, ThreadType _thread_type, void *arg);
void SetCreated(uptr _user_id, u64 _unique_id, bool _detached,
u32 _parent_tid, void *arg);
void Reset();

void SetDestroyed();
bool GetDestroyed();




virtual void OnDead() {}
virtual void OnJoined(void *arg) {}
virtual void OnFinished() {}
virtual void OnStarted(void *arg) {}
virtual void OnCreated(void *arg) {}
virtual void OnReset() {}
virtual void OnDetached(void *arg) {}

protected:
~ThreadContextBase();
};

typedef ThreadContextBase* (*ThreadContextFactory)(u32 tid);

class MUTEX ThreadRegistry {
public:
ThreadRegistry(ThreadContextFactory factory);
ThreadRegistry(ThreadContextFactory factory, u32 max_threads,
u32 thread_quarantine_size, u32 max_reuse);
void GetNumberOfThreads(uptr *total = nullptr, uptr *running = nullptr,
uptr *alive = nullptr);
uptr GetMaxAliveThreads();

void Lock() ACQUIRE() { mtx_.Lock(); }
void CheckLocked() const CHECK_LOCKED() { mtx_.CheckLocked(); }
void Unlock() RELEASE() { mtx_.Unlock(); }


ThreadContextBase *GetThreadLocked(u32 tid) {
return threads_.empty() ? nullptr : threads_[tid];
}

u32 CreateThread(uptr user_id, bool detached, u32 parent_tid, void *arg);

typedef void (*ThreadCallback)(ThreadContextBase *tctx, void *arg);


void RunCallbackForEachThreadLocked(ThreadCallback cb, void *arg);

typedef bool (*FindThreadCallback)(ThreadContextBase *tctx, void *arg);


u32 FindThread(FindThreadCallback cb, void *arg);


ThreadContextBase *FindThreadContextLocked(FindThreadCallback cb,
void *arg);
ThreadContextBase *FindThreadContextByOsIDLocked(tid_t os_id);

void SetThreadName(u32 tid, const char *name);
void SetThreadNameByUserId(uptr user_id, const char *name);
void DetachThread(u32 tid, void *arg);
void JoinThread(u32 tid, void *arg);

ThreadStatus FinishThread(u32 tid);
void StartThread(u32 tid, tid_t os_id, ThreadType thread_type, void *arg);
void SetThreadUserId(u32 tid, uptr user_id);

private:
const ThreadContextFactory context_factory_;
const u32 max_threads_;
const u32 thread_quarantine_size_;
const u32 max_reuse_;

BlockingMutex mtx_;

u64 total_threads_;

uptr alive_threads_;
uptr max_alive_threads_;
uptr running_threads_;

InternalMmapVector<ThreadContextBase *> threads_;
IntrusiveList<ThreadContextBase> dead_threads_;
IntrusiveList<ThreadContextBase> invalid_threads_;

void QuarantinePush(ThreadContextBase *tctx);
ThreadContextBase *QuarantinePop();
};

typedef GenericScopedLock<ThreadRegistry> ThreadRegistryLock;

}

#endif
