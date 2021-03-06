







#if !defined(SCUDO_TSD_EXCLUSIVE_H_)
#define SCUDO_TSD_EXCLUSIVE_H_

#include "tsd.h"

namespace scudo {

struct ThreadState {
bool DisableMemInit : 1;
enum {
NotInitialized = 0,
Initialized,
TornDown,
} InitState : 2;
};

template <class Allocator> void teardownThread(void *Ptr);

template <class Allocator> struct TSDRegistryExT {
void init(Allocator *Instance) {
DCHECK(!Initialized);
Instance->init();
CHECK_EQ(pthread_key_create(&PThreadKey, teardownThread<Allocator>), 0);
FallbackTSD.init(Instance);
Initialized = true;
}

void initOnceMaybe(Allocator *Instance) {
ScopedLock L(Mutex);
if (LIKELY(Initialized))
return;
init(Instance);
}

void unmapTestOnly(Allocator *Instance) {
DCHECK(Instance);
if (reinterpret_cast<Allocator *>(pthread_getspecific(PThreadKey))) {
DCHECK_EQ(reinterpret_cast<Allocator *>(pthread_getspecific(PThreadKey)),
Instance);
ThreadTSD.commitBack(Instance);
ThreadTSD = {};
}
CHECK_EQ(pthread_key_delete(PThreadKey), 0);
PThreadKey = {};
FallbackTSD.commitBack(Instance);
FallbackTSD = {};
State = {};
Initialized = false;
}

ALWAYS_INLINE void initThreadMaybe(Allocator *Instance, bool MinimalInit) {
if (LIKELY(State.InitState != ThreadState::NotInitialized))
return;
initThread(Instance, MinimalInit);
}

ALWAYS_INLINE TSD<Allocator> *getTSDAndLock(bool *UnlockRequired) {
if (LIKELY(State.InitState == ThreadState::Initialized &&
!atomic_load(&Disabled, memory_order_acquire))) {
*UnlockRequired = false;
return &ThreadTSD;
}
FallbackTSD.lock();
*UnlockRequired = true;
return &FallbackTSD;
}



void disable() {
Mutex.lock();
FallbackTSD.lock();
atomic_store(&Disabled, 1U, memory_order_release);
}

void enable() {
atomic_store(&Disabled, 0U, memory_order_release);
FallbackTSD.unlock();
Mutex.unlock();
}

bool setOption(Option O, UNUSED sptr Value) {
if (O == Option::ThreadDisableMemInit)
State.DisableMemInit = Value;
if (O == Option::MaxTSDsCount)
return false;
return true;
}

bool getDisableMemInit() { return State.DisableMemInit; }

private:



NOINLINE void initThread(Allocator *Instance, bool MinimalInit) {
initOnceMaybe(Instance);
if (UNLIKELY(MinimalInit))
return;
CHECK_EQ(
pthread_setspecific(PThreadKey, reinterpret_cast<void *>(Instance)), 0);
ThreadTSD.init(Instance);
State.InitState = ThreadState::Initialized;
Instance->callPostInitCallback();
}

pthread_key_t PThreadKey = {};
bool Initialized = false;
atomic_u8 Disabled = {};
TSD<Allocator> FallbackTSD;
HybridMutex Mutex;
static thread_local ThreadState State;
static thread_local TSD<Allocator> ThreadTSD;

friend void teardownThread<Allocator>(void *Ptr);
};

template <class Allocator>
thread_local TSD<Allocator> TSDRegistryExT<Allocator>::ThreadTSD;
template <class Allocator>
thread_local ThreadState TSDRegistryExT<Allocator>::State;

template <class Allocator> void teardownThread(void *Ptr) {
typedef TSDRegistryExT<Allocator> TSDRegistryT;
Allocator *Instance = reinterpret_cast<Allocator *>(Ptr);





if (TSDRegistryT::ThreadTSD.DestructorIterations > 1) {
TSDRegistryT::ThreadTSD.DestructorIterations--;

if (LIKELY(pthread_setspecific(Instance->getTSDRegistry()->PThreadKey,
Ptr) == 0))
return;
}
TSDRegistryT::ThreadTSD.commitBack(Instance);
TSDRegistryT::State.InitState = ThreadState::TornDown;
}

}

#endif
