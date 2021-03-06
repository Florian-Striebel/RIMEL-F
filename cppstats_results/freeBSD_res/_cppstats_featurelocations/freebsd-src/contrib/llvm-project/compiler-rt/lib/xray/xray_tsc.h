










#if !defined(XRAY_EMULATE_TSC_H)
#define XRAY_EMULATE_TSC_H

#include "sanitizer_common/sanitizer_common.h"

namespace __xray {
static constexpr uint64_t NanosecondsPerSecond = 1000ULL * 1000 * 1000;
}

#if SANITIZER_FUCHSIA
#include <zircon/syscalls.h>

namespace __xray {

inline bool probeRequiredCPUFeatures() XRAY_NEVER_INSTRUMENT { return true; }

ALWAYS_INLINE uint64_t readTSC(uint8_t &CPU) XRAY_NEVER_INSTRUMENT {
CPU = 0;
return _zx_ticks_get();
}

inline uint64_t getTSCFrequency() XRAY_NEVER_INSTRUMENT {
return _zx_ticks_per_second();
}

}

#else

#if defined(__x86_64__)
#include "xray_x86_64.inc"
#elif defined(__powerpc64__)
#include "xray_powerpc64.inc"
#elif defined(__arm__) || defined(__aarch64__) || defined(__mips__)










#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "xray_defs.h"
#include <cerrno>
#include <cstdint>
#include <time.h>

namespace __xray {

inline bool probeRequiredCPUFeatures() XRAY_NEVER_INSTRUMENT { return true; }

ALWAYS_INLINE uint64_t readTSC(uint8_t &CPU) XRAY_NEVER_INSTRUMENT {
timespec TS;
int result = clock_gettime(CLOCK_REALTIME, &TS);
if (result != 0) {
Report("clock_gettime(2) returned %d, errno=%d.", result, int(errno));
TS.tv_sec = 0;
TS.tv_nsec = 0;
}
CPU = 0;
return TS.tv_sec * NanosecondsPerSecond + TS.tv_nsec;
}

inline uint64_t getTSCFrequency() XRAY_NEVER_INSTRUMENT {
return NanosecondsPerSecond;
}

}

#else
#error Target architecture is not supported.
#endif
#endif

#endif
