







#if !defined(SCUDO_TSD_SHARED_H_)
#define SCUDO_TSD_SHARED_H_

#include "tsd.h"

#if SCUDO_HAS_PLATFORM_TLS_SLOT





#include "scudo_platform_tls_slot.h"
#endif

namespace scudo {

template <class Allocator, u32 TSDsArraySize, u32 DefaultTSDCount>
struct TSDRegistrySharedT {
void init(Allocator *Instance) {
DCHECK(!Initialized);
Instance->init();
for (u32 I = 0; I < TSDsArraySize; I++)
TSDs[I].init(Instance);
const u32 NumberOfCPUs = getNumberOfCPUs();
setNumberOfTSDs((NumberOfCPUs == 0) ? DefaultTSDCount
: Min(NumberOfCPUs, DefaultTSDCount));
Initialized = true;
}

void initOnceMaybe(Allocator *Instance) {
ScopedLock L(Mutex);
if (LIKELY(Initialized))
return;
init(Instance);
}

void unmapTestOnly(Allocator *Instance) {
for (u32 I = 0; I < TSDsArraySize; I++) {
TSDs[I].commitBack(Instance);
TSDs[I] = {};
}
setCurrentTSD(nullptr);
Initialized = false;
}

ALWAYS_INLINE void initThreadMaybe(Allocator *Instance,
UNUSED bool MinimalInit) {
if (LIKELY(getCurrentTSD()))
return;
initThread(Instance);
}

ALWAYS_INLINE TSD<Allocator> *getTSDAndLock(bool *UnlockRequired) {
TSD<Allocator> *TSD = getCurrentTSD();
DCHECK(TSD);
*UnlockRequired = true;

if (TSD->tryLock())
return TSD;

if (TSDsArraySize == 1U) {


TSD->lock();
return TSD;
}
return getTSDAndLockSlow(TSD);
}

void disable() {
Mutex.lock();
for (u32 I = 0; I < TSDsArraySize; I++)
TSDs[I].lock();
}

void enable() {
for (s32 I = static_cast<s32>(TSDsArraySize - 1); I >= 0; I--)
TSDs[I].unlock();
Mutex.unlock();
}

bool setOption(Option O, sptr Value) {
if (O == Option::MaxTSDsCount)
return setNumberOfTSDs(static_cast<u32>(Value));
if (O == Option::ThreadDisableMemInit)
setDisableMemInit(Value);

return true;
}

bool getDisableMemInit() const { return *getTlsPtr() & 1; }

private:
ALWAYS_INLINE uptr *getTlsPtr() const {
#if SCUDO_HAS_PLATFORM_TLS_SLOT
return reinterpret_cast<uptr *>(getPlatformAllocatorTlsSlot());
#else
static thread_local uptr ThreadTSD;
return &ThreadTSD;
#endif
}

static_assert(alignof(TSD<Allocator>) >= 2, "");

ALWAYS_INLINE void setCurrentTSD(TSD<Allocator> *CurrentTSD) {
*getTlsPtr() &= 1;
*getTlsPtr() |= reinterpret_cast<uptr>(CurrentTSD);
}

ALWAYS_INLINE TSD<Allocator> *getCurrentTSD() {
return reinterpret_cast<TSD<Allocator> *>(*getTlsPtr() & ~1ULL);
}

bool setNumberOfTSDs(u32 N) {
ScopedLock L(MutexTSDs);
if (N < NumberOfTSDs)
return false;
if (N > TSDsArraySize)
N = TSDsArraySize;
NumberOfTSDs = N;
NumberOfCoPrimes = 0;



for (u32 I = 0; I < N; I++) {
u32 A = I + 1;
u32 B = N;

while (B != 0) {
const u32 T = A;
A = B;
B = T % B;
}
if (A == 1)
CoPrimes[NumberOfCoPrimes++] = I + 1;
}
return true;
}

void setDisableMemInit(bool B) {
*getTlsPtr() &= ~1ULL;
*getTlsPtr() |= B;
}

NOINLINE void initThread(Allocator *Instance) {
initOnceMaybe(Instance);

const u32 Index = atomic_fetch_add(&CurrentIndex, 1U, memory_order_relaxed);
setCurrentTSD(&TSDs[Index % NumberOfTSDs]);
Instance->callPostInitCallback();
}

NOINLINE TSD<Allocator> *getTSDAndLockSlow(TSD<Allocator> *CurrentTSD) {



const u32 R = static_cast<u32>(CurrentTSD->getPrecedence());
u32 N, Inc;
{
ScopedLock L(MutexTSDs);
N = NumberOfTSDs;
DCHECK_NE(NumberOfCoPrimes, 0U);
Inc = CoPrimes[R % NumberOfCoPrimes];
}
if (N > 1U) {
u32 Index = R % N;
uptr LowestPrecedence = UINTPTR_MAX;
TSD<Allocator> *CandidateTSD = nullptr;

for (u32 I = 0; I < Min(4U, N); I++) {
if (TSDs[Index].tryLock()) {
setCurrentTSD(&TSDs[Index]);
return &TSDs[Index];
}
const uptr Precedence = TSDs[Index].getPrecedence();

if (Precedence && Precedence < LowestPrecedence) {
CandidateTSD = &TSDs[Index];
LowestPrecedence = Precedence;
}
Index += Inc;
if (Index >= N)
Index -= N;
}
if (CandidateTSD) {
CandidateTSD->lock();
setCurrentTSD(CandidateTSD);
return CandidateTSD;
}
}

CurrentTSD->lock();
return CurrentTSD;
}

atomic_u32 CurrentIndex = {};
u32 NumberOfTSDs = 0;
u32 NumberOfCoPrimes = 0;
u32 CoPrimes[TSDsArraySize] = {};
bool Initialized = false;
HybridMutex Mutex;
HybridMutex MutexTSDs;
TSD<Allocator> TSDs[TSDsArraySize];
};

}

#endif
