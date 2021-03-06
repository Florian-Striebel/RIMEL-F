













#if !defined(XRAY_SEGMENTED_ARRAY_H)
#define XRAY_SEGMENTED_ARRAY_H

#include "sanitizer_common/sanitizer_allocator.h"
#include "xray_allocator.h"
#include "xray_utils.h"
#include <cassert>
#include <type_traits>
#include <utility>

namespace __xray {







template <class T> class Array {
struct Segment {
Segment *Prev;
Segment *Next;
char Data[1];
};

public:


















static constexpr uint64_t AlignedElementStorageSize =
sizeof(typename std::aligned_storage<sizeof(T), alignof(T)>::type);

static constexpr uint64_t SegmentControlBlockSize = sizeof(Segment *) * 2;

static constexpr uint64_t SegmentSize = nearest_boundary(
SegmentControlBlockSize + next_pow2(sizeof(T)), kCacheLineSize);

using AllocatorType = Allocator<SegmentSize>;

static constexpr uint64_t ElementsPerSegment =
(SegmentSize - SegmentControlBlockSize) / next_pow2(sizeof(T));

static_assert(ElementsPerSegment > 0,
"Must have at least 1 element per segment.");

static Segment SentinelSegment;

using size_type = uint64_t;

private:

template <class U> class Iterator {
Segment *S = &SentinelSegment;
uint64_t Offset = 0;
uint64_t Size = 0;

public:
Iterator(Segment *IS, uint64_t Off, uint64_t S) XRAY_NEVER_INSTRUMENT
: S(IS),
Offset(Off),
Size(S) {}
Iterator(const Iterator &) NOEXCEPT XRAY_NEVER_INSTRUMENT = default;
Iterator() NOEXCEPT XRAY_NEVER_INSTRUMENT = default;
Iterator(Iterator &&) NOEXCEPT XRAY_NEVER_INSTRUMENT = default;
Iterator &operator=(const Iterator &) XRAY_NEVER_INSTRUMENT = default;
Iterator &operator=(Iterator &&) XRAY_NEVER_INSTRUMENT = default;
~Iterator() XRAY_NEVER_INSTRUMENT = default;

Iterator &operator++() XRAY_NEVER_INSTRUMENT {
if (++Offset % ElementsPerSegment || Offset == Size)
return *this;



DCHECK_EQ(Offset % ElementsPerSegment, 0);
DCHECK_NE(Offset, Size);
DCHECK_NE(S, &SentinelSegment);
DCHECK_NE(S->Next, &SentinelSegment);
S = S->Next;
DCHECK_NE(S, &SentinelSegment);
return *this;
}

Iterator &operator--() XRAY_NEVER_INSTRUMENT {
DCHECK_NE(S, &SentinelSegment);
DCHECK_GT(Offset, 0);

auto PreviousOffset = Offset--;
if (PreviousOffset != Size && PreviousOffset % ElementsPerSegment == 0) {
DCHECK_NE(S->Prev, &SentinelSegment);
S = S->Prev;
}

return *this;
}

Iterator operator++(int) XRAY_NEVER_INSTRUMENT {
Iterator Copy(*this);
++(*this);
return Copy;
}

Iterator operator--(int) XRAY_NEVER_INSTRUMENT {
Iterator Copy(*this);
--(*this);
return Copy;
}

template <class V, class W>
friend bool operator==(const Iterator<V> &L,
const Iterator<W> &R) XRAY_NEVER_INSTRUMENT {
return L.S == R.S && L.Offset == R.Offset;
}

template <class V, class W>
friend bool operator!=(const Iterator<V> &L,
const Iterator<W> &R) XRAY_NEVER_INSTRUMENT {
return !(L == R);
}

U &operator*() const XRAY_NEVER_INSTRUMENT {
DCHECK_NE(S, &SentinelSegment);
auto RelOff = Offset % ElementsPerSegment;



auto Base = &S->Data;
auto AlignedOffset = Base + (RelOff * AlignedElementStorageSize);
return *reinterpret_cast<U *>(AlignedOffset);
}

U *operator->() const XRAY_NEVER_INSTRUMENT { return &(**this); }
};

AllocatorType *Alloc;
Segment *Head;
Segment *Tail;



Segment *Freelist;
uint64_t Size;
































Segment *NewSegment() XRAY_NEVER_INSTRUMENT {




if (Freelist != &SentinelSegment) {












DCHECK_EQ(Freelist->Prev, &SentinelSegment);










auto *FreeSegment = Freelist;
Freelist = Freelist->Next;





if (Freelist != &SentinelSegment)
Freelist->Prev = &SentinelSegment;

FreeSegment->Next = &SentinelSegment;
FreeSegment->Prev = &SentinelSegment;


DCHECK_EQ(Freelist->Prev, &SentinelSegment);
DCHECK_NE(FreeSegment, &SentinelSegment);
return FreeSegment;
}

auto SegmentBlock = Alloc->Allocate();
if (SegmentBlock.Data == nullptr)
return nullptr;


new (SegmentBlock.Data) Segment{&SentinelSegment, &SentinelSegment, {0}};
auto SB = reinterpret_cast<Segment *>(SegmentBlock.Data);
return SB;
}

Segment *InitHeadAndTail() XRAY_NEVER_INSTRUMENT {
DCHECK_EQ(Head, &SentinelSegment);
DCHECK_EQ(Tail, &SentinelSegment);
auto S = NewSegment();
if (S == nullptr)
return nullptr;
DCHECK_EQ(S->Next, &SentinelSegment);
DCHECK_EQ(S->Prev, &SentinelSegment);
DCHECK_NE(S, &SentinelSegment);
Head = S;
Tail = S;
DCHECK_EQ(Head, Tail);
DCHECK_EQ(Tail->Next, &SentinelSegment);
DCHECK_EQ(Tail->Prev, &SentinelSegment);
return S;
}

Segment *AppendNewSegment() XRAY_NEVER_INSTRUMENT {
auto S = NewSegment();
if (S == nullptr)
return nullptr;
DCHECK_NE(Tail, &SentinelSegment);
DCHECK_EQ(Tail->Next, &SentinelSegment);
DCHECK_EQ(S->Prev, &SentinelSegment);
DCHECK_EQ(S->Next, &SentinelSegment);
S->Prev = Tail;
Tail->Next = S;
Tail = S;
DCHECK_EQ(S, S->Prev->Next);
DCHECK_EQ(Tail->Next, &SentinelSegment);
return S;
}

public:
explicit Array(AllocatorType &A) XRAY_NEVER_INSTRUMENT
: Alloc(&A),
Head(&SentinelSegment),
Tail(&SentinelSegment),
Freelist(&SentinelSegment),
Size(0) {}

Array() XRAY_NEVER_INSTRUMENT : Alloc(nullptr),
Head(&SentinelSegment),
Tail(&SentinelSegment),
Freelist(&SentinelSegment),
Size(0) {}

Array(const Array &) = delete;
Array &operator=(const Array &) = delete;

Array(Array &&O) XRAY_NEVER_INSTRUMENT : Alloc(O.Alloc),
Head(O.Head),
Tail(O.Tail),
Freelist(O.Freelist),
Size(O.Size) {
O.Alloc = nullptr;
O.Head = &SentinelSegment;
O.Tail = &SentinelSegment;
O.Size = 0;
O.Freelist = &SentinelSegment;
}

Array &operator=(Array &&O) XRAY_NEVER_INSTRUMENT {
Alloc = O.Alloc;
O.Alloc = nullptr;
Head = O.Head;
O.Head = &SentinelSegment;
Tail = O.Tail;
O.Tail = &SentinelSegment;
Freelist = O.Freelist;
O.Freelist = &SentinelSegment;
Size = O.Size;
O.Size = 0;
return *this;
}

~Array() XRAY_NEVER_INSTRUMENT {
for (auto &E : *this)
(&E)->~T();
}

bool empty() const XRAY_NEVER_INSTRUMENT { return Size == 0; }

AllocatorType &allocator() const XRAY_NEVER_INSTRUMENT {
DCHECK_NE(Alloc, nullptr);
return *Alloc;
}

uint64_t size() const XRAY_NEVER_INSTRUMENT { return Size; }

template <class... Args>
T *AppendEmplace(Args &&... args) XRAY_NEVER_INSTRUMENT {
DCHECK((Size == 0 && Head == &SentinelSegment && Head == Tail) ||
(Size != 0 && Head != &SentinelSegment && Tail != &SentinelSegment));
if (UNLIKELY(Head == &SentinelSegment)) {
auto R = InitHeadAndTail();
if (R == nullptr)
return nullptr;
}

DCHECK_NE(Head, &SentinelSegment);
DCHECK_NE(Tail, &SentinelSegment);

auto Offset = Size % ElementsPerSegment;
if (UNLIKELY(Size != 0 && Offset == 0))
if (AppendNewSegment() == nullptr)
return nullptr;

DCHECK_NE(Tail, &SentinelSegment);
auto Base = &Tail->Data;
auto AlignedOffset = Base + (Offset * AlignedElementStorageSize);
DCHECK_LE(AlignedOffset + sizeof(T),
reinterpret_cast<unsigned char *>(Base) + SegmentSize);


new (AlignedOffset) T{std::forward<Args>(args)...};
++Size;
return reinterpret_cast<T *>(AlignedOffset);
}

T *Append(const T &E) XRAY_NEVER_INSTRUMENT {



DCHECK((Size == 0 && Head == &SentinelSegment && Head == Tail) ||
(Size != 0 && Head != &SentinelSegment && Tail != &SentinelSegment));
if (UNLIKELY(Head == &SentinelSegment)) {
auto R = InitHeadAndTail();
if (R == nullptr)
return nullptr;
}

DCHECK_NE(Head, &SentinelSegment);
DCHECK_NE(Tail, &SentinelSegment);

auto Offset = Size % ElementsPerSegment;
if (UNLIKELY(Size != 0 && Offset == 0))
if (AppendNewSegment() == nullptr)
return nullptr;

DCHECK_NE(Tail, &SentinelSegment);
auto Base = &Tail->Data;
auto AlignedOffset = Base + (Offset * AlignedElementStorageSize);
DCHECK_LE(AlignedOffset + sizeof(T),
reinterpret_cast<unsigned char *>(Tail) + SegmentSize);


new (AlignedOffset) T(E);
++Size;
return reinterpret_cast<T *>(AlignedOffset);
}

T &operator[](uint64_t Offset) const XRAY_NEVER_INSTRUMENT {
DCHECK_LE(Offset, Size);

auto S = Head;
while (Offset >= ElementsPerSegment) {
S = S->Next;
Offset -= ElementsPerSegment;
DCHECK_NE(S, &SentinelSegment);
}
auto Base = &S->Data;
auto AlignedOffset = Base + (Offset * AlignedElementStorageSize);
auto Position = reinterpret_cast<T *>(AlignedOffset);
return *reinterpret_cast<T *>(Position);
}

T &front() const XRAY_NEVER_INSTRUMENT {
DCHECK_NE(Head, &SentinelSegment);
DCHECK_NE(Size, 0u);
return *begin();
}

T &back() const XRAY_NEVER_INSTRUMENT {
DCHECK_NE(Tail, &SentinelSegment);
DCHECK_NE(Size, 0u);
auto It = end();
--It;
return *It;
}

template <class Predicate>
T *find_element(Predicate P) const XRAY_NEVER_INSTRUMENT {
if (empty())
return nullptr;

auto E = end();
for (auto I = begin(); I != E; ++I)
if (P(*I))
return &(*I);

return nullptr;
}



void trim(uint64_t Elements) XRAY_NEVER_INSTRUMENT {
auto OldSize = Size;
Elements = Elements > Size ? Size : Elements;
Size -= Elements;




































auto F = [](uint64_t X) {
return X ? (X / ElementsPerSegment) +
(X < ElementsPerSegment || X % ElementsPerSegment ? 1 : 0)
: 0;
};
auto PS = F(OldSize);
auto CS = F(Size);
DCHECK_GE(PS, CS);
auto SegmentsToTrim = PS - CS;
for (auto I = 0uL; I < SegmentsToTrim; ++I) {







DCHECK_NE(Head, &SentinelSegment);
DCHECK_NE(Tail, &SentinelSegment);
DCHECK_EQ(Tail->Next, &SentinelSegment);

if (Freelist == &SentinelSegment) {




































auto SPT = Tail->Prev;
SPT->Next = &SentinelSegment;
Tail->Prev = &SentinelSegment;
Tail->Next = Freelist;
Freelist = Tail;
Tail = SPT;


DCHECK_EQ(Tail->Next, &SentinelSegment);
DCHECK_EQ(Freelist->Prev, &SentinelSegment);
} else {




























auto SFH = Freelist;
auto SPT = Tail->Prev;
auto ST = Tail;
SFH->Prev = ST;
ST->Next = Freelist;
ST->Prev = &SentinelSegment;
SPT->Next = &SentinelSegment;
Tail = SPT;
Freelist = ST;


DCHECK_EQ(Tail->Next, &SentinelSegment);
DCHECK_EQ(Freelist->Prev, &SentinelSegment);
DCHECK_EQ(Freelist->Next->Prev, Freelist);
}
}




if (Tail == &SentinelSegment)
Head = Tail;

DCHECK(
(Size == 0 && Head == &SentinelSegment && Tail == &SentinelSegment) ||
(Size != 0 && Head != &SentinelSegment && Tail != &SentinelSegment));
DCHECK(
(Freelist != &SentinelSegment && Freelist->Prev == &SentinelSegment) ||
(Freelist == &SentinelSegment && Tail->Next == &SentinelSegment));
}


Iterator<T> begin() const XRAY_NEVER_INSTRUMENT {
return Iterator<T>(Head, 0, Size);
}
Iterator<T> end() const XRAY_NEVER_INSTRUMENT {
return Iterator<T>(Tail, Size, Size);
}
Iterator<const T> cbegin() const XRAY_NEVER_INSTRUMENT {
return Iterator<const T>(Head, 0, Size);
}
Iterator<const T> cend() const XRAY_NEVER_INSTRUMENT {
return Iterator<const T>(Tail, Size, Size);
}
};




template <class T>
typename Array<T>::Segment Array<T>::SentinelSegment{
&Array<T>::SentinelSegment, &Array<T>::SentinelSegment, {'\0'}};

}

#endif
