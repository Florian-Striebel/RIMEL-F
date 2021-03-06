










#if !defined(LLVM_FUZZER_BUILTINS_MSVC_H)
#define LLVM_FUZZER_BUILTINS_MSVC_H

#include "FuzzerPlatform.h"

#if LIBFUZZER_MSVC
#include <intrin.h>
#include <cstdint>
#include <cstdlib>



#define GET_CALLER_PC() _ReturnAddress()

namespace fuzzer {

inline uint8_t Bswap(uint8_t x) { return x; }


inline uint16_t Bswap(uint16_t x) { return _byteswap_ushort(x); }
inline uint32_t Bswap(uint32_t x) { return _byteswap_ulong(x); }
inline uint64_t Bswap(uint64_t x) { return _byteswap_uint64(x); }




inline uint32_t Clzll(uint64_t X) {
unsigned long LeadZeroIdx = 0;

#if !defined(_M_ARM) && !defined(_M_X64)

if (_BitScanReverse(&LeadZeroIdx, static_cast<unsigned long>(X >> 32)))
return static_cast<int>(63 - (LeadZeroIdx + 32));

if (_BitScanReverse(&LeadZeroIdx, static_cast<unsigned long>(X)))
return static_cast<int>(63 - LeadZeroIdx);

#else
if (_BitScanReverse64(&LeadZeroIdx, X)) return 63 - LeadZeroIdx;
#endif
return 64;
}

inline int Popcountll(unsigned long long X) {
#if !defined(_M_ARM) && !defined(_M_X64)
return __popcnt(X) + __popcnt(X >> 32);
#else
return __popcnt64(X);
#endif
}

}

#endif
#endif
