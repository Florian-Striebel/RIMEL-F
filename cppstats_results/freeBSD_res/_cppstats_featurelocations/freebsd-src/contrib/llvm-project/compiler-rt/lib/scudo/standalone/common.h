







#if !defined(SCUDO_COMMON_H_)
#define SCUDO_COMMON_H_

#include "internal_defs.h"

#include "fuchsia.h"
#include "linux.h"
#include "trusty.h"

#include <stddef.h>
#include <string.h>

namespace scudo {

template <class Dest, class Source> inline Dest bit_cast(const Source &S) {
static_assert(sizeof(Dest) == sizeof(Source), "");
Dest D;
memcpy(&D, &S, sizeof(D));
return D;
}

inline constexpr uptr roundUpTo(uptr X, uptr Boundary) {
return (X + Boundary - 1) & ~(Boundary - 1);
}

inline constexpr uptr roundDownTo(uptr X, uptr Boundary) {
return X & ~(Boundary - 1);
}

inline constexpr bool isAligned(uptr X, uptr Alignment) {
return (X & (Alignment - 1)) == 0;
}

template <class T> constexpr T Min(T A, T B) { return A < B ? A : B; }

template <class T> constexpr T Max(T A, T B) { return A > B ? A : B; }

template <class T> void Swap(T &A, T &B) {
T Tmp = A;
A = B;
B = Tmp;
}

inline bool isPowerOfTwo(uptr X) { return (X & (X - 1)) == 0; }

inline uptr getMostSignificantSetBitIndex(uptr X) {
DCHECK_NE(X, 0U);
return SCUDO_WORDSIZE - 1U - static_cast<uptr>(__builtin_clzl(X));
}

inline uptr roundUpToPowerOfTwo(uptr Size) {
DCHECK(Size);
if (isPowerOfTwo(Size))
return Size;
const uptr Up = getMostSignificantSetBitIndex(Size);
DCHECK_LT(Size, (1UL << (Up + 1)));
DCHECK_GT(Size, (1UL << Up));
return 1UL << (Up + 1);
}

inline uptr getLeastSignificantSetBitIndex(uptr X) {
DCHECK_NE(X, 0U);
return static_cast<uptr>(__builtin_ctzl(X));
}

inline uptr getLog2(uptr X) {
DCHECK(isPowerOfTwo(X));
return getLeastSignificantSetBitIndex(X);
}

inline u32 getRandomU32(u32 *State) {



*State ^= *State << 13;
*State ^= *State >> 17;
*State ^= *State << 5;
return *State;
}

inline u32 getRandomModN(u32 *State, u32 N) {
return getRandomU32(State) % N;
}

template <typename T> inline void shuffle(T *A, u32 N, u32 *RandState) {
if (N <= 1)
return;
u32 State = *RandState;
for (u32 I = N - 1; I > 0; I--)
Swap(A[I], A[getRandomModN(&State, I + 1)]);
*RandState = State;
}



inline void yieldProcessor(u8 Count) {
#if defined(__i386__) || defined(__x86_64__)
__asm__ __volatile__("" ::: "memory");
for (u8 I = 0; I < Count; I++)
__asm__ __volatile__("pause");
#elif defined(__aarch64__) || defined(__arm__)
__asm__ __volatile__("" ::: "memory");
for (u8 I = 0; I < Count; I++)
__asm__ __volatile__("yield");
#endif
__asm__ __volatile__("" ::: "memory");
}



extern uptr PageSizeCached;
uptr getPageSizeSlow();
inline uptr getPageSizeCached() {

if (SCUDO_ANDROID)
return 4096U;
if (LIKELY(PageSizeCached))
return PageSizeCached;
return getPageSizeSlow();
}


u32 getNumberOfCPUs();

const char *getEnv(const char *Name);

u64 getMonotonicTime();

u32 getThreadID();



constexpr uptr MaxRandomLength = 256U;
bool getRandom(void *Buffer, uptr Length, bool Blocking = false);



#define MAP_ALLOWNOMEM (1U << 0)
#define MAP_NOACCESS (1U << 1)
#define MAP_RESIZABLE (1U << 2)
#define MAP_MEMTAG (1U << 3)









void *map(void *Addr, uptr Size, const char *Name, uptr Flags = 0,
MapPlatformData *Data = nullptr);



#define UNMAP_ALL (1U << 0)

void unmap(void *Addr, uptr Size, uptr Flags = 0,
MapPlatformData *Data = nullptr);

void setMemoryPermission(uptr Addr, uptr Size, uptr Flags,
MapPlatformData *Data = nullptr);

void releasePagesToOS(uptr BaseAddress, uptr Offset, uptr Size,
MapPlatformData *Data = nullptr);



void NORETURN dieOnMapUnmapError(uptr SizeIfOOM = 0);



void setAbortMessage(const char *Message);

struct BlockInfo {
uptr BlockBegin;
uptr BlockSize;
uptr RegionBegin;
uptr RegionEnd;
};

enum class Option : u8 {
ReleaseInterval,
MemtagTuning,
ThreadDisableMemInit,

MaxCacheEntriesCount,
MaxCacheEntrySize,
MaxTSDsCount,
};

constexpr unsigned char PatternFillByte = 0xAB;

enum FillContentsMode {
NoFill = 0,
ZeroFill = 1,
PatternOrZeroFill = 2

};

}

#endif
