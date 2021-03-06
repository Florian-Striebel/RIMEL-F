







#if !defined(SCUDO_ALLOCATOR_CONFIG_H_)
#define SCUDO_ALLOCATOR_CONFIG_H_

#include "combined.h"
#include "common.h"
#include "flags.h"
#include "primary32.h"
#include "primary64.h"
#include "secondary.h"
#include "size_class_map.h"
#include "tsd_exclusive.h"
#include "tsd_shared.h"

namespace scudo {






































struct DefaultConfig {
using SizeClassMap = DefaultSizeClassMap;
static const bool MaySupportMemoryTagging = true;

#if SCUDO_CAN_USE_PRIMARY64
typedef SizeClassAllocator64<DefaultConfig> Primary;
static const uptr PrimaryRegionSizeLog = 32U;
typedef uptr PrimaryCompactPtrT;
static const uptr PrimaryCompactPtrScale = 0;
static const bool PrimaryEnableRandomOffset = true;
static const uptr PrimaryMapSizeIncrement = 1UL << 18;
#else
typedef SizeClassAllocator32<DefaultConfig> Primary;
static const uptr PrimaryRegionSizeLog = 19U;
typedef uptr PrimaryCompactPtrT;
#endif
static const s32 PrimaryMinReleaseToOsIntervalMs = INT32_MIN;
static const s32 PrimaryMaxReleaseToOsIntervalMs = INT32_MAX;

typedef MapAllocatorCache<DefaultConfig> SecondaryCache;
static const u32 SecondaryCacheEntriesArraySize = 32U;
static const u32 SecondaryCacheQuarantineSize = 0U;
static const u32 SecondaryCacheDefaultMaxEntriesCount = 32U;
static const uptr SecondaryCacheDefaultMaxEntrySize = 1UL << 19;
static const s32 SecondaryCacheMinReleaseToOsIntervalMs = INT32_MIN;
static const s32 SecondaryCacheMaxReleaseToOsIntervalMs = INT32_MAX;

template <class A> using TSDRegistryT = TSDRegistryExT<A>;
};
struct AndroidConfig {
using SizeClassMap = AndroidSizeClassMap;
static const bool MaySupportMemoryTagging = true;

#if SCUDO_CAN_USE_PRIMARY64
typedef SizeClassAllocator64<AndroidConfig> Primary;
static const uptr PrimaryRegionSizeLog = 28U;
typedef u32 PrimaryCompactPtrT;
static const uptr PrimaryCompactPtrScale = SCUDO_MIN_ALIGNMENT_LOG;
static const bool PrimaryEnableRandomOffset = true;
static const uptr PrimaryMapSizeIncrement = 1UL << 18;
#else
typedef SizeClassAllocator32<AndroidConfig> Primary;
static const uptr PrimaryRegionSizeLog = 18U;
typedef uptr PrimaryCompactPtrT;
#endif
static const s32 PrimaryMinReleaseToOsIntervalMs = 1000;
static const s32 PrimaryMaxReleaseToOsIntervalMs = 1000;

typedef MapAllocatorCache<AndroidConfig> SecondaryCache;
static const u32 SecondaryCacheEntriesArraySize = 256U;
static const u32 SecondaryCacheQuarantineSize = 32U;
static const u32 SecondaryCacheDefaultMaxEntriesCount = 32U;
static const uptr SecondaryCacheDefaultMaxEntrySize = 2UL << 20;
static const s32 SecondaryCacheMinReleaseToOsIntervalMs = 0;
static const s32 SecondaryCacheMaxReleaseToOsIntervalMs = 1000;

template <class A>
using TSDRegistryT = TSDRegistrySharedT<A, 8U, 2U>;
};

struct AndroidSvelteConfig {
using SizeClassMap = SvelteSizeClassMap;
static const bool MaySupportMemoryTagging = false;

#if SCUDO_CAN_USE_PRIMARY64
typedef SizeClassAllocator64<AndroidSvelteConfig> Primary;
static const uptr PrimaryRegionSizeLog = 27U;
typedef u32 PrimaryCompactPtrT;
static const uptr PrimaryCompactPtrScale = SCUDO_MIN_ALIGNMENT_LOG;
static const bool PrimaryEnableRandomOffset = true;
static const uptr PrimaryMapSizeIncrement = 1UL << 18;
#else
typedef SizeClassAllocator32<AndroidSvelteConfig> Primary;
static const uptr PrimaryRegionSizeLog = 16U;
typedef uptr PrimaryCompactPtrT;
#endif
static const s32 PrimaryMinReleaseToOsIntervalMs = 1000;
static const s32 PrimaryMaxReleaseToOsIntervalMs = 1000;

typedef MapAllocatorCache<AndroidSvelteConfig> SecondaryCache;
static const u32 SecondaryCacheEntriesArraySize = 16U;
static const u32 SecondaryCacheQuarantineSize = 32U;
static const u32 SecondaryCacheDefaultMaxEntriesCount = 4U;
static const uptr SecondaryCacheDefaultMaxEntrySize = 1UL << 18;
static const s32 SecondaryCacheMinReleaseToOsIntervalMs = 0;
static const s32 SecondaryCacheMaxReleaseToOsIntervalMs = 0;

template <class A>
using TSDRegistryT = TSDRegistrySharedT<A, 2U, 1U>;
};

#if SCUDO_CAN_USE_PRIMARY64
struct FuchsiaConfig {
using SizeClassMap = FuchsiaSizeClassMap;
static const bool MaySupportMemoryTagging = false;

typedef SizeClassAllocator64<FuchsiaConfig> Primary;
static const uptr PrimaryRegionSizeLog = 30U;
typedef u32 PrimaryCompactPtrT;
static const bool PrimaryEnableRandomOffset = true;
static const uptr PrimaryMapSizeIncrement = 1UL << 18;
static const uptr PrimaryCompactPtrScale = SCUDO_MIN_ALIGNMENT_LOG;
static const s32 PrimaryMinReleaseToOsIntervalMs = INT32_MIN;
static const s32 PrimaryMaxReleaseToOsIntervalMs = INT32_MAX;

typedef MapAllocatorNoCache SecondaryCache;
template <class A>
using TSDRegistryT = TSDRegistrySharedT<A, 8U, 4U>;
};

struct TrustyConfig {
using SizeClassMap = TrustySizeClassMap;
static const bool MaySupportMemoryTagging = false;

typedef SizeClassAllocator64<TrustyConfig> Primary;

static const uptr PrimaryRegionSizeLog = 10U;
typedef u32 PrimaryCompactPtrT;
static const bool PrimaryEnableRandomOffset = false;

static const uptr PrimaryMapSizeIncrement = 1UL << 4;
static const uptr PrimaryCompactPtrScale = SCUDO_MIN_ALIGNMENT_LOG;
static const s32 PrimaryMinReleaseToOsIntervalMs = INT32_MIN;
static const s32 PrimaryMaxReleaseToOsIntervalMs = INT32_MAX;

typedef MapAllocatorNoCache SecondaryCache;
template <class A>
using TSDRegistryT = TSDRegistrySharedT<A, 1U, 1U>;
};
#endif

#if SCUDO_ANDROID
typedef AndroidConfig Config;
#elif SCUDO_FUCHSIA
typedef FuchsiaConfig Config;
#elif SCUDO_TRUSTY
typedef TrustyConfig Config;
#else
typedef DefaultConfig Config;
#endif

}

#endif
