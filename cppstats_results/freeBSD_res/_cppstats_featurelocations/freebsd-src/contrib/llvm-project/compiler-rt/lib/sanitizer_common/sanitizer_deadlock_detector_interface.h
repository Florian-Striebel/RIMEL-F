













#if !defined(SANITIZER_DEADLOCK_DETECTOR_INTERFACE_H)
#define SANITIZER_DEADLOCK_DETECTOR_INTERFACE_H

#if !defined(SANITIZER_DEADLOCK_DETECTOR_VERSION)
#define SANITIZER_DEADLOCK_DETECTOR_VERSION 1
#endif

#include "sanitizer_internal_defs.h"
#include "sanitizer_atomic.h"

namespace __sanitizer {





struct DDPhysicalThread;
struct DDLogicalThread;

struct DDMutex {
#if SANITIZER_DEADLOCK_DETECTOR_VERSION == 1
uptr id;
u32 stk;
#elif SANITIZER_DEADLOCK_DETECTOR_VERSION == 2
u32 id;
u32 recursion;
atomic_uintptr_t owner;
#else
#error "BAD SANITIZER_DEADLOCK_DETECTOR_VERSION"
#endif
u64 ctx;
};

struct DDFlags {
bool second_deadlock_stack;
};

struct DDReport {
enum { kMaxLoopSize = 20 };
int n;
struct {
u64 thr_ctx;
u64 mtx_ctx0;
u64 mtx_ctx1;
u32 stk[2];
} loop[kMaxLoopSize];
};

struct DDCallback {
DDPhysicalThread *pt;
DDLogicalThread *lt;

virtual u32 Unwind() { return 0; }
virtual int UniqueTid() { return 0; }

protected:
~DDCallback() {}
};

struct DDetector {
static DDetector *Create(const DDFlags *flags);

virtual DDPhysicalThread* CreatePhysicalThread() { return nullptr; }
virtual void DestroyPhysicalThread(DDPhysicalThread *pt) {}

virtual DDLogicalThread* CreateLogicalThread(u64 ctx) { return nullptr; }
virtual void DestroyLogicalThread(DDLogicalThread *lt) {}

virtual void MutexInit(DDCallback *cb, DDMutex *m) {}
virtual void MutexBeforeLock(DDCallback *cb, DDMutex *m, bool wlock) {}
virtual void MutexAfterLock(DDCallback *cb, DDMutex *m, bool wlock,
bool trylock) {}
virtual void MutexBeforeUnlock(DDCallback *cb, DDMutex *m, bool wlock) {}
virtual void MutexDestroy(DDCallback *cb, DDMutex *m) {}

virtual DDReport *GetReport(DDCallback *cb) { return nullptr; }

protected:
~DDetector() {}
};

}

#endif
