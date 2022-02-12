










#if !defined(TSAN_CLOCK_H)
#define TSAN_CLOCK_H

#include "tsan_defs.h"
#include "tsan_dense_alloc.h"

namespace __tsan {

typedef DenseSlabAlloc<ClockBlock, 1 << 22, 1 << 10> ClockAlloc;
typedef DenseSlabAllocCache ClockCache;


class SyncClock {
public:
SyncClock();
~SyncClock();

uptr size() const;


u64 get(unsigned tid) const;
u64 get_clean(unsigned tid) const;

void Resize(ClockCache *c, uptr nclk);
void Reset(ClockCache *c);

void DebugDump(int(*printf)(const char *s, ...));



class Iter {
public:
explicit Iter(SyncClock* parent);
Iter& operator++();
bool operator!=(const Iter& other);
ClockElem &operator*();

private:
SyncClock *parent_;

ClockElem *pos_;
ClockElem *end_;
int block_;

NOINLINE void Next();
};

Iter begin();
Iter end();

private:
friend class ThreadClock;
friend class Iter;
static const uptr kDirtyTids = 2;

struct Dirty {
u32 tid() const { return tid_ == kShortInvalidTid ? kInvalidTid : tid_; }
void set_tid(u32 tid) {
tid_ = tid == kInvalidTid ? kShortInvalidTid : tid;
}
u64 epoch : kClkBits;

private:

static const u64 kShortInvalidTid = (1ull << (64 - kClkBits)) - 1;
u64 tid_ : 64 - kClkBits;
};

static_assert(sizeof(Dirty) == 8, "Dirty is not 64bit");

unsigned release_store_tid_;
unsigned release_store_reused_;
Dirty dirty_[kDirtyTids];
































ClockBlock *tab_;
u32 tab_idx_;
u16 size_;
u16 blocks_;

void Unshare(ClockCache *c);
bool IsShared() const;
bool Cachable() const;
void ResetImpl();
void FlushDirty();
uptr capacity() const;
u32 get_block(uptr bi) const;
void append_block(u32 idx);
ClockElem &elem(unsigned tid) const;
};


class ThreadClock {
public:
typedef DenseSlabAllocCache Cache;

explicit ThreadClock(unsigned tid, unsigned reused = 0);

u64 get(unsigned tid) const;
void set(ClockCache *c, unsigned tid, u64 v);
void set(u64 v);
void tick();
uptr size() const;

void acquire(ClockCache *c, SyncClock *src);
void releaseStoreAcquire(ClockCache *c, SyncClock *src);
void release(ClockCache *c, SyncClock *dst);
void acq_rel(ClockCache *c, SyncClock *dst);
void ReleaseStore(ClockCache *c, SyncClock *dst);
void ResetCached(ClockCache *c);
void NoteGlobalAcquire(u64 v);

void DebugReset();
void DebugDump(int(*printf)(const char *s, ...));

private:
static const uptr kDirtyTids = SyncClock::kDirtyTids;

const unsigned tid_;
const unsigned reused_;

u64 last_acquire_;














































atomic_uint64_t global_acquire_;






u32 cached_idx_;
u16 cached_size_;
u16 cached_blocks_;


uptr nclk_;
u64 clk_[kMaxTidInClock];

bool IsAlreadyAcquired(const SyncClock *src) const;
bool HasAcquiredAfterRelease(const SyncClock *dst) const;
void UpdateCurrentThread(ClockCache *c, SyncClock *dst) const;
};

ALWAYS_INLINE u64 ThreadClock::get(unsigned tid) const {
DCHECK_LT(tid, kMaxTidInClock);
return clk_[tid];
}

ALWAYS_INLINE void ThreadClock::set(u64 v) {
DCHECK_GE(v, clk_[tid_]);
clk_[tid_] = v;
}

ALWAYS_INLINE void ThreadClock::tick() {
clk_[tid_]++;
}

ALWAYS_INLINE uptr ThreadClock::size() const {
return nclk_;
}

ALWAYS_INLINE void ThreadClock::NoteGlobalAcquire(u64 v) {



CHECK_LE(atomic_load_relaxed(&global_acquire_), v);
atomic_store_relaxed(&global_acquire_, v);
}

ALWAYS_INLINE SyncClock::Iter SyncClock::begin() {
return Iter(this);
}

ALWAYS_INLINE SyncClock::Iter SyncClock::end() {
return Iter(nullptr);
}

ALWAYS_INLINE uptr SyncClock::size() const {
return size_;
}

ALWAYS_INLINE SyncClock::Iter::Iter(SyncClock* parent)
: parent_(parent)
, pos_(nullptr)
, end_(nullptr)
, block_(-1) {
if (parent)
Next();
}

ALWAYS_INLINE SyncClock::Iter& SyncClock::Iter::operator++() {
pos_++;
if (UNLIKELY(pos_ >= end_))
Next();
return *this;
}

ALWAYS_INLINE bool SyncClock::Iter::operator!=(const SyncClock::Iter& other) {
return parent_ != other.parent_;
}

ALWAYS_INLINE ClockElem &SyncClock::Iter::operator*() {
return *pos_;
}
}

#endif
