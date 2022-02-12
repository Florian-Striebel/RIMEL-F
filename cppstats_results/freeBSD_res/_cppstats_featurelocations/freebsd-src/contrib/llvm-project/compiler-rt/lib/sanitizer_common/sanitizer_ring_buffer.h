










#if !defined(SANITIZER_RING_BUFFER_H)
#define SANITIZER_RING_BUFFER_H

#include "sanitizer_common.h"

namespace __sanitizer {



template<class T>
class RingBuffer {
public:
COMPILER_CHECK(sizeof(T) % sizeof(void *) == 0);
static RingBuffer *New(uptr Size) {
void *Ptr = MmapOrDie(SizeInBytes(Size), "RingBuffer");
RingBuffer *RB = reinterpret_cast<RingBuffer*>(Ptr);
uptr End = reinterpret_cast<uptr>(Ptr) + SizeInBytes(Size);
RB->last_ = RB->next_ = reinterpret_cast<T*>(End - sizeof(T));
return RB;
}
void Delete() {
UnmapOrDie(this, SizeInBytes(size()));
}
uptr size() const {
return last_ + 1 -
reinterpret_cast<T *>(reinterpret_cast<uptr>(this) +
2 * sizeof(T *));
}

static uptr SizeInBytes(uptr Size) {
return Size * sizeof(T) + 2 * sizeof(T*);
}

uptr SizeInBytes() { return SizeInBytes(size()); }

void push(T t) {
*next_ = t;
next_--;

if (next_ <= reinterpret_cast<T*>(&next_))
next_ = last_;
}

T operator[](uptr Idx) const {
CHECK_LT(Idx, size());
sptr IdxNext = Idx + 1;
if (IdxNext > last_ - next_)
IdxNext -= size();
return next_[IdxNext];
}

private:
RingBuffer() {}
~RingBuffer() {}
RingBuffer(const RingBuffer&) = delete;







T *last_;
T *next_;
T data_[1];
};




#if SANITIZER_WORDSIZE == 64
template <class T>
class CompactRingBuffer {


static constexpr int kPageSizeBits = 12;
static constexpr int kSizeShift = 56;
static constexpr uptr kNextMask = (1ULL << kSizeShift) - 1;

uptr GetStorageSize() const { return (long_ >> kSizeShift) << kPageSizeBits; }

void Init(void *storage, uptr size) {
CHECK_EQ(sizeof(CompactRingBuffer<T>), sizeof(void *));
CHECK(IsPowerOfTwo(size));
CHECK_GE(size, 1 << kPageSizeBits);
CHECK_LE(size, 128 << kPageSizeBits);
CHECK_EQ(size % 4096, 0);
CHECK_EQ(size % sizeof(T), 0);
CHECK_EQ((uptr)storage % (size * 2), 0);
long_ = (uptr)storage | ((size >> kPageSizeBits) << kSizeShift);
}

void SetNext(const T *next) {
long_ = (long_ & ~kNextMask) | (uptr)next;
}

public:
CompactRingBuffer(void *storage, uptr size) {
Init(storage, size);
}


CompactRingBuffer(const CompactRingBuffer &other, void *storage) {
uptr size = other.GetStorageSize();
internal_memcpy(storage, other.StartOfStorage(), size);
Init(storage, size);
uptr Idx = other.Next() - (const T *)other.StartOfStorage();
SetNext((const T *)storage + Idx);
}

T *Next() const { return (T *)(long_ & kNextMask); }

void *StartOfStorage() const {
return (void *)((uptr)Next() & ~(GetStorageSize() - 1));
}

void *EndOfStorage() const {
return (void *)((uptr)StartOfStorage() + GetStorageSize());
}

uptr size() const { return GetStorageSize() / sizeof(T); }

void push(T t) {
T *next = Next();
*next = t;
next++;
next = (T *)((uptr)next & ~GetStorageSize());
SetNext(next);
}

const T &operator[](uptr Idx) const {
CHECK_LT(Idx, size());
const T *Begin = (const T *)StartOfStorage();
sptr StorageIdx = Next() - Begin;
StorageIdx -= (sptr)(Idx + 1);
if (StorageIdx < 0)
StorageIdx += size();
return Begin[StorageIdx];
}

public:
~CompactRingBuffer() {}
CompactRingBuffer(const CompactRingBuffer &) = delete;

uptr long_;
};
#endif
}

#endif
