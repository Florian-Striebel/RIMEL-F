











#if !defined(SANITIZER_MUTEX_H)
#define SANITIZER_MUTEX_H

#include "sanitizer_atomic.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_libc.h"
#include "sanitizer_thread_safety.h"

namespace __sanitizer {

class MUTEX StaticSpinMutex {
public:
void Init() {
atomic_store(&state_, 0, memory_order_relaxed);
}

void Lock() ACQUIRE() {
if (LIKELY(TryLock()))
return;
LockSlow();
}

bool TryLock() TRY_ACQUIRE(true) {
return atomic_exchange(&state_, 1, memory_order_acquire) == 0;
}

void Unlock() RELEASE() { atomic_store(&state_, 0, memory_order_release); }

void CheckLocked() const CHECK_LOCKED() {
CHECK_EQ(atomic_load(&state_, memory_order_relaxed), 1);
}

private:
atomic_uint8_t state_;

void LockSlow();
};

class MUTEX SpinMutex : public StaticSpinMutex {
public:
SpinMutex() {
Init();
}

SpinMutex(const SpinMutex &) = delete;
void operator=(const SpinMutex &) = delete;
};




class Semaphore {
public:
constexpr Semaphore() {}
Semaphore(const Semaphore &) = delete;
void operator=(const Semaphore &) = delete;

void Wait();
void Post(u32 count = 1);

private:
atomic_uint32_t state_ = {0};
};

typedef int MutexType;

enum {


MutexInvalid = 0,
MutexThreadRegistry,

MutexLastCommon,

MutexUnchecked = -1,



MutexLeaf = -1,

MutexMulti = -3,
};



#define SANITIZER_CHECK_DEADLOCKS (SANITIZER_DEBUG && !SANITIZER_GO)

#if SANITIZER_CHECK_DEADLOCKS
struct MutexMeta {
MutexType type;
const char *name;




MutexType can_lock[10];
};
#endif

class CheckedMutex {
public:
constexpr CheckedMutex(MutexType type)
#if SANITIZER_CHECK_DEADLOCKS
: type_(type)
#endif
{
}

ALWAYS_INLINE void Lock() {
#if SANITIZER_CHECK_DEADLOCKS
LockImpl(GET_CALLER_PC());
#endif
}

ALWAYS_INLINE void Unlock() {
#if SANITIZER_CHECK_DEADLOCKS
UnlockImpl();
#endif
}



static void CheckNoLocks() {
#if SANITIZER_CHECK_DEADLOCKS
CheckNoLocksImpl();
#endif
}

private:
#if SANITIZER_CHECK_DEADLOCKS
const MutexType type_;

void LockImpl(uptr pc);
void UnlockImpl();
static void CheckNoLocksImpl();
#endif
};





class MUTEX Mutex : CheckedMutex {
public:
constexpr Mutex(MutexType type = MutexUnchecked) : CheckedMutex(type) {}

void Lock() ACQUIRE() {
CheckedMutex::Lock();
u64 reset_mask = ~0ull;
u64 state = atomic_load_relaxed(&state_);
const uptr kMaxSpinIters = 1500;
for (uptr spin_iters = 0;; spin_iters++) {
u64 new_state;
bool locked = (state & (kWriterLock | kReaderLockMask)) != 0;
if (LIKELY(!locked)) {

new_state = (state | kWriterLock) & reset_mask;
} else if (spin_iters > kMaxSpinIters) {


new_state = (state + kWaitingWriterInc) & reset_mask;
} else if ((state & kWriterSpinWait) == 0) {


new_state = state | kWriterSpinWait;
} else {

state = atomic_load(&state_, memory_order_relaxed);
continue;
}
if (UNLIKELY(!atomic_compare_exchange_weak(&state_, &state, new_state,
memory_order_acquire)))
continue;
if (LIKELY(!locked))
return;
if (spin_iters > kMaxSpinIters) {

writers_.Wait();
spin_iters = 0;
state = atomic_load(&state_, memory_order_relaxed);
DCHECK_NE(state & kWriterSpinWait, 0);
} else {

}




reset_mask = ~kWriterSpinWait;
}
}

void Unlock() RELEASE() {
CheckedMutex::Unlock();
bool wake_writer;
u64 wake_readers;
u64 new_state;
u64 state = atomic_load_relaxed(&state_);
do {
DCHECK_NE(state & kWriterLock, 0);
DCHECK_EQ(state & kReaderLockMask, 0);
new_state = state & ~kWriterLock;
wake_writer =
(state & kWriterSpinWait) == 0 && (state & kWaitingWriterMask) != 0;
if (wake_writer)
new_state = (new_state - kWaitingWriterInc) | kWriterSpinWait;
wake_readers =
(state & (kWriterSpinWait | kWaitingWriterMask)) != 0
? 0
: ((state & kWaitingReaderMask) >> kWaitingReaderShift);
if (wake_readers)
new_state = (new_state & ~kWaitingReaderMask) +
(wake_readers << kReaderLockShift);
} while (UNLIKELY(!atomic_compare_exchange_weak(&state_, &state, new_state,
memory_order_release)));
if (UNLIKELY(wake_writer))
writers_.Post();
else if (UNLIKELY(wake_readers))
readers_.Post(wake_readers);
}

void ReadLock() ACQUIRE_SHARED() {
CheckedMutex::Lock();
bool locked;
u64 new_state;
u64 state = atomic_load_relaxed(&state_);
do {
locked =
(state & kReaderLockMask) == 0 &&
(state & (kWriterLock | kWriterSpinWait | kWaitingWriterMask)) != 0;
if (LIKELY(!locked))
new_state = state + kReaderLockInc;
else
new_state = state + kWaitingReaderInc;
} while (UNLIKELY(!atomic_compare_exchange_weak(&state_, &state, new_state,
memory_order_acquire)));
if (UNLIKELY(locked))
readers_.Wait();
DCHECK_EQ(atomic_load_relaxed(&state_) & kWriterLock, 0);
DCHECK_NE(atomic_load_relaxed(&state_) & kReaderLockMask, 0);
}

void ReadUnlock() RELEASE_SHARED() {
CheckedMutex::Unlock();
bool wake;
u64 new_state;
u64 state = atomic_load_relaxed(&state_);
do {
DCHECK_NE(state & kReaderLockMask, 0);
DCHECK_EQ(state & (kWaitingReaderMask | kWriterLock), 0);
new_state = state - kReaderLockInc;
wake = (new_state & (kReaderLockMask | kWriterSpinWait)) == 0 &&
(new_state & kWaitingWriterMask) != 0;
if (wake)
new_state = (new_state - kWaitingWriterInc) | kWriterSpinWait;
} while (UNLIKELY(!atomic_compare_exchange_weak(&state_, &state, new_state,
memory_order_release)));
if (UNLIKELY(wake))
writers_.Post();
}







void CheckWriteLocked() const CHECK_LOCKED() {
CHECK(atomic_load(&state_, memory_order_relaxed) & kWriterLock);
}

void CheckLocked() const CHECK_LOCKED() { CheckWriteLocked(); }

void CheckReadLocked() const CHECK_LOCKED() {
CHECK(atomic_load(&state_, memory_order_relaxed) & kReaderLockMask);
}

private:
atomic_uint64_t state_ = {0};
Semaphore writers_;
Semaphore readers_;
























static constexpr u64 kCounterWidth = 20;
static constexpr u64 kReaderLockShift = 0;
static constexpr u64 kReaderLockInc = 1ull << kReaderLockShift;
static constexpr u64 kReaderLockMask = ((1ull << kCounterWidth) - 1)
<< kReaderLockShift;
static constexpr u64 kWaitingReaderShift = kCounterWidth;
static constexpr u64 kWaitingReaderInc = 1ull << kWaitingReaderShift;
static constexpr u64 kWaitingReaderMask = ((1ull << kCounterWidth) - 1)
<< kWaitingReaderShift;
static constexpr u64 kWaitingWriterShift = 2 * kCounterWidth;
static constexpr u64 kWaitingWriterInc = 1ull << kWaitingWriterShift;
static constexpr u64 kWaitingWriterMask = ((1ull << kCounterWidth) - 1)
<< kWaitingWriterShift;
static constexpr u64 kWriterLock = 1ull << (3 * kCounterWidth);
static constexpr u64 kWriterSpinWait = 1ull << (3 * kCounterWidth + 1);

Mutex(const Mutex &) = delete;
void operator=(const Mutex &) = delete;
};

void FutexWait(atomic_uint32_t *p, u32 cmp);
void FutexWake(atomic_uint32_t *p, u32 count);

class MUTEX BlockingMutex {
public:
explicit constexpr BlockingMutex(LinkerInitialized)
: opaque_storage_ {0, }, owner_ {0} {}
BlockingMutex();
void Lock() ACQUIRE();
void Unlock() RELEASE();








void CheckLocked() const CHECK_LOCKED();

private:

ALIGNED(8) uptr opaque_storage_[10];
uptr owner_;
};


class MUTEX RWMutex {
public:
RWMutex() {
atomic_store(&state_, kUnlocked, memory_order_relaxed);
}

~RWMutex() {
CHECK_EQ(atomic_load(&state_, memory_order_relaxed), kUnlocked);
}

void Lock() ACQUIRE() {
u32 cmp = kUnlocked;
if (atomic_compare_exchange_strong(&state_, &cmp, kWriteLock,
memory_order_acquire))
return;
LockSlow();
}

void Unlock() RELEASE() {
u32 prev = atomic_fetch_sub(&state_, kWriteLock, memory_order_release);
DCHECK_NE(prev & kWriteLock, 0);
(void)prev;
}

void ReadLock() ACQUIRE_SHARED() {
u32 prev = atomic_fetch_add(&state_, kReadLock, memory_order_acquire);
if ((prev & kWriteLock) == 0)
return;
ReadLockSlow();
}

void ReadUnlock() RELEASE_SHARED() {
u32 prev = atomic_fetch_sub(&state_, kReadLock, memory_order_release);
DCHECK_EQ(prev & kWriteLock, 0);
DCHECK_GT(prev & ~kWriteLock, 0);
(void)prev;
}

void CheckLocked() const CHECK_LOCKED() {
CHECK_NE(atomic_load(&state_, memory_order_relaxed), kUnlocked);
}

private:
atomic_uint32_t state_;

enum {
kUnlocked = 0,
kWriteLock = 1,
kReadLock = 2
};

void NOINLINE LockSlow() {
for (int i = 0;; i++) {
if (i < 10)
proc_yield(10);
else
internal_sched_yield();
u32 cmp = atomic_load(&state_, memory_order_relaxed);
if (cmp == kUnlocked &&
atomic_compare_exchange_weak(&state_, &cmp, kWriteLock,
memory_order_acquire))
return;
}
}

void NOINLINE ReadLockSlow() {
for (int i = 0;; i++) {
if (i < 10)
proc_yield(10);
else
internal_sched_yield();
u32 prev = atomic_load(&state_, memory_order_acquire);
if ((prev & kWriteLock) == 0)
return;
}
}

RWMutex(const RWMutex &) = delete;
void operator=(const RWMutex &) = delete;
};

template <typename MutexType>
class SCOPED_LOCK GenericScopedLock {
public:
explicit GenericScopedLock(MutexType *mu) ACQUIRE(mu) : mu_(mu) {
mu_->Lock();
}

~GenericScopedLock() RELEASE() { mu_->Unlock(); }

private:
MutexType *mu_;

GenericScopedLock(const GenericScopedLock &) = delete;
void operator=(const GenericScopedLock &) = delete;
};

template <typename MutexType>
class SCOPED_LOCK GenericScopedReadLock {
public:
explicit GenericScopedReadLock(MutexType *mu) ACQUIRE(mu) : mu_(mu) {
mu_->ReadLock();
}

~GenericScopedReadLock() RELEASE() { mu_->ReadUnlock(); }

private:
MutexType *mu_;

GenericScopedReadLock(const GenericScopedReadLock &) = delete;
void operator=(const GenericScopedReadLock &) = delete;
};

typedef GenericScopedLock<StaticSpinMutex> SpinMutexLock;
typedef GenericScopedLock<BlockingMutex> BlockingMutexLock;
typedef GenericScopedLock<RWMutex> RWMutexLock;
typedef GenericScopedReadLock<RWMutex> RWMutexReadLock;
typedef GenericScopedLock<Mutex> Lock;
typedef GenericScopedReadLock<Mutex> ReadLock;

}

#endif
