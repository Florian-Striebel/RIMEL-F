














#if !defined(SCUDO_ALLOCATOR_SECONDARY_H_)
#define SCUDO_ALLOCATOR_SECONDARY_H_

#if !defined(SCUDO_ALLOCATOR_H_)
#error "This file must be included inside scudo_allocator.h."
#endif





























namespace LargeChunk {
struct Header {
ReservedAddressRange StoredRange;
uptr CommittedSize;
uptr Size;
};
constexpr uptr getHeaderSize() {
return RoundUpTo(sizeof(Header), MinAlignment);
}
static Header *getHeader(uptr Ptr) {
return reinterpret_cast<Header *>(Ptr - getHeaderSize());
}
static Header *getHeader(const void *Ptr) {
return getHeader(reinterpret_cast<uptr>(Ptr));
}
}

class LargeMmapAllocator {
public:
void Init() {
internal_memset(this, 0, sizeof(*this));
}

void *Allocate(AllocatorStats *Stats, uptr Size, uptr Alignment) {
const uptr UserSize = Size - Chunk::getHeaderSize();


uptr ReservedSize = Size + LargeChunk::getHeaderSize();
if (UNLIKELY(Alignment > MinAlignment))
ReservedSize += Alignment;
const uptr PageSize = GetPageSizeCached();
ReservedSize = RoundUpTo(ReservedSize, PageSize);

ReservedSize += 2 * PageSize;

ReservedAddressRange AddressRange;
uptr ReservedBeg = AddressRange.Init(ReservedSize, SecondaryAllocatorName);
if (UNLIKELY(ReservedBeg == ~static_cast<uptr>(0)))
return nullptr;

DCHECK(IsAligned(ReservedBeg, PageSize));
uptr ReservedEnd = ReservedBeg + ReservedSize;



uptr CommittedBeg = ReservedBeg + PageSize;
uptr UserBeg = CommittedBeg + HeadersSize;
uptr UserEnd = UserBeg + UserSize;
uptr CommittedEnd = RoundUpTo(UserEnd, PageSize);




if (UNLIKELY(Alignment > MinAlignment)) {
if (!IsAligned(UserBeg, Alignment)) {
UserBeg = RoundUpTo(UserBeg, Alignment);
CommittedBeg = RoundDownTo(UserBeg - HeadersSize, PageSize);
const uptr NewReservedBeg = CommittedBeg - PageSize;
DCHECK_GE(NewReservedBeg, ReservedBeg);
if (!SANITIZER_WINDOWS && NewReservedBeg != ReservedBeg) {
AddressRange.Unmap(ReservedBeg, NewReservedBeg - ReservedBeg);
ReservedBeg = NewReservedBeg;
}
UserEnd = UserBeg + UserSize;
CommittedEnd = RoundUpTo(UserEnd, PageSize);
}
const uptr NewReservedEnd = CommittedEnd + PageSize;
DCHECK_LE(NewReservedEnd, ReservedEnd);
if (!SANITIZER_WINDOWS && NewReservedEnd != ReservedEnd) {
AddressRange.Unmap(NewReservedEnd, ReservedEnd - NewReservedEnd);
ReservedEnd = NewReservedEnd;
}
}

DCHECK_LE(UserEnd, CommittedEnd);
const uptr CommittedSize = CommittedEnd - CommittedBeg;

CHECK_EQ(CommittedBeg, AddressRange.Map(CommittedBeg, CommittedSize));
const uptr Ptr = UserBeg - Chunk::getHeaderSize();
LargeChunk::Header *H = LargeChunk::getHeader(Ptr);
H->StoredRange = AddressRange;
H->Size = CommittedEnd - Ptr;
H->CommittedSize = CommittedSize;




{
SpinMutexLock l(&StatsMutex);
Stats->Add(AllocatorStatAllocated, CommittedSize);
Stats->Add(AllocatorStatMapped, CommittedSize);
AllocatedBytes += CommittedSize;
if (LargestSize < CommittedSize)
LargestSize = CommittedSize;
NumberOfAllocs++;
}

return reinterpret_cast<void *>(Ptr);
}

void Deallocate(AllocatorStats *Stats, void *Ptr) {
LargeChunk::Header *H = LargeChunk::getHeader(Ptr);


ReservedAddressRange AddressRange = H->StoredRange;
const uptr Size = H->CommittedSize;
{
SpinMutexLock l(&StatsMutex);
Stats->Sub(AllocatorStatAllocated, Size);
Stats->Sub(AllocatorStatMapped, Size);
FreedBytes += Size;
NumberOfFrees++;
}
AddressRange.Unmap(reinterpret_cast<uptr>(AddressRange.base()),
AddressRange.size());
}

static uptr GetActuallyAllocatedSize(void *Ptr) {
return LargeChunk::getHeader(Ptr)->Size;
}

void PrintStats() {
Printf("Stats: LargeMmapAllocator: allocated %zd times (%zd K), "
"freed %zd times (%zd K), remains %zd (%zd K) max %zd M\n",
NumberOfAllocs, AllocatedBytes >> 10, NumberOfFrees,
FreedBytes >> 10, NumberOfAllocs - NumberOfFrees,
(AllocatedBytes - FreedBytes) >> 10, LargestSize >> 20);
}

private:
static constexpr uptr HeadersSize =
LargeChunk::getHeaderSize() + Chunk::getHeaderSize();

StaticSpinMutex StatsMutex;
u32 NumberOfAllocs;
u32 NumberOfFrees;
uptr AllocatedBytes;
uptr FreedBytes;
uptr LargestSize;
};

#endif
