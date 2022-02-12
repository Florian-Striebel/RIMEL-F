










#if !defined(TSAN_SYNC_H)
#define TSAN_SYNC_H

#include "sanitizer_common/sanitizer_atomic.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_deadlock_detector_interface.h"
#include "tsan_defs.h"
#include "tsan_clock.h"
#include "tsan_dense_alloc.h"

namespace __tsan {



enum MutexFlags {
MutexFlagLinkerInit = 1 << 0,
MutexFlagWriteReentrant = 1 << 1,
MutexFlagReadReentrant = 1 << 2,
MutexFlagReadLock = 1 << 3,
MutexFlagTryLock = 1 << 4,
MutexFlagTryLockFailed = 1 << 5,
MutexFlagRecursiveLock = 1 << 6,
MutexFlagRecursiveUnlock = 1 << 7,
MutexFlagNotStatic = 1 << 8,



MutexFlagBroken = 1 << 30,

MutexFlagDoPreLockOnPostLock = 1 << 29,

MutexCreationFlagMask = MutexFlagLinkerInit |
MutexFlagWriteReentrant |
MutexFlagReadReentrant |
MutexFlagNotStatic,
};

struct SyncVar {
SyncVar();

uptr addr;
Mutex mtx;
u64 uid;
u32 creation_stack_id;
u32 owner_tid;
u64 last_lock;
int recursion;
atomic_uint32_t flags;
u32 next;
DDMutex dd;
SyncClock read_clock;


SyncClock clock;

void Init(ThreadState *thr, uptr pc, uptr addr, u64 uid);
void Reset(Processor *proc);

u64 GetId() const {

return GetLsb((u64)addr | (uid << 48), 60);
}
bool CheckId(u64 uid) const {
CHECK_EQ(uid, GetLsb(uid, 14));
return GetLsb(this->uid, 14) == uid;
}
static uptr SplitId(u64 id, u64 *uid) {
*uid = id >> 48;
return (uptr)GetLsb(id, 48);
}

bool IsFlagSet(u32 f) const {
return atomic_load_relaxed(&flags) & f;
}

void SetFlags(u32 f) {
atomic_store_relaxed(&flags, atomic_load_relaxed(&flags) | f);
}

void UpdateFlags(u32 flagz) {

if (!(flagz & MutexCreationFlagMask))
return;
u32 current = atomic_load_relaxed(&flags);
if (current & MutexCreationFlagMask)
return;


atomic_store_relaxed(&flags, current | (flagz & MutexCreationFlagMask));
}
};





class MetaMap {
public:
MetaMap();

void AllocBlock(ThreadState *thr, uptr pc, uptr p, uptr sz);
uptr FreeBlock(Processor *proc, uptr p);
bool FreeRange(Processor *proc, uptr p, uptr sz);
void ResetRange(Processor *proc, uptr p, uptr sz);
MBlock* GetBlock(uptr p);

SyncVar* GetOrCreateAndLock(ThreadState *thr, uptr pc,
uptr addr, bool write_lock);
SyncVar* GetIfExistsAndLock(uptr addr, bool write_lock);

void MoveMemory(uptr src, uptr dst, uptr sz);

void OnProcIdle(Processor *proc);

private:
static const u32 kFlagMask = 3u << 30;
static const u32 kFlagBlock = 1u << 30;
static const u32 kFlagSync = 2u << 30;
typedef DenseSlabAlloc<MBlock, 1 << 18, 1 << 12, kFlagMask> BlockAlloc;
typedef DenseSlabAlloc<SyncVar, 1 << 20, 1 << 10, kFlagMask> SyncAlloc;
BlockAlloc block_alloc_;
SyncAlloc sync_alloc_;
atomic_uint64_t uid_gen_;

SyncVar* GetAndLock(ThreadState *thr, uptr pc, uptr addr, bool write_lock,
bool create);
};

}

#endif
