







#if !defined(SCUDO_CHECKSUM_H_)
#define SCUDO_CHECKSUM_H_

#include "internal_defs.h"







#if defined(__SSE4_2__)
#include <smmintrin.h>
#define CRC32_INTRINSIC FIRST_32_SECOND_64(_mm_crc32_u32, _mm_crc32_u64)
#endif
#if defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
#define CRC32_INTRINSIC FIRST_32_SECOND_64(__crc32cw, __crc32cd)
#endif

namespace scudo {

enum class Checksum : u8 {
BSD = 0,
HardwareCRC32 = 1,
};





inline u16 computeBSDChecksum(u16 Sum, uptr Data) {
for (u8 I = 0; I < sizeof(Data); I++) {
Sum = static_cast<u16>((Sum >> 1) | ((Sum & 1) << 15));
Sum = static_cast<u16>(Sum + (Data & 0xff));
Data >>= 8;
}
return Sum;
}

bool hasHardwareCRC32();
WEAK u32 computeHardwareCRC32(u32 Crc, uptr Data);

}

#endif
