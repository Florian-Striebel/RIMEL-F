







#if !defined(SCUDO_COMBINED_H_)
#define SCUDO_COMBINED_H_

#include "chunk.h"
#include "common.h"
#include "flags.h"
#include "flags_parser.h"
#include "local_cache.h"
#include "memtag.h"
#include "options.h"
#include "quarantine.h"
#include "report.h"
#include "secondary.h"
#include "stack_depot.h"
#include "string_utils.h"
#include "tsd.h"

#include "scudo/interface.h"

#if defined(GWP_ASAN_HOOKS)
#include "gwp_asan/guarded_pool_allocator.h"
#include "gwp_asan/optional/backtrace.h"
#include "gwp_asan/optional/segv_handler.h"
#endif

extern "C" inline void EmptyCallback() {}

#if defined(HAVE_ANDROID_UNSAFE_FRAME_POINTER_CHASE)


extern "C" size_t android_unsafe_frame_pointer_chase(scudo::uptr *buf,
size_t num_entries);
#endif

namespace scudo {

template <class Params, void (*PostInitCallback)(void) = EmptyCallback>
class Allocator {
public:
using PrimaryT = typename Params::Primary;
using CacheT = typename PrimaryT::CacheT;
typedef Allocator<Params, PostInitCallback> ThisT;
typedef typename Params::template TSDRegistryT<ThisT> TSDRegistryT;

void callPostInitCallback() {
pthread_once(&PostInitNonce, PostInitCallback);
}

struct QuarantineCallback {
explicit QuarantineCallback(ThisT &Instance, CacheT &LocalCache)
: Allocator(Instance), Cache(LocalCache) {}



void recycle(void *Ptr) {
Chunk::UnpackedHeader Header;
Chunk::loadHeader(Allocator.Cookie, Ptr, &Header);
if (UNLIKELY(Header.State != Chunk::State::Quarantined))
reportInvalidChunkState(AllocatorAction::Recycling, Ptr);

Chunk::UnpackedHeader NewHeader = Header;
NewHeader.State = Chunk::State::Available;
Chunk::compareExchangeHeader(Allocator.Cookie, Ptr, &NewHeader, &Header);

if (allocatorSupportsMemoryTagging<Params>())
Ptr = untagPointer(Ptr);
void *BlockBegin = Allocator::getBlockBegin(Ptr, &NewHeader);
Cache.deallocate(NewHeader.ClassId, BlockBegin);
}




void *allocate(UNUSED uptr Size) {
const uptr QuarantineClassId = SizeClassMap::getClassIdBySize(
sizeof(QuarantineBatch) + Chunk::getHeaderSize());
void *Ptr = Cache.allocate(QuarantineClassId);

if (UNLIKELY(!Ptr))
reportOutOfMemory(SizeClassMap::getSizeByClassId(QuarantineClassId));

Ptr = reinterpret_cast<void *>(reinterpret_cast<uptr>(Ptr) +
Chunk::getHeaderSize());
Chunk::UnpackedHeader Header = {};
Header.ClassId = QuarantineClassId & Chunk::ClassIdMask;
Header.SizeOrUnusedBytes = sizeof(QuarantineBatch);
Header.State = Chunk::State::Allocated;
Chunk::storeHeader(Allocator.Cookie, Ptr, &Header);



if (UNLIKELY(useMemoryTagging<Params>(Allocator.Primary.Options.load())))
storeTags(reinterpret_cast<uptr>(Ptr),
reinterpret_cast<uptr>(Ptr) + sizeof(QuarantineBatch));

return Ptr;
}

void deallocate(void *Ptr) {
const uptr QuarantineClassId = SizeClassMap::getClassIdBySize(
sizeof(QuarantineBatch) + Chunk::getHeaderSize());
Chunk::UnpackedHeader Header;
Chunk::loadHeader(Allocator.Cookie, Ptr, &Header);

if (UNLIKELY(Header.State != Chunk::State::Allocated))
reportInvalidChunkState(AllocatorAction::Deallocating, Ptr);
DCHECK_EQ(Header.ClassId, QuarantineClassId);
DCHECK_EQ(Header.Offset, 0);
DCHECK_EQ(Header.SizeOrUnusedBytes, sizeof(QuarantineBatch));

Chunk::UnpackedHeader NewHeader = Header;
NewHeader.State = Chunk::State::Available;
Chunk::compareExchangeHeader(Allocator.Cookie, Ptr, &NewHeader, &Header);
Cache.deallocate(QuarantineClassId,
reinterpret_cast<void *>(reinterpret_cast<uptr>(Ptr) -
Chunk::getHeaderSize()));
}

private:
ThisT &Allocator;
CacheT &Cache;
};

typedef GlobalQuarantine<QuarantineCallback, void> QuarantineT;
typedef typename QuarantineT::CacheT QuarantineCacheT;

void init() {
performSanityChecks();



if (&computeHardwareCRC32 && hasHardwareCRC32())
HashAlgorithm = Checksum::HardwareCRC32;

if (UNLIKELY(!getRandom(&Cookie, sizeof(Cookie))))
Cookie = static_cast<u32>(getMonotonicTime() ^
(reinterpret_cast<uptr>(this) >> 4));

initFlags();
reportUnrecognizedFlags();


if (getFlags()->may_return_null)
Primary.Options.set(OptionBit::MayReturnNull);
if (getFlags()->zero_contents)
Primary.Options.setFillContentsMode(ZeroFill);
else if (getFlags()->pattern_fill_contents)
Primary.Options.setFillContentsMode(PatternOrZeroFill);
if (getFlags()->dealloc_type_mismatch)
Primary.Options.set(OptionBit::DeallocTypeMismatch);
if (getFlags()->delete_size_mismatch)
Primary.Options.set(OptionBit::DeleteSizeMismatch);
if (allocatorSupportsMemoryTagging<Params>() &&
systemSupportsMemoryTagging())
Primary.Options.set(OptionBit::UseMemoryTagging);
Primary.Options.set(OptionBit::UseOddEvenTags);

QuarantineMaxChunkSize =
static_cast<u32>(getFlags()->quarantine_max_chunk_size);

Stats.init();
const s32 ReleaseToOsIntervalMs = getFlags()->release_to_os_interval_ms;
Primary.init(ReleaseToOsIntervalMs);
Secondary.init(&Stats, ReleaseToOsIntervalMs);
Quarantine.init(
static_cast<uptr>(getFlags()->quarantine_size_kb << 10),
static_cast<uptr>(getFlags()->thread_local_quarantine_size_kb << 10));
}



void initGwpAsan() {
#if defined(GWP_ASAN_HOOKS)
gwp_asan::options::Options Opt;
Opt.Enabled = getFlags()->GWP_ASAN_Enabled;
Opt.MaxSimultaneousAllocations =
getFlags()->GWP_ASAN_MaxSimultaneousAllocations;
Opt.SampleRate = getFlags()->GWP_ASAN_SampleRate;
Opt.InstallSignalHandlers = getFlags()->GWP_ASAN_InstallSignalHandlers;



Opt.InstallForkHandlers = false;
Opt.Backtrace = gwp_asan::backtrace::getBacktraceFunction();
GuardedAlloc.init(Opt);

if (Opt.InstallSignalHandlers)
gwp_asan::segv_handler::installSignalHandlers(
&GuardedAlloc, Printf,
gwp_asan::backtrace::getPrintBacktraceFunction(),
gwp_asan::backtrace::getSegvBacktraceFunction());

GuardedAllocSlotSize =
GuardedAlloc.getAllocatorState()->maximumAllocationSize();
Stats.add(StatFree, static_cast<uptr>(Opt.MaxSimultaneousAllocations) *
GuardedAllocSlotSize);
#endif
}

ALWAYS_INLINE void initThreadMaybe(bool MinimalInit = false) {
TSDRegistry.initThreadMaybe(this, MinimalInit);
}

void unmapTestOnly() {
TSDRegistry.unmapTestOnly(this);
Primary.unmapTestOnly();
Secondary.unmapTestOnly();
#if defined(GWP_ASAN_HOOKS)
if (getFlags()->GWP_ASAN_InstallSignalHandlers)
gwp_asan::segv_handler::uninstallSignalHandlers();
GuardedAlloc.uninitTestOnly();
#endif
}

TSDRegistryT *getTSDRegistry() { return &TSDRegistry; }


void initCache(CacheT *Cache) { Cache->init(&Stats, &Primary); }






void commitBack(TSD<ThisT> *TSD) {
Quarantine.drain(&TSD->QuarantineCache,
QuarantineCallback(*this, TSD->Cache));
TSD->Cache.destroy(&Stats);
}

ALWAYS_INLINE void *getHeaderTaggedPointer(void *Ptr) {
if (!allocatorSupportsMemoryTagging<Params>())
return Ptr;
auto UntaggedPtr = untagPointer(Ptr);
if (UntaggedPtr != Ptr)
return UntaggedPtr;



return addHeaderTag(Ptr);
}

ALWAYS_INLINE uptr addHeaderTag(uptr Ptr) {
if (!allocatorSupportsMemoryTagging<Params>())
return Ptr;
return addFixedTag(Ptr, 2);
}

ALWAYS_INLINE void *addHeaderTag(void *Ptr) {
return reinterpret_cast<void *>(addHeaderTag(reinterpret_cast<uptr>(Ptr)));
}

NOINLINE u32 collectStackTrace() {
#if defined(HAVE_ANDROID_UNSAFE_FRAME_POINTER_CHASE)

constexpr uptr DiscardFrames = 2;
uptr Stack[MaxTraceSize + DiscardFrames];
uptr Size =
android_unsafe_frame_pointer_chase(Stack, MaxTraceSize + DiscardFrames);
Size = Min<uptr>(Size, MaxTraceSize + DiscardFrames);
return Depot.insert(Stack + Min<uptr>(DiscardFrames, Size), Stack + Size);
#else
return 0;
#endif
}

uptr computeOddEvenMaskForPointerMaybe(Options Options, uptr Ptr,
uptr ClassId) {
if (!Options.get(OptionBit::UseOddEvenTags))
return 0;






return 0x5555U << ((Ptr >> SizeClassMap::getSizeLSBByClassId(ClassId)) & 1);
}

NOINLINE void *allocate(uptr Size, Chunk::Origin Origin,
uptr Alignment = MinAlignment,
bool ZeroContents = false) {
initThreadMaybe();

const Options Options = Primary.Options.load();
if (UNLIKELY(Alignment > MaxAlignment)) {
if (Options.get(OptionBit::MayReturnNull))
return nullptr;
reportAlignmentTooBig(Alignment, MaxAlignment);
}
if (Alignment < MinAlignment)
Alignment = MinAlignment;

#if defined(GWP_ASAN_HOOKS)
if (UNLIKELY(GuardedAlloc.shouldSample())) {
if (void *Ptr = GuardedAlloc.allocate(Size, Alignment)) {
if (UNLIKELY(&__scudo_allocate_hook))
__scudo_allocate_hook(Ptr, Size);
Stats.lock();
Stats.add(StatAllocated, GuardedAllocSlotSize);
Stats.sub(StatFree, GuardedAllocSlotSize);
Stats.unlock();
return Ptr;
}
}
#endif

const FillContentsMode FillContents = ZeroContents ? ZeroFill
: TSDRegistry.getDisableMemInit()
? NoFill
: Options.getFillContentsMode();






const uptr NeededSize =
roundUpTo(Size, MinAlignment) +
((Alignment > MinAlignment) ? Alignment : Chunk::getHeaderSize());


static_assert(MaxAllowedMallocSize < UINTPTR_MAX - MaxAlignment, "");
if (UNLIKELY(Size >= MaxAllowedMallocSize)) {
if (Options.get(OptionBit::MayReturnNull))
return nullptr;
reportAllocationSizeTooBig(Size, NeededSize, MaxAllowedMallocSize);
}
DCHECK_LE(Size, NeededSize);

void *Block = nullptr;
uptr ClassId = 0;
uptr SecondaryBlockEnd = 0;
if (LIKELY(PrimaryT::canAllocate(NeededSize))) {
ClassId = SizeClassMap::getClassIdBySize(NeededSize);
DCHECK_NE(ClassId, 0U);
bool UnlockRequired;
auto *TSD = TSDRegistry.getTSDAndLock(&UnlockRequired);
Block = TSD->Cache.allocate(ClassId);




if (UNLIKELY(!Block)) {
while (ClassId < SizeClassMap::LargestClassId && !Block)
Block = TSD->Cache.allocate(++ClassId);
if (!Block)
ClassId = 0;
}
if (UnlockRequired)
TSD->unlock();
}
if (UNLIKELY(ClassId == 0))
Block = Secondary.allocate(Options, Size, Alignment, &SecondaryBlockEnd,
FillContents);

if (UNLIKELY(!Block)) {
if (Options.get(OptionBit::MayReturnNull))
return nullptr;
reportOutOfMemory(NeededSize);
}

const uptr BlockUptr = reinterpret_cast<uptr>(Block);
const uptr UnalignedUserPtr = BlockUptr + Chunk::getHeaderSize();
const uptr UserPtr = roundUpTo(UnalignedUserPtr, Alignment);

void *Ptr = reinterpret_cast<void *>(UserPtr);
void *TaggedPtr = Ptr;
if (LIKELY(ClassId)) {











if (UNLIKELY(useMemoryTagging<Params>(Options))) {
uptr PrevUserPtr;
Chunk::UnpackedHeader Header;
const uptr BlockSize = PrimaryT::getSizeByClassId(ClassId);
const uptr BlockEnd = BlockUptr + BlockSize;

































uptr TaggedUserPtr;
if (getChunkFromBlock(BlockUptr, &PrevUserPtr, &Header) &&
PrevUserPtr == UserPtr &&
(TaggedUserPtr = loadTag(UserPtr)) != UserPtr) {
uptr PrevEnd = TaggedUserPtr + Header.SizeOrUnusedBytes;
const uptr NextPage = roundUpTo(TaggedUserPtr, getPageSizeCached());
if (NextPage < PrevEnd && loadTag(NextPage) != NextPage)
PrevEnd = NextPage;
TaggedPtr = reinterpret_cast<void *>(TaggedUserPtr);
resizeTaggedChunk(PrevEnd, TaggedUserPtr + Size, Size, BlockEnd);
if (UNLIKELY(FillContents != NoFill && !Header.OriginOrWasZeroed)) {






memset(TaggedPtr, 0,
Min(Size, roundUpTo(PrevEnd - TaggedUserPtr,
archMemoryTagGranuleSize())));
} else if (Size) {


memset(TaggedPtr, 0, archMemoryTagGranuleSize());
}
} else {
const uptr OddEvenMask =
computeOddEvenMaskForPointerMaybe(Options, BlockUptr, ClassId);
TaggedPtr = prepareTaggedChunk(Ptr, Size, OddEvenMask, BlockEnd);
}
storePrimaryAllocationStackMaybe(Options, Ptr);
} else {
Block = addHeaderTag(Block);
Ptr = addHeaderTag(Ptr);
if (UNLIKELY(FillContents != NoFill)) {


memset(Block, FillContents == ZeroFill ? 0 : PatternFillByte,
PrimaryT::getSizeByClassId(ClassId));
}
}
} else {
Block = addHeaderTag(Block);
Ptr = addHeaderTag(Ptr);
if (UNLIKELY(useMemoryTagging<Params>(Options))) {
storeTags(reinterpret_cast<uptr>(Block), reinterpret_cast<uptr>(Ptr));
storeSecondaryAllocationStackMaybe(Options, Ptr, Size);
}
}

Chunk::UnpackedHeader Header = {};
if (UNLIKELY(UnalignedUserPtr != UserPtr)) {
const uptr Offset = UserPtr - UnalignedUserPtr;
DCHECK_GE(Offset, 2 * sizeof(u32));




reinterpret_cast<u32 *>(Block)[0] = BlockMarker;
reinterpret_cast<u32 *>(Block)[1] = static_cast<u32>(Offset);
Header.Offset = (Offset >> MinAlignmentLog) & Chunk::OffsetMask;
}
Header.ClassId = ClassId & Chunk::ClassIdMask;
Header.State = Chunk::State::Allocated;
Header.OriginOrWasZeroed = Origin & Chunk::OriginMask;
Header.SizeOrUnusedBytes =
(ClassId ? Size : SecondaryBlockEnd - (UserPtr + Size)) &
Chunk::SizeOrUnusedBytesMask;
Chunk::storeHeader(Cookie, Ptr, &Header);

if (UNLIKELY(&__scudo_allocate_hook))
__scudo_allocate_hook(TaggedPtr, Size);

return TaggedPtr;
}

NOINLINE void deallocate(void *Ptr, Chunk::Origin Origin, uptr DeleteSize = 0,
UNUSED uptr Alignment = MinAlignment) {






initThreadMaybe(true);

if (UNLIKELY(&__scudo_deallocate_hook))
__scudo_deallocate_hook(Ptr);

if (UNLIKELY(!Ptr))
return;

#if defined(GWP_ASAN_HOOKS)
if (UNLIKELY(GuardedAlloc.pointerIsMine(Ptr))) {
GuardedAlloc.deallocate(Ptr);
Stats.lock();
Stats.add(StatFree, GuardedAllocSlotSize);
Stats.sub(StatAllocated, GuardedAllocSlotSize);
Stats.unlock();
return;
}
#endif

if (UNLIKELY(!isAligned(reinterpret_cast<uptr>(Ptr), MinAlignment)))
reportMisalignedPointer(AllocatorAction::Deallocating, Ptr);

void *TaggedPtr = Ptr;
Ptr = getHeaderTaggedPointer(Ptr);

Chunk::UnpackedHeader Header;
Chunk::loadHeader(Cookie, Ptr, &Header);

if (UNLIKELY(Header.State != Chunk::State::Allocated))
reportInvalidChunkState(AllocatorAction::Deallocating, Ptr);

const Options Options = Primary.Options.load();
if (Options.get(OptionBit::DeallocTypeMismatch)) {
if (UNLIKELY(Header.OriginOrWasZeroed != Origin)) {

if (Header.OriginOrWasZeroed != Chunk::Origin::Memalign ||
Origin != Chunk::Origin::Malloc)
reportDeallocTypeMismatch(AllocatorAction::Deallocating, Ptr,
Header.OriginOrWasZeroed, Origin);
}
}

const uptr Size = getSize(Ptr, &Header);
if (DeleteSize && Options.get(OptionBit::DeleteSizeMismatch)) {
if (UNLIKELY(DeleteSize != Size))
reportDeleteSizeMismatch(Ptr, DeleteSize, Size);
}

quarantineOrDeallocateChunk(Options, TaggedPtr, &Header, Size);
}

void *reallocate(void *OldPtr, uptr NewSize, uptr Alignment = MinAlignment) {
initThreadMaybe();

const Options Options = Primary.Options.load();
if (UNLIKELY(NewSize >= MaxAllowedMallocSize)) {
if (Options.get(OptionBit::MayReturnNull))
return nullptr;
reportAllocationSizeTooBig(NewSize, 0, MaxAllowedMallocSize);
}


DCHECK_NE(OldPtr, nullptr);
DCHECK_NE(NewSize, 0);

#if defined(GWP_ASAN_HOOKS)
if (UNLIKELY(GuardedAlloc.pointerIsMine(OldPtr))) {
uptr OldSize = GuardedAlloc.getSize(OldPtr);
void *NewPtr = allocate(NewSize, Chunk::Origin::Malloc, Alignment);
if (NewPtr)
memcpy(NewPtr, OldPtr, (NewSize < OldSize) ? NewSize : OldSize);
GuardedAlloc.deallocate(OldPtr);
Stats.lock();
Stats.add(StatFree, GuardedAllocSlotSize);
Stats.sub(StatAllocated, GuardedAllocSlotSize);
Stats.unlock();
return NewPtr;
}
#endif

void *OldTaggedPtr = OldPtr;
OldPtr = getHeaderTaggedPointer(OldPtr);

if (UNLIKELY(!isAligned(reinterpret_cast<uptr>(OldPtr), MinAlignment)))
reportMisalignedPointer(AllocatorAction::Reallocating, OldPtr);

Chunk::UnpackedHeader OldHeader;
Chunk::loadHeader(Cookie, OldPtr, &OldHeader);

if (UNLIKELY(OldHeader.State != Chunk::State::Allocated))
reportInvalidChunkState(AllocatorAction::Reallocating, OldPtr);




if (Options.get(OptionBit::DeallocTypeMismatch)) {
if (UNLIKELY(OldHeader.OriginOrWasZeroed != Chunk::Origin::Malloc))
reportDeallocTypeMismatch(AllocatorAction::Reallocating, OldPtr,
OldHeader.OriginOrWasZeroed,
Chunk::Origin::Malloc);
}

void *BlockBegin = getBlockBegin(OldTaggedPtr, &OldHeader);
uptr BlockEnd;
uptr OldSize;
const uptr ClassId = OldHeader.ClassId;
if (LIKELY(ClassId)) {
BlockEnd = reinterpret_cast<uptr>(BlockBegin) +
SizeClassMap::getSizeByClassId(ClassId);
OldSize = OldHeader.SizeOrUnusedBytes;
} else {
BlockEnd = SecondaryT::getBlockEnd(BlockBegin);
OldSize = BlockEnd - (reinterpret_cast<uptr>(OldTaggedPtr) +
OldHeader.SizeOrUnusedBytes);
}



if (reinterpret_cast<uptr>(OldTaggedPtr) + NewSize <= BlockEnd) {
if (NewSize > OldSize || (OldSize - NewSize) < getPageSizeCached()) {
Chunk::UnpackedHeader NewHeader = OldHeader;
NewHeader.SizeOrUnusedBytes =
(ClassId ? NewSize
: BlockEnd -
(reinterpret_cast<uptr>(OldTaggedPtr) + NewSize)) &
Chunk::SizeOrUnusedBytesMask;
Chunk::compareExchangeHeader(Cookie, OldPtr, &NewHeader, &OldHeader);
if (UNLIKELY(useMemoryTagging<Params>(Options))) {
if (ClassId) {
resizeTaggedChunk(reinterpret_cast<uptr>(OldTaggedPtr) + OldSize,
reinterpret_cast<uptr>(OldTaggedPtr) + NewSize,
NewSize, untagPointer(BlockEnd));
storePrimaryAllocationStackMaybe(Options, OldPtr);
} else {
storeSecondaryAllocationStackMaybe(Options, OldPtr, NewSize);
}
}
return OldTaggedPtr;
}
}





void *NewPtr = allocate(NewSize, Chunk::Origin::Malloc, Alignment);
if (LIKELY(NewPtr)) {
memcpy(NewPtr, OldTaggedPtr, Min(NewSize, OldSize));
quarantineOrDeallocateChunk(Options, OldTaggedPtr, &OldHeader, OldSize);
}
return NewPtr;
}




void disable() {
initThreadMaybe();
#if defined(GWP_ASAN_HOOKS)
GuardedAlloc.disable();
#endif
TSDRegistry.disable();
Stats.disable();
Quarantine.disable();
Primary.disable();
Secondary.disable();
}

void enable() {
initThreadMaybe();
Secondary.enable();
Primary.enable();
Quarantine.enable();
Stats.enable();
TSDRegistry.enable();
#if defined(GWP_ASAN_HOOKS)
GuardedAlloc.enable();
#endif
}






uptr getStats(char *Buffer, uptr Size) {
ScopedString Str;
disable();
const uptr Length = getStats(&Str) + 1;
enable();
if (Length < Size)
Size = Length;
if (Buffer && Size) {
memcpy(Buffer, Str.data(), Size);
Buffer[Size - 1] = '\0';
}
return Length;
}

void printStats() {
ScopedString Str;
disable();
getStats(&Str);
enable();
Str.output();
}

void releaseToOS() {
initThreadMaybe();
Primary.releaseToOS();
Secondary.releaseToOS();
}




void iterateOverChunks(uptr Base, uptr Size, iterate_callback Callback,
void *Arg) {
initThreadMaybe();
if (archSupportsMemoryTagging())
Base = untagPointer(Base);
const uptr From = Base;
const uptr To = Base + Size;
bool MayHaveTaggedPrimary = allocatorSupportsMemoryTagging<Params>() &&
systemSupportsMemoryTagging();
auto Lambda = [this, From, To, MayHaveTaggedPrimary, Callback,
Arg](uptr Block) {
if (Block < From || Block >= To)
return;
uptr Chunk;
Chunk::UnpackedHeader Header;
if (MayHaveTaggedPrimary) {



ScopedDisableMemoryTagChecks x;
if (!getChunkFromBlock(Block, &Chunk, &Header) &&
!getChunkFromBlock(addHeaderTag(Block), &Chunk, &Header))
return;
} else {
if (!getChunkFromBlock(addHeaderTag(Block), &Chunk, &Header))
return;
}
if (Header.State == Chunk::State::Allocated) {
uptr TaggedChunk = Chunk;
if (allocatorSupportsMemoryTagging<Params>())
TaggedChunk = untagPointer(TaggedChunk);
if (useMemoryTagging<Params>(Primary.Options.load()))
TaggedChunk = loadTag(Chunk);
Callback(TaggedChunk, getSize(reinterpret_cast<void *>(Chunk), &Header),
Arg);
}
};
Primary.iterateOverBlocks(Lambda);
Secondary.iterateOverBlocks(Lambda);
#if defined(GWP_ASAN_HOOKS)
GuardedAlloc.iterate(reinterpret_cast<void *>(Base), Size, Callback, Arg);
#endif
}

bool canReturnNull() {
initThreadMaybe();
return Primary.Options.load().get(OptionBit::MayReturnNull);
}

bool setOption(Option O, sptr Value) {
initThreadMaybe();
if (O == Option::MemtagTuning) {







if (Value == M_MEMTAG_TUNING_BUFFER_OVERFLOW)
Primary.Options.set(OptionBit::UseOddEvenTags);
else if (Value == M_MEMTAG_TUNING_UAF)
Primary.Options.clear(OptionBit::UseOddEvenTags);
return true;
} else {



const bool PrimaryResult = Primary.setOption(O, Value);
const bool SecondaryResult = Secondary.setOption(O, Value);
const bool RegistryResult = TSDRegistry.setOption(O, Value);
return PrimaryResult && SecondaryResult && RegistryResult;
}
return false;
}






uptr getUsableSize(const void *Ptr) {
initThreadMaybe();
if (UNLIKELY(!Ptr))
return 0;

#if defined(GWP_ASAN_HOOKS)
if (UNLIKELY(GuardedAlloc.pointerIsMine(Ptr)))
return GuardedAlloc.getSize(Ptr);
#endif

Ptr = getHeaderTaggedPointer(const_cast<void *>(Ptr));
Chunk::UnpackedHeader Header;
Chunk::loadHeader(Cookie, Ptr, &Header);

if (UNLIKELY(Header.State != Chunk::State::Allocated))
reportInvalidChunkState(AllocatorAction::Sizing, const_cast<void *>(Ptr));
return getSize(Ptr, &Header);
}

void getStats(StatCounters S) {
initThreadMaybe();
Stats.get(S);
}




bool isOwned(const void *Ptr) {
initThreadMaybe();
#if defined(GWP_ASAN_HOOKS)
if (GuardedAlloc.pointerIsMine(Ptr))
return true;
#endif
if (!Ptr || !isAligned(reinterpret_cast<uptr>(Ptr), MinAlignment))
return false;
Ptr = getHeaderTaggedPointer(const_cast<void *>(Ptr));
Chunk::UnpackedHeader Header;
return Chunk::isValid(Cookie, Ptr, &Header) &&
Header.State == Chunk::State::Allocated;
}

bool useMemoryTaggingTestOnly() const {
return useMemoryTagging<Params>(Primary.Options.load());
}
void disableMemoryTagging() {






TSDRegistry.initOnceMaybe(this);
if (allocatorSupportsMemoryTagging<Params>()) {
Secondary.disableMemoryTagging();
Primary.Options.clear(OptionBit::UseMemoryTagging);
}
}

void setTrackAllocationStacks(bool Track) {
initThreadMaybe();
if (Track)
Primary.Options.set(OptionBit::TrackAllocationStacks);
else
Primary.Options.clear(OptionBit::TrackAllocationStacks);
}

void setFillContents(FillContentsMode FillContents) {
initThreadMaybe();
Primary.Options.setFillContentsMode(FillContents);
}

void setAddLargeAllocationSlack(bool AddSlack) {
initThreadMaybe();
if (AddSlack)
Primary.Options.set(OptionBit::AddLargeAllocationSlack);
else
Primary.Options.clear(OptionBit::AddLargeAllocationSlack);
}

const char *getStackDepotAddress() const {
return reinterpret_cast<const char *>(&Depot);
}

const char *getRegionInfoArrayAddress() const {
return Primary.getRegionInfoArrayAddress();
}

static uptr getRegionInfoArraySize() {
return PrimaryT::getRegionInfoArraySize();
}

const char *getRingBufferAddress() const {
return reinterpret_cast<const char *>(&RingBuffer);
}

static uptr getRingBufferSize() { return sizeof(RingBuffer); }

static const uptr MaxTraceSize = 64;

static void collectTraceMaybe(const StackDepot *Depot,
uintptr_t (&Trace)[MaxTraceSize], u32 Hash) {
uptr RingPos, Size;
if (!Depot->find(Hash, &RingPos, &Size))
return;
for (unsigned I = 0; I != Size && I != MaxTraceSize; ++I)
Trace[I] = (*Depot)[RingPos + I];
}

static void getErrorInfo(struct scudo_error_info *ErrorInfo,
uintptr_t FaultAddr, const char *DepotPtr,
const char *RegionInfoPtr, const char *RingBufferPtr,
const char *Memory, const char *MemoryTags,
uintptr_t MemoryAddr, size_t MemorySize) {
*ErrorInfo = {};
if (!allocatorSupportsMemoryTagging<Params>() ||
MemoryAddr + MemorySize < MemoryAddr)
return;

auto *Depot = reinterpret_cast<const StackDepot *>(DepotPtr);
size_t NextErrorReport = 0;



if (extractTag(FaultAddr) != 0)
getInlineErrorInfo(ErrorInfo, NextErrorReport, FaultAddr, Depot,
RegionInfoPtr, Memory, MemoryTags, MemoryAddr,
MemorySize, 0, 2);



getRingBufferErrorInfo(ErrorInfo, NextErrorReport, FaultAddr, Depot,
RingBufferPtr);



if (extractTag(FaultAddr) != 0)
getInlineErrorInfo(ErrorInfo, NextErrorReport, FaultAddr, Depot,
RegionInfoPtr, Memory, MemoryTags, MemoryAddr,
MemorySize, 2, 16);
}

private:
using SecondaryT = MapAllocator<Params>;
typedef typename PrimaryT::SizeClassMap SizeClassMap;

static const uptr MinAlignmentLog = SCUDO_MIN_ALIGNMENT_LOG;
static const uptr MaxAlignmentLog = 24U;
static const uptr MinAlignment = 1UL << MinAlignmentLog;
static const uptr MaxAlignment = 1UL << MaxAlignmentLog;
static const uptr MaxAllowedMallocSize =
FIRST_32_SECOND_64(1UL << 31, 1ULL << 40);

static_assert(MinAlignment >= sizeof(Chunk::PackedHeader),
"Minimal alignment must at least cover a chunk header.");
static_assert(!allocatorSupportsMemoryTagging<Params>() ||
MinAlignment >= archMemoryTagGranuleSize(),
"");

static const u32 BlockMarker = 0x44554353U;








static const sptr MemTagAllocationTraceIndex = -2;
static const sptr MemTagAllocationTidIndex = -1;

u32 Cookie = 0;
u32 QuarantineMaxChunkSize = 0;

GlobalStats Stats;
PrimaryT Primary;
SecondaryT Secondary;
QuarantineT Quarantine;
TSDRegistryT TSDRegistry;
pthread_once_t PostInitNonce = PTHREAD_ONCE_INIT;

#if defined(GWP_ASAN_HOOKS)
gwp_asan::GuardedPoolAllocator GuardedAlloc;
uptr GuardedAllocSlotSize = 0;
#endif

StackDepot Depot;

struct AllocationRingBuffer {
struct Entry {
atomic_uptr Ptr;
atomic_uptr AllocationSize;
atomic_u32 AllocationTrace;
atomic_u32 AllocationTid;
atomic_u32 DeallocationTrace;
atomic_u32 DeallocationTid;
};

atomic_uptr Pos;
#if defined(SCUDO_FUZZ)
static const uptr NumEntries = 2;
#else
static const uptr NumEntries = 32768;
#endif
Entry Entries[NumEntries];
};
AllocationRingBuffer RingBuffer = {};


NOINLINE void performSanityChecks() {








Chunk::UnpackedHeader Header = {};
const uptr MaxPrimaryAlignment = 1UL << getMostSignificantSetBitIndex(
SizeClassMap::MaxSize - MinAlignment);
const uptr MaxOffset =
(MaxPrimaryAlignment - Chunk::getHeaderSize()) >> MinAlignmentLog;
Header.Offset = MaxOffset & Chunk::OffsetMask;
if (UNLIKELY(Header.Offset != MaxOffset))
reportSanityCheckError("offset");






const uptr MaxSizeOrUnusedBytes = SizeClassMap::MaxSize - 1;
Header.SizeOrUnusedBytes = MaxSizeOrUnusedBytes;
if (UNLIKELY(Header.SizeOrUnusedBytes != MaxSizeOrUnusedBytes))
reportSanityCheckError("size (or unused bytes)");

const uptr LargestClassId = SizeClassMap::LargestClassId;
Header.ClassId = LargestClassId;
if (UNLIKELY(Header.ClassId != LargestClassId))
reportSanityCheckError("class ID");
}

static inline void *getBlockBegin(const void *Ptr,
Chunk::UnpackedHeader *Header) {
return reinterpret_cast<void *>(
reinterpret_cast<uptr>(Ptr) - Chunk::getHeaderSize() -
(static_cast<uptr>(Header->Offset) << MinAlignmentLog));
}


inline uptr getSize(const void *Ptr, Chunk::UnpackedHeader *Header) {
const uptr SizeOrUnusedBytes = Header->SizeOrUnusedBytes;
if (LIKELY(Header->ClassId))
return SizeOrUnusedBytes;
if (allocatorSupportsMemoryTagging<Params>())
Ptr = untagPointer(const_cast<void *>(Ptr));
return SecondaryT::getBlockEnd(getBlockBegin(Ptr, Header)) -
reinterpret_cast<uptr>(Ptr) - SizeOrUnusedBytes;
}

void quarantineOrDeallocateChunk(Options Options, void *TaggedPtr,
Chunk::UnpackedHeader *Header, uptr Size) {
void *Ptr = getHeaderTaggedPointer(TaggedPtr);
Chunk::UnpackedHeader NewHeader = *Header;



const bool BypassQuarantine = !Quarantine.getCacheSize() ||
((Size - 1) >= QuarantineMaxChunkSize) ||
!NewHeader.ClassId;
if (BypassQuarantine)
NewHeader.State = Chunk::State::Available;
else
NewHeader.State = Chunk::State::Quarantined;
NewHeader.OriginOrWasZeroed = useMemoryTagging<Params>(Options) &&
NewHeader.ClassId &&
!TSDRegistry.getDisableMemInit();
Chunk::compareExchangeHeader(Cookie, Ptr, &NewHeader, Header);

if (UNLIKELY(useMemoryTagging<Params>(Options))) {
u8 PrevTag = extractTag(reinterpret_cast<uptr>(TaggedPtr));
storeDeallocationStackMaybe(Options, Ptr, PrevTag, Size);
if (NewHeader.ClassId) {
if (!TSDRegistry.getDisableMemInit()) {
uptr TaggedBegin, TaggedEnd;
const uptr OddEvenMask = computeOddEvenMaskForPointerMaybe(
Options, reinterpret_cast<uptr>(getBlockBegin(Ptr, &NewHeader)),
NewHeader.ClassId);


setRandomTag(Ptr, Size, OddEvenMask | (1UL << PrevTag), &TaggedBegin,
&TaggedEnd);
}
}
}
if (BypassQuarantine) {
if (allocatorSupportsMemoryTagging<Params>())
Ptr = untagPointer(Ptr);
void *BlockBegin = getBlockBegin(Ptr, &NewHeader);
const uptr ClassId = NewHeader.ClassId;
if (LIKELY(ClassId)) {
bool UnlockRequired;
auto *TSD = TSDRegistry.getTSDAndLock(&UnlockRequired);
TSD->Cache.deallocate(ClassId, BlockBegin);
if (UnlockRequired)
TSD->unlock();
} else {
if (UNLIKELY(useMemoryTagging<Params>(Options)))
storeTags(reinterpret_cast<uptr>(BlockBegin),
reinterpret_cast<uptr>(Ptr));
Secondary.deallocate(Options, BlockBegin);
}
} else {
bool UnlockRequired;
auto *TSD = TSDRegistry.getTSDAndLock(&UnlockRequired);
Quarantine.put(&TSD->QuarantineCache,
QuarantineCallback(*this, TSD->Cache), Ptr, Size);
if (UnlockRequired)
TSD->unlock();
}
}

bool getChunkFromBlock(uptr Block, uptr *Chunk,
Chunk::UnpackedHeader *Header) {
*Chunk =
Block + getChunkOffsetFromBlock(reinterpret_cast<const char *>(Block));
return Chunk::isValid(Cookie, reinterpret_cast<void *>(*Chunk), Header);
}

static uptr getChunkOffsetFromBlock(const char *Block) {
u32 Offset = 0;
if (reinterpret_cast<const u32 *>(Block)[0] == BlockMarker)
Offset = reinterpret_cast<const u32 *>(Block)[1];
return Offset + Chunk::getHeaderSize();
}













void storeEndMarker(uptr End, uptr Size, uptr BlockEnd) {
DCHECK_EQ(BlockEnd, untagPointer(BlockEnd));
uptr UntaggedEnd = untagPointer(End);
if (UntaggedEnd != BlockEnd) {
storeTag(UntaggedEnd);
if (Size == 0)
*reinterpret_cast<u8 *>(UntaggedEnd) = extractTag(End);
}
}

void *prepareTaggedChunk(void *Ptr, uptr Size, uptr ExcludeMask,
uptr BlockEnd) {





storeTag(reinterpret_cast<uptr>(Ptr) - archMemoryTagGranuleSize());

uptr TaggedBegin, TaggedEnd;
setRandomTag(Ptr, Size, ExcludeMask, &TaggedBegin, &TaggedEnd);

storeEndMarker(TaggedEnd, Size, BlockEnd);
return reinterpret_cast<void *>(TaggedBegin);
}

void resizeTaggedChunk(uptr OldPtr, uptr NewPtr, uptr NewSize,
uptr BlockEnd) {
uptr RoundOldPtr = roundUpTo(OldPtr, archMemoryTagGranuleSize());
uptr RoundNewPtr;
if (RoundOldPtr >= NewPtr) {


RoundNewPtr = roundUpTo(NewPtr, archMemoryTagGranuleSize());
} else {



RoundNewPtr = storeTags(RoundOldPtr, NewPtr);
}
storeEndMarker(RoundNewPtr, NewSize, BlockEnd);
}

void storePrimaryAllocationStackMaybe(Options Options, void *Ptr) {
if (!UNLIKELY(Options.get(OptionBit::TrackAllocationStacks)))
return;
auto *Ptr32 = reinterpret_cast<u32 *>(Ptr);
Ptr32[MemTagAllocationTraceIndex] = collectStackTrace();
Ptr32[MemTagAllocationTidIndex] = getThreadID();
}

void storeRingBufferEntry(void *Ptr, u32 AllocationTrace, u32 AllocationTid,
uptr AllocationSize, u32 DeallocationTrace,
u32 DeallocationTid) {
uptr Pos = atomic_fetch_add(&RingBuffer.Pos, 1, memory_order_relaxed);
typename AllocationRingBuffer::Entry *Entry =
&RingBuffer.Entries[Pos % AllocationRingBuffer::NumEntries];





atomic_store_relaxed(&Entry->Ptr, 0);

__atomic_signal_fence(__ATOMIC_SEQ_CST);
atomic_store_relaxed(&Entry->AllocationTrace, AllocationTrace);
atomic_store_relaxed(&Entry->AllocationTid, AllocationTid);
atomic_store_relaxed(&Entry->AllocationSize, AllocationSize);
atomic_store_relaxed(&Entry->DeallocationTrace, DeallocationTrace);
atomic_store_relaxed(&Entry->DeallocationTid, DeallocationTid);
__atomic_signal_fence(__ATOMIC_SEQ_CST);

atomic_store_relaxed(&Entry->Ptr, reinterpret_cast<uptr>(Ptr));
}

void storeSecondaryAllocationStackMaybe(Options Options, void *Ptr,
uptr Size) {
if (!UNLIKELY(Options.get(OptionBit::TrackAllocationStacks)))
return;

u32 Trace = collectStackTrace();
u32 Tid = getThreadID();

auto *Ptr32 = reinterpret_cast<u32 *>(Ptr);
Ptr32[MemTagAllocationTraceIndex] = Trace;
Ptr32[MemTagAllocationTidIndex] = Tid;

storeRingBufferEntry(untagPointer(Ptr), Trace, Tid, Size, 0, 0);
}

void storeDeallocationStackMaybe(Options Options, void *Ptr, u8 PrevTag,
uptr Size) {
if (!UNLIKELY(Options.get(OptionBit::TrackAllocationStacks)))
return;

auto *Ptr32 = reinterpret_cast<u32 *>(Ptr);
u32 AllocationTrace = Ptr32[MemTagAllocationTraceIndex];
u32 AllocationTid = Ptr32[MemTagAllocationTidIndex];

u32 DeallocationTrace = collectStackTrace();
u32 DeallocationTid = getThreadID();

storeRingBufferEntry(addFixedTag(untagPointer(Ptr), PrevTag),
AllocationTrace, AllocationTid, Size,
DeallocationTrace, DeallocationTid);
}

static const size_t NumErrorReports =
sizeof(((scudo_error_info *)0)->reports) /
sizeof(((scudo_error_info *)0)->reports[0]);

static void getInlineErrorInfo(struct scudo_error_info *ErrorInfo,
size_t &NextErrorReport, uintptr_t FaultAddr,
const StackDepot *Depot,
const char *RegionInfoPtr, const char *Memory,
const char *MemoryTags, uintptr_t MemoryAddr,
size_t MemorySize, size_t MinDistance,
size_t MaxDistance) {
uptr UntaggedFaultAddr = untagPointer(FaultAddr);
u8 FaultAddrTag = extractTag(FaultAddr);
BlockInfo Info =
PrimaryT::findNearestBlock(RegionInfoPtr, UntaggedFaultAddr);

auto GetGranule = [&](uptr Addr, const char **Data, uint8_t *Tag) -> bool {
if (Addr < MemoryAddr || Addr + archMemoryTagGranuleSize() < Addr ||
Addr + archMemoryTagGranuleSize() > MemoryAddr + MemorySize)
return false;
*Data = &Memory[Addr - MemoryAddr];
*Tag = static_cast<u8>(
MemoryTags[(Addr - MemoryAddr) / archMemoryTagGranuleSize()]);
return true;
};

auto ReadBlock = [&](uptr Addr, uptr *ChunkAddr,
Chunk::UnpackedHeader *Header, const u32 **Data,
u8 *Tag) {
const char *BlockBegin;
u8 BlockBeginTag;
if (!GetGranule(Addr, &BlockBegin, &BlockBeginTag))
return false;
uptr ChunkOffset = getChunkOffsetFromBlock(BlockBegin);
*ChunkAddr = Addr + ChunkOffset;

const char *ChunkBegin;
if (!GetGranule(*ChunkAddr, &ChunkBegin, Tag))
return false;
*Header = *reinterpret_cast<const Chunk::UnpackedHeader *>(
ChunkBegin - Chunk::getHeaderSize());
*Data = reinterpret_cast<const u32 *>(ChunkBegin);



if (Header->SizeOrUnusedBytes == 0)
*Tag = static_cast<u8>(*ChunkBegin);

return true;
};

if (NextErrorReport == NumErrorReports)
return;

auto CheckOOB = [&](uptr BlockAddr) {
if (BlockAddr < Info.RegionBegin || BlockAddr >= Info.RegionEnd)
return false;

uptr ChunkAddr;
Chunk::UnpackedHeader Header;
const u32 *Data;
uint8_t Tag;
if (!ReadBlock(BlockAddr, &ChunkAddr, &Header, &Data, &Tag) ||
Header.State != Chunk::State::Allocated || Tag != FaultAddrTag)
return false;

auto *R = &ErrorInfo->reports[NextErrorReport++];
R->error_type =
UntaggedFaultAddr < ChunkAddr ? BUFFER_UNDERFLOW : BUFFER_OVERFLOW;
R->allocation_address = ChunkAddr;
R->allocation_size = Header.SizeOrUnusedBytes;
collectTraceMaybe(Depot, R->allocation_trace,
Data[MemTagAllocationTraceIndex]);
R->allocation_tid = Data[MemTagAllocationTidIndex];
return NextErrorReport == NumErrorReports;
};

if (MinDistance == 0 && CheckOOB(Info.BlockBegin))
return;

for (size_t I = Max<size_t>(MinDistance, 1); I != MaxDistance; ++I)
if (CheckOOB(Info.BlockBegin + I * Info.BlockSize) ||
CheckOOB(Info.BlockBegin - I * Info.BlockSize))
return;
}

static void getRingBufferErrorInfo(struct scudo_error_info *ErrorInfo,
size_t &NextErrorReport,
uintptr_t FaultAddr,
const StackDepot *Depot,
const char *RingBufferPtr) {
auto *RingBuffer =
reinterpret_cast<const AllocationRingBuffer *>(RingBufferPtr);
uptr Pos = atomic_load_relaxed(&RingBuffer->Pos);

for (uptr I = Pos - 1; I != Pos - 1 - AllocationRingBuffer::NumEntries &&
NextErrorReport != NumErrorReports;
--I) {
auto *Entry = &RingBuffer->Entries[I % AllocationRingBuffer::NumEntries];
uptr EntryPtr = atomic_load_relaxed(&Entry->Ptr);
if (!EntryPtr)
continue;

uptr UntaggedEntryPtr = untagPointer(EntryPtr);
uptr EntrySize = atomic_load_relaxed(&Entry->AllocationSize);
u32 AllocationTrace = atomic_load_relaxed(&Entry->AllocationTrace);
u32 AllocationTid = atomic_load_relaxed(&Entry->AllocationTid);
u32 DeallocationTrace = atomic_load_relaxed(&Entry->DeallocationTrace);
u32 DeallocationTid = atomic_load_relaxed(&Entry->DeallocationTid);

if (DeallocationTid) {



if (FaultAddr < EntryPtr || FaultAddr >= EntryPtr + EntrySize)
continue;
} else {




if (FaultAddr < EntryPtr - getPageSizeCached() ||
FaultAddr >= EntryPtr + EntrySize + getPageSizeCached())
continue;





bool Found = false;
for (uptr J = 0; J != NextErrorReport; ++J) {
if (ErrorInfo->reports[J].allocation_address == UntaggedEntryPtr) {
Found = true;
break;
}
}
if (Found)
continue;
}

auto *R = &ErrorInfo->reports[NextErrorReport++];
if (DeallocationTid)
R->error_type = USE_AFTER_FREE;
else if (FaultAddr < EntryPtr)
R->error_type = BUFFER_UNDERFLOW;
else
R->error_type = BUFFER_OVERFLOW;

R->allocation_address = UntaggedEntryPtr;
R->allocation_size = EntrySize;
collectTraceMaybe(Depot, R->allocation_trace, AllocationTrace);
R->allocation_tid = AllocationTid;
collectTraceMaybe(Depot, R->deallocation_trace, DeallocationTrace);
R->deallocation_tid = DeallocationTid;
}
}

uptr getStats(ScopedString *Str) {
Primary.getStats(Str);
Secondary.getStats(Str);
Quarantine.getStats(Str);
return Str->length();
}
};

}

#endif
