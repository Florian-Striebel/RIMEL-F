







#if !defined(GWP_ASAN_STACK_TRACE_COMPRESSOR_)
#define GWP_ASAN_STACK_TRACE_COMPRESSOR_

#include <stddef.h>
#include <stdint.h>






namespace gwp_asan {
namespace compression {




size_t pack(const uintptr_t *Unpacked, size_t UnpackedSize, uint8_t *Packed,
size_t PackedMaxSize);




size_t unpack(const uint8_t *Packed, size_t PackedSize, uintptr_t *Unpacked,
size_t UnpackedMaxSize);

}
}

#endif
